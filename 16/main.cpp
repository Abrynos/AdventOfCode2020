#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/qi.hpp>

namespace qi = boost::spirit::qi;
namespace ph = boost::phoenix;

typedef std::pair<int, int> intrange;
typedef std::vector<intrange> rangevec;
typedef std::pair<std::string, rangevec> ruledef;

static int isValid(const std::vector<ruledef> & rules, std::vector<int> ticket) {
	for(const int value : ticket) {
		bool ok = false;
		for(const ruledef & rule : rules) {
			for(const intrange & range : rule.second) {
				if(value >= range.first && value <= range.second) {
					ok = true;
					break;
				}
			}
			if(ok) {
				break;
			}
		}
		if(!ok) {
			return false;
		}
	}
	return true;
}

static int getError(const std::vector<ruledef> & rules, std::vector<int> ticket) {
	int error = 0;
	for(const int value : ticket) {
		bool ok = false;
		for(const ruledef & rule : rules) {
			for(const intrange & range : rule.second) {
				if(value >= range.first && value <= range.second) {
					ok = true;
					break;
				}
			}
			if(ok) {
				break;
			}
		}
		if(!ok) {
			error += value;
		}
	}
	return error;
}

static int size(const std::vector<int> & defs) {
	return defs.size();
}

static void restrictField(std::vector<ruledef> & possibilities, int value) {
	std::vector<ruledef> problems;
	for(const ruledef & rule : possibilities) {
		bool ok = false;
		for(const intrange & range : rule.second) {
			if(value >= range.first && value <= range.second) {
				ok = true;
				break;
			}
		}
		if(ok) {
			continue;
		}

		problems.push_back(rule);
	}
	for(const ruledef & problem : problems) {
		std::vector<ruledef>::iterator it = std::find(possibilities.begin(), possibilities.end(), problem);
		if(it != possibilities.end()) {
			possibilities.erase(it);
		}
	}
}

static bool removeRedundancies(std::vector<std::vector<ruledef>> & possibilities) {
	bool changed = false;
	for(int i = 0; i < possibilities.size(); ++i) {
		if(possibilities[i].size() != 1) {
			continue;
		}

		for(int j = 0; j < possibilities.size(); ++j) {
			if(j == i) {
				continue;
			}

			std::vector<ruledef>::iterator it = std::find(possibilities[j].begin(), possibilities[j].end(), possibilities[i][0]);
			if(it != possibilities[j].end()) {
				possibilities[j].erase(it);
				changed = true;
			}
		}
	}
	return changed;
}

static void restrict(std::vector<std::vector<ruledef>> & possibilities, const std::vector<int> & ticket) {
	for(int i = 0; i < ticket.size(); ++i) {
		restrictField(possibilities[i], ticket[i]);
	}

	while(removeRedundancies(possibilities));
}

static bool startsWith(std::string s, std::string p) {
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

static long computeResult2(const std::vector<std::vector<ruledef>> & finalRules, const std::vector<int> & ticket) {
	long result = 1;
	int first = 0;
	for(; !startsWith(finalRules[first][0].first, "departure"); ++first);
	for(; first < ticket.size(); ++first) {
		if(startsWith(finalRules[first][0].first, "departure")) {
			result *= ticket[first];
		}
	}
	return result;
}

template<typename Iterator>
struct Validator : qi::grammar<Iterator, std::pair<int, long>, qi::space_type> {
	private:
		template<typename result>
		using rule = qi::rule<Iterator, result, qi::space_type>;
		template<typename result, typename local>
		using lrule = qi::rule<Iterator, result, qi::locals<local>, qi::space_type>;
		template<typename result, typename local1, typename local2>
		using llrule = qi::rule<Iterator, result, qi::locals<local1, local2>, qi::space_type>;
		template<typename result, typename local1, typename local2, typename local3, typename local4>
		using llllrule = qi::rule<Iterator, result, qi::locals<local1, local2, local3, local4>, qi::space_type>;

		rule<std::pair<int, long>> program;
		llllrule<std::pair<int, long>, std::vector<ruledef>, std::vector<std::vector<ruledef>>, std::vector<int>, int> program_loc;
		lrule<ruledef, std::string> law;
		rule<rangevec> ranges;
		lrule<intrange, int> range;
 		rule<std::vector<int>> ticket;
		rule<std::string()> ident;

	public:
		Validator() : Validator::base_type(program) {
			program = program_loc;
			program_loc =
				*law[ph::push_back(qi::_a, qi::_1)]
				>> "your ticket:"
				>> ticket[
					qi::_c = qi::_1,
					qi::_b = ph::construct<std::vector<std::vector<ruledef>>>(ph::bind(&size, qi::_c), qi::_a),
					ph::bind(&restrict, qi::_b, qi::_c),
					qi::_d = 0
				]
				>> "nearby tickets:"
				>> (*ticket[
						qi::_d += ph::bind(&getError, qi::_a, qi::_1),
						ph::if_(ph::bind(&isValid, qi::_a, qi::_1))[// becuase the error is 0 if the value of the invalid field is 0 - cost me 2.5 hours
						ph::bind(&restrict, qi::_b, qi::_1)
					]
				])[qi::_val = ph::construct<std::pair<int, long>>(qi::_d, ph::bind(&computeResult2, qi::_b, qi::_c))];
			ticket =
				qi::int_[ph::push_back(qi::_val, qi::_1)]
				>> *(
						','
						>> qi::int_[ph::push_back(qi::_val, qi::_1)]
					);
			law =
				ident[qi::_a = qi::_1]
				>> ":"
				>> ranges[qi::_val = ph::construct<std::pair<std::string, rangevec>>(qi::_a, qi::_1)];
			ident =
				qi::lexeme[*(qi::alpha | ' ')];
			ranges =
				range[ph::push_back(qi::_val, qi::_1)]
				>> *(
						"or"
						>> range[ph::push_back(qi::_val, qi::_1)]
					);
			range =
				qi::int_[qi::_a = qi::_1]
				>> '-'
				>> qi::int_[qi::_val = ph::construct<intrange>(qi::_a, qi::_1)];
		}
};

int main() {
	std::ifstream file("input.txt");
	if(!file.is_open()) {
		std::cout << "Could not open file!" << std::endl;
		return 1;
	}

	file >> std::noskipws;
	boost::spirit::istream_iterator iter(file);
	boost::spirit::istream_iterator end;

	Validator<decltype(iter)> grammar;
	std::pair<int, long> result;
	bool success = qi::phrase_parse(iter, end, grammar, qi::space, result);

	file.close();
	if(!success) {
		std::cout << "Unable to parse input" << std::endl;
		return 1;
	}

	std::cout << "result 1: " << result.first << std::endl;

	std::cout << "result 2: " << result.second << std::endl;

	return 0;
}

