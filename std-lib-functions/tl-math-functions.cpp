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
	void print_int(const char* log, int num) {
		char buffer[256] = {0};
		unsigned int counter = 0;
		while (*log && counter < 252) {
			buffer[counter] = *log;
			counter += 1 ;
			log += 1;
		}
		buffer[counter] = '%';
		buffer[counter + 1] = 'd';
		buffer[counter + 2] = '\n';
		buffer[counter + 3] = '\0';

		printf(buffer, num);
	}
}
