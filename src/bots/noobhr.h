#include "../bot.h"
#include <algorithm>
#include <cstdlib>
#include <cmath>

/* n_set_bits(row)
 *
 * Return the number of bits (for the first 10) that are set in row.
 */
int n_set_bits(int row) {
	int ans = 0;
	for (int mask = 1<<9; mask > 0; mask>>=1) {
		ans += row & mask;
	}
	return ans;
}

/* match_pattern(row, pos, neg)
 *
 * Score how well a row matches a pattern.
 * The pattern is made of 2 masks: pos and neg.
 * The set bits in pos represent bits in row that need to be set,
 * and conversely for the set bits in neg.
 *
 * If pos contains P set bits and neg contains N set bits,
 * and row matches p bits in pos and n in neg,
 * then score = 0.1p/P + 0.9n/N.
 */
double match_pattern(int row, int pos, int neg) {
	return
		0.1*((double) n_set_bits(row & pos) / n_set_bits(pos)) +
		0.9*((double) n_set_bits(~row & neg) / n_set_bits(neg));
}

/* gen_heights(heights, grid)
 *
 * Store the heights of the grid into heights.
 * Returns the maximum height.
 */
int gen_heights(int heights[], const int grid[]) {
	int ans = 0;
	for (int x = 0; x < 10; x++) {
		int col_mask = 1<<x;
		for (int y = 2; y <= 22; y++) {
			if (grid[y] & col_mask) {
				heights[x] = 22-y;
				if (heights[x] > ans)
					ans = heights[x];
			}
		}
	}
	return ans;
}

/* noobhr(decision)
 *
 * In honor of Jonas Neubauer.
 * Aim for high-efficiency play, sloping left field.
 */
double noobhr(decision_t decision) {
	double ans = 0;

	const double PERFORATION_PENALTY = 200;
	const double ROUGHNESS_PENALTY = 10;
	const double STACK_PENALTY = 5;
	const double TETRIS_BONUS = 10000;

	int heights [10];
	int perforation [10];
	int stack [10];
	for (int x = 0; x < 10; x++) {
		int col_mask = 1<<x;
		bool air = false;
		perforation[x] = 0;
		stack[x] = 0;
		for (int y = 22; y > 1; y--) {
			if (decision.grid[y] & col_mask) {
				heights[x] = 22 - y;
				if (air) {
					air = false;
					perforation[x]++;
				}
			}
			else {
				if (!air) {
					air = true;
					perforation[x]++;
				}
				if (stack[x] == 0) {
					stack[x] = 22 - y - 1;
				}
			}
		}
	}

	for (int x = 0; x < 10; x++) {
		ans -= PERFORATION_PENALTY*perforation[x];
	}

	for (int x = 0; x < 9; x++) {
		ans -= STACK_PENALTY*pow(6 - stack[x], 2);
	}
	ans -= STACK_PENALTY*heights[9];

	for (int x = 1; x < 9; x++) {
		int diff = heights[x] - heights[x-1];
		ans -= ROUGHNESS_PENALTY*pow(diff+1, 2);
	}

	if (decision.n_tetrises > 0)
		ans += TETRIS_BONUS;

	return ans;
}
