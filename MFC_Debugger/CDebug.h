#pragma once
#include <psapi.h>
#include <tlhelp32.h>
#include <sstream>
#include "CMyPoint.h"
#include <vector>
using std::vector;



constexpr PCHAR gszRegDrs[] = {
	"\nContextFlags",
	"Dr0",
	"Dr1",
	"Dr2",
	"Dr3",
	"Dr6",
	"Dr7"
};

constexpr PCHAR gszRegGs[] = {
	"\nSegGs",
	"SegFs",
	"SegEs",
	"SegDs"
};

constexpr PCHAR gszRegEs[] = {
	"\nEdi",
	"Esi",
	"Ebx",
	"Edx",
	"Ecx",
	"Eax",
	"\nEbp",
	"Eip",
	"SegCs",              // MUST BE SANITIZED
	"EFlags",             // MUST BE SANITIZED
	"Esp",
	"SegSs"
};


typedef struct _CodeLine		//Դ��ṹ��
{
	DWORD Address;				//��ַ
	std::string str;			//Դ���Ӧ��
}CodeLine,*LPCodeLine;


class CDebug :public CMyPoint
{
private:
	vector<CodeLine> m_codeline;	//Դ������
	CStringA mPath;					//���ļ�·��
public:
	CDebug();
	~CDebug();
	//��ʼ��������
	BOOL InitDebug(PWCHAR Path);

	// �����쳣�ַ�����
	DWORD OnExceptionHandler(LPDEBUG_EVENT pDbg_event);
	//����ϵͳ�ϵ�
	DWORD OnException_BreakPoint(LPVOID Address, LPDEBUG_EVENT pDbg_event);
	//��������Ӳ������
	DWORD OnException_SingleStep(LPVOID Address, LPDEBUG_EVENT pDbg_event);
	//�����ڴ����
	DWORD OnException_MemPoint(LPVOID Address, LPDEBUG_EVENT pDbg_event);
	//�����������
	DWORD OnLine();
	
	//�����
	DWORD DisASM(LPVOID Address, DWORD ReadLen);
	//��÷�����
	BOOL GetSymName(LPVOID Address, char* Str);
	//����Դ��
	BOOL LoadCode(LPCCH szPath);
	//��ʾԴ��
	BOOL ShowCode(DWORD Address = 0);

	// ����ϵ㴦����
	DWORD SetBreakPoint(LPVOID Address, WORD Type, BOOL isBreak);
	// Ӳ���ϵ㴦����
	DWORD SetHardPoint(LPBreakPoint Point, WORD Type, BOOL isBreak);

	//��ʾ�Ĵ���
	void ShowRegister();
	//��ʾģ��
	void ShowDlls(BYTE* Address = 0, int id = 0);
	//��ʾ�ڴ�
	void ShowMem(LPVOID Address);
};

