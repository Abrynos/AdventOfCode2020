#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <algorithm>

#define MAX_LOWER 3
#define HIGHER_THAN_MAX 3

static std::unordered_map<int, long> cache;

static long task2(const std::unordered_map<int, std::vector<int>> & sources, int current) {
	auto cit = cache.find(current);
	if(cit != cache.end()) {
		return cit->second;
	}

	long result = 0;
	for(const int source : sources.at(current)) {
		result += task2(sources, source);
	}

	cache[current] = result;
	return result;
}

int main() {
	std::ifstream file("input.txt");
	if(!file.is_open()) {
		std::cout << "Could not open file!" << std::endl;
		return 1;
	}

	std::vector<int> ratings(0);
	for(int rating; file >> rating; ) {
		ratings.push_back(rating);
	}

	file.close();

	std::sort(ratings.begin(), ratings.end());
	ratings.push_back(ratings[ratings.size() - 1] + HIGHER_THAN_MAX);

	std::vector<int> counts(MAX_LOWER, 0);
	int prev = 0;
	for(const int joltage : ratings) {
		int diff = joltage - prev;
		if(diff > MAX_LOWER) {
			std::cout << "No possible solution" << std::endl;
			return 1;
		}

		++counts[diff];
		prev = joltage;
	}

	std::cout << "result 1: " << (counts[1] * counts[3]) << std::endl;

	std::unordered_map<int, std::vector<int>> compat;
	for(int i = ratings.size() - 1; i >= 0; --i) {
		int rating = ratings[i];
		int j = i - 1;
		std::vector<int> sources;
		while(j >= 0 && ratings[j] >= rating - MAX_LOWER) {
			sources.push_back(ratings[j--]);
		}
		compat[rating] = sources;
	}

	for(int i = 0; ratings[i] <= MAX_LOWER; ++i) {
		compat[ratings[i]].push_back(0);
	}

	compat[0] = std::vector<int>(0);
	cache[0] = 1;


	std::cout << "result 2: " << task2(compat, ratings[ratings.size() - 1]) << std::endl;


	return 0;
}

