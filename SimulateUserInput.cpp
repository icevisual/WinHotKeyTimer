#include "stdafx.h"
#include<sstream>
#include <map>
using namespace std;

static FileConfig g_argv;

DWORD WINAPI ThreadFunction(LPVOID lpParam)
{
	
	INT SleepMilliSeconds = g_argv.GetArgv_INT("t", 2000);
	string SimulateString = g_argv.GetArgv_string("s", "");
	if (SimulateString.length() > 0)
	{
		Sleep(SleepMilliSeconds);
		ConvertChar2KeyWordAndSimulate(SimulateString);
		WORD Keys[] = { VK_RETURN };
		SimulateKeyArrayInput(Keys, sizeof(Keys) / sizeof(WORD));
	}
	return 0;
}

INT mainCRTStartup(int argc, CHAR * argv[])
{
	g_argv.ParseArgvs(argc, argv);

	INT SleepMilliSeconds = g_argv.GetArgv_INT("t", 2000);
	string SimulateString = g_argv.GetArgv_string("s", "");
	if (SimulateString.length() > 0)
	{
		Sleep(SleepMilliSeconds);
		ConvertChar2KeyWordAndSimulate(SimulateString);
		WORD Keys[] = { VK_RETURN };
		SimulateKeyArrayInput(Keys, sizeof(Keys) / sizeof(WORD));
	}
	return 0;
}


INT mainSUI(int argc, CHAR * argv[]) 
{
	g_argv.ParseArgvs(argc, argv);

	DWORD   dwThreadId;
	HANDLE  hThread;
	hThread = CreateThread(
		NULL,                   // default security attributes
		0,                      // use default stack size  
		ThreadFunction,       // thread function name
		NULL,          // argument to thread function 
		0,                      // use default creation flags 
		&dwThreadId);   // returns the thread identifier 

	if (hThread == NULL)
	{
		printf("CreateThread Error \n");
		return -1;
	}

	WaitForSingleObject(hThread, 15000);
	CloseHandle(hThread);
	// thread

	// -t  2000
	// --s=QGX78eMAw5C2UWdL
	return 0;
}
