// WinGHotKey.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"




static TCHAR g_CunnentTime[64] = { 0 };



INT main(int argc, TCHAR * argv[]) {

	HWND hWnd = NULL;		// 窗口句柄
	HANDLE hThread = NULL;	// 多线程句柄
	TCHAR sourceFilename[] = L"input.srt";

	_tprintf(L"Load DLL ...\n");

	MSG msg = { 0 };		// 消息
	DWORD dwThreadId = 0;	// 线程 ID
	DWORD error = 0;
	ATOM m_HotKeyId1 = GlobalAddAtom(_T("WinHotKeySRCtl-PlayScript")) - 0xc000;
	ATOM m_HotKeyId2 = GlobalAddAtom(_T("WinHotKeySRCtl-StopScript")) - 0xc000;
	ATOM m_HotKeyId3 = GlobalAddAtom(_T("WinHotKeySRCtl-Terminate")) - 0xc000;
	_tprintf(L"Register HotKeys ...\n");
	LocalRegisterHotKey(hWnd, m_HotKeyId1, MOD_NOREPEAT, VK_NUMPAD1);
	LocalRegisterHotKey(hWnd, m_HotKeyId2, MOD_NOREPEAT, VK_NUMPAD2);
	LocalRegisterHotKey(hWnd, m_HotKeyId3, MOD_NOREPEAT, VK_NUMPAD3);

	_tprintf(L"Press Key `1` To Play Script\n");
	_tprintf(L"Press Key `2` To Stop Script\n");
	_tprintf(L"Press Key `3` To Exit\n");

	while (GetMessage(&msg, NULL, 0, 0) != 0) {
		if (msg.message == WM_HOTKEY) {
			GetHMS(g_CunnentTime);

			if (m_HotKeyId1 == msg.wParam) {
				// Start Script
				_tprintf(_T("[%s] Button `1`,Play Script \n"), g_CunnentTime);

				BYTE vkb = VK_RETURN;
				CHAR * targetStr = "jinyanlin1";
				for (int i = 0; i < strlen(targetStr); i++)
				{
					if (targetStr[i] >= 'a' && targetStr[i] <= 'z')
					{
						vkb = targetStr[i] - 'a' + 0x41;
					}
					else if (targetStr[i] >= '0' && targetStr[i] <= '9')
					{
						vkb = targetStr[i] - '0' + 0x30;
					}
					keybd_event(vkb, MapVirtualKey(vkb, 0), 0, 0);   //按下A键
				}
				Sleep(100);
				for (int i = 0; i < strlen(targetStr); i++)
				{
					if (targetStr[i] >= 'a' && targetStr[i] <= 'z')
					{
						vkb = targetStr[i] - 'a' + 0x41;
					}
					else if (targetStr[i] >= '0' && targetStr[i] <= '9')
					{
						vkb = targetStr[i] - '0' + 0x30;
					}
					keybd_event(vkb, MapVirtualKey(vkb, 0), KEYEVENTF_KEYUP, 0);   //释放A键
				}
				vkb = VK_RETURN;
				keybd_event(vkb, MapVirtualKey(vkb, 0), 0, 0);   //按下A键
				Sleep(100);
				keybd_event(vkb, MapVirtualKey(vkb, 0), KEYEVENTF_KEYUP, 0);   //释放A键

			}
			else if (m_HotKeyId2 == msg.wParam) {
				
				// Stop Script
				_tprintf(_T("[%s] Button `2`,Stop Script \n"), g_CunnentTime);
			}
			else if (m_HotKeyId3 == msg.wParam) {
				// Exit
				_tprintf(_T("[%s] Button `3`,Exit\n"), g_CunnentTime);
				break;
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