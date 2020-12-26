#include <iostream>
#include <fstream>
#include <vector>
#include <utility>
#include <limits>

long long findX(long long a, long long b) {
	if(b == 1) {
		return 1;
	}

	long long b0 = b;

	long long x0 = 0;
	long long x1 = 1;
	while(a > 1) {
		long long q = a / b;
		long long amb = a % b;
		a = b;
		b = amb;

		long long xqx = x1 - q * x0;
		x1 = x0;
		x0 = xqx;
	}

	if(x1 < 0) {
		x1 += b0;
	}

	return x1;
}

long long chineseRemainder(const std::vector<std::pair<int, int>> & busses) {
	long long M = 1;
	for(const std::pair<int, int> & bus : busses) {
		M *= bus.first;
	}

	long long sum = 0;
	for(const std::pair<int, int> & bus : busses) {
		long long id = bus.first;
		long long offset = bus.second;
		long long Mi = M / id;
		sum += offset * Mi * findX(Mi % id, id);
	}

	return sum % M;
}

int main() {
	std::ifstream file("input.txt");
	if(!file.is_open()) {
		std::cout << "Could not open file!" << std::endl;
		return 1;
	}

	std::vector<std::pair<int, int>> busses;

	int earliest;
	file >> earliest;
	int minWaitingTime = std::numeric_limits<int>::max();
	int minBusId = 0;

	int offset = 0;
	for(int bus; file >> bus;) {
		int waitingTime = bus - (earliest % bus);
		if(waitingTime < minWaitingTime) {
			minWaitingTime = waitingTime;
			minBusId = bus;
		}

		busses.push_back(std::pair<int, int>(bus, bus - offset++));

		if(!file.eof()) {
			char ch;
			file >> ch;// ","
		}

		while(file.peek() == 'x') {
			++offset;
			char ch;
			file >> ch >> ch;// "x" ","
		}
	}

	file.close();

	std::cout << "result 1: " << (minBusId * minWaitingTime) << std::endl;

	std::cout << "result 2: " << chineseRemainder(busses) << std::endl;

	return 0;
}

