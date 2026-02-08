#ifndef TOMS_LANE_PLATFORM_ASSERT
#define TOMS_LANE_PLATFORM_ASSERT

#include <stdio.h>
#include <assert.h>
#include <stdint.h>

template<typename T>
static void assert_whole_number(T actual, T expected, const char* message) {
	if (actual != expected) {
		printf("\nFAIL - ");
		printf(message);
		printf(" - Actual: %d, Expected: %d", actual, expected);
		printf("\n");

		assert(false);
	}}

void assert_float(float actual, float expected, const char* message) {
	if (actual != expected) {
		printf("\nFAIL - ");
		printf(message);
		printf(" - Actual: %.4f, Expected: %.4f", actual, expected);
		printf("\n");

		assert(false);
	}}

void assert_int(int actual, int expected, const char* message) {
	assert_whole_number<int>(actual, expected, message);
}

void assert_int32_t(int32_t actual, int32_t expected, const char* message) {
	assert_whole_number<int32_t>(actual, expected, message);
}

void assert_uint32_t(uint32_t actual, uint32_t expected, const char* message) {
	assert_whole_number<uint32_t>(actual, expected, message);
}
void assert_uint16_t(uint16_t actual, uint16_t expected, const char* message) {
	assert_whole_number<uint16_t>(actual, expected, message);
}
void assert_uint8_t(uint8_t actual, uint8_t expected, const char* message) {
	assert_whole_number<uint8_t>(actual, expected, message);
}

#endif
