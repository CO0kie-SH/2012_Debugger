#include "CProInfo1.h"

DWORD gPEB, gTEB;
PPEB gPPEB;
HMODULE ghNtDll;

CProInfo1::CProInfo1()
{
	DWORD PID, TID;
	_asm
	{
		mov eax, fs: [0x20] ;	//找到PID
		mov PID, eax;
		mov eax, fs: [0x24] ;	//找到TID
		mov TID, eax;
		mov eax, fs: [0x30] ;	//找到PEB
		mov gPEB, eax;
		mov gPPEB, eax;
		mov eax, fs: [0x18] ;	//找到TEB
		mov gTEB, eax;
	}


	//加载NTDLL
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
		mov eax, fs: [0x30] ;//找到PEB
		mov al, byte ptr ds : [eax + 0x2] ; // 取出一个字节
		mov bRet, al;
	}
	return bRet;
}

BOOL CProInfo1::NtGlobalFlag()
{
	UINT NtGlobalFlag = 0;
	__asm
	{
		mov eax, dword ptr fs : [0x30] ;          // 获取PEB地址
		mov eax, dword ptr ds : [eax + 0x68] ; // 获取PEB.NtGloba...
		mov NtGlobalFlag, eax;
	}
	printf("NtGlobalFlag=0x%lX\t", NtGlobalFlag);
	return NtGlobalFlag == 0x70 ? true : false;
}

/*	函数：查询调试端口
	原文：先知社区https://xz.aliyun.com/t/5339
	优化：CO0kie丶
*/
BOOL CProInfo1::NQIP_DebugPort()
{
	DWORD nDebugPort = 0;
	NTSTATUS status = NtQueryInformationProcess(
		GetCurrentProcess(), 	// 目标进程句柄
		ProcessDebugPort, 		// 查询信息类型
		&nDebugPort, 			// 输出查询信息
		sizeof(nDebugPort), 	// 查询类型大小
		NULL); 					// 实际返回数据大小
	std::cout << "nDebugPort=" << nDebugPort << '\t';
	return !status && nDebugPort != 0;
}

BOOL CProInfo1::NQIP_ProcessDebugObjectHandle()
{
	//从WindowsXP开始，将为调试的进程创建一个“调试对象”。
	//以下就是检查当前进程调试对象的例子：
	HANDLE hProcessDebugObject = 0;
	NTSTATUS status = NtQueryInformationProcess(
		GetCurrentProcess(), 			// 目标进程句柄
		(PROCESSINFOCLASS)0x1E, 		// 查询信息类型  ProcessDebugObjectHandle=0x1E
		&hProcessDebugObject, 			// 输出查询信息
		sizeof(hProcessDebugObject),	// 查询类型大小
		NULL); 							// 实际返回大小
	std::cout << "hProcessDebugObject=" << hProcessDebugObject << '\t';
	return !status && NULL != hProcessDebugObject;
}

BOOL CProInfo1::NQIP_ProcessDebugFlag()
{
	//ProcessDebugFlags = 0x1F
	//当检查该标识时，它会返回到EPROCESS内核结构的NoDebugInherit位的反转值。
	//如果NtQueryInformationProcess函数的返回值为0，
	//则正在调试该进程。以下是此类反调试检查的示例：
	BOOL bProcessDebugFlag = 0;
	NTSTATUS status = NtQueryInformationProcess(
		GetCurrentProcess(), 			// 目标进程句柄
		(PROCESSINFOCLASS)0x1F, 		// 查询信息类型  ProcessDebugFlags=0x1F
		&bProcessDebugFlag, 			// 输出查询信息
		sizeof(bProcessDebugFlag),		// 查询类型大小
		NULL); 							// 实际返回大小
	return status && NULL != bProcessDebugFlag;
}

DWORD CProInfo1::NQIP_ParentPID()
{
	struct PROCESS_BASIC_INFORMATION {
		DWORD ExitStatus; 		     // 进程返回码
		PPEB  PebBaseAddress; 		 // PEB地址
		DWORD AffinityMask; 		 // CPU亲和性掩码
		LONG  BasePriority; 		 // 基本优先级
		DWORD UniqueProcessId; 		 // 本进程PID
		DWORD InheritedFromUniqueProcessId; // 父进程PID
	}stcProcInfo;

	NtQueryInformationProcess(
		GetCurrentProcess(),
		ProcessBasicInformation, // 进程基本信息
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
	// 查询系统信息
	NtQuerySystemInformation(
		(SYSTEM_INFORMATION_CLASS)0x23,       // 查询信息类型 有没有内核调试器
		&DebuggerInfo, 			                // 输出查询信息
		sizeof(DebuggerInfo), 			         // 查询类型大小
		NULL); 					         // 实际返回大小
	return DebuggerInfo.DebuggerEanbled;
}