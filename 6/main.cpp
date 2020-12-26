#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

int main() {
	std::ifstream file("input.txt");
	if(!file.is_open()) {
		std::cout << "Could not open file!" << std::endl;
		return 1;
	}

	int sum = 0;
	int sum2 = 0;

	std::vector<bool> vals(26, false);
	std::vector<int> vals2(26, 0);
	int people = 0;
	for(std::string line; std::getline(file, line);) {
		if(line.size() == 0) {
			sum += std::count(vals.begin(), vals.end(), true);
			sum2 += std::count(vals2.begin(), vals2.end(), people);

			vals = std::vector<bool>(26, false);
			vals2 = std::vector<int>(26, 0);
			people = 0;
			continue;
		}

		for(char ch : line) {
			vals[ch - 'a'] = true;
			++vals2[ch - 'a'];
		}

		++people;
	}

	// no empty line at end of file -> execute increase manually once more for the last group
	sum += std::count(vals.begin(), vals.end(), true);
	sum2 += std::count(vals2.begin(), vals2.end(), people);

	file.close();

	std::cout << "result 1: " << sum << std::endl;
	std::cout << "result 2: " << sum2 << std::endl;

	return 0;
}

