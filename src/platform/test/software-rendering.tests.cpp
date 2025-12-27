#include "../software-rendering.hpp"
#include <assert.h>
#include <iostream>

#include "software-rendering.tests.common.cpp"
#include "software-rendering.tests.sprite.cpp"
#include "software-rendering.tests.draw-line.cpp"
#include "software-rendering.tests.color.cpp"
#include "draw-rect.tests.cpp"

void RunSoftwareRenderingTests()
{
	std::cout << "Running software rendering tests" << std::endl;

	/**
	 * SHORT, SHALLOW GRADIENT - LEFT TO RIGHT
	 *
	 *	    0   1   2   3
	 *	  |---|---|---|---|
	 *	0 |   |   |   | O |
	 *	  |---|---|---|---|
	 *	1 |   | x | x |   |
	 *	  |---|---|---|---|
	 *	2 | O |   |   |   |
	 *	  |---|---|---|---|
	 *	3 |   |   |   |   |
	 *	  |---|---|---|---|
	 */
	uint32_t el11[16] = {
		EMPTY,	EMPTY,	EMPTY,	FILLED,
		EMPTY,	FILLED,	FILLED,	EMPTY,
		FILLED,	EMPTY,	EMPTY,	EMPTY,
		EMPTY,	EMPTY,	EMPTY,	EMPTY
	};
	RunLineDrawTest(tl::Vec2<int>{ 0, 2 }, tl::Vec2<int>{ 3, 0 }, el11);

	RunLineDrawingTests();

	RunSpriteTests();

	RunColorTests();

	RunDrawRectTests();
}
