#include "bot.h"
#include "bot_list.h"
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>
#include <limits>
#include <vector>
using namespace std;

int in_grid [24];
int in_piece;
int in_piece_next;

ofstream log_file;

/* place(piece, grid)
 *
 * Find all the possible decisions that can be made by placing piece onto grid.
 */
vector<decision_t> place(int piece, const int grid[]) {
	int drop_ys [19][10];
	vector<decision_t> ans;

	// Find locations for drops, use this information for other actions
	for (int i = 0; i < N_ROTATIONS[piece]; i++) {
		for (int x = piece_min_x[piece]; x <= piece_max_x[piece]; x++) {
			decision_t temp = decision_t(piece, x, drop(piece, x, 0, grid), no_action, grid);
			ans.push_back(temp);
			drop_ys[piece][x] = temp.y;
		}
		piece = RIGHT_ROTATION[piece];
	}

	// Tucks and spins
	for (int i = 0; i < N_ROTATIONS[piece]; i++) {
		int piece_l = LEFT_ROTATION[piece];
		int piece_r = RIGHT_ROTATION[piece];

		// Left tucks
		for (int x = piece_min_x[piece]+1; x <= piece_max_x[piece]; x++) {
			for (int y = drop_ys[piece][x]; y > drop_ys[piece][x-1]; y--) {
				if (!piece_overlaps(piece, x-1, y, grid)) {
					decision_t temp = decision_t(piece, x, y, tuck_left, grid);
					ans.push_back(temp);
				}
			}
		}
		// Right tucks
		for (int x = piece_min_x[piece]; x <= piece_max_x[piece]-1; x++) {
			for (int y = drop_ys[piece][x]; y > drop_ys[piece][x+1]; y--) {
				if (!piece_overlaps(piece, x+1, y, grid)) {
					decision_t temp = decision_t(piece, x, y, tuck_right, grid);
					ans.push_back(temp);
				}
			}
		}
		// Left spins
		for (int x = max(piece_min_x[piece], piece_min_x[piece_l]); x <= min(piece_max_x[piece], piece_max_x[piece_l]); x++) {
			for (int y = drop_ys[piece][x]; y > drop_ys[piece_l][x]; y--) {
				if (!piece_overlaps(piece_l, x, y, grid)) {
					decision_t temp = decision_t(piece, x, y, spin_left, grid);
					ans.push_back(temp);
				}
			}
		}
		// Right spins
		for (int x = max(piece_min_x[piece], piece_min_x[piece_r]); x <= min(piece_max_x[piece], piece_max_x[piece_r]); x++) {
			for (int y = drop_ys[piece][x]; y > drop_ys[piece_r][x]; y--) {
				if (!piece_overlaps(piece_r, x, y, grid)) {
					decision_t temp = decision_t(piece, x, y, spin_right, grid);
					ans.push_back(temp);
				}
			}
		}

		piece = piece_r;
	}

	return ans;
}

/* decide(curr_piece, next_piece, grid)
 *
 * Make a decision based on an evaluation function.
 * Evaluation functions receive a decision_t and output a double, higher means more likely to choose.
 */
decision_t decide(int curr_piece, int next_piece, const int grid[], double (*function)(decision_t)) {
	decision_t ans;
	double score = -numeric_limits<double>::infinity();

	int n_branches = 0;

	for (decision_t first : place(curr_piece, grid)) {
		for (decision_t second : place(next_piece, first.grid)) {
			second.chain_to(first);
			double temp = function(second);
			if (temp > score) {
				ans = second;
				score = temp;
			}

			n_branches++;
		}
	}

	return ans;
}

/* get_bot(name)
 *
 * Search bot_list for the specified bot.
 * Will return the first bot in bot_list if none is found.
 */
bot_t get_bot(string name) {
	for (bot_t bot : bot_list) {
		if (bot.name == name) {
			return bot;
		}
	}

	return bot_list[0];
}

int main(int argc, char** argv) {
	ios_base::sync_with_stdio(false);
	cin.tie(NULL);

	log_file.open("log\\" + to_string(time(NULL)) + ".log");

	// Get bot from args
	string bot_name = string(argv[1]);
	bot_t bot = get_bot(bot_name);
	log_file << "BOT NAME: " << bot.name << '\n';
	log_file.flush();

	if (bot_name != bot.name) {
		cout << "Could not find " << bot_name << ", using " << bot.name << " instead.\n";
	}
	else {
		cout << "Using " << bot.name << ".\n";
	}

	init();
	cout.flush();

	while (true) {
		cin >> in_piece;
		if (in_piece > 19)
			break;
		cin >> in_piece_next;
		for (int i = 2; i < 22; i++)
			cin >> in_grid[i];
		fill_n(&in_grid[0], 2, EMPTY_ROW);
		fill_n(&in_grid[22], 2, FULL_ROW);

		decision_t decision = decide(in_piece, in_piece_next, in_grid, bot.function);

		cout <<
			decision.piece << ' ' <<
			decision.x << ' ' <<
			decision.y << ' ' <<
			decision.action;
		cout.flush();
		log_file <<
			decision.piece << ' ' <<
			decision.x << ' ' <<
			decision.y << ' ' <<
			decision.action << '\n';
		log_file.flush();
	}

	log_file.close();
}
