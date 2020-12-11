#include "pch.h"
#include "CConsole.h"


CConsole::CConsole()
{
	OutputDebugString(L"CConsole()\n");
	AllocConsole();
	FILE* pFile;
	freopen_s(&pFile,"CON", "r", stdin);
	freopen_s(&pFile,"CON", "w", stdout);
	freopen_s(&pFile,"CON", "w", stderr);
	std::cout << "����̨����\tPID=0x" << std::hex << gINFO_mWind.wPID << ""
		<< std::dec << gINFO_mWind.wPID << "\tTID=" << gINFO_mWind.wTID << std::endl;
	HWND hwCon = (HWND)gAPI.FindWindowByTID(gINFO_mWind.wTID);
	if (!hwCon)
	{
		MessageBox(0, L"��ʼ����־�ؼ�ʧ�ܣ�������0x001", 0, 0);
		ExitProcess(1);
	}
	//::SetParent(hwCon, gINFO_mWind.hwMFC);
	//::SetWindowLong(hwCon, GWL_STYLE, 369164288);


	//ɾ����С������󻯰�ť
	::SetWindowLong(hwCon, GWL_STYLE, ::GetWindowLong(hwCon, GWL_STYLE)
		& ~(WS_MAXIMIZEBOX| WS_MINIMIZEBOX));

	//ɾ���رհ�ť
	RemoveMenu(::GetSystemMenu(hwCon, 0), SC_CLOSE, 0);
	gINFO_mWind.hwCON = hwCon;

	//���ÿ��ٱ༭ģʽ
	HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
	DWORD mode;
	::GetConsoleMode(hStdin, &mode);
	mode &= ~ENABLE_QUICK_EDIT_MODE;
	::SetConsoleMode(hStdin, mode);
}

CConsole::~CConsole()
{
	//std::cout << "����̨���ر�\n";
	FreeConsole();
}

void CConsole::SetTitle(PWCHAR text)
{
	::SetWindowText(gINFO_mWind.hwCON, text);
}
