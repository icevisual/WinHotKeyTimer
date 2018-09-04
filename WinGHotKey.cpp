// WinGHotKey.cpp : �������̨Ӧ�ó������ڵ㡣
//
#include "stdafx.h"


#define MAX_INSTRUCTION_NUM 100
#define MAX_LINE_CHAR_NUM 255

#define MAX_SLEEP_PERIOD 10000 // Ϊ��֤��ʱ����ʱЧ�ԣ���ʱ���˯�߷ָ��С�Σ�����ʱ��У׼

// TODO ���� MAX_SLEEP_PERIOD ��С����ֵ

BOOLEAN g_isRunnung = FALSE;
BOOLEAN g_isStop = FALSE;
TCHAR g_CunnentTime[64] = { 0 };

// �ű�ָ��
INT g_Script[MAX_INSTRUCTION_NUM][3] = { 0 };
INT g_ScriptLength = 0;

struct ScentrealmRuntime g_srRuntime = {
	DS_UNLOAD,
	0L,
	CS_UNKNOWN,
	false
};

HINSTANCE g_ScentRealm_DLL;

VOID RunScriptTimer();

DWORD WINAPI PlayScriptThreadFunction(LPVOID lpParam) {
	g_isRunnung = TRUE;

	g_isStop = FALSE;

	RunScriptTimer();

	g_isRunnung = FALSE;
	return 0;
}

// �� srt �ļ� ��ζ����ָ��
BOOLEAN LoadSrtInstructionFromFile(TCHAR * filename) {
	FILE * fp = NULL;
	errno_t error = _tfopen_s(&fp, filename, _T("r"));
	if (0 != error) {
		ShowLastErrorMsg(L"_tfopen_s");
		return FALSE;
	}
	CHAR line[MAX_LINE_CHAR_NUM] = { 0 };
	INT pos[2] = { 0 };
	size_t lLength = 0, smellID = 0;
	BOOLEAN attemptToGetSmellIDNextLine = FALSE;
	g_ScriptLength = 0;
	while (!feof(fp))
	{
		fgets(line, sizeof(line), fp);
		lLength = strlen(line);
		if (attemptToGetSmellIDNextLine == TRUE) {
			if ((smellID = GetSmellID(line, lLength)) > 0) {
				// pos smellID
				g_Script[g_ScriptLength][0] = pos[0];
				g_Script[g_ScriptLength][1] = pos[1];
				g_Script[g_ScriptLength][2] = smellID;
				g_ScriptLength ++;
			}
			attemptToGetSmellIDNextLine = FALSE;
		}

		if (GetSrtTimePeriod(line, lLength, pos)) {
			attemptToGetSmellIDNextLine = TRUE;
		}
		else {
			attemptToGetSmellIDNextLine = FALSE;
		}

		TrimNewLine(line, lLength);
		// printf("%s %d\n", line, strlen(line));
	}
	fclose(fp);
	return TRUE;
}
BOOLEAN SleepWithInterrupt(_In_ DWORD dwMilliseconds, BOOLEAN * Flag)
{
	UINT Step = 1000;
	INT ActuallyStep = 0;
	while (dwMilliseconds > 0)
	{
		if (dwMilliseconds >= Step)
		{
			ActuallyStep = Step;
			dwMilliseconds -= Step;
		}
		else
		{
			ActuallyStep = dwMilliseconds;
			dwMilliseconds = 0;
		}
		Sleep(ActuallyStep);

		if (*Flag == TRUE)
			return FALSE;
	}
	return TRUE;
}

VOID RunScriptTimer() {
	INT StartMillSecond = GetMilliSecondOfDay();
	INT CurrentMillSecond;
	INT PreT = 0;
	INT offset = 0;
	INT duration = 0;
	INT playSecond = 0;
	TCHAR RelativeTime[20] = { 0 };
	for (int i = 0; i < g_ScriptLength; i++)
	{
		if (g_isStop == TRUE) {
			// �ֶ�ֹͣ�ű�����
			GetHMS(g_CunnentTime);
			_tprintf(_T("[%s] Script Stoped Manually\n"), g_CunnentTime);
			return;
		}
		// �ȴ�������ζ�� ʱ��� ����
		// TODO ��ʱ��ķָ�
		//Sleep(g_Script[i][0] - PreT - offset);
		if (FALSE == SleepWithInterrupt(g_Script[i][0] - PreT - offset, &g_isStop))
		{
			// �ֶ�ֹͣ�ű�����
			GetHMS(g_CunnentTime);
			_tprintf(_T("[%s] Script Stoped Manually\n"), g_CunnentTime);
			return;
		}

		CurrentMillSecond = GetMilliSecondOfDay(); // ��ǰʱ�����
		if (CurrentMillSecond < StartMillSecond) {
			// TODO ע�� �� 24 ��
		}

		MilliSecond2HMSF(CurrentMillSecond, g_CunnentTime);// ��ǰʱ�� HMSF
		MilliSecond2HMSF(CurrentMillSecond - StartMillSecond, RelativeTime); // ��ǰ���ʱ��
		duration = g_Script[i][1] - g_Script[i][0]; // ��ζ����ʱ�䣨���룩
		offset = CurrentMillSecond - StartMillSecond - g_Script[i][0]; // ��ʱ����ֵ����

		playSecond = (INT)ceil(duration / 1000.0);
		_tprintf(_T("[%s]I[%02d]T[%s]C[%02d] Play Smell [%d] For [%d] MS [%d] S \n"),
			g_CunnentTime,
			(i + 1),
			RelativeTime,
			offset, // ��ζ����ʵ��ʱ�� �� Ԥ��ʱ��Ĳ�ֵ
			g_Script[i][2],
			duration,
			playSecond
			);
		// ���� ������ζ ָ��
		// TODO test the return value of PlaySmell
		// TODO Reconnect CTL
		g_srRuntime.pPlaySmell(g_Script[i][2], playSecond);
		
		// �ȴ�������ζ������ ʱ��� ����
		//Sleep(duration - offset);

		if (FALSE == SleepWithInterrupt(duration - offset, &g_isStop))
		{
			// �ֶ�ֹͣ�ű�����
			GetHMS(g_CunnentTime);
			_tprintf(_T("[%s] Script Stoped Manually\n"), g_CunnentTime);
			return;
		}

		// ���� ֹͣ���� ָ��
		// TODO StopPlay
		g_srRuntime.pStopPlay();

		CurrentMillSecond = GetMilliSecondOfDay();// ��ǰʱ�����
		MilliSecond2HMSF(CurrentMillSecond, g_CunnentTime);// ��ǰʱ�� HMSF
		MilliSecond2HMSF(CurrentMillSecond - StartMillSecond,RelativeTime);// ��ǰ���ʱ��
		offset = CurrentMillSecond - StartMillSecond - g_Script[i][1]; // ��ʱ����ֵ����
		_tprintf(_T("[%s]I[%02d]T[%s]C[%02d] Stop Play\n"), 
			g_CunnentTime, 
			(i + 1), 
			RelativeTime, 
			offset // ��ζֹͣʵ��ʱ�� �� Ԥ��ʱ��Ĳ�ֵ
			);
		
		PreT = g_Script[i][1];
	}

	GetHMS(g_CunnentTime);
	_tprintf(_T("[%s] Script Play Over\n"), g_CunnentTime);
}




INT main_1(int argc, TCHAR * argv[]) {

	HWND hWnd = NULL;		// ���ھ��
	HANDLE hThread = NULL;	// ���߳̾��
	TCHAR sourceFilename[] = L"input.srt";
	_tprintf(L"Load DLL ...\n");

	InitScentrealmFunctions(g_ScentRealm_DLL,&g_srRuntime);

	if (CS_DEV_CONNECTED != g_srRuntime.ConnectState) {
		system("pause");
		return 3;
	}

	_tprintf(L"Load SRT File (%s)...\n", sourceFilename);
	if (FALSE == LoadSrtInstructionFromFile(sourceFilename)) {
		system("pause");
		return 2;
	}

	MSG msg = { 0 };		// ��Ϣ
	DWORD dwThreadId = 0;	// �߳� ID
	DWORD error = 0;
	ATOM m_HotKeyId1 = GlobalAddAtom(_T("WinHotKeySRCtl-PlayScript")) - 0xc000;
	ATOM m_HotKeyId2 = GlobalAddAtom(_T("WinHotKeySRCtl-StopScript")) - 0xc000;
	ATOM m_HotKeyId3 = GlobalAddAtom(_T("WinHotKeySRCtl-Terminate"))  - 0xc000;
	_tprintf(L"Register HotKeys ...\n");
	LocalRegisterHotKey(hWnd, m_HotKeyId1, MOD_NOREPEAT, VK_NUMPAD1);
	LocalRegisterHotKey(hWnd, m_HotKeyId2, MOD_NOREPEAT, VK_NUMPAD2);
	LocalRegisterHotKey(hWnd, m_HotKeyId3, MOD_NOREPEAT, VK_NUMPAD3);

	_tprintf(L"Press Key `1` To Play Script\n");
	_tprintf(L"Press Key `2` To Stop Script\n");
	_tprintf(L"Press Key `3` To Exit\n");

	while (GetMessage(&msg, NULL, 0, 0) != 0) {
		DispatchMessage(&msg);
		if (msg.message == WM_HOTKEY) {
			GetHMS(g_CunnentTime);

			if (m_HotKeyId1 == msg.wParam) {
				// Start Script
				if (g_isRunnung == TRUE) {
					_tprintf(_T("[%s] PlayScriptThreadFunction IS running \n"), g_CunnentTime);
					continue;
				}
				else {
					_tprintf(_T("[%s] Button `1`,Play Script \n"), g_CunnentTime);
				}
				hThread = CreateThread(NULL, 0, PlayScriptThreadFunction, NULL, 0, &dwThreadId);
			}
			else if (m_HotKeyId2 == msg.wParam) {
				g_isStop = TRUE;
				g_srRuntime.pStopPlay();
				// Stop Script
				_tprintf(_T("[%s] Button `2`,Stop Script \n"), g_CunnentTime);
			}
			else if (m_HotKeyId3 == msg.wParam) {
				g_isStop = TRUE;
				g_srRuntime.pStopPlay();
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
	FreeLibrary(g_ScentRealm_DLL);
	//system("pause");

	return 0;
}