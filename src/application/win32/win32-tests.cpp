#include "./win32-file.tests.cpp"
#include "./win32-input.tests.cpp"

int main()
{
	RunFileTests();

	RunInputTests();

	printf("\n===== SUCCESS! =====\n");
}
