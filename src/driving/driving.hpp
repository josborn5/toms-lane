struct Tile
{
	tl::Rect<float> rect;
	uint32_t color;
};

struct State
{
	int tileCount = 3;
	Tile tiles[3];
	tl::Camera<float> camera;
};