// WinGHotKey.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
//
#include <sstream>  
#include <iostream>  
#include <map> 

using namespace std;

// 从 srt 文件 气味播放指令
BOOLEAN FileConfig::LoadConfigFromFile(TCHAR * filename) {

	if (_taccess(filename, 0) == -1)
	{
		wprintf(L"Config File (%s) Not Found\n", filename);
		return FALSE;
	}
	FILE * fp = NULL;
	errno_t error = _tfopen_s(&fp, filename, _T("r"));
	if (0 != error) {
		ShowLastErrorMsg(L"_tfopen_s");
		return FALSE;
	}
	CHAR line[255] = { 0 };
	INT pos[2] = { 0 }, smellID = 0;
	size_t lLength = 0;
	INT EqPos = -1;
	while (!feof(fp))
	{
		fgets(line, sizeof(line), fp);
		lLength = strlen(line);

		TrimNewLine(line, lLength);
		lLength = strlen(line);
		if ((EqPos = IndexOf(line, lLength, '=')) > 0 && EqPos < lLength - 1)
		{
			string key_str(line, EqPos);
			string value_str(line + EqPos + 1);
			this->g_Config.insert(pair<string, string>(key_str, value_str));
		}
		printf("%s %zd\n", line, strlen(line));
	}
	fclose(fp);
	return TRUE;
}
INT FileConfig::GetConfig_INT(string key_str, INT Default)
{
	printf("Get_From_G_Config (%s %d)\n", key_str.c_str(), Default);
	map<string, string>::iterator finded;
	finded = this->g_Config.find(key_str);
	if (finded != this->g_Config.end())
	{
		stringstream value_stream(finded->second);
		int value_int = 0;
		value_stream >> value_int;
		return value_int;
	}
	return Default;
}

string FileConfig::GetConfig_string(string key_str, string Default)
{
	printf("Get_From_G_Config (%s %s)\n", key_str.c_str(), Default.c_str());
	map<string, string>::iterator finded;
	finded = this->g_Config.find(key_str);
	if (finded != this->g_Config.end())
	{
		return finded->second;
	}
	return Default;
}



VOID FileConfig::ParseArgvs(int argc, CHAR * argv[])
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
INT FileConfig::GetArgv_INT(string key, INT defaultValue)
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

string FileConfig::GetArgv_string(string key, string defailtValue)
{
	map<string, string>::iterator finded;
	finded = g_Argvs.find(key);
	if (finded != g_Argvs.end())
	{
		return finded->second;
	}
	return defailtValue;
}
