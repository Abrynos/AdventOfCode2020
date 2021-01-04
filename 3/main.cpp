#include <iostream>
#include <fstream>
#include <vector>
#include <utility>

int main() {
	std::ifstream file("input.txt");
	if(!file.is_open()) {
		std::cout << "Could not open file!" << std::endl;
		return 1;
	}

	std::vector<std::pair<int, int>> traversals = {
		std::pair<int, int>(1, 1),
		std::pair<int, int>(1, 3),
		std::pair<int, int>(1, 5),
		std::pair<int, int>(1, 7),
		std::pair<int, int>(2, 1)
	};

	std::vector<int> positions(traversals.size(), 0);
	std::vector<int> counts(traversals.size(), 0);

	std::string line;
	std::getline(file, line);// we don't want to go right in the first line - 0 % x is always 0
	for(int curLine = 1; std::getline(file, line); ++curLine) {
		if(line.size() == 0) {
			break;
		}

		for(int i = 0; i < traversals.size(); ++i) {
			if((curLine % traversals[i].first) != 0) {
				continue;
			}

			positions[i] += traversals[i].second;
			positions[i] %= line.size();
			if(line[positions[i]] == '#') {
				++counts[i];
			}
		}
	}

	file.close();

	std::cout << "result 1: " << counts[1] << std::endl;

	long res = 1;
	for(const int count : counts) {
		res += count;
	}
	std::cout << "result 2: " << res << std::endl;

	return 0;
}

