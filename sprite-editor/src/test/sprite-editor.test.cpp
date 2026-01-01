#include "./commands.test.cpp"
#include "./editor.test.cpp"
#include "./ring-buffer.test.cpp"
#include <stdio.h>

int main()
{
	RunCommandTests();
	RunEditorTests();
	run_ring_buffer_tests();

	printf("All tests complete!!!\n");
}

