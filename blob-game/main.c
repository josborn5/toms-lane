

typedef struct pixel_bow {
	unsigned int shots;
	char type;
} pixel_bow; 

int main() {
	char grid[4][4]; // [x][y]

	pixel_bow* pixel_bow_slots[3];

	pixel_bow pixel_bow_queue[12];

	// fill the grid
	for (int i = 0; i < 4; i += 1) {
		grid[i][0] = 'a';
		grid[i][1] = 'b';
		grid[i][2] = 'c';
		grid[i][3] = 'd';
	}

	// fill the queue (0 is closest / head of the queue)
	pixel_bow_queue[0].shots = 4;
	pixel_bow_queue[0].type = 'a';

	pixel_bow_queue[1].shots = 4;
	pixel_bow_queue[1].type = 'b';

	pixel_bow_queue[2].shots = 4;
	pixel_bow_queue[2].type = 'c';

	pixel_bow_queue[3].shots = 4;
	pixel_bow_queue[3].type = 'd';

	return 0;
}
