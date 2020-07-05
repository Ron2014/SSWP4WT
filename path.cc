#include "path.h"

#if _WIN32
#include <windows.h>
#include <io.h>
#else
#include <errno.h>
#include <dirent.h>
#endif

#include <algorithm>
#include <iostream>

static std::string::size_type 
FindMax(std::string::size_type a, std::string::size_type b) {
	if (a == std::string::npos) {
		return b;
	}

	if (b == std::string::npos) {
		return a;
	}

	return a > b ? a : b;
}

std::string getPath(const std::string& path) {
	std::string::size_type pos = path.rfind("/");
	// in case we are using backslashes on a platform that doesn't use backslashes
	pos = FindMax(pos, path.rfind("\\"));
	if (pos != std::string::npos) {
		return path.substr(0, pos);
	}
	return "";
}

std::string getExtension(const std::string& fileName, bool includeDot) {
	std::string::size_type dotPos = fileName.rfind(".");
	if (dotPos != std::string::npos) {
		string extension = fileName.substr(dotPos + (includeDot ? 0 : 1));
        std::for_each(extension.begin(), extension.end(), [](char &c) { c = ::tolower(c); });
        return extension;
	}
	return "";
}

#ifdef _WIN32

void searchFilesByType(const string &path, const string &type, vector<string> &files) {
    _finddata_t file;
    std::string filepath = path + "/*";
    intptr_t handle;

    if ( (int)(handle = _findfirst(filepath.c_str(), &file)) == -1 ) {
        return;
    }

    do {
        if (file.attrib & _A_SUBDIR) {
            std::string s = file.name;
            if (s == "." || s == "..")
                continue;

            std::string dirname = path + '/' + file.name;
            searchFilesByType(dirname, type, files);

        } else {
            if (getExtension(file.name).compare(type)==0) {
                std::string filename = path + '/' + file.name;
                files.push_back(filename);
            }
        }

    } while( _findnext(handle, &file) == 0 );

    _findclose(handle);
}

#else

void searchFilesByType(const string &path, const string &type, vector<string> files) {
    DIR *p_dir;
    struct dirent *p_dirent;

    if((p_dir = opendir(path.c_str())) == NULL) {
        return;
    }

    while((p_dirent = readdir(p_dir))) {
        if (DT_REG == p_dirent->d_type) {
            // normal type file
            if (getExtension(p_dirent->d_name).compare(type)==0) {
                std::string filepath = path + '/' + p_dirent->d_name;
                files.push_back(filepath);
            }

        }else if (DT_DIR == p_dirent->d_type) {
            // directory
            std::string s = p_dirent->d_name;
            if (s == "." || s == "..")
                continue;
                
            std::string dirpath = path + '/' + p_dirent->d_name;
            searchFilesByType(dirpath, type, files);
        }
    }

    closedir(p_dir);
}

#endif

void AdaptWorkingPath() {
    // __FILE__ is the path of source file, not the executable
    //     string path = getPath(__FILE__);
    cout << "__FILE__:" << __FILE__ << endl;
    cout << "_pgmptr:" << _pgmptr << endl;
    string path = getPath(_pgmptr);
#ifdef _WIN32
    SetCurrentDirectory(path.c_str());
#else
    chdir(path.c_str());
#endif
}