#pragma once
#include <map>
using std::map;


#define defBP_�ϵ���ͣ 1
#define defBP_���ִ�� 2

typedef struct _BreakPoint
{
	WORD TYPES;	//����
	WORD STATU;	//״̬
	DWORD OLD;	//�ɵ�״̬
	DWORD_PTR Address;	//��ַ
}BreakPoint, * LPBreakPoint;


class CDebug
{
private:
	map<LPVOID, BreakPoint> mBreakPoint;
public:
	~CDebug()
	{
		gDATA.SetPS();
		puts("�������߳̽�����");
	}
	BOOL InitDebug(PWCHAR Path);

	// �����쳣�ַ�����
	DWORD OnExceptionHandler(LPDEBUG_EVENT pDbg_event);
	//�����
	DWORD DisASM(LPVOID Address, DWORD ReadLen);
	//��������ϵ�
	BOOL AddSoftPoint(LPVOID Address);
	// ����ϵ㴦����
	DWORD SetBreakPoint(LPVOID Address, WORD Type, BOOL isBreak);
};

