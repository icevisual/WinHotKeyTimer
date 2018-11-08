// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#include "utils.h"

typedef int(*pScentrealmInit)();
typedef int(*pScentrealmPlaySmell)(int smell, int duration);
typedef int(*pScentrealmStopPlay)();
typedef int(*pScentrealmGetConnectStatus)();
typedef int(*pScentrealmClose)();

struct ScentrealmRuntime
{
	unsigned char DllState;  // is dll imported & function initialized
	unsigned long LastErrorCode;   // dll error code
	unsigned char ConnectState;    // connect status ,0 device connected，1 device unconnected，2 controller unconnected

	bool isReconnecting;

	int(*pInit)();
	int(*pPlaySmell)(int smell, int duration);
	int(*pStopPlay)();
	int(*pGetConnectStatus)();
	int(*pClose)();
};

#define CS_UNKNOWN 99
#define CS_DEV_CONNECTED 0
#define CS_DEV_UNCONNECTED 1
#define CS_CTR_UNCONNECTED 2
#define DS_UNLOAD 0XA0
#define DS_LOADED 0XFF
#define DS_LOADOV 0X01


