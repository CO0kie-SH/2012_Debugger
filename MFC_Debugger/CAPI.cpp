#include "pch.h"
#include "CAPI.h"

CAPI gAPI;
CString jstr;

/*	函数：循环窗口，判断游戏是否成功开启
	作者：特招
	链接：https://blog.csdn.net/dgyanyong/article/details/52994204
	优化：CO0kie丶
*/
BOOL CAPI::FindWindowByTID(DWORD TID)
{
	HWND hwndRet = NULL, hwndWindow = NULL;
	DWORD dwTID;
	WCHAR buff[MAX_PATH];
	if (0 == (hwndWindow = ::GetTopWindow(0)))
		return FALSE;
	do
	{
		// 通过窗口句柄取得进程ID
		dwTID = ::GetWindowThreadProcessId(hwndWindow, 0);
		if (dwTID == TID)
		{
			GetClassNameW(hwndWindow, buff, MAX_PATH);
			std::cout << "TID=" << dwTID << "\tHWND=0x" << std::hex <<
				hwndWindow << " " << std::dec << (DWORD)hwndWindow;
			printf("类名：%S\n", buff);

			if (wcscmp(buff, L"ConsoleWindowClass") == 0)
				return (int)hwndWindow;
		}
		// 取得下一个窗口句柄
	} while (hwndWindow = ::GetNextWindow(hwndWindow, GW_HWNDNEXT));
	return 0;
}
