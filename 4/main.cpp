#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <utility>
#include <algorithm>

enum fields {
	byr,
	iyr,
	eyr,
	hgt,
	hcl,
	ecl,
	pid,
	cid
};

template<int x>
static bool firstX(const std::vector<bool> & v) {
	for(int i = 0; i < x; ++i) {
		if(!v[i]) {
			return false;
		}
	}
	return true;
}

constexpr unsigned int str2int(const char* str, int h = 0) {
	return !str[h] ? 5381 : (str2int(str, h + 1) * 33) ^ str[h];
}

static const std::vector<std::string> eyeColors = {
	"amb",
	"blu",
	"brn",
	"gry",
	"grn",
	"hzl",
	"oth"
};

int main() {
	std::ifstream file("input.txt");
	if(!file.is_open()) {
		std::cout << "Could not open file!" << std::endl;
		return 1;
	}

	int allPresent = 0;
	int allValid = 0;

	std::vector<bool> present(7, false);
	std::vector<bool> valid(7, false);
	for(std::string line; std::getline(file, line);) {
		if(line.size() == 0) {
			if(firstX<7>(present)) {
				++allPresent;
			}
			present = std::vector<bool>(7, false);

			if(firstX<7>(valid)) {
				++allValid;
			}
			valid = std::vector<bool>(7, false);
		}

		std::istringstream is(line);
		for(std::string field; is >> field;) {
			std::string f = field.substr(0, 3);
			std::istringstream vs(field.substr(4));
			int year;
			char ch;
			switch(str2int(f.c_str())) {
				case str2int("byr"):
					present[fields::byr] = true;

					vs >> year;
					if(year >= 1920 && year <= 2002 && vs.eof()) {
						valid[fields::byr] = true;
					}
					break;
				case str2int("iyr"):
					present[fields::iyr] = true;

					vs >> year;
					if(year >= 2010 && year <= 2020 && vs.eof()) {
						valid[fields::iyr] = true;
					}
					break;
				case str2int("eyr"):
					present[fields::eyr] = true;

					vs >> year;
					if(year >= 2020 && year <= 2030 && vs.eof()) {
						valid[fields::eyr] = true;
					}
					break;
				case str2int("hgt"):
					present[fields::hgt] = true;

					int hgt;
					{
						std::string s;
						vs >> hgt >> s;

						if(s.size() == 2 && s[0] == 'c' && s[1] == 'm') {
							if(hgt >= 150 && hgt <= 193) {
								valid[fields::hgt] = true;
							}
						}
						if(s.size() == 2 && s[0] == 'i' && s[1] == 'n') {
							if(hgt >= 59 && hgt <= 76) {
								valid[fields::hgt] = true;
							}
						}
					}
					break;
				case str2int("hcl"):
					present[fields::hcl] = true;

					vs >> ch;
					if(ch == '#') {
						int count = 0;
						while(vs >> ch) {
							if((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f')) {
								++count;
							} else {
								break;
							}
						}
						if(count == 6) {
							valid[fields::hcl] = true;
						}
					}

					break;
				case str2int("ecl"):
					present[fields::ecl] = true;

					{
						std::string clr;
						vs >> clr;
						if(std::find(eyeColors.begin(), eyeColors.end(), clr) != eyeColors.end() && vs.eof()) {
							valid[fields::ecl] = true;
						}
					}
					break;
				case str2int("pid"):
					present[fields::pid] = true;

					{
						int count = 0;
						while(vs >> ch) {
							if(ch >= '0' && ch <= '9') {
								++count;
							} else {
								break;
							}
						}
						if(count == 9) {
							valid[fields::pid] = true;
						}
					}
					break;
				case str2int("cid"):
					present[fields::cid] = true;

					break;
			}
		}
	}

	if(firstX<7>(present)) {
		++allPresent;
	}
	if(firstX<7>(valid)) {
		++allValid;
	}

	file.close();

	std::cout << "result 1: " << allPresent << std::endl;
	std::cout << "result 2: " << allValid << std::endl;

	return 0;
}

