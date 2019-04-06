#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <ctime>
#include "ai.h"
using namespace std;

const int LIKE_TWO_BILLION = ~(1<<31);
// cost, isbsrn, getris, srvive
int (*F)(decision_t) = getris;

int in_grid [24];
int in_piece;
int in_piece_next;

ofstream log;

/* find
Finds relevant ways of placing piece on grid,
including drops, tucks, and spins.
*/
vector<decision_t> find(int piece, const int grid []) {
	int drop_ys [19][10];
	vector<decision_t> ans;

	// Find locations for drops, use this information for other actions
	for (int i = 0; i < n_rotations[piece]; i++) {
		int piece_l = left_rotation[piece];
		int piece_r = right_rotation[piece];

		for (int x = piece_min_x[piece]; x <= piece_max_x[piece]; x++) {
			decision_t temp = drop(piece, x, grid);
			ans.push_back(temp);
			drop_ys[piece][x] = temp.y;
		}
		piece = piece_r;
	}

	// Tucks and spins
	for (int i = 0; i < n_rotations[piece]; i++) {
		int piece_l = left_rotation[piece];
		int piece_r = right_rotation[piece];

		// Left tucks
		for (int x = piece_min_x[piece]+1; x <= piece_max_x[piece]; x++) {
			for (int y = drop_ys[piece][x]; y >= drop_ys[piece][x-1]; y--) {
				if (!piece_overlaps(piece, x-1, y, grid)) {
					decision_t temp = {piece, x, y, tuck_left};
					overlay(piece, x-1, y, temp.grid, grid);
					ans.push_back(temp);
				}
			}
		}
		// Right tucks
		for (int x = piece_min_x[piece]; x <= piece_max_x[piece]-1; x++) {
			for (int y = drop_ys[piece][x]; y >= drop_ys[piece][x+1]; y--) {
				if (!piece_overlaps(piece, x+1, y, grid)) {
					decision_t temp = {piece, x, y, tuck_right};
					overlay(piece, x+1, y, temp.grid, grid);
					ans.push_back(temp);
				}
			}
		}
		// Left spins
		for (int x = max(piece_min_x[piece], piece_min_x[piece_l]); x <= min(piece_max_x[piece], piece_max_x[piece_l]); x++) {
			for (int y = drop_ys[piece][x]; y > drop_ys[piece_l][x]; y--) {
				if (!piece_overlaps(piece_l, x, y, grid)) {
					decision_t temp = {piece, x, y, spin_left};
					overlay(piece_l, x, y, temp.grid, grid);
					ans.push_back(temp);
				}
			}
		}
		// Right spins
		for (int x = max(piece_min_x[piece], piece_min_x[piece_r]); x <= min(piece_max_x[piece], piece_max_x[piece_r]); x++) {
			for (int y = drop_ys[piece][x]; y > drop_ys[piece_r][x]; y--) {
				if (!piece_overlaps(piece_r, x, y, grid)) {
					decision_t temp = {piece, x, y, spin_right};
					overlay(piece_r, x, y, temp.grid, grid);
					ans.push_back(temp);
				}
			}
		}

		piece = piece_r;
	}

	return ans;
}

decision_t best(vector<decision_t> decisions, int piece_next, int (*f)(decision_t)) {
	decision_t ans = decisions[0];
	int min_cost = LIKE_TWO_BILLION;

	log << "evaluating ";
	int n_branches = 0;

	for (auto it = decisions.begin(); it != decisions.end(); it++) {
		decision_t decision = *it;
		vector<decision_t> predictions = find(piece_next, decision.grid);

		for (auto it_ = predictions.begin(); it_ != predictions.end(); it_++) {
			int temp = f(*it_);
			if (temp < min_cost) {
				min_cost = temp;
				ans = decision;
			}
		}
		
		int temp = f(decision);
		if (temp < min_cost) {
			min_cost = temp;
			ans = decision;
		}

		n_branches += predictions.size();
	}

	log << n_branches << " branches, with best cost " << min_cost << " ";
	log.flush();

	return ans;
}

int main() {
	ios_base::sync_with_stdio(false);
	cin.tie(NULL);

	// Open log file
	log.open("log");

	init_piecemasks();

	while (true) {
		cin >> in_piece;
		if (in_piece > 19)
			break;
		cin >> in_piece_next;
		for (int i = 2; i < 22; i++)
			cin >> in_grid[i];
		fill_n(&in_grid[0], 2, EMPTY_ROW);
		fill_n(&in_grid[22], 2, FULL_ROW);

		clock_t start_time = clock();
		decision_t decision = best(find(in_piece, in_grid), in_piece_next, F);
		clock_t stop_time = clock();

		cout <<
			decision.piece << ' ' <<
			decision.x << ' ' <<
			decision.y << ' ' <<
			decision.action;
		cout.flush();

		log << "made decision (" <<
			decision.piece << " " <<
			decision.x << " " <<
			decision.y << " " <<
			decision.action << ") in " <<
			((1000*(stop_time - start_time)) / CLOCKS_PER_SEC) << "ms" << '\n';
		log.flush();
	}

	log.close();
}
