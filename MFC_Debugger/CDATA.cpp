#include "pch.h"
#include "CDATA.h"



MAINWIND gINFO_mWind;
CDATA gDATA;



CDATA::CDATA() :OEP(0), CDEBUG(0)
{
	OutputDebugString(L"CDATA()\n");
	gINFO_mWind = {
		GetCurrentProcessId(),
		GetCurrentThreadId()
	};
	ZeroMemory(&this->PS, sizeof(PROCESS_INFORMATION));
}

CDATA::~CDATA()
{
	OutputDebugString(L"~CDATA()\n");
}
