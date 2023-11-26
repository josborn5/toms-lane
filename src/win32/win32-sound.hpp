namespace tl
{

typedef struct Win32Sound Win32Sound;

int win32_sound_interface_initialize(
	Win32Sound& sound,
	HWND window
);

int win32_sound_interface_buffer_initialize(
	const Win32Sound& sound
	int gameUpdateHz,
	LARGE_INTEGER frameStartCounter,
	int targetMicroSecondsPerFrame,
	const Win32Time& timer,
	SoundBuffer& soundBuffer
);

int win32_sound_interface_buffer_process(
	const Win32Sound& sound,
	const SoundBuffer& soundBuffer
);

}