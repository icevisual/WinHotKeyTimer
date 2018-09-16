#pragma once
//

#include <iostream>  
#include <map> 

using namespace std;

class FileConfig {

private:
	map<string, string> g_Config;
	map<string, string> g_Argvs;

public:
	BOOLEAN LoadConfigFromFile(TCHAR * filename);
	INT GetConfig_INT(string key_str, INT Default);
	string GetConfig_string(string key_str, string Default);


	VOID ParseArgvs(int argc, CHAR * argv[]);
	INT GetArgv_INT(string key, INT defaultValue);
	string GetArgv_string(string key, string defailtValue);

};