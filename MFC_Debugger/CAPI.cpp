#include "pch.h"
#include "CAPI.h"

CAPI gAPI;
CString jstr;

/*	������ѭ�����ڣ��ж���Ϸ�Ƿ�ɹ�����
	���ߣ�����
	���ӣ�https://blog.csdn.net/dgyanyong/article/details/52994204
	�Ż���CO0kieؼ
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
		// ͨ�����ھ��ȡ�ý���ID
		dwTID = ::GetWindowThreadProcessId(hwndWindow, 0);
		if (dwTID == TID)
		{
			GetClassNameW(hwndWindow, buff, MAX_PATH);
			std::cout << "TID=" << dwTID << "\tHWND=0x" << std::hex <<
				hwndWindow << " " << std::dec << (DWORD)hwndWindow;
			printf("������%S\n", buff);

			if (wcscmp(buff, L"ConsoleWindowClass") == 0)
				return (int)hwndWindow;
		}
		// ȡ����һ�����ھ��
	} while (hwndWindow = ::GetNextWindow(hwndWindow, GW_HWNDNEXT));
	return 0;
}
