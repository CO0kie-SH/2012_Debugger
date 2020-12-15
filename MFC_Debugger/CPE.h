#pragma once
#include "CMyPoint.h"
class CPE
{
	
public:
	CPE(CMyPoint* pcPoint);
	~CPE();
	void ShowImports(LPVOID Add, DWORD Size);
	void ShowExports(LPVOID Add, DWORD Size);
private:
	HANDLE mHeap;
	CMyPoint* mcPoint;
};


