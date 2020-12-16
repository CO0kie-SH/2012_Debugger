#include "pch.h"
#include "CAPI.h"
#include <tlhelp32.h>

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

BOOL CAPI::GetTIDByPID(DWORD PID, DWORD* TID)
{
	// 2. �����������ڱ����̣߳����� 1 ΪTH32CS_SNAPTHREAD�� ���� 2 û������
	//    ��ʵ�ʵ�ʹ���У�����2������ʲô���������Ķ������е��߳�
	HANDLE Snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, NULL);

	// 3. ��ʼ���ṹ�����ڱ�����������̵߳�����
	THREADENTRY32 ThreadInfo = { sizeof(THREADENTRY32) };

	// 4. ���Ա��������еĵ�һ���߳�
	if (Thread32First(Snapshot, &ThreadInfo))
	{
		// 5. �����Ӧ����Ϣ
		do {
			// [PID �� TID ����ͬһ������]

			// 5.1 ɸѡ����Ӧ���̵������߳�
			if (ThreadInfo.th32OwnerProcessID == PID)
			{
				if (TID)	*TID = ThreadInfo.th32ThreadID;
				return TRUE;
			}
			// 6. ������һ���߳�
		} while (Thread32Next(Snapshot, &ThreadInfo));
	}
	return 0;
}
