#include <ncurses.h>
#include <utility>
#include <random>
#include <vector>
#include <locale.h>

#define LOSE_PAUSE 1000
#define WIN_PAUSE 2000
#define DRAW_PAUSE 100
#define CONTRIBUTION_RATE 0.02

#define SPEED_UP 4

#define GRID 1
#define HIT  2
#define MISS 3
#define SUPERBALL_HIT 4
#define SUPERBALL_MISS 5
#define PICK 6

std::random_device rd;  // Obtain a random number from hardware
std::mt19937 g(rd());   // Seed the generator

std::vector<int> picks, draws, balls;

std::pair<int,int> get_pos(int n) {
	int row, col;
	col = (n%10)*3;
	row = (n/10);
	return {row,col};
}

void get_quick_picks() {
	std::shuffle(balls.begin(), balls.end(), g);
	picks.clear();
	picks.insert(picks.end(), balls.begin(), balls.begin()+10);
}

void get_draws() {
	std::shuffle(balls.begin(), balls.end(), g);
	draws.clear();
	draws.insert(draws.end(), balls.begin(), balls.begin()+20);
}

void draw_number(int n, int p) {
	auto [row, col] = get_pos(n);
	attron(COLOR_PAIR(p));
	mvprintw(row, col, "%02d", n);
}

void draw_number_grid() {
	for (auto n : balls) {
		draw_number(n, GRID);
	}
	for (auto n : picks) {
		draw_number(n, PICK);
	}
}

int get_hits(std::vector<int> &picks, std::vector<int> &draws) {
	int hits=0;
	for (auto n : picks) {
		if (std::find(draws.begin(), draws.end(), n) != draws.end()) hits++;
	}
	return hits;
}

bool is_super(std::vector<int> &picks, std::vector<int> &draws) {
	return (std::find(picks.begin(), picks.end(), draws.back()) != picks.end());
}

int get_payout(int hits, bool super) {
	int payout;
	payout =
		(hits == 8 ? 600 :
		(hits == 7 ? 125 :
		(hits == 6 ? 45 :
		(hits == 5 ? 20 :
		(hits == 4 ? 5 : 0)))));
	if (super) payout *= 4;
	return payout;
}

void display_stats(double jackpot, double net, int hand, double payout) {
	attron(COLOR_PAIR(1));
	mvprintw(10, 0, "hand #%d", hand);
	mvprintw(11, 0, "payout:  $%'8.2f", payout);
	mvprintw(12, 0, "net: $%'8.2f", net);
	mvprintw(14, 0, "jackpot: $%'8.2f\n", jackpot);
}

void paint_draws(std::vector<int> &draws) {
	bool last;
	for (auto draw : draws) {
		last = (draw == draws.back());
		if (std::find(picks.begin(), picks.end(), draw) == picks.end()) {
			draw_number(draw, (last ? SUPERBALL_MISS : MISS));
		} else {
			draw_number(draw, (last ? SUPERBALL_HIT : HIT));
		}
		refresh();
		napms(DRAW_PAUSE/SPEED_UP);
	}
}




// trying out the basics

int main() {
	double jackpot = 143000, net=0;
	bool isJackpot = false;
	int hand=0;
	int hits;
	double payout;
	bool super;

	// initialize the balls
	for (int n=0; n<80; n++) balls.push_back(n);

	setlocale(LC_ALL, "");

	initscr();
	start_color();
	init_pair(GRID, COLOR_GREEN, COLOR_BLACK);		// number grid
	init_pair(HIT, COLOR_RED, COLOR_WHITE);
	init_pair(MISS, COLOR_RED, COLOR_BLACK);
	init_pair(SUPERBALL_HIT, COLOR_RED, COLOR_BLUE);
	init_pair(SUPERBALL_MISS, COLOR_BLUE, COLOR_BLACK);
	init_pair(PICK, COLOR_WHITE, COLOR_GREEN);		// picks
	curs_set(0);
	get_quick_picks();
	

	while (!isJackpot) {
		hand++;
		draw_number_grid();
		net -= 5;
		get_draws();
		hits = get_hits(picks, draws);
		super = is_super(picks, draws);
		isJackpot = (hits >= 9);
		payout = get_payout(hits, super);
		if (isJackpot) {
			payout = jackpot;
		}
		net += payout;
		jackpot += 5*CONTRIBUTION_RATE;	// 1% contribution
		refresh();
		paint_draws(draws);
		display_stats(jackpot, net, hand, payout);
		refresh();
		napms(((payout > 0) ? WIN_PAUSE : LOSE_PAUSE)/SPEED_UP);
		//isJackpot = true;
	}
	
}