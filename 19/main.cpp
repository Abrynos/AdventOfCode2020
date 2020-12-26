#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <unordered_map>

static inline std::regex createRegex(const std::string & target, std::unordered_map<std::string, std::string> rules) {
	std::regex idPattern(R"(\((\d+)\))");
	std::unordered_map<std::string, std::string> finishedRules;

	while(!rules.empty()) {
		std::unordered_map<std::string, std::string>::iterator ruleIterator = rules.begin();
		for(; ruleIterator != rules.end(); ++ruleIterator) {
			std::sregex_iterator idIterator = std::sregex_iterator(ruleIterator->second.begin(), ruleIterator->second.end(), idPattern);
			if(idIterator == std::sregex_iterator()) {
				finishedRules.insert(*ruleIterator);
				ruleIterator = rules.erase(ruleIterator);
				break;
			}

			for(; idIterator != std::sregex_iterator(); ++idIterator) {
				std::string match = (*idIterator)[1];
				std::unordered_map<std::string, std::string>::iterator finIt = finishedRules.find(match);
				if(finIt != finishedRules.end()) {
					std::string value = ruleIterator->second;
					int position = ruleIterator->second.find(match);
					ruleIterator->second.replace(position, match.size(), finIt->second);
					break;
				}
			}
		}
	}

	return std::regex(finishedRules[target], std::regex::nosubs | std::regex::optimize);
}

int main() {
	std::ifstream file("input.txt");
	if(!file.is_open()) {
		std::cout << "Could not open file!" << std::endl;
		return 1;
	}

	std::regex charRulePattern(R"((\d+): "(\w)\")");
	std::regex orFollowRulePattern(R"((\d+): (\d+) (\d+) \| (\d+) (\d+))");
	std::regex orRulePattern(R"((\d+): (\d+) \| (\d+))");// invalid per task description but in input
	std::regex followRulePattern(R"((\d+): (\d+) (\d+))");
	std::regex equalRulePattern(R"((\d+): (\d+))");// invalid per task description but in input

	std::unordered_map<std::string, std::string> rules;

	for(std::string line; std::getline(file, line) && !line.empty();) {
		std::smatch match;
		if(std::regex_match(line, match, charRulePattern)) {
			rules[match[1]] = match[2];
		} else if (std::regex_match(line, match, orFollowRulePattern)) {
			std::ostringstream os;
			os << "((" << match[2] << ")(" << match[3] << ")|(" << match[4] << ")(" << match[5] << "))";
			rules[match[1]] = os.str();
		} else if (std::regex_match(line, match, followRulePattern)) {
			std::ostringstream os;
			os << "((" << match[2] << ")(" << match[3] << "))";
			rules[match[1]] = os.str();
		} else if (std::regex_match(line, match, orRulePattern)) {
			std::ostringstream os;
			os << "((" << match[2] << ")|(" << match[3] << "))";
			rules[match[1]] = os.str();
		} else if (std::regex_match(line, match, equalRulePattern)) {
			std::ostringstream os;
			os << '(' << match[2] << ')';
			rules[match[1]] = os.str();
		}
	}

	std::regex reg0 = createRegex("0", rules);
	// "8: 42 | 42 8"
	// theoretically this is an infinite repitition of rule 42
	// it works tho, if we just repeat it 5 times manually
	// I'll call this a cheap hack
	rules["8"] = "((42)|((42)(42))|((42)(42)(42))|((42)(42)(42)(42))|((42)(42)(42)(42)(42)))";
	// "11: 42 31 | 42 11 31"
	// either this is rule 42 followed by 31 or it is any (theoretically infinite) amount of occurences of 42 followed by the exact same amount of occurences of rule 31
	// I'll use the same hack here, tho this time 4 repititions is enough to make it work
	rules["11"] = "(((42)(31))|((42)(42)(31)(31))|((42)(42)(42)(31)(31)(31))|((42)(42)(42)(42)(31)(31)(31)(31)))";
	std::regex reg1 = createRegex("0", rules);

	int valid0 = 0;
	int valid1 = 0;
	for(std::string line; std::getline(file, line);) {
		std::smatch match;
		if(std::regex_match(line, match, reg0)) {
			++valid0;
		}
		if(std::regex_match(line, match, reg1)) {
			++valid1;
		}
	}

	file.close();

	std::cout << "result 1: " << valid0 << std::endl;
	std::cout << "result 2: " << valid1 << std::endl;

	return 0;
}

