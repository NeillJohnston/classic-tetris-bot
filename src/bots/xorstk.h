#include "../bot.h"
#include <cmath>

// This file contains multiple bots.

/* xorstk
 *
 * XOR-Stack. A bot that does WAY too well for how simple it is.
 * With good RNG it's managed to score just over 300 lines with just under 400K points.
 */
double xorstk(decision_t decision) {
	double ans = decision.n_lines*pow(2,10);

	double multiplier = 1;
	for (int y = 22; y > 1; y--) {
		ans += multiplier*(~decision.grid[y] ^ decision.grid[y-1]);
		multiplier /= 8;
	}

	return ans;
}

/* atomic
 *
 * Precursor to xorstk, performs a little worse but may be more intuitive.
 */
double atomic(decision_t decision) {
	double ans = decision.n_lines*pow(2,10);

	double multiplier = 1;
	for (int y = 22; y > 1; y--) {
		ans += multiplier*(decision.grid[y]);
		multiplier /= 8;
	}

	return ans;
}
