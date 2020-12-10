#pragma once

typedef struct _MAINWIND
{
	DWORD	wPID;
	DWORD	wTID;
	HWND	hwMFC;
	HWND	hwCON;
}MAINWIND, * LPMAINWIND;


extern MAINWIND gINFO_mWind;

class CDATA
{
public:
	CDATA();
	~CDATA();

private:

};

extern CDATA gDATA;
