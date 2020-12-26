#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <set>
#include <vector>
#include <algorithm>
#include <utility>

static bool startsWith(const std::string & s, const std::string & p) {
	if(s.size() < p.size()) {
		return false;
	}

	for(int i = 0; i < p.size(); ++i) {
		if(s[i] != p[i]) {
			return false;
		}
	}

	return true;
}

static std::string readBagColor(std::istream & is) {
	std::string result = "";
	for(std::string word; is >> word && !startsWith(word, "bag");) {
		result += word;
	}

	return result;
}

static std::map<std::string, int> containsCache;

static int contains(const std::map<std::string, std::vector<std::pair<int, std::string>>> & info, const std::string & bag) {
	auto res = containsCache.find(bag);
	if(res != containsCache.end()) {
		return res->second;
	}

	std::vector<std::pair<int, std::string>> contained = info.at(bag);
	int sum = 0;
	for(const std::pair<int, std::string> & pair : contained) {
		sum += pair.first * contains(info, pair.second) + pair.first;
	}
	containsCache[bag] = sum;
	return sum;
}

static std::set<std::string> canContain(const std::map<std::string, std::set<std::string>> & info, std::string x) {
	std::set<std::string> result;
	for(const std::string & s : info.at(x)) {
		result.insert(s);
	}
	return result;
}

int main() {
	std::ifstream file("input.txt");
	if(!file.is_open()) {
		std::cout << "Could not open file!" << std::endl;
		return 1;
	}

	std::map<std::string, std::vector<std::pair<int, std::string>>> res;
	for(std::string line; std::getline(file, line);) {
		std::istringstream is(line);
		std::string color = readBagColor(is);
		is >> line;// "contain"

		std::vector<std::pair<int, std::string>> pairs(0);
		for(int amount; is >> amount;) {
			std::string contained = readBagColor(is);
			pairs.push_back(std::pair<int, std::string>(amount, contained));
		}

		res[color] = pairs;
	}

	file.close();

	std::map<std::string, std::set<std::string>> containedIn;
	for(const std::pair<std::string, std::vector<std::pair<int, std::string>>> & result : res) {
		for(std::pair<int, std::string> p : result.second) {
			std::map<std::string, std::set<std::string>>::iterator it = containedIn.find(p.second);
			if(it == containedIn.end()) {
				containedIn[p.second] = std::set<std::string>();
				it = containedIn.find(p.second);
			}
			it->second.insert(result.first);
		}
 	}

	std::set<std::string> outer = canContain(containedIn, "shinygold");
	std::cout << "result 1: " << outer.size() << std::endl;

	std::cout << "result 2: " << contains(res, "shinygold") << std::endl;

	return 0;
}

