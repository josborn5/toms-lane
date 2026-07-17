#include <stdio.h>
#include <math.h>

namespace tl {
	float square_root(float input) {
		return sqrtf(input);
	}
	float sine(float input) {
		return sinf(input);
	}
	float cosine(float input) {
		return cosf(input);
	}


	void put_string(const char* log) {
		puts(log);
	}
}
