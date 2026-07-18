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
		char buffer[256];
		const char* copy = log;
		unsigned int counter = 0;
		while (*copy && counter < 252) {
			buffer[counter] = *copy;
			
			counter += 1 ;
			copy += 1;
		}
		buffer[counter] = '{';
		buffer[counter] = '%';
		buffer[counter] = 'd';
		buffer[counter] = '}';
		buffer[counter] = '\n';
		printf(buffer, num);
	}
}
