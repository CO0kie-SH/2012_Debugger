#include "pch.h"
#include "CDATA.h"



MAINWIND gINFO_mWind;
CDATA gDATA;



CDATA::CDATA()
{
	OutputDebugString(L"CDATA()\n");
	gINFO_mWind = {
		GetCurrentProcessId(),
		GetCurrentThreadId()
	};
}

CDATA::~CDATA()
{
}
