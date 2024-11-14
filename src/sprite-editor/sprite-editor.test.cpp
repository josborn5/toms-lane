#include "./commands.test.cpp"
#include "./editor.test.cpp"
#include <stdio.h>

int main()
{
	RunCommandTests();
	RunEditorTests();

	printf("All tests complete!!!\n");
}

