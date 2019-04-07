#include "../bot.h"

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
	return 0;
}
