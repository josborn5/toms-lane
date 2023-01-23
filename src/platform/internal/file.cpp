#include <fstream>
#include <strstream>
#include <string>
#include <iostream>
#include <vector>
#include "./file.hpp"
#include "./data-structures.hpp"

namespace tl
{
	template<typename T>
	bool ReadObjFileToArray4(std::string const &filename, HeapArray<Triangle4d<T>> &triangles)
	{
		std::ifstream objFile;
		objFile.open(filename, std::ios::in);
		if (!objFile.is_open())
		{
			return false;
		}

		std::vector<tl::Vec4<T>> vertices;

		while (!objFile.eof())
		{
			char junk;
			std::string line;
			std::getline(objFile, line);

			std::strstream stringStream;
			stringStream << line;

			if (line[0] == 'v')
			{
				tl::Vec4<T> vertex;
				// expect line to have syntax 'v x y z' where x, y & z are the ordinals of the point position
				stringStream >> junk >> vertex.x >> vertex.y >> vertex.z;
				vertex.w = (T)1.0;
				vertices.push_back(vertex);
			}

			if (line[0] == 'f')
			{
				int points[3];
				stringStream >> junk >> points[0] >> points[1] >> points[2];
				// expect line to have syntax 'f 1 2 3' where 1, 2 & 3 are the 1-indexed positions of the points in the file
				tl::Triangle4d<T> newTriangle = { vertices[points[0] - 1], vertices[points[1] - 1], vertices[points[2] - 1] };
				
				int nextTriangleIndex = triangles.length + 1;
				if (nextTriangleIndex < triangles.capacity)
				{
					triangles.content[nextTriangleIndex] = newTriangle;
					triangles.length = nextTriangleIndex;
				}
			}
		}

		objFile.close();

		return true;
	}
	template bool ReadObjFileToArray4(std::string const &filename, HeapArray<Triangle4d<float>> &triangles);
}
