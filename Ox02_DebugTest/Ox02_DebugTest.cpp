// Ox02_DebugTest.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "CProInfo1.h"

void show(int isDbg)
{
    cout << (isDbg ? "【被调试】\n" : "[正常]\n");
}

void show(CProInfo1& cInfo)
{
    BOOL isDBG = -1;
    cout << "\n\t开始检查FS标志\n";

#pragma region FS标志
    // 使用FS:[18h]检查PEB块+2
    cout << "isFS[18h]+2\t";
    show(cInfo.IsBeginDebuged());

    // IsDebuggerPresent 就是在检查 PEB.BeingDebugged 字段
    cout << "IsDebuggerPresent=\t";
    show(IsDebuggerPresent());

    //参考https://xz.aliyun.com/t/5309
    cout << "NtGlobalFlag=\t";
    show(cInfo.NtGlobalFlag());
#pragma endregion

    cout << "\n\t开始检查NQIP标志\n";
#pragma region NtQueryInformationProcess
    //参考维基百科https://www.wanweibaike.com/wiki-%E8%BF%9B%E7%A8%8B%E7%8E%AF%E5%A2%83%E5%9D%97
    CheckRemoteDebuggerPresent(GetCurrentProcess(), &isDBG);
    cout << "CheckRemoteDebuggerPresent=\t";
    show(isDBG);


    //参考先知社区https://xz.aliyun.com/t/5339
    cout << "NQIP_DebugPort\t=\t";
    show(cInfo.NQIP_DebugPort());
    cout << "NQIP_DebugHandle=\t";
    show(cInfo.NQIP_ProcessDebugObjectHandle());
    cout << "NQIP_ParentPID\t=\t" << cInfo.NQIP_ParentPID() << endl;
#pragma endregion
}

int i = 9;

int main()
{
    std::cout << "Hello World!\n";
    CProInfo1 cInfo;
    show(cInfo);
    while (i)
    {
        i--;
        _asm xor eax, eax;
        _asm xor eax, eax;
        printf("%d\n", i);
        Sleep(333);
    }
    getchar();
}