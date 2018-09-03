#include "stdafx.h"


// 模拟键盘输入，低四位分别 代表 是否 Shift、Ctrl、Win、Alt
VOID SimulateKeyInput1(WORD  Keys[], CHAR Count)
{
	INT RevK = 0;
	INPUT * input = new INPUT[Count];
	CHAR KeyCount = Count * 2;
	memset(input, 0, Count * sizeof(INPUT));
	WORD VKArray[] = { VK_SHIFT ,VK_CONTROL ,VK_LWIN,VK_MENU };
	for (int i = 0; i < Count; i++)
	{
		RevK = KeyCount - 1 - i;
		input[i].ki.wVk = input[RevK].ki.wVk = Keys[i];
		input[RevK].ki.dwFlags = KEYEVENTF_KEYUP;
		input[i].type = input[RevK].type = INPUT_KEYBOARD;
	}
	//该函数合成键盘事件和鼠标事件，用来模拟鼠标或者键盘操作。事件将被插入在鼠标或者键盘处理队列里面
	SendInput(KeyCount, input, sizeof(INPUT));
	delete []input;
}

VOID si()
{
	CHAR SpecialCharsNoShift[][2] = {
		{ '`',VK_OEM_3 } ,{ '-',VK_OEM_MINUS },{ '=',VK_OEM_PLUS } ,{ '+',VK_ADD } ,
		{ '[',VK_OEM_4 } ,{ ']',VK_OEM_6 },{ '\\',VK_OEM_5 } ,
		{ ';',VK_OEM_1 } ,{ '\'',VK_OEM_7 },
		{ ',',VK_OEM_COMMA } ,{ '.',VK_DECIMAL } ,{ '/',VK_OEM_2 }
	};

	CHAR SpecialCharsShift[][2] = {
		{ ')',0x30 } ,{ '!',0x31 } ,{ '@',0x32 } ,{ '#',0x33 } ,{ '$',0x34 } ,
		{ '%',0x35 } ,{ '^',0x36 } ,{ '&',0x37 } ,{ '*',0x38 } ,{ '(',0x39 } ,
		{ '~',VK_OEM_3 } ,{ '_',VK_OEM_MINUS },
		{ '{',VK_OEM_4 } ,{ '}',VK_OEM_6 },{ '|',VK_OEM_5 } ,
		{ ':',VK_OEM_1 } ,{ '"',VK_OEM_7 },
		{ '<',VK_OEM_COMMA } ,{ '>',VK_OEM_PERIOD } ,{ '?',VK_OEM_2 }
	};


	INT Count = sizeof(SpecialCharsShift) /(sizeof(CHAR) * 2);
	Sleep(2000);
	for (int i = 0; i < Count; i++)
	{
		printf("%c\t", SpecialCharsShift[i][0]);
		WORD Keys[] = {VK_SHIFT, SpecialCharsShift[i][1] & 0xff ,VK_RETURN };

		SimulateKeyInput1(Keys, sizeof(Keys) / sizeof(WORD));
		
		printf("\n");
	}
}

INT main(int argc, TCHAR * argv[]) 
{

	Sleep(2000);
	ConvertChar2KeyWord("QGX78eMAw5C2UWdL");

	WORD Keys[] = { VK_RETURN };
	SimulateKeyInput1(Keys, sizeof(Keys) / sizeof(WORD));

	return 1;



    getchar();
	getchar();
	getchar();
	getchar();
	getchar();
	getchar();
	getchar();
	getchar();
	return 0;
}
