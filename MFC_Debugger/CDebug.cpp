#include "pch.h"
#include "CDebug.h"
#include ".\capstone\include\capstone.h"
#include "RegStruct.h"

//1. ����ͷ�ļ�
#ifdef _WIN64 // 64λƽ̨���������Զ����������
#pragma 
comment(lib, "capstone/lib/capstone_x64.lib")
#else
#pragma comment(lib,".\\capstone\\lib\\capstone_x86.lib")
#endif // _64

BOOL CDebug::InitDebug(PWCHAR Path)
{
	STARTUPINFO si = { sizeof(STARTUPINFO) };
	PROCESS_INFORMATION ps;
	// �Ը��ӷ�ʽ����
	// BOOL bRet = DebugActiveProcess(3008);

	//�Ե��Է�ʽ��������
	BOOL bRet = CreateProcess(
		Path,											//	����Ŀ��·��
		NULL,												//  �����в���
		NULL,												//  ���̰�ȫ����	
		NULL,												//  �̰߳�ȫ����	
		FALSE,												//  �Ƿ�̳о����
		DEBUG_ONLY_THIS_PROCESS | CREATE_NEW_CONSOLE,		//  ��ֻ���Է�ʽ��������
		NULL,												//  ��������
		NULL,												//  ����Ŀ¼
		&si, &ps);
	if (bRet == FALSE)
	{
		MessageBox(0, L"���̴���ʧ��", 0, 0);
		return 0;
	}

	gDATA.SetPS(&ps);
	DEBUG_EVENT dbg_event;
	BOOL Loop_Debug = TRUE;
	DWORD dbg_status = DBG_CONTINUE;  //�쳣������
	// ����ѭ��
	while (Loop_Debug)
	{
		// �ȴ������¼�
		WaitForDebugEvent(&dbg_event, -1);

		// ���������Ϣ
		switch (dbg_event.dwDebugEventCode)/*�ж��¼�����*/
		{
		case EXCEPTION_DEBUG_EVENT:     // �쳣�����¼�
			printf("�쳣�����¼�����\n");
			dbg_status = OnExceptionHandler(&dbg_event);
			break;
		case CREATE_THREAD_DEBUG_EVENT: // �̴߳����¼�
			printf("�̴߳����¼�����\n");
			break;
		case CREATE_PROCESS_DEBUG_EVENT: {// ���̴����¼�
			gDATA.OEP = dbg_event.u.CreateProcessInfo.lpStartAddress; // �������
			printf("���̴����¼�����,OEP=%p\n", gDATA.OEP);
			this->DisASM(gDATA.OEP, 5);
			printf("����OEP����ϵ�=%d\n",
				this->AddSoftPoint(gDATA.OEP));
		}break;
		case EXIT_THREAD_DEBUG_EVENT:   // �˳��߳��¼�
			printf("�߳��˳��¼�����\n");
			break;
		case EXIT_PROCESS_DEBUG_EVENT:  // �˳������¼�
			Loop_Debug = FALSE;
			printf("�����˳��¼�����\n");
			break;
		case LOAD_DLL_DEBUG_EVENT:      // ӳ��DLL�¼�
			printf("DLL�����¼�����\n");
			break;
		case UNLOAD_DLL_DEBUG_EVENT:    // ж��DLL�¼� 
			printf("DLLж���¼�����\n");
			break;
		case OUTPUT_DEBUG_STRING_EVENT: // �����ַ�������¼�
			printf("�ַ�������¼�����\n");
			break;
		case RIP_EVENT:                 // RIP�¼�(�ڲ�����)
			break;
		}

		// �ظ�������ϵͳ������
		ContinueDebugEvent(
			dbg_event.dwProcessId,   //���Խ���ID,�����DEBUG_EVNET�л�ȡ
			dbg_event.dwThreadId,    //�����߳�ID,�����DEBUG_EVNET�л�ȡ
			dbg_status);			 //�쳣�Ƿ���ֻ���쳣��Ч

	}

	CloseHandle(ps.hThread);
	::TerminateProcess(ps.hProcess, 0);
	CloseHandle(ps.hProcess);
	return 0;
}

// �����쳣�ַ�����
DWORD CDebug::OnExceptionHandler(LPDEBUG_EVENT pDbg_event)
{
	// ��ȡ�쳣��ַ
	LPVOID ExceptionAddress = pDbg_event->u.Exception.ExceptionRecord.ExceptionAddress;
	// �쳣�����Ĵ�����
	DWORD ExceptionCode = pDbg_event->u.Exception.ExceptionRecord.ExceptionCode;
	// ��ȡ���̾��
	//HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pDbg_event->dwProcessId);
	// �߳̾��
	//HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, pDbg_event->dwThreadId);
	// ����״̬��
	DWORD status_code = DBG_EXCEPTION_NOT_HANDLED;
	printf("\t�����룺0x%lX,�쳣��ַ=%p", ExceptionCode, ExceptionAddress);

	// �����쳣����
	switch (ExceptionCode)
	{
		// ����3���쳣
	case EXCEPTION_ACCESS_VIOLATION:      // �Ƿ������쳣
		break;
	case EXCEPTION_BREAKPOINT:            // �ϵ��쳣
		if (ExceptionAddress == gDATA.OEP)
		{
			printf("\n\t��⵽OEP�ϵ㣬�����޸�");
			SetBreakPoint(ExceptionAddress, defBP_�ϵ���ͣ,false);
			//printf("����OEP�����£��봦��");
		}
		// ��������ϵ�
		//status_code = OnException_BreakPoint(ExceptionAddress, hProcess, hThread);
		break;
	case EXCEPTION_SINGLE_STEP:           // ������Ӳ���ϵ��쳣
		//status_code = OnException_SingleStep(ExceptionAddress, hProcess, hThread);
		break;
	case EXCEPTION_DATATYPE_MISALIGNMENT: // �ڴ�����쳣
	case EXCEPTION_ILLEGAL_INSTRUCTION:   // ��Чָ��
	case EXCEPTION_INT_DIVIDE_BY_ZERO:    // ��0����
	case EXCEPTION_PRIV_INSTRUCTION:      // ָ�֧�ֵ�ǰģʽ
		break;
	}
	this->DisASM(ExceptionAddress, 5);
	return DBG_CONTINUE;
}

DWORD CDebug::DisASM(LPVOID Address,DWORD ReadLen)
{
	csh handle;				// �����������
	cs_insn* pInsn;			// ���淴���õ���ָ��Ļ������׵�ַ
	DWORD dwSize = 0;		// ʵ�ʶ�ȡ����
	unsigned int count = 0; // ����õ��ķ�����ָ������
	unsigned char szOpcode[200] = {0}; // Ŀ������е�OPCODE
	PROCESS_INFORMATION& ps = gDATA.PS;
	CStringA strA;
	//��ʼ������������,(x86_64�ܹ�,32λģʽ,���)
	cs_open(CS_ARCH_X86,  /*x86ָ�*/
		CS_MODE_32, /*ʹ��32λģʽ����opcode*/
		&handle /*����ķ������*/
	);


	ReadProcessMemory(ps.hProcess, Address, szOpcode, 200, &dwSize);
	printf("\t��ȡָ���=%lu\n", dwSize);
	count = cs_disasm(handle,		/*����������,��cs_open�����õ�*/
		szOpcode,					/*��Ҫ������opcode�Ļ������׵�ַ*/
		sizeof(szOpcode),			/*opcode���ֽ���*/
		(uint64_t)Address,			/*opcode�����ڵ��ڴ��ַ*/
		ReadLen,				/*��Ҫ������ָ������,�����0,�򷴻���ȫ��*/
		&pInsn/*��������*/
	);
	size_t j;
	for (j = 0; j < count; j++) {
		strA = "";
		for (byte k = 0,by; k < pInsn[j].size; k++)
		{
			by = pInsn[j].bytes[k];
			strA.AppendFormat("%02X ", by);
		}
		printf("0x%p |%-16s|%s %s\n",
			(LPVOID)pInsn[j].address, /*ָ���ַ*/
			strA.GetBuffer(),
			pInsn[j].mnemonic,/*ָ�������*/
			pInsn[j].op_str/*ָ�������*/
		);
	}
	// �ͷű���ָ��Ŀռ�
	cs_free(pInsn, count);
	// �رվ��
	cs_close(&handle);
	return 0;
}

BOOL CDebug::AddSoftPoint(LPVOID Address)
{
	BYTE buff[16] = { 0 };
	BOOL bRet = false;
	BreakPoint tmp = this->mBreakPoint[Address];
	if (tmp.TYPES)
	{
		printf("�ϵ��Ѵ��ڣ�%p", Address);
		return 0;
	}

	else
	{
		tmp = BreakPoint{
			defBP_���ִ��,1,0,
			(DWORD_PTR)Address
		};
		buff[0] = 0xCC;

		if (NULL == ReadProcessMemory(gDATA.PS.hProcess,Address , &tmp.OLD, 1, 0))
			return 0;
		bRet = WriteProcessMemory(gDATA.PS.hProcess, Address, buff, 1, 0);
		this->mBreakPoint[Address] = tmp;
	}
	return bRet;
}

DWORD CDebug::SetBreakPoint(LPVOID Address, WORD Type, BOOL isBreak)
{
	BYTE buff[16] = { 0 };
	BOOL bRet = false;
	BreakPoint &tmp = this->mBreakPoint[Address];
	if (tmp.TYPES == 0)
	{
		printf("�ϵ㲻���ڣ����ø�������%p\n", Address);
		return 0;
	}
	if (tmp.TYPES == Type)
	{
		printf("�ϵ�״̬��ͬ�����ø�������%p\n", Address);
		return 0;
	}

	switch (Type)
	{
	case 0:			//������
		puts("״̬Ϊ�գ����ø�������");
		return 0;
	case 1: {		//��ͣ
		//����ϵ�
		if (tmp.TYPES == defBP_���ִ��) {
			bRet = WriteProcessMemory(gDATA.PS.hProcess, Address, &tmp.OLD, 1, 0);
			if (!bRet) {	//��ԭ�ֽ�ʧ��
				printf("д��Ŀ�����%p->%lX ʧ��\n", Address, tmp.OLD);
				return 0;
			}
			//�����������
			if (isBreak)
			{
				buff[0] = 0xCC;
				bRet = WriteProcessMemory(gDATA.PS.hProcess, Address, buff, 1, 0);
				if (!bRet)
				{
					printf("д��Ŀ�����%p->INT3 ʧ��\n", Address);
					return 0;
				}
			}

			// eip -1 ���޸������쳣
			CONTEXT context = { CONTEXT_FULL };
			GetThreadContext(gDATA.PS.hThread, &context);
			context.Eip -= 1;
			SetThreadContext(gDATA.PS.hThread, &context);
			tmp.TYPES = defBP_�ϵ���ͣ;
		}
	}break;
	default:
		break;
	}
	return bRet;
}
