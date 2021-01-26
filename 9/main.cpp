#include <iostream>
#include <fstream>
#include <vector>
#include <limits>

#define PREAMBLE 25

static bool isValidAtEnd(const std::vector<int> & v, int n) {
	for(int i = v.size() - PREAMBLE; i < v.size(); ++i) {
		for(int j = i + 1; j < v.size(); ++j) {
			if((v[i] + v[j]) == n) {
				return true;
			}
		}
	}

	return false;
}

int main() {
	std::ifstream file("input.txt");
	if(!file.is_open()) {
		std::cout << "Could not open file!" << std::endl;
		return 1;
	}

	std::vector<int> numbers(0);
	for(int i = 0; i < PREAMBLE; ++i) {
		int n;
		file >> n;
		numbers.push_back(n);
	}

	int n;
	while(file >> n) {
		if(!isValidAtEnd(numbers, n)) {
			break;
		}

		numbers.push_back(n);
	}

	file.close();

	int sum = 0;
	int i = 0;
	int j = 0;
	int min = std::numeric_limits<int>::max();
	int max = std::numeric_limits<int>::min();
	while(j < numbers.size()) {
		if(sum == n) {
			while(i <= j) {
				if(numbers[i] < min) {
					min = numbers[i];
				}
				if(numbers[i] > max) {
					max = numbers[i];
				}
				++i;
			}
		}

		if(sum > n) {
			sum -= numbers[i++];
			continue;
		}

		sum += numbers[j++];
	}

	std::cout << "result 1: " << n << std::endl;
	std::cout << "result 2: " << (min + max) << std::endl;

	return 0;
}

