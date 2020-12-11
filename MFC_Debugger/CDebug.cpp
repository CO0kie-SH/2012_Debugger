#include "pch.h"
#include "CDebug.h"

BOOL CDebug::InitDebug(PWCHAR Path)
{
	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION ps = {};
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
			//dbg_status = OnExceptionHandler(&dbg_event);
			break;
		case CREATE_THREAD_DEBUG_EVENT: // �̴߳����¼�
			printf("�̴߳����¼�����\n");
			break;
		case CREATE_PROCESS_DEBUG_EVENT: {// ���̴����¼�
			LPVOID g_Oep = dbg_event.u.CreateProcessInfo.lpStartAddress; // �������
			printf("���̴����¼�����,OEP=%p\n", g_Oep);
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

	// �ر��߳̾�����߳̾��
	CloseHandle(ps.hProcess);
	CloseHandle(ps.hThread);
	return 0;
}
