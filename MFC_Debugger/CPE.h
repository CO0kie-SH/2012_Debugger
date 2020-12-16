#pragma once
#include "CMyPoint.h"
class CPE
{
	
public:
	CPE(CMyPoint* pcPoint);
	~CPE();
	void ShowImports(LPVOID Add, DWORD Size);
	void ShowExports(LPVOID Add, DWORD Size);
	void DUMP(LPVOID Add, DWORD Size, PWCHAR Name);
private:
	HANDLE mHeap;
	CMyPoint* mcPoint;
};


