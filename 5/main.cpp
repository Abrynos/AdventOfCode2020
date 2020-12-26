#include <iostream>
#include <fstream>
#include <vector>

template<int n>
struct Pow2 {
	enum {
		RET = Pow2<n - 1>::RET * 2
	};
};

template<>
struct Pow2<1> {
	enum {
		RET = 2
	};
};

template<int n, char upper>
inline int binPart(std::istream & is) {
	int result = 0;
	int fact = Pow2<n>::RET;
	char ch;
	for(int i = 0; i < n; ++i) {
		fact >>= 1;
		is >> ch;
		if(ch == upper) {
			result += fact;
		}
	}

	return result;
}

class pass {
	private:
		int row, col;
		friend std::istream & operator >> (std::istream & is, pass & p);
	public:
		pass() {
			row = -1;
			col = -1;
		}
		pass(int r, int c) {
			row = r;
			col = c;
		}
		int getCol() {
			return col;
		}
		int getRow() {
			return row;
		}
		int getId() {
			return row * 8 + col;
		}
};

#define ROWS 7
#define COLS 3

std::istream & operator >> (std::istream & is, pass & p) {
	p.row = binPart<ROWS, 'B'>(is);
	p.col = binPart<COLS, 'R'>(is);

	return is;
}

int main() {
	std::ifstream file("input.txt");
	if(!file.is_open()) {
		std::cout << "Could not open file!" << std::endl;
		return 1;
	}

	std::vector<std::vector<bool>> seats(Pow2<ROWS>::RET, std::vector<bool>(Pow2<COLS>::RET, false));
	int maxId = 0;
	pass p;
	for(pass p; file >> p;) {
		seats[p.getRow()][p.getCol()] = true;
		int id = p.getId();
		if(id > maxId) {
			maxId = id;
		}
	}

	file.close();

	bool now = false;
	bool done = false;
	for(int r = 0; r < seats.size() && !done; ++r) {
		std::vector<bool> row = seats[r];
		for(int s = 0; s < row.size() && !done; ++s) {
			if(row[s]) {
				now = true;
			}
			if(!row[s] && now) {
				std::cout << "result 2: " << pass(r, s).getId() << std::endl;
				done = true;
			}
		}
	}

	std::cout << "result 1: " << maxId << std::endl;

	return 0;
}

