#include <iostream>
#include <Windows.h>
int main()
{
    std::cout << "Hello World!\n";
	for (int i = 0; i < 9; i++)
	{
		std::cout << i << "\n";
	}
	MessageBoxA(0, "ABCDE", 0, 0);
}
