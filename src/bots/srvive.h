#include "..\bot.h"

/* srvive(x)
 *
 * A copy of my 2nd-ever function to maxout, SRVIVE.
 * A board will, in general, score worse if it is taller and has more holes than another.
 * Beyond this there aren't too many factors, the board-roughness factor is minimal and basically just there to break ties.
 */
double srvive(decision_t decision) {
	const double BOARD_PENALTY = 1;
	const double HEIGHT_PENALTY = 15;
	const double PERFORATION_PENALTY = 50;
	const double HIGH_HOLE_PENALTY = 20;

	double ans = 0;

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
		ans -= BOARD_PENALTY*(diff-1)*diff;
	}

	// Max-height penalty
	ans -= HEIGHT_PENALTY*max_height*max_height;

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
			ans -= PERFORATION_PENALTY*perforation*perforation;
			// High-hole penalty
			ans -= HIGH_HOLE_PENALTY*heights[x];
		}
	}

	return ans;
}
