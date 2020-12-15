#pragma once
#include <map>
using std::map;
#include "RegStruct.h"

#define defBP_�ϵ���ͣ 1
#define defBP_���ִ�� 2
#define defBP_�������� 3
#define defBP_�ϵ�ɾ�� 4
#define defBP_Ӳ��ִ�� 5
#define defBP_Ӳ��д�� 6
#define defBP_Ӳ����д 7
#define defBP_Ӳ����ͣ 8
#define defBP_�ڴ�ִ�� 9
#define defBP_�ڴ�д�� 10
#define defBP_�ڴ��ȡ 11
#define defBP_�ڴ��д 12
#define defBP_�ڴ���ͣ 13
#define defBP_�ڴ����� 14
#define defBP_�ڴ渴д 15
#define defBP_�ڴ�ɾ�� 16

enum EM_BUG_NUM
{
	BUG_1,
	BUG_2,
	BUG_3,
	BUG_4,
	BUG_�ڴ��쳣����
};

constexpr LPVOID gszCC = "\xCC\xCC";
constexpr PWCHAR gszBP[] = {
	L"�ϵ����",
	L"�ϵ���ͣ",
	L"����ϵ�",
	L"��������",
	L"�ϵ�ɾ��",
	L"Ӳ��ִ��",
	L"Ӳ��д��",
	L"Ӳ����д",
	L"Ӳ����ͣ",
	L"�ڴ�ִ��",
	L"�ڴ�д��",
	L"�ڴ��ȡ",
	L"�ڴ��д",
	L"�ڴ���ͣ",
	L"�ڴ�����",
	L"�ڴ渴д"
};

typedef struct _BreakPoint
{
	WORD TYPES = 0;	//����
	WORD STATU = 0;	//״̬
	DWORD OLD = 0;	//�ɵ�״̬
	DWORD Cout = 0;	//���д���
	LPVOID Address = 0;	//��ַ
	PWCHAR str = 0;
}BreakPoint, * LPBreakPoint;

typedef struct _WaitPoint	//�ϵ�ṹ�壺�����ж��Ƿ��Ƕϵ�
{
	BYTE SYSPoint;			//ϵͳ����ϵ�
	BYTE OEPPoint;			//��������ϵ�
	BYTE PassPoint;			//�����ϵ�
	BYTE StepPoint;			//�����ϵ�
	LPBreakPoint SoftPoint;	//����ϵ�
	LPBreakPoint HardPoint;	//Ӳ���ϵ�
	LPBreakPoint MemyPoint;	//�ڴ�ϵ�
	PCHAR LineShow;			//�ϵ��־��
}WaitPoint, * LPWaitPoint;



class CMyPoint
{
	friend class CPE;
public:
	WaitPoint mWait;
	map<LPVOID, BreakPoint> mBreakPoint;
	LPBreakPoint MemyPoint;
public:
	CMyPoint()
	{
		ZeroMemory(&mWait, sizeof(WaitPoint));
	}
	CMyPoint* GetThis()
	{
		return this;
	}
	//��������ϵ�
	BOOL AddSoftPoint(LPVOID Address, WORD Type, PWCHAR Text = 0);
	//���Ӳ���ϵ�
	BOOL AddHardPoint(LPVOID Address, WORD Type, PWCHAR Text = 0);
	//�����ڴ�ϵ�
	BOOL AddMemPoint(LPVOID Address, WORD Type, PWCHAR Text = 0);
	//��������ϵ�
	BOOL ReSetSoftPoint(LPBreakPoint pPoint);
	//�����ڴ�ϵ�
	BOOL SetMemPoint(LPVOID Address, BOOL isRe = 0);

	SIZE_T ReadMemory(LPVOID Address, LPVOID ReadBuff, DWORD_PTR ReadLen);

	SIZE_T WriteMemory(LPVOID Address, LPVOID WriteBuff, DWORD_PTR WriteLen);
	int Bug(int ID)
	{
		printf("�����룺%X��", ID);
		this->mWait.LineShow = "����ϵ�����޸���";
		//this->OnLine();
		::TerminateProcess(gDATA.PS.hProcess, 0);
		WaitForSingleObject(gDATA.PS.hProcess, 1000);
		return 0;
	}
};
