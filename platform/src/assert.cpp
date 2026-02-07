#ifndef TOMS_LANE_PLATFORM_ASSERT
#define TOMS_LANE_PLATFORM_ASSERT

#include <stdio.h>
#include <assert.h>
#include <stdint.h>

void assert_float(float actual, float expected, const char* message) {
	if (actual == expected) {
		printf("\nPass: ");
		printf(message);
		printf("\n");
	} else {
		printf("\n FAIL: ");
		printf(message);
		printf("Actual: %.4f, Expected: %.4f", actual, expected);
		printf("\n");

		assert(false);
	}
}

void assert_int(int actual, int expected, const char* message) {
	if (actual == expected) {
		printf("Pass: ");
		printf(message);
		printf("\n");
	} else {
		printf("\n FAIL: ");
		printf(message);
		printf("Actual: %d, Expected: %d", actual, expected);
		printf("\n");

		assert(false);
	}
}

void assert_uint32_t(uint32_t actual, uint32_t expected, const char* message) {
	if (actual == expected) {
		printf("Pass: ");
		printf(message);
		printf("\n");
	} else {
		printf("\n FAIL: ");
		printf(message);
		printf("Actual: %d, Expected: %d", actual, expected);
		printf("\n");

		assert(false);
	}
}

#endif
