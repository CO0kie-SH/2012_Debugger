#pragma once
#include <map>
using std::map;


#define defBP_�ϵ���ͣ 1
#define defBP_���ִ�� 2
#define defBP_�������� 3
#define defBP_�ϵ�ɾ�� 4
#define defBP_Ӳ��ִ�� 4

constexpr LPVOID gszCC = "\xCC";


typedef struct _BreakPoint
{
	WORD TYPES;	//����
	WORD STATU;	//״̬
	DWORD OLD;	//�ɵ�״̬
	DWORD_PTR Address;	//��ַ
}BreakPoint, * LPBreakPoint;


class CDebug
{
public:
	BOOL IsSystemBreakPoint;
	BOOL mb_Wait;
	BOOL mb_Flagt;
	BOOL mb_Flagp;
	PCHAR msz_Line;
private:
	map<LPVOID, BreakPoint> mBreakPoint;
public:
	CDebug() :IsSystemBreakPoint(1),
		mb_Wait(1), mb_Flagt(0), mb_Flagp(0), msz_Line(0)
	{
		OutputDebugString(L"CDebug()\n");
	}
	~CDebug()
	{
		OutputDebugString(L"~CDebug()\n");
		gDATA.SetPS();
		puts("�������߳̽�����");
	}
	BOOL InitDebug(PWCHAR Path);

	// �����쳣�ַ�����
	DWORD OnExceptionHandler(LPDEBUG_EVENT pDbg_event);
	//����ϵͳ�ϵ�
	DWORD OnException_BreakPoint(LPVOID Address, LPDEBUG_EVENT pDbg_event);
	//��������Ӳ������
	DWORD OnException_SingleStep(LPVOID Address, LPDEBUG_EVENT pDbg_event);
	//�����������
	DWORD OnLine();

	//�����
	DWORD DisASM(LPVOID Address, DWORD ReadLen);
	//��������ϵ�
	BOOL AddSoftPoint(LPVOID Address, WORD Type);
	//����Ӳ���ϵ�
	BOOL AddHardPoint(LPVOID Address, WORD Type);
	// ����ϵ㴦����
	DWORD SetBreakPoint(LPVOID Address, WORD Type, BOOL isBreak);

	SIZE_T ReadMemory(LPVOID Address, LPVOID ReadBuff, DWORD_PTR ReadLen);
	SIZE_T WriteMemory(LPVOID Address, LPVOID Buff, DWORD_PTR ReadLen);
};

