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
	std::cout << "控制台启动\tPID=0x" << std::hex << gINFO_mWind.wPID << ""
		<< std::dec << gINFO_mWind.wPID << "\tTID=" << gINFO_mWind.wTID << std::endl;
	HWND hwCon = (HWND)gAPI.FindWindowByTID(gINFO_mWind.wTID);
	if (!hwCon)
	{
		MessageBox(0, L"初始化日志控件失败，错误码0x001", 0, 0);
		ExitProcess(1);
	}
	//::SetParent(hwCon, gINFO_mWind.hwMFC);
	//::SetWindowLong(hwCon, GWL_STYLE, 369164288);


	//删除最小化、最大化按钮
	::SetWindowLong(hwCon, GWL_STYLE, ::GetWindowLong(hwCon, GWL_STYLE)
		& ~(WS_MAXIMIZEBOX| WS_MINIMIZEBOX));

	//删除关闭按钮
	RemoveMenu(::GetSystemMenu(hwCon, 0), SC_CLOSE, 0);
	gINFO_mWind.hwCON = hwCon;

	//禁用快速编辑模式
	HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
	DWORD mode;
	::GetConsoleMode(hStdin, &mode);
	mode &= ~ENABLE_QUICK_EDIT_MODE;
	::SetConsoleMode(hStdin, mode);
}

CConsole::~CConsole()
{
	//std::cout << "控制台将关闭\n";
	FreeConsole();
}

void CConsole::SetTitle(PWCHAR text)
{
	::SetWindowText(gINFO_mWind.hwCON, text);
}
