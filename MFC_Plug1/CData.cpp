#include "pch.h"
#include "CData.h"
#include "CDLG_Plug.h"

HINSTANCE ghInstance;
CData gData;

void CData::InitDLG(LPVOID DLG)
{
	CDLG_Plug* dlg = (CDLG_Plug*)DLG;
	this->hDLG = dlg->GetSafeHwnd();
	printf("´°¿Ú%p±»¼ÓÔØ¡£\n", this->hDLG);
}
