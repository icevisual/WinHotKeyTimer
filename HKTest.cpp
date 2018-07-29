// WinGHotKey.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"




static TCHAR g_CunnentTime[64] = { 0 };


INT RandomInt(INT min, INT max)
{
	static time_t i = 0;
	if (i == 0)
	{
		srand((unsigned)time(NULL));
		i++;
	}

	INT rd = rand();

	rd %= (max - min);

	return rd + min;
}

// 从 srt 文件 气味播放指令
BOOLEAN LoadConfigFromFile(TCHAR * filename) {
	FILE * fp = NULL;
	errno_t error = _tfopen_s(&fp, filename, _T("r"));
	if (0 != error) {
		ShowLastErrorMsg(L"_tfopen_s");
		return FALSE;
	}
	CHAR line[255] = { 0 };
	INT pos[2] = { 0 }, lLength = 0, smellID = 0;
	while (!feof(fp))
	{
		fgets(line, sizeof(line), fp);
		lLength = strlen(line);

		Trim(line, lLength);
		printf("%s %d\n", line, strlen(line));
	}
	fclose(fp);
	return TRUE;
}
VOID RunSomething(INT trytime)
{
	BYTE VK_E = 0x45;
	BYTE VK_R = 0x52;
	BYTE vkc = VK_ESCAPE;
	INT DefaultSleep = 100;
	INT vks[] = {
		VK_ESCAPE , // option
		0xff + 200,
		VK_E,
		0xff + DefaultSleep,
		VK_UP,  // target
		0xff + DefaultSleep,
		VK_E, // use
		0xff + DefaultSleep,
		VK_E, // use
		0xff, // random sleep
		VK_ESCAPE,
		0xff + DefaultSleep,
		VK_RIGHT,  // souls
		0xff + DefaultSleep,
		VK_E,
		0xff + 200,
		VK_E,
		0xff + 200,
		VK_E,
		0xff + 300,
		VK_E,
		0xff + 300,
		VK_E,
		0xff + 300,
		VK_E,
		0xff + 300,
		VK_E,
		0xff + 300,
		VK_E,
	};
	int len = sizeof(vks) / sizeof(vks[0]);

	for (int j = 0; j < trytime; j++)
	{
		for (int i = 0; i < len; i++)
		{
			if (vks[i] == 0xff)
			{
				INT sleep = RandomInt(28, 32);
				_tprintf(_T("Sleep %d \n"), sleep);
				Sleep(sleep);
			}
			else if (vks[i] < 0xff)
			{
				INPUT input[2];
				memset(input, 0, sizeof(input));
				//按下 向下方向键
				input[0].ki.wVk = vks[i];
				input[0].type = INPUT_KEYBOARD;
				//松开 向下方向键
				input[1].ki.wVk = vks[i]; //你的字符
				input[1].type = INPUT_KEYBOARD;
				input[1].ki.dwFlags = KEYEVENTF_KEYUP;
				//该函数合成键盘事件和鼠标事件，用来模拟鼠标或者键盘操作。事件将被插入在鼠标或者键盘处理队列里面
				SendInput(2, input, sizeof(INPUT));
			}
			else
			{
				Sleep(vks[i] - 0xff);
			}
		}
		if(j < trytime - 1)
			Sleep(5000);
	}

}

INT main(int argc, TCHAR * argv[]) {

	HWND hWnd = NULL;		// 窗口句柄
	HANDLE hThread = NULL;	// 多线程句柄
	TCHAR sourceFilename[] = L"config.conf";
	LoadConfigFromFile(sourceFilename);
	_tprintf(L"Load config ...\n");

	MSG msg = { 0 };		// 消息
	DWORD dwThreadId = 0;	// 线程 ID
	DWORD error = 0;
	ATOM m_HotKeyId1 = GlobalAddAtom(_T("WinHotKeySRCtl-PlayScript")) - 0xc000;
	ATOM m_HotKeyId2 = GlobalAddAtom(_T("WinHotKeySRCtl-StopScript")) - 0xc000;
	ATOM m_HotKeyId3 = GlobalAddAtom(_T("WinHotKeySRCtl-Terminate")) - 0xc000;
	ATOM m_HotKeyId4 = GlobalAddAtom(_T("WinHotKeySRCtl-Simulate")) - 0xc000;
	ATOM m_HotKeyId5 = GlobalAddAtom(_T("WinHotKeySRCtl-RepeatSimulate")) - 0xc000;


	ATOM m_HotKeyId9 = GlobalAddAtom(_T("WinHotKeySRCtl-Test")) - 0xc000;
	_tprintf(L"Register HotKeys ...\n");
	LocalRegisterHotKey(hWnd, m_HotKeyId1, MOD_NOREPEAT, VK_NUMPAD1);
	LocalRegisterHotKey(hWnd, m_HotKeyId2, MOD_NOREPEAT, VK_NUMPAD2);
	LocalRegisterHotKey(hWnd, m_HotKeyId3, MOD_NOREPEAT, VK_NUMPAD3);
	LocalRegisterHotKey(hWnd, m_HotKeyId4, MOD_NOREPEAT, VK_NUMPAD4);
	LocalRegisterHotKey(hWnd, m_HotKeyId9, MOD_NOREPEAT, VK_NUMPAD9);
	LocalRegisterHotKey(hWnd, m_HotKeyId5, MOD_NOREPEAT, VK_NUMPAD5);


	
	_tprintf(L"Press Key `1` To Play Script\n");
	_tprintf(L"Press Key `2` To Stop Script\n");
	_tprintf(L"Press Key `3` To Exit\n");


	while (GetMessage(&msg, NULL, 0, 0) != 0) {
		if (msg.message == WM_HOTKEY) {
			GetHMS(g_CunnentTime);

			if (m_HotKeyId1 == msg.wParam) {
				// Start Script
				_tprintf(_T("[%s] Button `1`,Save \n"), g_CunnentTime);
				system("1.bat");
			}
			else if (m_HotKeyId2 == msg.wParam) {
				
				// Stop Script
				_tprintf(_T("[%s] Button `2`,Reset \n"), g_CunnentTime);
				system("2.bat");
			}
			else if (m_HotKeyId3 == msg.wParam) {
				// Exit
				_tprintf(_T("[%s] Button `3`,Exit\n"), g_CunnentTime);
				break;
			}
			else if (m_HotKeyId4 == msg.wParam) {
				RunSomething(1);
				
				//keybd_event(VK_E, MapVirtualKey(VK_E, 0), 0, 0);   //按下A键
				//keybd_event(VK_E, MapVirtualKey(VK_E, 0), KEYEVENTF_KEYUP, 0);   //释放A键
				//INT sleep = RandomInt(1, 200);
				//_tprintf(_T("Sleep %d \n"), sleep);
				//Sleep(sleep);
				//keybd_event(vkc, MapVirtualKey(vkc, 0), 0, 0);   //按下A键
				//keybd_event(vkc, MapVirtualKey(vkc, 0), KEYEVENTF_KEYUP, 0);   //释放A键
			}
			else if (m_HotKeyId5 == msg.wParam) {
				RunSomething(10);
			}
			else if (m_HotKeyId9 == msg.wParam) {
				BYTE VK_E = 0x45;
				BYTE VK_R = 0x52;
				BYTE vkc = VK_ESCAPE;
				INT DefaultSleep = 100;
				INT vks[] = {
					VK_E, // use
					0xff, // random sleep
					VK_ESCAPE,
				};
				int len = sizeof(vks) / sizeof(vks[0]);

				for (int i = 0; i < len; i++)
				{
					if (vks[i] == 0xff)
					{
						INT sleep = RandomInt(28, 32);
						_tprintf(_T("Sleep %d \n"), sleep);
						Sleep(sleep);
					}
					else if (vks[i] < 0xff)
					{
						INPUT input[2];
						memset(input, 0, sizeof(input));
						//按下 向下方向键
						input[0].ki.wVk = vks[i];
						input[0].type = INPUT_KEYBOARD;
						//松开 向下方向键
						input[1].ki.wVk = vks[i]; //你的字符
						input[1].type = INPUT_KEYBOARD;
						input[1].ki.dwFlags = KEYEVENTF_KEYUP;
						//该函数合成键盘事件和鼠标事件，用来模拟鼠标或者键盘操作。事件将被插入在鼠标或者键盘处理队列里面
						SendInput(2, input, sizeof(INPUT));
					}
					else
					{
						Sleep(vks[i] - 0xff);
					}
				}
			}
		}
	}
	CloseHandle(hThread);
	UnregisterHotKey(hWnd, m_HotKeyId1);
	UnregisterHotKey(hWnd, m_HotKeyId2);
	UnregisterHotKey(hWnd, m_HotKeyId3);
	GlobalDeleteAtom(m_HotKeyId1);
	GlobalDeleteAtom(m_HotKeyId2);
	GlobalDeleteAtom(m_HotKeyId3);
	//system("pause");

	return 0;
}