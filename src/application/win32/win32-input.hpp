#ifndef APPLICATION_WIN32_INPUT_HPP
#define APPLICATION_WIN32_INPUT_HPP

namespace tl
{
	bool win32_input_interface_process_message(const MSG& message, Input& input);
	void win32_input_interface_reset(Input& input);
}

#endif
