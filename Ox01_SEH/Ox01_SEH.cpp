// Ox01_SEH.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <windows.h>

// 带有异常处理函数的函数
void test1()
{
	// 在 VS 的同一个函数中无论编写了多少个 SEH， 编译
	// 器实际上只会安装一个叫做 except_handler4 的函数
	__try
	{
		printf("__try1 { ... }\n");
		__try
		{
			printf("__try2 { ... }\n");
		}
		__except (1)
		{
			printf("__except (1) { ... }\n");
		}
	}
	__except (1)
	{
		printf("__except (1) { ... }\n");
	}
}

// 没有异常处理函数的函数
void test2() { }

// 遍历当前程序中已经存在的异常处理函数
void ShowSEH()
{
	// 定义一个结构体指针，用于保存 SEH 链表的头节点
	EXCEPTION_REGISTRATION_RECORD* header = nullptr;

	// 通过 FS:[0] 找到 ExceptionList 的头节点
	__asm push fs : [0]
		__asm pop header

	// 遍历异常处理链表，链表以 -1 结尾
	while (header && header != (EXCEPTION_REGISTRATION_RECORD*)-1)
	{
		printf("function: %08X\n", (DWORD)header->Handler);
		header = header->Next;
	}

	printf("\n");
}

EXCEPTION_DISPOSITION NTAPI ExceptionRoutine(
	// 产生的异常信息
	_Inout_ struct _EXCEPTION_RECORD* ExceptionRecord,
	_In_ PVOID EstablisherFrame,
	// 产生异常时的线程上下文
	_Inout_ struct _CONTEXT* ContextRecord,
	_In_ PVOID DispatcherContext
)
{
	printf("自定义SEH: ExceptionCode: %X\n", ExceptionRecord->ExceptionCode);

	// 如果当前产生的异常是除零异常，那么就通过修改寄存器处理异常
	if (EXCEPTION_INT_DIVIDE_BY_ZERO == ExceptionRecord->ExceptionCode)
	{
		// 通过查看汇编代码可以知道产生异常的指令是 idiv eax, ecx
		// 在这个位置对寄存器执行的所有修改都会直接被应用到程序中
		ContextRecord->Eax = 30;
		ContextRecord->Edx = 0;
		ContextRecord->Ecx = 1;

		// 异常如果被处理了，那么就返回重新执行当前的代码
		return ExceptionContinueExecution;
	}

	// 如果不是自己能够处理的异常，就不处理只报告
	return ExceptionContinueSearch;

}

void* getTIB2() {
	void* header;
	__asm {
		__asm push fs : [0] ;
		__asm pop header;
	}
	return header;
}
void* getTIB() {
#ifdef _M_IX86
	return (void*)__readfsdword(0x18);
#elif _M_AMD64
	return (void*)__readgsqword(0x30);
#endif
}

int main()
{
	DWORD PID = GetCurrentProcessId(), pid1, pid2;
	__asm {
		push fs : [0x20] ;
		pop pid1;
		push fs : [0x6F4] ;
		pop pid1;
	}
	printf("%d,%d,%d\n", PID, pid1, pid2);


	test1();
	test2();

	// 遍历到了异常处理函数
	ShowSEH();
	printf("*TIB=%p,%p\n", getTIB(), getTIB2());

	// 手动的安装一个异常处理函数，操作 FS:[0]
	__asm {
		push ExceptionRoutine;	//异常回调函数_exception_handler的地址，即handler
		push fs : [0] ;				//保存前一个异常回调函数的地址，即prev
		mov fs : [0] , esp;			//安装新的EXCEPTION_REGISTRATION结构（两个成员:prev,handler）。
									//此时栈顶分别是prev和handler，为新的EXCEPTION_REGISTRATION结
									//构，mov fs:[0],esp，就可以让fs:[0]指向该指构。
	}
	printf("*TIB=%p,%p,%p\n", getTIB(), getTIB2(), ExceptionRoutine);
	int number = 0;
	number /= 0;

	// 遍历到了异常处理函数
	ShowSEH();

	// 卸载一个异常处理函数
	__asm {
		mov eax, fs: [0] ;	// 获取到了安装完之后的节点  
		mov eax, [eax];		// 上一个 SEH 节点，修改前的
		mov fs : [0] , eax;	// 修改前的重新设置为 SEH 头节点
		add esp, 8;
	}

	// 遍历到了异常处理函数
	ShowSEH();

	return 0;
}


/*
  PEXCEPTION_REGISTRATION_RECORD ExceptionList = nullptr;
  __asm push fs:[0]
  __asm pop ExceptionList


  __asm push ExceptionRoutine
  __asm push fs : [0]
  __asm mov fs : [0], esp


  __asm mov eax, ExceptionList
  __asm mov fs:[0], eax
  __asm add esp, 0x08
*/