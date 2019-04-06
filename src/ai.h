#ifndef AI_H
#define AI_H

#include <vector>
#include "ai_util.h"

// Original cost function
// Probably still in the old Lua files
int cost(const decision_t decision) {
	int ans = 0;
	int heights [10];

	// Board penalty
	for (int x = 0; x < 10; x++) {
		int mask = 1<<x;
		for (int y = 0; y <= 22; y++) {
			if (some_overlap(decision.grid[y], mask)) {
				heights[x] = y;
				break;
			}
		}
	}
	for (int x = 0; x < 9; x++) {
		int diff = abs(heights[x] - heights[x+1]);
		ans += diff*diff;
	}

	// Hole weight penalty
	for (int x = 0; x < 10; x++) {
		int mask = 1<<x;
		int has_hole = false;

		int y;
		for (y = 21; y >= heights[x]; y--) {
			if (no_overlap(decision.grid[y], mask)) {
				has_hole = true;
				break;
			}
		}
		for (; y >= heights[x]; y--) {
			if (some_overlap(decision.grid[y], mask)) {
				ans += 100;
			}
		}
		if (has_hole)
			ans += 400;
	}

	// Line clearing bonus
	ans -= decision.n_lines*decision.n_lines*10;

	return ans;
}

// I Should Be Studying Right Now
// First function to maxout (+next-piece planning)
int isbsrn(const decision_t decision) {
	int ans = 0;
	int heights [10];
	int max_height = 0;

	// Board penalty
	for (int x = 0; x < 10; x++) {
		int mask = 1<<x;
		for (int y = 0; y <= 22; y++) {
			if (some_overlap(decision.grid[y], mask)) {
				heights[x] = 22-y;
				if (heights[x] > max_height)
					max_height = heights[x];
				break;
			}
		}
	}
	for (int x = 0; x < 9; x++) {
		int diff = abs(heights[x] - heights[x+1]);
		ans += diff*diff;
	}
	if (max_height > 8) {
		ans += (max_height-8)*80;
	}

	// Perforation penalty
	for (int x = 0; x < 10; x++) {
		int perforation = 0;
		int mask = 1<<x;
		bool air = true;
		for (int y = 2; y < 23; y++) {
			if (some_overlap(decision.grid[y], mask)) {
				if (air)
					perforation++;
				air = false;
			}
			else {
				if (!air)
					perforation++;
				air = true;
			}
		}		
		if (perforation > 0) {
			ans += perforation*perforation*100;
			// High-hole penalty
			ans += heights[x]*20;
		}
	}

	// Line clearing bonus
	ans -= decision.n_lines*decision.n_lines*10;

	return ans;
}

// Get Tetris
int getris(const decision_t decision) {
	const int POS_HIT = 1;
	const int NEG_HIT = 5;
	const int PATTERN_BONUS = 100;
	const int PERFORATION_PENALTY = 100;
	const int WEIGHT_PENALTY = 100;
	const int BOARD_PENALTY = 1;
	const int HEIGHT_LIMIT = 10;
	const int HEIGHT_PENALTY = 200;
	const int TETRIS_BONUS = 1000000;

	int ans = 0;

	// Match pattern
	for (int y = 21; y > 17; y--) {
		int pattern_pos = 0b0111111111;
		int pattern_neg = 0b1000000000;
		int row_pos = decision.grid[y] & pattern_pos;
		int row_neg = ~decision.grid[y] & pattern_neg;
		int score = 0;
		for (int x = 0; x < 10; x++) {
			int col_mask = 1<<x;
			if (some_overlap(row_pos, col_mask)) {
				score += POS_HIT;
			}
			if (some_overlap(row_neg, col_mask)) {
				score += NEG_HIT;
			}
		}

		ans -= score*PATTERN_BONUS;
	}

	// Board
	int heights [10];
	int max_height = 0;
	for (int x = 0; x < 10; x++) {
		int col_mask = 1<<x;
		for (int y = 2; y < 23; y++) {
			if (some_overlap(decision.grid[y], col_mask)) {
				heights[x] = 22-y;
				if (max_height < heights[x])
					max_height = heights[x];
				break;
			}
		}
	}
	for (int x = 1; x < 10; x++) {
		int diff = heights[x] - heights[x-1];
		ans += diff*diff*BOARD_PENALTY;
	}

	// Max-height penalty
	if (max_height > HEIGHT_LIMIT) {
		int diff = max_height - HEIGHT_LIMIT;
		ans += diff*diff*HEIGHT_PENALTY;
	}

	// Perforation penalty
	for (int x = 0; x < 10; x++) {
		int col_mask = 1<<x;
		bool air = false;
		int perforation = 0;
		for (int y = 22; y > 1; y--) {
			if (some_overlap(decision.grid[y], col_mask)) {
				if (air) {
					air = false;
					perforation++;
				}
			}
			else {
				if (!air) {
					air = true;
					perforation++;
				}
			}
		}

		ans += perforation*perforation*PERFORATION_PENALTY;
		if (perforation != 0)
			ans += heights[x]*WEIGHT_PENALTY;
	}

	// Tetris bonus
	if (decision.n_lines == 4)
		ans -= TETRIS_BONUS;

	return ans;
}

// SENIOR VIVE
// Or more accurately, survive
// 2nd function to maxout!
int srvive(const decision_t decision) {
	int ans = 0;

	int heights [10];
	int max_height = 0;

	// Board penalty
	for (int x = 0; x < 10; x++) {
		int mask = 1<<x;
		for (int y = 0; y <= 22; y++) {
			if (some_overlap(decision.grid[y], mask)) {
				heights[x] = 22-y;
				if (heights[x] > max_height)
					max_height = heights[x];
				break;
			}
		}
	}
	for (int x = 0; x < 9; x++) {
		int diff = abs(heights[x] - heights[x+1]);
		ans += (diff-1)*diff;
	}

	// Literally try to keep the board as low as possible
	ans += max_height*max_height*15;

	// Perforation penalty
	for (int x = 0; x < 10; x++) {
		int perforation = 0;
		int mask = 1<<x;
		bool air = true;
		for (int y = 2; y < 23; y++) {
			if (some_overlap(decision.grid[y], mask)) {
				if (air)
					perforation++;
				air = false;
			}
			else {
				if (!air)
					perforation++;
				air = true;
			}
		}		
		if (perforation > 0) {
			ans += perforation*perforation*50;
			// High-hole penalty
			ans += heights[x]*20;
		}
	}

	return ans;
}

#endif
