#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <tuple>
#include <utility>
#include <limits>

enum state {
	floor,
	empty,
	occupied
};

typedef std::pair<int, int> position;
typedef std::vector<position> seatlist;
typedef std::vector<std::vector<state>> layout;
typedef std::vector<std::vector<seatlist>> adjacencyMatrix;

template<int maxNeighbors>
static state newState(const layout & l, int x, int y, const seatlist & adjacentSeats) {
	switch(l[x][y]) {
		case state::empty:
			for(const position & p : adjacentSeats) {
				if(l[p.first][p.second] == state::occupied) {
					return state::empty;
				}
			}

			return state::occupied;

		case state::occupied:
			int count = 0;
			for(const position & p : adjacentSeats) {
				if(l[p.first][p.second] == state::occupied) {
					++count;
					if(count >= maxNeighbors) {
						return state::empty;
					}
				}
			}
			break;
	}

	return l[x][y];
}

template<int maxNeighbors>
static std::tuple<layout, bool, int> changeSeats(const layout & cur, const adjacencyMatrix & adjacentSeats) {
	layout result = cur;
	int count = 0;
	bool changed = false;
	for(int i = 0; i < cur.size(); ++i) {
		for(int j = 0; j < cur[i].size(); ++j) {
			state ns = newState<maxNeighbors>(cur, i, j, adjacentSeats[i][j]);
			if(ns != cur[i][j]) {
				changed = true;
			}

			result[i][j] = ns;
			if(ns == state::occupied) {
				++count;
			}
		}
	}

	return {result, changed, count};
}

inline int abs(int x) {
	if(x < 0) {
		return x * -1;
	}

	return x;
}

template<unsigned int maxDistance>
inline bool isFurther(int x, int y) {
	return abs(x - y) > maxDistance;
}

template<>
inline bool isFurther<std::numeric_limits<int>::max()>(int x, int y) {
	return false;// value is inlined in binary code and probably optimized away by compiler
}

template<unsigned int maxDistance = std::numeric_limits<int>::max()>
static position searchDir(const layout & l, int x, int y, int xDir, int yDir) {
	if(xDir == 0 && yDir == 0) {
		return { -1, -1 };
	}

	int i = x + xDir;
	int j = y + yDir;
	while(i >= 0 && j >= 0 && i < l.size() && j < l[i].size()) {
		if(isFurther<maxDistance>(x, i) || isFurther<maxDistance>(y, j)) {// whole block is removed in binary code if generic default value is present
			return { -1, -1 };
		}

		if(l[i][j] != state::floor) {
			return { i, j };
		}

		i += xDir;
		j += yDir;
	}

	return { -1, -1 };
}

template<unsigned int maxDistance = std::numeric_limits<int>::max()>
static adjacencyMatrix getAdjacentSeats(const layout & l) {
	adjacencyMatrix adjacentSeats(l.size(), std::vector<seatlist>(l[0].size(), seatlist(0)));
	for(int i = 0; i < l.size(); ++i) {
		for(int j = 0; j < l[i].size(); ++j) {
			if(l[i][j] == state::floor) {
				continue;
			}

			for(int x = -1; x <= 1; ++x) {
				for(int y = -1; y <= 1; ++y) {
					std::pair<int, int> pos = searchDir<maxDistance>(l, i, j, x, y);
					if(pos.first < 0 && pos.second < 0) {
						continue;
					}

					adjacentSeats[i][j].push_back(pos);
				}
			}
		}
	}

	return adjacentSeats;
}

int main() {
	std::ifstream file("input.txt");
	if(!file.is_open()) {
		std::cout << "Could not open file!" << std::endl;
		return 1;
	}

	std::vector<std::vector<state>> l;

	std::string line;
	while(std::getline(file, line)) {
		if(line.size() == 0) {
			continue;
		}

		std::vector<state> lv(line.size());
		std::istringstream ls(line);
		char ch;
		int i = 0;
		while(ls >> ch) {
			switch(ch) {
				case '.':
					lv[i] = state::floor;
					break;
				case 'L':
					lv[i] = state::occupied;// after first round everything is occupied anyways
					break;
			}
			++i;
		}
		l.push_back(lv);
	}

	file.close();

	adjacencyMatrix m = getAdjacentSeats<1>(l);
	std::tuple<std::vector<std::vector<state>>, bool, int> roundResult = changeSeats<4>(l, m);
	while(std::get<1>(roundResult)) {
		roundResult = changeSeats<4>(std::get<0>(roundResult), m);
	}
	std::cout << "result 1: " << std::get<2>(roundResult) << std::endl;

	m = getAdjacentSeats(l);
	roundResult = changeSeats<5>(l, m);
	while(std::get<1>(roundResult)) {
		roundResult = changeSeats<5>(std::get<0>(roundResult), m);
	}
	std::cout << "result 2: " << std::get<2>(roundResult) << std::endl;

	return 0;
}

