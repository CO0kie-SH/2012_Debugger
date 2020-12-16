#include "pch.h"
#include "CAPI.h"
#include <tlhelp32.h>

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

BOOL CAPI::GetTIDByPID(DWORD PID, DWORD* TID)
{
	// 2. 创建快照用于遍历线程，参数 1 为TH32CS_SNAPTHREAD， 参数 2 没有意义
	//    在实际的使用中，参数2不管是什么，遍历到的都是所有的线程
	HANDLE Snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, NULL);

	// 3. 初始化结构体用于保存遍历到的线程的数据
	THREADENTRY32 ThreadInfo = { sizeof(THREADENTRY32) };

	// 4. 尝试遍历快照中的第一个线程
	if (Thread32First(Snapshot, &ThreadInfo))
	{
		// 5. 输出对应的信息
		do {
			// [PID 和 TID 共用同一套数据]

			// 5.1 筛选出对应进程的所有线程
			if (ThreadInfo.th32OwnerProcessID == PID)
			{
				if (TID)	*TID = ThreadInfo.th32ThreadID;
				return TRUE;
			}
			// 6. 遍历下一个线程
		} while (Thread32Next(Snapshot, &ThreadInfo));
	}
	return 0;
}
