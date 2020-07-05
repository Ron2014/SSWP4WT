#include <Windows.h>
#include <iostream>
#include <process.h>
#include <assert.h>
using namespace std;
 
int main(int argc, char* argv[])
{
	//HDESK hDesk = CreateDesktop(_T("MyDesk"), NULL, NULL, 0, GENERIC_ALL, NULL);
	//assert(hDesk);
 
	STARTUPINFO si = {sizeof(si)};
	si.lpDesktop = "MyDesk";        //不需要CreateDesktop,只要不为空就行
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
	//si.lpTitle = NULL;
	//si.cb = sizeof(STARTUPINFO);
	PROCESS_INFORMATION pi = {0};
 
	DWORD dwExitCode = 0;
	if(CreateProcess(NULL, argv[1], NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{
		CloseHandle(pi.hThread);
		// CloseHandle(pi.hProcess);
 
		//等待进程结束
		WaitForSingleObject(pi.hProcess, INFINITE);
		GetExitCodeProcess(pi.hProcess, &dwExitCode);
		CloseHandle(pi.hProcess);
 
		printf("the process exit. hProcess: %p, processId: %d, dwExitCode: %d\n", 
			pi.hProcess, pi.dwProcessId, dwExitCode);
	}
 
	return 0;
}