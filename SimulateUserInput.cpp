#include "stdafx.h"
#include<sstream>
#include <map>
using namespace std;

map<string, string> g_Argvs;
VOID ParseArgvs(int argc, CHAR * argv[])
{
	for (int i = 1; i < argc; i++)
	{
		string line(argv[i]);
		INT length = line.length();
		if (line[0] == '-' && length >= 2)
		{
			size_t loc = line.find('=', 0);
			if (loc != string::npos)
			{
				int t = 0;
				while (t < length && argv[i][t++] == '-');
				string k_str = line.substr(t - 1, loc - t + 1);
				string v_str = line.substr(loc + 1);
				g_Argvs.insert(pair<string, string>(k_str, v_str));
				printf("K = |%s| V = |%s|\n"
					, k_str.c_str()
					, v_str.c_str());
			}
			else
			{
				// -t 
				int t = 0;
				while (t < length && argv[i][t++] == '-');
				string k_str = line.substr(t - 1);
				if (argc > i + 1)
				{
					string v_str(argv[i + 1]);
					printf("K = |%s| V = |%s|\n"
						, k_str.c_str()
						, v_str.c_str());
					g_Argvs.insert(pair<string, string>(k_str, v_str));
					i++;
				}
				else
				{
					g_Argvs.insert(pair<string, string>(k_str, ""));
				}
			}
		}
		else
		{
			printf("Unresolved Params %s\n", line.c_str());
		}
	}
}
INT GetSettingInt(string key,INT defaultValue)
{
	map<string, string>::iterator finded;
	finded = g_Argvs.find(key);
	if (finded != g_Argvs.end())
	{
		INT r = 0;
		stringstream stream(finded->second);
		stream >> r;
		return r;
	}
	return defaultValue;
}

string GetSettingStr(string key,string defailtValue)
{
	map<string, string>::iterator finded;
	finded = g_Argvs.find(key);
	if (finded != g_Argvs.end())
	{
		return finded->second;
	}
	return defailtValue;
}

DWORD WINAPI ThreadFunction(LPVOID lpParam)
{
	
	INT SleepMilliSeconds = GetSettingInt("t", 2000);
	string SimulateString = GetSettingStr("s", "");
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
	ParseArgvs(argc, argv);

	INT SleepMilliSeconds = GetSettingInt("t", 2000);
	string SimulateString = GetSettingStr("s", "");
	if (SimulateString.length() > 0)
	{
		Sleep(SleepMilliSeconds);
		ConvertChar2KeyWordAndSimulate(SimulateString);
		WORD Keys[] = { VK_RETURN };
		SimulateKeyArrayInput(Keys, sizeof(Keys) / sizeof(WORD));
	}
	return 0;
}


INT main(int argc, CHAR * argv[]) 
{
	ParseArgvs(argc, argv);

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
