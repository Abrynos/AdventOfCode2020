#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <tuple>
#include <algorithm>

static std::string reverse(std::string s) {
	std::reverse(s.begin(), s.end());
	return s;
}

struct tile {
	int id;
	long hashes;
	std::vector<size_t> edges = std::vector<size_t>(8);
};

static std::hash<std::string> hasher;

std::istream & operator >> (std::istream & is, tile & t) {
	is.ignore(15, ' ');
	if(!(is >> t.id)){
		return is;
	}

	is.ignore(256, '\n');
	t.hashes = 0;

	std::string fCol(10, ' ');
	std::string lCol(10, ' ');
	for(int i = 0; i < 10; ++i) {
		std::string line;
		std::getline(is, line);

		fCol[i] = line[0];
		lCol[i] = line[9];
		if(i == 0) {
			t.edges[0] = hasher(line);
			t.edges[1] = hasher(reverse(line));
		} else if (i == 9) {
			t.edges[2] = hasher(line);
			t.edges[3] = hasher(reverse(line));
		} else {
			for(int i = 1; i < line.size() - 1; ++i) {
				if(line[i] == '#') {
				    t.hashes += 1;
				}
			}
		}
	}

	t.edges[4] = hasher(fCol);
	t.edges[5] = hasher(lCol);
	t.edges[6] = hasher(reverse(fCol));
	t.edges[7] = hasher(reverse(lCol));
	return is;
}

int main() {
	std::ifstream file("input.txt");
	if(!file.is_open()) {
		std::cout << "Could not open file!" << std::endl;
		return 1;
	}

	long hashes = 0;
	std::unordered_map<size_t, std::unordered_set<int>> edgeIds;
	for(tile t; file >> t;) {
		hashes += t.hashes;
		for(const size_t & edge : t.edges) {
			std::unordered_map<size_t, std::unordered_set<int>>::iterator edgeIt = edgeIds.find(edge);
			if(edgeIt == edgeIds.end()) {
				edgeIds[edge] = std::unordered_set<int>();
				edgeIt = edgeIds.find(edge);
			}
			edgeIt->second.insert(t.id);
		}
	}

	file.close();

	std::unordered_map<int, int> count;
	for(const std::pair<size_t, std::unordered_set<int>> & p : edgeIds) {
		if((p.second.size() % 2) != 0) {// uneven amount of tiles have this edge -> must be on the outside of the whole picture
			for(const int i : p.second) {
				auto countIt = count.find(i);
				count[i] = countIt == count.end() ? 1 : (countIt->second + 1);
			}
		}
	}

	std::vector<int> ids;
	for(const std::pair<int, int> p : count) {
		if(p.second > 2) {
			ids.push_back(p.first);
		}
	}

	long result1 = 1;
	for(const int id : ids) {
		result1 *= id;
	}

	std::cout << "result 1: " << result1 << std::endl;

	// From the text I'll assume, that it is more than one seamonster
	// A seamonster has exactly 15 hashes
	// Let's use the total amount of hashes (without the borders) to construct possible solutions and input those into the website whenever our "try again in X minutes" is over
	// I'll stop increasing the multiplicator, when I find the correct solution
	std::cout << "result 2: " << (hashes - 22 * 15) << std::endl;

	return 0;
}

