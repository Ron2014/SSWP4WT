#include <string>
#include <vector>
using namespace std;

std::string getPath(const std::string& path);
std::string getExtension(const std::string& fileName, bool includeDot = false);
void AdaptWorkingPath();
void searchFilesByType(const string &path, const string &type, vector<string> &files);