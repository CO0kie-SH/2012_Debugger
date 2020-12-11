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
			printf("����OEPӲ���ϵ�=%d\n",
				this->AddHardPoint(gDATA.OEP, defBP_Ӳ��ִ��));
				//this->AddSoftPoint(gDATA.OEP, defBP_���ִ��));
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
		status_code = OnException_BreakPoint(ExceptionAddress, pDbg_event);
		break;
	case EXCEPTION_SINGLE_STEP:           // ������Ӳ���ϵ��쳣
		status_code = OnException_SingleStep(ExceptionAddress, pDbg_event);
		break;
	case EXCEPTION_DATATYPE_MISALIGNMENT: // �ڴ�����쳣
	case EXCEPTION_ILLEGAL_INSTRUCTION:   // ��Чָ��
	case EXCEPTION_INT_DIVIDE_BY_ZERO:    // ��0����
	case EXCEPTION_PRIV_INSTRUCTION:      // ָ�֧�ֵ�ǰģʽ
		break;
	default:
		std::cout << "\n\tδ֪������£�";
		this->DisASM(ExceptionAddress, 5);
		break;
	}
	return DBG_CONTINUE;
}

DWORD CDebug::OnException_BreakPoint(LPVOID Address, LPDEBUG_EVENT pDbg_event)
{
	bool isASM = true;
	if (IsSystemBreakPoint)
	{
		printf("\n\t��⵽ϵͳ����ϵ�,");
		IsSystemBreakPoint = FALSE;
		this->msz_Line = "�����������£��봦��";
	}
	else if (Address == gDATA.OEP)
	{
		printf("\n\t��⵽OEP�ϵ㣬�����޸�");
		SetBreakPoint(Address, defBP_�ϵ���ͣ, false);
		this->msz_Line = "����OEP�����£��봦��";
	}
	else if (this->mb_Flagp)
	{
		printf("\n\t���������������޸�");
		SetBreakPoint(Address, defBP_�ϵ���ͣ, defBP_�ϵ�ɾ��);
		this->mb_Flagp = false;
	}

	if(isASM)
		this->DisASM(Address, 5);
	if (this->mb_Wait)
		this->OnLine();
	return DBG_CONTINUE;
}

DWORD CDebug::OnException_SingleStep(LPVOID Address, LPDEBUG_EVENT pDbg_event)
{
	bool isASM = true;
	this->mb_Wait = true;
	// �ж��Ƿ�����ͨ����
	if (this->mb_Flagt)
	{
		printf("\n\t��⵽��������,\t");
		this->mb_Flagt = false;
		this->msz_Line = "�Ѿ��������£��봦��";
	}
	else
	{
		BreakPoint tmp = this->mBreakPoint[Address];
		if (tmp.TYPES == 0)
			std::cout << "\n\tδ֪���󣿲����ڸöϵ�";
		else if (tmp.TYPES == defBP_Ӳ��ִ��)
		{
			printf("\n\t��⵽Ӳ��ִ�жϵ�");
			this->msz_Line = "Ӳ��ִ�ж��£��봦��";
		}
	}
	if (isASM)
		this->DisASM(Address, 5);
	if (this->mb_Wait)
		this->OnLine();
	return DBG_CONTINUE;
}

DWORD CDebug::OnLine()
{
	char cmdline[200] = {};
	char cmd[100] = {};
	DWORD address = {};
	while (true)
	{
		if (msz_Line)
			std::cout << msz_Line;
		// ��ȡ����
		gets_s(cmdline, 200); // bp 0x3333333

		// ��������
		if (sscanf_s(cmdline, "%s", cmd, 100))
		{
			// �ж��Ƿ���g����
			if (strcmp(cmd, "g") == 0)
			{
				break; // ���г���
			}
			//// �ж��Ƿ��Ƕϵ�����
			//if (strcmp(cmd, "bp") == 0)
			//{
			//	sscanf_s(cmdline, "%s %x", cmd, 100, &address);
			//	// ��������ϵ�
			//	SetBreakPoint(hProcess, (LPVOID)address);
			//}
			//// ���õ����ϵ�
			else if (strcmp(cmd, "t") == 0)
			{
				// �߳�������
				CONTEXT context = { CONTEXT_FULL };
				// ��ȡ�߳�������
				GetThreadContext(gDATA.PS.hThread, &context);
				((PEFLAGS)&context.EFlags)->TF = 1;
				// �����߳�������
				SetThreadContext(gDATA.PS.hThread, &context);
				this->mb_Flagt = TRUE;
				break;
			}
			else if (strcmp(cmd, "p") == 0)
			{
				std::cout << "\t����������һ���ϵ�";
				this->mb_Flagp = TRUE;
				// �߳�������
				CONTEXT context = { CONTEXT_FULL };
				// ��ȡ�߳�������
				GetThreadContext(gDATA.PS.hThread, &context);
				this->DisASM((LPVOID)context.Eip, 5);
				break;
			}
		}
		else {
			printf("�����������\n");
		}
	}
	msz_Line = 0;
	return 0;
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
	count = cs_disasm(handle,		/*����������,��cs_open�����õ�*/
		szOpcode,					/*��Ҫ������opcode�Ļ������׵�ַ*/
		sizeof(szOpcode),			/*opcode���ֽ���*/
		(uint64_t)Address,			/*opcode�����ڵ��ڴ��ַ*/
		ReadLen,				/*��Ҫ������ָ������,�����0,�򷴻���ȫ��*/
		&pInsn/*��������*/
	);
	if (this->mb_Flagp)			//���õ�������
	{
		if (/*strcmp(pInsn[0].mnemonic, "jmp") == 0 ||*/
			strcmp(pInsn[0].mnemonic, "call") == 0)
		{
			std::cout << "(CC)";
			szOpcode[pInsn[0].size] = 0xCC;
			count = cs_disasm(handle,		/*����������,��cs_open�����õ�*/
				szOpcode,					/*��Ҫ������opcode�Ļ������׵�ַ*/
				sizeof(szOpcode),			/*opcode���ֽ���*/
				(uint64_t)Address,			/*opcode�����ڵ��ڴ��ַ*/
				ReadLen,				/*��Ҫ������ָ������,�����0,�򷴻���ȫ��*/
				&pInsn/*��������*/
			);
			DWORD_PTR add = pInsn[0].size + (DWORD_PTR)Address;
			this->AddSoftPoint((LPVOID)add, defBP_��������);
		}
		else {
			std::cout << "(TF)\n";
			// �߳�������
			CONTEXT context = { CONTEXT_FULL };
			// ��ȡ�߳�������
			GetThreadContext(gDATA.PS.hThread, &context);
			((PEFLAGS)&context.EFlags)->TF = 1;
			// �����߳�������
			SetThreadContext(gDATA.PS.hThread, &context);
			this->mb_Flagt = TRUE;
			this->mb_Flagp = false;
			cs_free(pInsn, count);
			cs_close(&handle);
			return 0;
		}
	}
	printf("\t��ȡָ���=%lu\n", dwSize);
	for (size_t j = 0; j < count; j++) {
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

BOOL CDebug::AddSoftPoint(LPVOID Address, WORD Type)
{
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
			Type,1,0,
			(DWORD_PTR)Address
		};
		if (NULL == ReadMemory(Address, &tmp.OLD, 1))
			return 0;
		bRet = WriteMemory(Address, gszCC, 1) != 0;
		this->mBreakPoint[Address] = tmp;
	}
	return bRet;
}

BOOL CDebug::AddHardPoint(LPVOID Address, WORD Type)
{
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
			Type,1,0,
			(DWORD_PTR)Address
		};
		
		//
		CONTEXT ct = { CONTEXT_DEBUG_REGISTERS };
		GetThreadContext(gDATA.PS.hThread, &ct);
		PDBG_REG7 pDr7 = (PDBG_REG7)&ct.Dr7;
		if (pDr7->L0 == 0) {	//DR0û�б�ʹ��
			ct.Dr0 = (DWORD)Address;
			pDr7->RW0 = 0;
			pDr7->LEN0 = 0;
			pDr7->L0 = 1;		//�����ϵ�
			bRet = true;
		}
		else
		{
			MessageBox(gINFO_mWind.hwMFC, L"�޿���Ӳ���ϵ�", 0, 0);
			return 0;
		}
		SetThreadContext(gDATA.PS.hThread, &ct);
		this->mBreakPoint[Address] = tmp;
	}
	return bRet;
}

DWORD CDebug::SetBreakPoint(LPVOID Address, WORD Type, BOOL isBreak)
{
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
		if (tmp.TYPES == defBP_���ִ��
			|| tmp.TYPES == defBP_��������) {
			if (!!this->WriteMemory(Address, &tmp.OLD, 1)) {	//��ԭ�ֽ�ʧ��
				printf("д��Ŀ�����%p->%lX ʧ��\n", Address, tmp.OLD);
				return 0;
			}
			if (isBreak == 0)					//��������ʱɾ������ʱ�ϵ�
				tmp.TYPES = defBP_�ϵ���ͣ;
			if (isBreak == defBP_�ϵ�ɾ��)		//��������ʱɾ������ʱ�ϵ�
				this->mBreakPoint.erase(Address);
			else if (isBreak == TRUE)			//�����Զϵ㣬�ٴ�����CC
			{
				if (!this->WriteMemory(Address,gszCC,1))
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
		}
	}break;
	default: break;
	}
	return bRet;
}

SIZE_T CDebug::ReadMemory(LPVOID Address, LPVOID ReadBuff, DWORD_PTR ReadLen)
{
	SIZE_T Read;
	if (NULL == ReadProcessMemory(gDATA.PS.hProcess, Address, ReadBuff, ReadLen, &Read))
		return 0;
	return Read;
}

SIZE_T CDebug::WriteMemory(LPVOID Address, LPVOID Buff, DWORD_PTR ReadLen)
{
	SIZE_T Len;
	if (NULL == WriteProcessMemory(gDATA.PS.hProcess, Address, Buff, ReadLen, &Len))
		return 0;
	return Len;
}
