// WinGHotKey.cpp : �������̨Ӧ�ó������ڵ㡣
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

// �� srt �ļ� ��ζ����ָ��
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


HBITMAP ScreenCapture(LPCSTR filename, WORD BitCount, LPRECT lpRect)
{
	HBITMAP hBitmap;
	// ��ʾ����ĻDC
	HDC hScreenDC = CreateDC(L"DISPLAY", NULL, NULL, NULL);
	HDC hmemDC = CreateCompatibleDC(hScreenDC);
	// ��ʾ����Ļ�Ŀ�͸�
	int ScreenWidth = GetDeviceCaps(hScreenDC, HORZRES);
	int ScreenHeight = GetDeviceCaps(hScreenDC, VERTRES);
	// �ɵ�BITMAP�������������ȡ��λ�ý���
	HBITMAP hOldBM;
	// ����λͼ����
	PVOID lpvpxldata;
	// ������ȡ�ĳ������
	INT ixStart;
	INT iyStart;
	INT iX;
	INT iY;
	// λͼ���ݴ�С
	DWORD dwBitmapArraySize;
	// ������С
	DWORD nBitsOffset;
	DWORD lImageSize;
	DWORD lFileSize;
	// λͼ��Ϣͷ
	BITMAPINFO bmInfo;
	// λͼ�ļ�ͷ
	BITMAPFILEHEADER bmFileHeader;
	// д�ļ���
	HANDLE hbmfile;
	DWORD dwWritten;

	// ���LPRECT ΪNULL ��ȡ������Ļ
	if (lpRect == NULL)
	{
		ixStart = iyStart = 0;
		iX = ScreenWidth;
		iY = ScreenHeight;
	}
	else
	{
		ixStart = lpRect->left;
		iyStart = lpRect->top;
		iX = lpRect->right - lpRect->left;
		iY = lpRect->bottom - lpRect->top;
	}
	// ����BTIMAP
	hBitmap = CreateCompatibleBitmap(hScreenDC, iX, iY);
	// ��BITMAPѡ�����ڴ�DC��
	hOldBM = (HBITMAP)SelectObject(hmemDC, hBitmap);
	// BitBlt��ĻDC���ڴ�DC�����������ȡ�Ļ�ȡ���ò���
	BitBlt(hmemDC, 0, 0, iX, iY, hScreenDC, ixStart, iyStart, SRCCOPY);
	// ���ɵ�BITMAP����ѡ����ڴ�DC������ֵΪ���滻�Ķ��󣬼�����ȡ��λͼ
	hBitmap = (HBITMAP)SelectObject(hmemDC, hOldBM);
	if (filename == NULL)
	{
		DeleteDC(hScreenDC);
		DeleteDC(hmemDC);
		return hBitmap;
	}
	// Ϊλͼ���������ڴ�ռ�
	dwBitmapArraySize = ((((iX * 32) + 31) & ~31) >> 3)* iY;
	lpvpxldata = HeapAlloc(GetProcessHeap(), HEAP_NO_SERIALIZE, dwBitmapArraySize);
	ZeroMemory(lpvpxldata, dwBitmapArraySize);

	// ��� BITMAPINFO �ṹ
	ZeroMemory(&bmInfo, sizeof(BITMAPINFO));
	bmInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmInfo.bmiHeader.biWidth = iX;
	bmInfo.bmiHeader.biHeight = iY;
	bmInfo.bmiHeader.biPlanes = 1;
	bmInfo.bmiHeader.biBitCount = BitCount;
	bmInfo.bmiHeader.biCompression = BI_RGB;

	// ��� BITMAPFILEHEADER �ṹ
	ZeroMemory(&bmFileHeader, sizeof(BITMAPFILEHEADER));
	nBitsOffset = sizeof(BITMAPFILEHEADER) + bmInfo.bmiHeader.biSize;
	lImageSize =
		((((bmInfo.bmiHeader.biWidth * bmInfo.bmiHeader.biBitCount) + 31) & ~31) >> 3)
		* bmInfo.bmiHeader.biHeight;
	lFileSize = nBitsOffset + lImageSize;
	bmFileHeader.bfType = 'B' + ('M' << 8);
	bmFileHeader.bfSize = lFileSize;
	bmFileHeader.bfOffBits = nBitsOffset;

	// ��ȡDIB����д�뵽�ļ�
	GetDIBits(hmemDC, hBitmap, 0, bmInfo.bmiHeader.biHeight,
		lpvpxldata, &bmInfo, DIB_RGB_COLORS);
	// д�ļ�
	hbmfile = CreateFileA(filename,
		GENERIC_WRITE,
		FILE_SHARE_WRITE,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (hbmfile == INVALID_HANDLE_VALUE)
	{
		MessageBoxA(NULL, "create file error", "error", MB_OK);
	}

	WriteFile(hbmfile, &bmFileHeader, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);
	WriteFile(hbmfile, &bmInfo, sizeof(BITMAPINFO), &dwWritten, NULL);
	WriteFile(hbmfile, lpvpxldata, lImageSize, &dwWritten, NULL);
	CloseHandle(hbmfile);
	//	MessageBox(NULL,"�ļ�д��ɹ�","error",MB_OK);

	// �ͷ��ڴ棬�����ͬ��DC��
	// ����û��ɾ��BITMAP����������ʾ��ɺ�ɾ��
	HeapFree(GetProcessHeap(), HEAP_NO_SERIALIZE, lpvpxldata);
	ReleaseDC(0, hScreenDC);
	DeleteDC(hmemDC);
	return hBitmap;
}

BOOL G_StopCycle = FALSE;

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
				//���� ���·����
				input[0].ki.wVk = vks[i];
				input[0].type = INPUT_KEYBOARD;
				//�ɿ� ���·����
				input[1].ki.wVk = vks[i]; //����ַ�
				input[1].type = INPUT_KEYBOARD;
				input[1].ki.dwFlags = KEYEVENTF_KEYUP;
				//�ú����ϳɼ����¼�������¼�������ģ�������߼��̲������¼����������������߼��̴����������
				SendInput(2, input, sizeof(INPUT));
			}
			else
			{
				Sleep(vks[i] - 0xff);
			}
		}
		if(j < trytime - 1)
			Sleep(5000);

		if (G_StopCycle == TRUE)
		{
			G_StopCycle = FALSE;
			break;
		}
	}

}

INT main(int argc, TCHAR * argv[]) {



	HWND hWnd = NULL;		// ���ھ��
	HANDLE hThread = NULL;	// ���߳̾��
	TCHAR sourceFilename[] = L"config.conf";
	LoadConfigFromFile(sourceFilename);
	_tprintf(L"Load config ...\n");

	MSG msg = { 0 };		// ��Ϣ
	DWORD dwThreadId = 0;	// �߳� ID
	DWORD error = 0;
	ATOM m_HotKeyId1 = GlobalAddAtom(_T("WinHotKeySRCtl-PlayScript")) - 0xc000;
	ATOM m_HotKeyId2 = GlobalAddAtom(_T("WinHotKeySRCtl-StopScript")) - 0xc000;
	ATOM m_HotKeyId3 = GlobalAddAtom(_T("WinHotKeySRCtl-Terminate")) - 0xc000;
	ATOM m_HotKeyId4 = GlobalAddAtom(_T("WinHotKeySRCtl-Simulate")) - 0xc000;
	ATOM m_HotKeyId5 = GlobalAddAtom(_T("WinHotKeySRCtl-RepeatSimulate")) - 0xc000;
	ATOM m_HotKeyId7 = GlobalAddAtom(_T("WinHotKeySRCtl-ScreenShot")) - 0xc000;

	ATOM m_HotKeyId8 = GlobalAddAtom(_T("WinHotKeySRCtl-StopCycle")) - 0xc000;

	ATOM m_HotKeyId9 = GlobalAddAtom(_T("WinHotKeySRCtl-Test")) - 0xc000;
	_tprintf(L"Register HotKeys ...\n");
	LocalRegisterHotKey(hWnd, m_HotKeyId1, MOD_NOREPEAT, VK_NUMPAD1);
	LocalRegisterHotKey(hWnd, m_HotKeyId2, MOD_NOREPEAT, VK_NUMPAD2);
	LocalRegisterHotKey(hWnd, m_HotKeyId3, MOD_NOREPEAT, VK_NUMPAD3);
	LocalRegisterHotKey(hWnd, m_HotKeyId4, MOD_NOREPEAT, VK_NUMPAD4);
	LocalRegisterHotKey(hWnd, m_HotKeyId9, MOD_NOREPEAT, VK_NUMPAD9);
	LocalRegisterHotKey(hWnd, m_HotKeyId5, MOD_NOREPEAT, VK_NUMPAD5);
	LocalRegisterHotKey(hWnd, m_HotKeyId7, MOD_NOREPEAT, VK_NUMPAD7);
	LocalRegisterHotKey(hWnd, m_HotKeyId8, MOD_NOREPEAT, VK_NUMPAD8);

	_tprintf(L"Press Key `1` To Start 1.bat \n");
	_tprintf(L"Press Key `2` To Start 2.bat \n");
	_tprintf(L"Press Key `3` To Exit\n");
	_tprintf(L"Press Key `4` To Ka Bug\n");
	_tprintf(L"Press Key `5` To Cycle Ka Bug 50\n");
	//_tprintf(L"Press Key `6` To Exit\n");
	_tprintf(L"Press Key `7` To ScreenCapture\n");
	_tprintf(L"Press Key `8` To Stop Cycle Ka Bug\n");
	_tprintf(L"Press Key `9` To Ka Bug Key Frame\n");



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
				
				//keybd_event(VK_E, MapVirtualKey(VK_E, 0), 0, 0);   //����A��
				//keybd_event(VK_E, MapVirtualKey(VK_E, 0), KEYEVENTF_KEYUP, 0);   //�ͷ�A��
				//INT sleep = RandomInt(1, 200);
				//_tprintf(_T("Sleep %d \n"), sleep);
				//Sleep(sleep);
				//keybd_event(vkc, MapVirtualKey(vkc, 0), 0, 0);   //����A��
				//keybd_event(vkc, MapVirtualKey(vkc, 0), KEYEVENTF_KEYUP, 0);   //�ͷ�A��
			}
			else if (m_HotKeyId5 == msg.wParam) {
				RunSomething(50);
			}
			else if (m_HotKeyId7 == msg.wParam) {
				LPSTR addr = "ScreenCapture.png";
				ScreenCapture(addr, 32, NULL);
			}
			else if (m_HotKeyId8 == msg.wParam) {
				G_StopCycle = TRUE;
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
						//���� ���·����
						input[0].ki.wVk = vks[i];
						input[0].type = INPUT_KEYBOARD;
						//�ɿ� ���·����
						input[1].ki.wVk = vks[i]; //����ַ�
						input[1].type = INPUT_KEYBOARD;
						input[1].ki.dwFlags = KEYEVENTF_KEYUP;
						//�ú����ϳɼ����¼�������¼�������ģ�������߼��̲������¼����������������߼��̴����������
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