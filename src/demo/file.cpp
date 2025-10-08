#include <strstream>
#include <string>
#include <stdio.h>
#include "../tl-library.hpp"

struct Triangle4d
{
	tl::Vec4<float> p[3];
	unsigned int color;
};



bool ReadObjFileToArray4(const char* filename, tl::array<Triangle4d>& triangles, tl::MemorySpace& transient)
{
	std::FILE* file_pointer = fopen(filename, "r");

	if (file_pointer == nullptr) {
		return false;
	}

	const unsigned int string_buffer_size = 256;
	char string_buffer[string_buffer_size];

	tl::array<tl::Vec4<float>> heapVertices = tl::array<tl::Vec4<float>>(transient);
	while (fgets(string_buffer, string_buffer_size, file_pointer)) {

		std::string line = std::string(string_buffer);

		char junk;

		std::strstream stringStream;
		stringStream << line;

		if (line[0] == 'v' && line[1] == ' ')
		{
			tl::Vec4<float> vertex;
			// expect line to have syntax 'v x y z' where x, y & z are the ordinals of the point position
			stringStream >> junk >> vertex.x >> vertex.y >> vertex.z;
			vertex.w = 1.0f;
			heapVertices.append(vertex);
		}

		if (line[0] == 'f' && line[1] == ' ')
		{
			int points[3];
			stringStream >> junk >> points[0] >> points[1] >> points[2];
			// expect line to have syntax 'f 1 2 3' where 1, 2 & 3 are the 1-indexed positions of the points in the file
			Triangle4d newTriangle = {
				heapVertices.get(points[0] - 1),
				heapVertices.get(points[1] - 1),
				heapVertices.get(points[2] - 1)
			};

			triangles.append(newTriangle);
		}
	}

	fclose(file_pointer);

	return true;
}
