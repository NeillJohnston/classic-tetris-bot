#ifndef BOT_H
#define BOT_H

#include <algorithm>
#include <string>

const int EMPTY_ROW = 0b0000000000;
const int FULL_ROW = 0b1111111111;

const int N_ROTATIONS [19] = {
	4, 4, 4, 4,
	4, 4, 4, 4,
	2, 2,
	1,
	2, 2,
	4, 4, 4, 4,
	2, 2
};

const int LEFT_ROTATION [19] = {
	3, 0, 1, 2,
	7, 4, 5, 6,
	9, 8,
	10,
	12, 11,
	16, 13, 14, 15,
	18, 17
};

const int RIGHT_ROTATION [19] = {
	1, 2, 3, 0,
	5, 6, 7, 4,
	9, 8,
	10,
	12, 11,
	14, 15, 16, 13,
	18, 17
};

int piecemasks [19][10][5];
int piece_min_x [19];
int piece_max_x [19];

/* 5x5 grids indexable by piece id, used to generate piecemasks.
 * The masks may look backward on both axes - this->is to ensure that:
 * 1. The lowest bit corresponds to the lowest x-coordinate
 * 2. The highest index corresponds to the highest y-coordinate
 */
int piecemasks_at_x_2 [19][5] = {
	{0b00000, 0b00100, 0b01110, 0b00000, 0b00000},
	{0b00000, 0b00100, 0b01100, 0b00100, 0b00000},
	{0b00000, 0b00000, 0b01110, 0b00100, 0b00000},
	{0b00000, 0b00100, 0b00110, 0b00100, 0b00000},
	{0b00000, 0b00100, 0b00100, 0b00110, 0b00000},
	{0b00000, 0b00010, 0b01110, 0b00000, 0b00000},
	{0b00000, 0b01100, 0b00100, 0b00100, 0b00000},
	{0b00000, 0b00000, 0b01110, 0b01000, 0b00000},
	{0b00000, 0b00000, 0b00110, 0b01100, 0b00000},
	{0b00000, 0b01000, 0b01100, 0b00100, 0b00000},
	{0b00000, 0b00000, 0b00110, 0b00110, 0b00000},
	{0b00000, 0b00000, 0b01100, 0b00110, 0b00000},
	{0b00000, 0b00100, 0b01100, 0b01000, 0b00000},
	{0b00000, 0b00100, 0b00100, 0b01100, 0b00000},
	{0b00000, 0b00000, 0b01110, 0b00010, 0b00000},
	{0b00000, 0b00110, 0b00100, 0b00100, 0b00000},
	{0b00000, 0b01000, 0b01110, 0b00000, 0b00000},
	{0b00100, 0b00100, 0b00100, 0b00100, 0b00000},
	{0b00000, 0b00000, 0b01111, 0b00000, 0b00000}
};

/* init()
 *
 * Generate piecemasks and max/min x coordinates.
 * Should be called once before the program runs.
 */
void init() {
	for (int piece = 0; piece < 19; piece++) {
		bool flag = false;
		for (int x = 2; x >= 0 && !flag; x--) {
			for (int y = 0; y < 5; y++) {
				int p2 = piecemasks_at_x_2[piece][y];
				int px = p2 >> (2-x);
				if ((px << (2-x)) != p2) {
					piece_min_x[piece] = x+1;
					flag = true;
					break;
				}
				piecemasks[piece][x][y] = px;
			}
		}
		flag = false;
		for (int x = 2; x <= 10 && !flag; x++) {
			for (int y = 0; y < 5; y++) {
				int p2 = piecemasks_at_x_2[piece][y];
				int px = p2 << (x-2);
				if ((px & 0b10000000000) != 0) {
					piece_max_x[piece] = x-1;
					flag = true;
					break;
				}
				piecemasks[piece][x][y] = px;
			}
		}
	}
}

/* all_overlap(data, mask)
 *
 * Return whether every set bit in mask corresponds to a set bit in data.
 */
inline bool all_overlap(int data, int mask) {
	return (data & mask) == mask;
}

/* some_overlap(data, mask)
 *
 * Return whether any of the set bits in mask correspond to a set bit in data.
 */
inline bool some_overlap(int data, int mask) {
	return (data & mask) != 0;
}

/* no_overlap(data, mask)
 *
 * Return whether none of the set bits in mask correspond to a set bit in data.
 */
inline bool no_overlap(int data, int mask) {
	return (data & mask) == 0;
}

/* piece_overlaps(piece, x, y, grid)
 *
 * Return whether placing piece at (x,y) will collide with grid.
 */
bool piece_overlaps(int piece, int x, int y, const int grid[]) {
	for (int i = 0; i < 5; i++) {
		int piecemask = piecemasks[piece][x][i];
		if (some_overlap(grid[y+i], piecemask))
			return true;
	}
	return false;
}

/* overlay(piece, x, y, dest, src)
 *
 * Copy src into dest, placing piece at (x,y).
 */
void overlay(int piece, int x, int y, int dest[], const int src[]) {
	std::copy(&src[0], &src[24], &dest[0]);

	for (int i = 0; i < 5; i++) {
		dest[y+i] = src[y+i] | piecemasks[piece][x][i];
	}
}

/* clear(grid)
 *
 * Clear full lines from grid, return the number of lines cleared.
 */
int clear(int grid[]) {
	int n_lines = 0;
	for (int y = 21; y > 1; y--) {
		if (grid[y] == FULL_ROW) {
			n_lines++;
			grid[y] = EMPTY_ROW;
		}
	}
	int offset = 0;
	for (int y = 21; y > 1 && offset <= n_lines; y--) {
		if (grid[y] == EMPTY_ROW) {
			grid[y] = grid[y-offset];
			grid[y-offset] = EMPTY_ROW;
		}
		if (grid[y] == EMPTY_ROW) {
			offset++;
			y++;
		}
	}

	return n_lines;
}

/* drop(piece, x, grid)
 *
 * Get the y-value of a piece that drops due to gravity alone.
 */
int drop(int piece, int x, int y, int const grid[]) {
	for (; y <= 20; y++) {
		if (piece_overlaps(piece, x, y, grid)) {
			return y-1;
		}
	}

	return y;
}

/* action_t
 *
 * Used to pass information to the emulator script about what to do when the piece reaches a certain y-value.
 */
enum action_t {
	no_action = 0,
	tuck_left,
	tuck_right,
	spin_left,
	spin_right
};

/* decision_t
 *
 * Used to identify decisions and pass information to the emulator script.
 *
 * Grid represents the consequence of making this->decision.
 * It has 2 rows of padding on both sides to make piece checking and such easier.
 * The top 2 padding rows (grid[0] and grid[1]) should be all 0s,
 * the bottom 2 padding rows (grid[22] and grid[23]) should be all 1s.
 */
struct decision_t {
	// Information needed to make the decision
	int piece;
	int x;
	int y;
	action_t action;
	// Consequence of the decision
	int grid [24];
	int n_lines;
	int n_singles;
	int n_doubles;
	int n_triples;
	int n_tetrises;

	decision_t() {
	}

	decision_t(int piece, int x, int y, action_t action, const int grid[]) {
		this->piece = piece;
		this->x = x;
		this->y = y;
		this->action = action;
		if (action == no_action)
			overlay(piece, x, y, this->grid, grid);
		else if (action == tuck_left)
			overlay(piece, x-1, drop(piece, x-1, y, grid), this->grid, grid);
		else if (action == tuck_right)
			overlay(piece, x+1, drop(piece, x+1, y, grid), this->grid, grid);
		else if (action == spin_left)
			overlay(LEFT_ROTATION[piece], x, drop(LEFT_ROTATION[piece], x, y, grid), this->grid, grid);
		else if (action == spin_right)
			overlay(RIGHT_ROTATION[piece], x, drop(RIGHT_ROTATION[piece], x, y, grid), this->grid, grid);

		this->n_lines = clear(this->grid);

		this->n_singles = 0;
		this->n_doubles = 0;
		this->n_triples = 0;
		this->n_tetrises = 0;

		if (this->n_lines == 1)
			this->n_singles++;
		else if (this->n_lines == 2)
			this->n_doubles++;
		else if (this->n_lines == 3)
			this->n_triples++;
		else if (this->n_lines == 4)
			this->n_tetrises++;
	}

	void chain_to(const decision_t& parent) {
		this->piece = parent.piece;
		this->x = parent.x;
		this->y = parent.y;
		this->action = parent.action;

		this->n_lines += parent.n_lines;
		this->n_singles += parent.n_singles;
		this->n_doubles += parent.n_doubles;
		this->n_triples += parent.n_triples;
		this->n_tetrises += parent.n_tetrises;
	}
};

/* bot
 *
 * Holds information for the roster in bot_list.h.
 */
struct bot_t {
	std::string name;
	double (*function)(decision_t);
};

#endif
