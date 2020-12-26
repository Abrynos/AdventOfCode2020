#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <algorithm>

typedef std::string ingredient;
typedef std::string allergen;
typedef std::unordered_set<ingredient> ingredientlist;
typedef ingredientlist product;

inline bool startsWith(const std::string & s, char ch) {
	return !s.empty() && s[0] == ch;
}

inline bool endsWith(const std::string & s, char ch) {
	return !s.empty() && s[s.size() - 1] == ch;
}

template<typename Predicate>
inline void discard_if(std::unordered_set<std::string> & set, Predicate pred) {
	for(auto it = set.begin(), end = set.end(); it != end;) {
		if(pred(*it)) {
			it = set.erase(it);
		} else{
			++it;
		}
	}
}

inline std::unordered_map<allergen, ingredient> isolate(std::unordered_map<allergen, ingredientlist> possibilities) {
	std::unordered_map<allergen, ingredient> result;
	while(!possibilities.empty()) {
		for(const std::pair<allergen, ingredientlist> & p : possibilities) {
			if(p.second.size() == 1) {
				ingredient ing = *p.second.begin();

				result[p.first] = ing;
				for(std::unordered_map<allergen, ingredientlist>::iterator it = possibilities.begin(); it != possibilities.end(); ++it) {
					it->second.erase(ing);
				}
				possibilities.erase(p.first);
				break;
			}
		}
	}
	return result;
}

template<typename T>
inline std::unordered_set<T> intersect(const std::unordered_set<T> & s1, const std::unordered_set<T> & s2) {
	std::unordered_set<T> res;
	for(const T & t : s1) {
		if(s2.find(t) != s2.end()) {
			res.insert(t);
		}
	}
	return res;
}

int main() {
	std::ifstream file("input.txt");
	if(!file.is_open()) {
		std::cout << "Could not open file!" << std::endl;
		return 1;
	}

	std::vector<product> products;

	std::unordered_map<allergen, ingredientlist> allergenProducts;

	for(std::string line; std::getline(file, line);) {
		product product;

		std::istringstream is(line);
		for(ingredient ing; is >> ing && !startsWith(ing, '(');) {
			product.insert(ing);
		}

		for(allergen all; is >> all;) {
			all.erase(std::prev(all.end()));// either closing bracket or comma

			if(all.empty()) {
				continue;
			}

			std::unordered_map<allergen, ingredientlist>::iterator it = allergenProducts.find(all);
			if(it == allergenProducts.end()){
				allergenProducts[all] = product;
			} else {
				allergenProducts[all] = intersect(it->second, product);
			}
		}

		products.push_back(product);
	}

	file.close();

	std::unordered_set<ingredient> ignorables;
	for(const std::pair<allergen, ingredientlist> & p : allergenProducts) {
		for(const ingredient & ing : p.second) {
			ignorables.insert(ing);
		}
	}

	int count = 0;
	for(const product & prod : products) {
		for(const ingredient & ing : prod) {
			if(ignorables.find(ing) == ignorables.end()) {
				++count;
			}
		}
	}

	std::cout << "result 1: " << count << std::endl;

	std::unordered_map<allergen, ingredient> isolatedSet = isolate(allergenProducts);
	std::vector<std::pair<allergen, ingredient>> isolated(isolatedSet.begin(), isolatedSet.end());
	std::sort(isolated.begin(), isolated.end(), [](std::pair<allergen, ingredient> p1, std::pair<allergen, ingredient> p2) -> bool {
		return std::less<std::string>()(p1.first, p2.first);
	});

	std::string canDanIngLis = isolated[0].second;
	for(int i = 1; i < isolated.size(); ++i) {
		canDanIngLis += ',' + isolated[i].second;
	}

	std::cout << "result 2: " << canDanIngLis << std::endl;

	return 0;
}

