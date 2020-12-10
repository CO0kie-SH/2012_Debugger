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
	::SetParent(hwCon, gINFO_mWind.hwMFC);
	//::ShowWindow(hwCon, SW_HIDE);
}

CConsole::~CConsole()
{
	std::cout << "����̨���ر�\n";
	FreeConsole();
}