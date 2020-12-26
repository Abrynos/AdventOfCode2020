#include <iostream>
#include <fstream>
#include <vector>

int main() {
	std::ifstream file("input.txt");
	if(!file.is_open()) {
		std::cout << "Could not open file!" << std::endl;
		return 1;
	}
	
	std::vector<int> numbers;
	for(int n; file >> n;) {
		for(int i = 0; i < numbers.size(); ++i) {
			int ni = numbers[i];
			if((ni + n) == 2020) {
				std::cout << "result 1: " << (ni * n) << std::endl;
			}

			for(int j = i + 1; j < numbers.size(); ++j) {
				int nj = numbers[j];
				if((n + ni + nj) == 2020) {
					std::cout << "result 2: " << (n * ni * nj) << std::endl;
				}
			}
		}

		numbers.push_back(n);
	}

	file.close();

	return 0;
}

