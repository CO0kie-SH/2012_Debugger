#include "CProInfo1.h"

DWORD gPEB, gTEB;
PPEB gPPEB;
HMODULE ghNtDll;

CProInfo1::CProInfo1()
{
	DWORD PID, TID;
	_asm
	{
		mov eax, fs: [0x20] ;	//�ҵ�PID
		mov PID, eax;
		mov eax, fs: [0x24] ;	//�ҵ�TID
		mov TID, eax;
		mov eax, fs: [0x30] ;	//�ҵ�PEB
		mov gPEB, eax;
		mov gPPEB, eax;
		mov eax, fs: [0x18] ;	//�ҵ�TEB
		mov gTEB, eax;
	}


	//����NTDLL
	//ghNtDll = LoadLibrary(TEXT("ntdll.dll"));
	this->PPID = this->NQIP_ParentPID();
	printf("PID=%lu,TID=%lu,PPID=%lu\n", PID, TID, PPID);
	printf("PEB=0x%lX,TEB=0x%lX,*pPEB,0x%p\n", gPEB, gTEB, gPPEB);

	//if (NULL != ghNtDll)
	//{
	//	gNtQueryInformationProcess = (pfnNtQueryInformationProcess)
	//		GetProcAddress(ghNtDll, "NtQueryInformationProcess");
	//}
}

BOOL CProInfo1::IsBeginDebuged()
{
	bool bRet = TRUE;
	_asm
	{
		mov eax, fs: [0x30] ;//�ҵ�PEB
		mov al, byte ptr ds : [eax + 0x2] ; // ȡ��һ���ֽ�
		mov bRet, al;
	}
	return bRet;
}

BOOL CProInfo1::NtGlobalFlag()
{
	UINT NtGlobalFlag = 0;
	__asm
	{
		mov eax, dword ptr fs : [0x30] ;          // ��ȡPEB��ַ
		mov eax, dword ptr ds : [eax + 0x68] ; // ��ȡPEB.NtGloba...
		mov NtGlobalFlag, eax;
	}
	printf("NtGlobalFlag=0x%lX\t", NtGlobalFlag);
	return NtGlobalFlag == 0x70 ? true : false;
}

/*	��������ѯ���Զ˿�
	ԭ�ģ���֪����https://xz.aliyun.com/t/5339
	�Ż���CO0kieؼ
*/
BOOL CProInfo1::NQIP_DebugPort()
{
	DWORD nDebugPort = 0;
	NTSTATUS status = NtQueryInformationProcess(
		GetCurrentProcess(), 	// Ŀ����̾��
		ProcessDebugPort, 		// ��ѯ��Ϣ����
		&nDebugPort, 			// �����ѯ��Ϣ
		sizeof(nDebugPort), 	// ��ѯ���ʹ�С
		NULL); 					// ʵ�ʷ������ݴ�С
	std::cout << "nDebugPort=" << nDebugPort << '\t';
	return !status && nDebugPort != 0;
}

BOOL CProInfo1::NQIP_ProcessDebugObjectHandle()
{
	//��WindowsXP��ʼ����Ϊ���ԵĽ��̴���һ�������Զ��󡱡�
	//���¾��Ǽ�鵱ǰ���̵��Զ�������ӣ�
	HANDLE hProcessDebugObject = 0;
	NTSTATUS status = NtQueryInformationProcess(
		GetCurrentProcess(), 			// Ŀ����̾��
		(PROCESSINFOCLASS)0x1E, 		// ��ѯ��Ϣ����  ProcessDebugObjectHandle=0x1E
		&hProcessDebugObject, 			// �����ѯ��Ϣ
		sizeof(hProcessDebugObject),	// ��ѯ���ʹ�С
		NULL); 							// ʵ�ʷ��ش�С
	std::cout << "hProcessDebugObject=" << hProcessDebugObject << '\t';
	return !status && NULL != hProcessDebugObject;
}

BOOL CProInfo1::NQIP_ProcessDebugFlag()
{
	//ProcessDebugFlags = 0x1F
	//�����ñ�ʶʱ�����᷵�ص�EPROCESS�ں˽ṹ��NoDebugInheritλ�ķ�תֵ��
	//���NtQueryInformationProcess�����ķ���ֵΪ0��
	//�����ڵ��Ըý��̡������Ǵ��෴���Լ���ʾ����
	BOOL bProcessDebugFlag = 0;
	NTSTATUS status = NtQueryInformationProcess(
		GetCurrentProcess(), 			// Ŀ����̾��
		(PROCESSINFOCLASS)0x1F, 		// ��ѯ��Ϣ����  ProcessDebugFlags=0x1F
		&bProcessDebugFlag, 			// �����ѯ��Ϣ
		sizeof(bProcessDebugFlag),		// ��ѯ���ʹ�С
		NULL); 							// ʵ�ʷ��ش�С
	return status && NULL != bProcessDebugFlag;
}

DWORD CProInfo1::NQIP_ParentPID()
{
	struct PROCESS_BASIC_INFORMATION {
		DWORD ExitStatus; 		     // ���̷�����
		PPEB  PebBaseAddress; 		 // PEB��ַ
		DWORD AffinityMask; 		 // CPU�׺�������
		LONG  BasePriority; 		 // �������ȼ�
		DWORD UniqueProcessId; 		 // ������PID
		DWORD InheritedFromUniqueProcessId; // ������PID
	}stcProcInfo;

	NtQueryInformationProcess(
		GetCurrentProcess(),
		ProcessBasicInformation, // ���̻�����Ϣ
		&stcProcInfo,
		sizeof(stcProcInfo),
		NULL);
	return stcProcInfo.InheritedFromUniqueProcessId;
}

BOOL CProInfo1::NQSI_SystemKernelDebuggerInformation() {
	struct _SYSTEM_KERNEL_DEBUGGER_INFORMATION {
		BOOLEAN DebuggerEanbled;
		BOOLEAN DebuggerNotPresent;
	}DebuggerInfo = { 0 };
	// ��ѯϵͳ��Ϣ
	NtQuerySystemInformation(
		(SYSTEM_INFORMATION_CLASS)0x23,       // ��ѯ��Ϣ���� ��û���ں˵�����
		&DebuggerInfo, 			                // �����ѯ��Ϣ
		sizeof(DebuggerInfo), 			         // ��ѯ���ʹ�С
		NULL); 					         // ʵ�ʷ��ش�С
	return DebuggerInfo.DebuggerEanbled;
}