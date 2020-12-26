#include <iostream>
#include <fstream>
#include <algorithm>

int main() {
	std::ifstream file("input.txt");
	if(!file.is_open()) {
		std::cout << "Could not open file!" << std::endl;
		return 1;
	}
	
	int min;
	int max;
	char ch;
	char xtra;
	std::string pwd;

	int valid = 0;
	int valid2 = 0;
	while(file >> min >> xtra >> max >> ch >> xtra >> pwd) {
		int count = std::count(pwd.begin(), pwd.end(), ch);
		if(count >= min && count <= max) {
			++valid;
		}

		bool p1 = pwd[min - 1] == ch;
		bool p2 = pwd[max - 1] == ch;
		if(p1 ^ p2) {
			++valid2;
		}
	}

	std::cout << "result 1: " << valid << std::endl;
	std::cout << "result 2: " << valid2 << std::endl;

	file.close();

	return 0;
}

