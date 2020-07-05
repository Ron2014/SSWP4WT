// #pragma comment( linker, "/subsystem:/"windows/" /entry:/"mainCRTStartup/"" ) 

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include <string>
#include <sstream>
#include "path.h"
#include "hideconsole.h"
using namespace std;

void mySystem(const string& cmd, const string& par)
{
    SHELLEXECUTEINFO ShExecInfo = {0};
    ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
    ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
    ShExecInfo.hwnd = NULL;
    ShExecInfo.lpVerb = NULL;
    ShExecInfo.lpFile = cmd.c_str();//调用的程序名
    ShExecInfo.lpParameters = par.c_str();//调用程序的命令行参数
    ShExecInfo.lpDirectory = NULL;
    ShExecInfo.nShow = SW_HIDE;//窗口状态为隐藏
    ShExecInfo.hInstApp = NULL;
    ShellExecuteEx(&ShExecInfo); //启动新的程序
    WaitForSingleObject(ShExecInfo.hProcess,INFINITE);////等到该进程结束
}

int main(int argc, const char *argv[]) {
    if (argc <= 1) return 1;
    
	HideConsole();
    AdaptWorkingPath();

    stringstream param;
    for (int i=2; *(argv+i); i++) {
        if (param.str().length()) param << " ";
        param << argv+i;
    }

    mySystem(argv[1], param.str());
    return 0;
}