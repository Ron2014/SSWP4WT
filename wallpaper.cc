#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <time.h>
#include "SimpleIni.h"
#include "path.h"
#include "upenv.h"
#include "hideconsole.h"
using namespace std;

#define CONFIG "config.ini"
#define DATA "data.ini"
#define BUF_LEN 256

struct Node {
    string name;
    string filepath;
    time_t tm;
};

vector<string> files;
map<string, Node*> datas;
time_t livetime;
int next_idx;

int getFileIdx(const string &filepath) {
    auto it = find(files.begin(), files.end(), filepath);
    if (it!=files.end()) return it-files.begin();
    return -1;
}

void updateData(string &filepath, time_t &tm, time_t now, Node *node) {
    if (node && node->tm > now - livetime) {
        // dont need update
        filepath = node->filepath;
        tm = node->tm;
    } else {
        next_idx = (next_idx + 1) % (files.size());
        filepath = files[next_idx];
        tm = now;
    }
}

int main(int argc, char *argv[]) {
    HideConsole();
    AdaptWorkingPath();

    freopen("./out.log", "w", stdout);
    freopen("./err.log", "w", stderr);

	bool utf8 = true;
	bool multiKey = true;
    CSimpleIniA config(utf8, multiKey);

    // load from a data file
    SI_Error rc = config.LoadFile(CONFIG);
    if (rc != SI_OK) {
        cerr << "loading " << CONFIG << " ERROR" << endl;
        fclose(stdout);
        fclose(stderr);
        return 1;
    }
    
    livetime = atoll(config.GetValue("TIME_OUT", "livetime"));

	CSimpleIniA::TNamesDepend pathes;
    config.GetAllValues("PATH", "path", pathes);
	pathes.sort(CSimpleIniA::Entry::LoadOrder());

	CSimpleIniA::TNamesDepend types;
    config.GetAllValues("TYPE", "type", types);
	types.sort(CSimpleIniA::Entry::LoadOrder());

	CSimpleIniA::TNamesDepend names;
    config.GetAllValues("NAME", "name", names);
	names.sort(CSimpleIniA::Entry::LoadOrder());

	CSimpleIniA::TNamesDepend::const_iterator it0;
	CSimpleIniA::TNamesDepend::const_iterator it1;
    files.clear();
	for (it0 = pathes.begin(); it0 != pathes.end(); ++it0) {
		for (it1 = types.begin(); it1 != types.end(); ++it1) {
            searchFilesByType(it0->pItem, it1->pItem, files);
        }
	}
    sort(files.begin(), files.end());

    CSimpleIniA data(utf8, !multiKey);
    rc = data.LoadFile(DATA);

    datas.clear();
    next_idx = 0;

	CSimpleIniA::TNamesDepend::const_iterator it3;
    if (rc == SI_OK) {
	    for (it3 = names.begin(); it3 != names.end(); ++it3) {
            if (!data.GetSection(it3->pItem)) continue;
            Node *node = new Node;
            node->name = it3->pItem;
            node->filepath = data.GetValue(it3->pItem, "filepath");
            node->tm = atoll(data.GetValue(it3->pItem, "tm"));
            datas.insert({node->name, node});
            int idx = getFileIdx(node->filepath);
            if (idx > next_idx) next_idx = idx;
        }
    }

    char s_tmp[BUF_LEN];
    time_t now = time(NULL);
	for (it3 = names.begin(); it3 != names.end(); ++it3) {
        string filepath;
        time_t tm;
        updateData(filepath, tm, now, datas[it3->pItem]);
        
        // modify data
        data.SetValue(it3->pItem, "filepath", filepath.c_str());
        snprintf(s_tmp, BUF_LEN, "%lld", tm);
        data.SetValue(it3->pItem, "tm", s_tmp);

        // modify system environment variable

        // 1. system call
        // snprintf(s_tmp, BUF_LEN, "set %s=\"%s\"", it3->pItem, filepath.c_str());
        // system(s_tmp);

        // 2. win32 API
        // snprintf(s_tmp, BUF_LEN, "\"%s\"", filepath.c_str());
        // bool suc = SetEnvironmentVariable(it3->pItem, s_tmp);
        // if (!suc) cerr << GetLastError() << endl;

        // 3. putenv
        // snprintf(s_tmp, BUF_LEN, "%s=\"%s\"", it3->pItem, filepath.c_str());
        // putenv(s_tmp);
        
        // snprintf(s_tmp, BUF_LEN, "\"%s\"", filepath.c_str());
        setEnvVar(it3->pItem, filepath.c_str());
    }

    // save file
    rc = data.SaveFile(DATA);
    if (rc != SI_OK) {
        cerr << "SAVING " << DATA << " ERROR" << endl;
        fclose(stdout);
        fclose(stderr);
        return 1;
    }

    fclose(stdout);
    fclose(stderr);
    return 0;
}