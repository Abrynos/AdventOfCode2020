#include <iostream>
#include <fstream>
#include <unordered_map>

// normal map is a red-black tree and much slower than a hashmap
// using a simple array with n as the index would speed this way up (even javascript can do it in less than 5 seconds then) but I'm too lazy to change the code now
typedef std::unordered_map<int, int> memory;

static inline int speak(memory & mem, int n, int turn) {
	memory::iterator it = mem.find(n);

	int next;
	if(it == mem.end()) {
		next = 0;
	} else {
		next = turn - it->second;
	}

	mem[n] = turn;
	return next;
}

int main() {
	std::ifstream file("input.txt");
	if(!file.is_open()) {
		std::cout << "Could not open file!" << std::endl;
		return 1;
	}

	int turn = 0;// don't forget to adjust for 0 indexing in the break conditions of the loops below
	int n;
	memory mem;
	for(; file >> n; ++turn) {
		mem[n] = turn;

		if(!file.eof()) {
			file.ignore();// ","
		}
	}

	file.close();

	for(n = 0; turn < 2020 - 1; ++turn) {
		n = speak(mem, n, turn);
	}
	std::cout << "result 1: " << n << std::endl;

	for(; turn < 30000000 - 1; ++turn) {
		n = speak(mem, n, turn);
	}
	std::cout << "result 2: " << n << std::endl;

	return 0;
}

