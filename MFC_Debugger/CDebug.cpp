#include "pch.h"
#include "CDebug.h"
#include ".\capstone\include\capstone.h"

//1. ����ͷ�ļ�
#ifdef _WIN64 // 64λƽ̨���������Զ����������
#pragma 
comment(lib, "capstone/lib/capstone_x64.lib")
#else
#pragma comment(lib,".\\capstone\\lib\\capstone_x86.lib")
#endif // _64



//CMyPoint::CMyPoint(LPWaitPoint Wait)
//	:mpWait(Wait)
//{
//}
//
//CMyPoint::~CMyPoint()
//{
//}




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
			//printf("����OEPӲ���ϵ�=%d\n", this->AddHardPoint(gDATA.OEP, defBP_Ӳ��ִ��));
			//printf("����OEP����ϵ�=%d\n", this->AddSoftPoint(gDATA.OEP, defBP_���ִ��, L"OEP���"));
			printf("����OEP�ڴ�ϵ�=%d\n", this->AddMemPoint(gDATA.OEP, defBP_�ڴ�ִ��, L"OEP���"));
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
		status_code = OnException_MemPoint(ExceptionAddress, pDbg_event);
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
		this->mWait.LineShow = "δ֪������£��봦��";
		this->DisASM(ExceptionAddress, 5);
		this->OnLine();
		break;
	}
	return status_code;
}

//����ϵ��쳣
DWORD CDebug::OnException_BreakPoint(LPVOID Address, LPDEBUG_EVENT pDbg_event)
{
	BOOL isASM = true, isWait = true;
	if (this->mWait.SYSPoint)
	{
		printf("\n\t��⵽ϵͳ����ϵ�,");
		this->mWait.SYSPoint = FALSE;
		this->mWait.LineShow = "�����������£��������";
	}
	else if (Address == gDATA.OEP)
	{
		printf("\n\t��⵽OEP�ϵ㣬\t");
		SetBreakPoint(Address, defBP_�ϵ���ͣ, FALSE);
		this->mWait.LineShow = "����OEP�����£��������";
	}
	else if (this->mWait.StepPoint)
	{
		printf("\n\t���������������޸�");
		LPBreakPoint tmp = &this->mBreakPoint[Address];
		tmp->STATU = defBP_�ϵ�ɾ��;
		this->ReSetSoftPoint(tmp);
		this->mWait.StepPoint = false;
		this->mWait.LineShow = "�������룬�������";
	}
	else
	{
		BreakPoint tmp = this->mBreakPoint[Address];
		if (tmp.TYPES == 0)
			printf("�������������ڸöϵ�%p\n", Address);
		else if (tmp.TYPES != defBP_���ִ��)
			printf("���������޷��жϸöϵ�%p\n", Address);
		else
		{
			printf("\n\t��⵽����ϵ�%p", Address);
			SetBreakPoint(Address, defBP_�ϵ���ͣ, TRUE);
			this->mWait.LineShow = "���ִ�ж��£��������";
		}
	}

	if(isASM)
		this->DisASM(Address, 5);
	if (isWait)
		this->OnLine();
	return DBG_CONTINUE;
}

//��������Ӳ������������ϵ�
DWORD CDebug::OnException_SingleStep(LPVOID Address, LPDEBUG_EVENT pDbg_event)
{
	bool isASM = true, isWait = true, isRet = false, isNULL = false;
	BreakPoint& tmp = this->mBreakPoint[Address];
	//�ж��Ƿ�Ϊ���öϵ����
	if (this->mWait.SoftPoint)			//��������ñ�־
	{
		
		printf("\n\t��⵽����ϵ㸴д���ٴ�д��CC\n");
		if (NULL == this->WriteMemory(					//��ԭָ��
			this->mWait.SoftPoint->Address,
			&this->mWait.SoftPoint->OLD, 1))
			return this->Bug(2);
		this->mWait.SoftPoint->TYPES = defBP_���ִ��;
		this->mWait.SoftPoint = 0;
		isRet = true;
	}
	if (this->mWait.HardPoint)			//���������Ӳ����־
	{
		printf("\n\t��⵽Ӳ���ϵ�%p->%lu��д���ٴ�����\n",
			this->mWait.HardPoint->Address, this->mWait.HardPoint->OLD);
		SetHardPoint(this->mWait.HardPoint, this->mWait.HardPoint->TYPES, FALSE);
		this->mWait.HardPoint = 0;
		isRet = true;
	}
	if (this->mWait.MemyPoint)
	{
		if (!this->SetMemPoint(this->mWait.MemyPoint))
			this->Bug(6);
		this->mWait.MemyPoint = 0;
		isRet = true;
	}

	//�޸����öϵ���ж��Ƿ񷵻�
	if (isRet)
		return DBG_CONTINUE;

	// �ж��Ƿ�����ͨ����
	if (this->mWait.PassPoint)
	{
		isNULL = this->mWait.StepPoint;
		printf("\n\t��⵽����%s,\t", isNULL ? "����" : "����");
		this->mWait.PassPoint = FALSE;
		this->mWait.StepPoint = isNULL ? TRUE : FALSE;
		this->mWait.LineShow = isNULL ? "�����������������" : "�Ѿ��������£��������";
	}
	else if (tmp.TYPES == defBP_Ӳ��ִ��)
	{
		printf("\n\t��⵽Ӳ��ִ�жϵ�");
		this->SetHardPoint(&tmp, defBP_�ϵ���ͣ, TRUE);
		this->mWait.LineShow = "Ӳ��ִ�ж��£��������";
	}
	if (isASM)
		this->DisASM(Address, 5);
	if (isWait)
		this->OnLine();
	return DBG_CONTINUE;
}

//�����ڴ��쳣
DWORD CDebug::OnException_MemPoint(LPVOID Address, LPDEBUG_EVENT pDbg_event)
{
	bool isASM = true, isWait = true, isRet = false, isNULL = false;
	BreakPoint& tmp = this->mBreakPoint[Address];
	if (tmp.TYPES == defBP_�ڴ�����)	//�޴˶ϵ�
	{
		printf("\n\t��⵽�ڴ��쳣,%p", Address);
		//this->mWait.PassPoint = FALSE;
		//this->mWait.LineShow = "�Ѿ��������£��������";
	}
	else if (tmp.OLD)
	{
		DWORD dwtmp;
		printf("\n\t��⵽�ڴ�ϵ�");
		VirtualProtectEx(gDATA.PS.hProcess, Address, 1, tmp.OLD, &dwtmp);
		//this->mWait.MemyPoint = &tmp;
		//this->SetTFPoint(0);
	}
	else if (Address == (LPVOID)0x401072)
	{
		printf("\n\t��⵽SEH��������\n");
		return DBG_EXCEPTION_NOT_HANDLED;
	}
	else
		this->Bug(4);


	if (isASM)
		this->DisASM(Address, 5);
	if (isWait)
		this->OnLine();
	return DBG_CONTINUE;
}

DWORD CDebug::OnLine()
{
	char cmdline[200] = {};
	char cmd[100] = {};
	DWORD address;
	while (true)
	{
		if (this->mWait.LineShow)
			std::cout << this->mWait.LineShow;
		// ��ȡ����
		gets_s(cmdline, 200); // bp 0x3333333

		// ��������
		if (sscanf_s(cmdline, "%s", cmd, 100))
		{
			// �ж��Ƿ���g����
			if (strcmp(cmd, "e") == 0)
			{
				::TerminateProcess(gDATA.PS.hProcess, 0);
				break;
			}
			else if (strcmp(cmd, "g") == 0)
			{
				break; // ���г���
			}
			// �ж��Ƿ��Ƕϵ�����
			else if (strcmp(cmd, "bp") == 0)
			{
				sscanf_s(cmdline, "%s %x", cmd, 100, &address);
				while (true)
				{
					printf("ȷ��Ҫ������ϵ��� 0x%lX��y/n��", address);
					gets_s(cmdline, 200);
					if (strcmp(cmdline, "n") == 0)
						break;
					else if (strcmp(cmdline, "y") == 0)
					{
						// ��������ϵ�
						wsprintfA(cmd, "���öϵ�0x%lX %s���������", address,
							this->AddSoftPoint((LPVOID)address, defBP_���ִ��) ?
							"�ɹ�" : "ʧ��");
						this->mWait.LineShow = cmd;
						break;
					}
				}
				printf("\n");
			}
			// ���õ����ϵ�
			else if (strcmp(cmd, "t") == 0)
			{
				this->SetTFPoint();
				break;
			}
			else if (strcmp(cmd, "p") == 0)
			{
				std::cout << "\t����������һ���ϵ�";
				this->mWait.StepPoint = TRUE;
				// �߳�������
				CONTEXT context = { CONTEXT_FULL };
				// ��ȡ�߳�������
				GetThreadContext(gDATA.PS.hThread, &context);
				this->DisASM((LPVOID)context.Eip, 5);
				break;
			}
			else if (strcmp(cmd, "ba") == 0)
			{
				sscanf_s(cmdline, "%s %x", cmd, 100, &address);
				while (true)
				{
					printf("ȷ��Ҫ��Ӳ���ϵ��� 0x%lX��r/w/e/n��", address);
					gets_s(cmdline, 200);
					if (strcmp(cmdline, "n") == 0)
						break;
					else if (strcmp(cmdline, "e") == 0)
					{
						// ����Ӳ��ִ�жϵ�
						wsprintfA(cmd, "���öϵ�0x%lX %s���������", address,
							this->AddHardPoint((LPVOID)address, defBP_Ӳ��ִ��) ?
							"�ɹ�" : "ʧ��");
						this->mWait.LineShow = cmd;
						break;
					}
					else if (strcmp(cmdline, "w") == 0)
					{
						// ����Ӳ��д��ϵ�
						wsprintfA(cmd, "���öϵ�0x%lX %s���������", address,
							this->AddHardPoint((LPVOID)address, defBP_Ӳ��д��) ?
							"�ɹ�" : "ʧ��");
						this->mWait.LineShow = cmd;
						break;
					}
				}
				printf("\n");
			}
			else if (strcmp(cmd, "bm") == 0)
			{
				sscanf_s(cmdline, "%s %x", cmd, 100, &address);
				while (true)
				{
					printf("ȷ��Ҫ������ϵ��� 0x%lX��r/w/e/n��", address);
					gets_s(cmdline, 200);
					if (strcmp(cmdline, "n") == 0)
						break;
					else if (strcmp(cmdline, "e") == 0)
					{
						// �������ִ�жϵ�
						wsprintfA(cmd, "���öϵ�0x%lX %s���������", address,
							this->AddMemPoint((LPVOID)address, defBP_�ڴ�ִ��) ?
							"�ɹ�" : "ʧ��");
						this->mWait.LineShow = cmd;
						break;
					}
				}
				printf("\n");
			}
		}
		else {
			printf("�����������\n");
		}
	}
	this->mWait.LineShow = 0;
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
	if (this->mWait.StepPoint)			//���õ�������
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
				ReadLen,					/*��Ҫ������ָ������,�����0,�򷴻���ȫ��*/
				&pInsn						/*��������*/
			);
			DWORD_PTR add = pInsn[0].size + (DWORD_PTR)Address;
			this->AddSoftPoint((LPVOID)add, defBP_��������);
		}
		else {
			std::cout << "(TF)\n";
			this->SetTFPoint();
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

//BOOL CDebug::AddMemPoint(LPVOID Address, WORD Type, PWCHAR Text/* = 0*/)
//{
//	BOOL bRet = 0;
//	BreakPoint tmp = this->mBreakPoint[Address];
//	if (tmp.TYPES)
//	{
//		printf("�ϵ��Ѵ��ڣ�%p", Address);
//		return 0;
//	}
//	else if (Type == defBP_�ڴ�ִ��)
//	{
//		tmp = BreakPoint{ Type,1,0,0,Address };
//		if (Text)
//			tmp.str = Text;
//		bRet = VirtualProtectEx(gDATA.PS.hProcess, Address, 1, PAGE_READWRITE, &tmp.OLD);
//		this->mBreakPoint[Address] = tmp;
//		//DWORD_PTR MemFitst = (DWORD_PTR)Address / 1000;
//		//MemFitst *= 1000;
//		//this->mBreakPoint[(LPVOID)MemFitst] = { defBP_�ڴ�����,1,tmp.OLD,0,Address };
//	}
//	return bRet;
//}

DWORD CDebug::SetBreakPoint(LPVOID Address, WORD Type, BOOL isBreak)
{
	BreakPoint& tmp = this->mBreakPoint[Address];
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
	case defBP_�ϵ���ͣ: {		//��ͣ
		if (tmp.TYPES == defBP_���ִ��) {
			//��ԭ�ֽ�ʧ��
			if (!this->WriteMemory(Address, &tmp.OLD, 1))
				return 0;

			//��ȡ�߳�������
			CONTEXT context = { CONTEXT_FULL };
			GetThreadContext(gDATA.PS.hThread, &context);

			if (isBreak == FALSE)					//���öϵ��־Ϊ��ͣ
				tmp.TYPES = defBP_�ϵ���ͣ;
			//else if (isBreak == defBP_�ϵ�ɾ��)	//��������ʱɾ������ʱ�ϵ�
			//	this->mBreakPoint.erase(Address);
			else if (isBreak == TRUE) {			//�����Զϵ㣬��������������CC
				this->mWait.SoftPoint = &tmp;	//����ϵ��־
				((PEFLAGS)&context.EFlags)->TF = 1;
			}
			context.Eip -= 1;
			SetThreadContext(gDATA.PS.hThread, &context);
		}
	}break;
	default: break;
	}
	return 0;
}

DWORD CDebug::SetHardPoint(LPBreakPoint Point, WORD Type, BOOL isBreak)
{
	BOOL bRet = false;
	CONTEXT ct = { CONTEXT_DEBUG_REGISTERS };
	GetThreadContext(gDATA.PS.hThread, &ct);
	PDBG_REG7 pDr7 = (PDBG_REG7)&ct.Dr7;

	if (Point->TYPES == Type)	//�޸�Ӳ��
	{
		if (Point->OLD == 70)	//�Ĵ���1
		{
			pDr7->L0 = 1;		//�����ϵ�
		}
		else if (Point->OLD == 71)	//�Ĵ���2
		{
			pDr7->L1 = 1;		//�����ϵ�
		}
	}
	else
	{
		switch (Type)
		{
		case 0: this->Bug(3); return 0;
		case defBP_�ϵ���ͣ:
			if (Point->OLD == 70)	//�Ĵ���1
			{
				pDr7->L0 = 0;		//��ͣ�ϵ�
			}
			if (isBreak == TRUE)
			{
				this->mWait.HardPoint = Point;
				this->SetTFPoint(0);
			}
			break;
		default:
			break;
		}
	}
	return (DWORD)SetThreadContext(gDATA.PS.hThread, &ct);
}

BOOL CDebug::SetTFPoint(BOOL isSetFlag/* = TRUE*/)
{
	if(isSetFlag)
		this->mWait.PassPoint = TRUE;
	// �߳�������
	CONTEXT context = { CONTEXT_FULL };
	// ��ȡ�߳�������
	GetThreadContext(gDATA.PS.hThread, &context);
	((PEFLAGS)&context.EFlags)->TF = 1;
	// �����߳�������
	return SetThreadContext(gDATA.PS.hThread, &context);
}


