#include <iostream>
#include <fstream>
#include <vector>
#include <utility>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/qi.hpp>

namespace qi = boost::spirit::qi;
namespace ph = boost::phoenix;

enum stat{
	jmp,
	acc,
	nop
};

template<typename Iterator>
struct ChristmasGrammar : qi::grammar<Iterator, std::vector<std::pair<stat, int>>, qi::space_type> {
	private:
		template<typename result>
		using rule = qi::rule<Iterator, result, qi::space_type>;

		rule<std::vector<std::pair<stat, int>>>	program;
		rule<std::pair<stat, int>>				statement, nop, acc, jmp;
		rule<std::string()> ident;

	public:
		ChristmasGrammar() : ChristmasGrammar::base_type(program) {
			program =
				*statement[ph::push_back(qi::_val, qi::_1)];
			statement =
				nop
				| acc
				| jmp;
			jmp =
				qi::lit("jmp") >> qi::int_[qi::_val = ph::construct<std::pair<stat, int>>(stat::jmp, qi::_1)];
			acc =
				qi::lit("acc") >> qi::int_[qi::_val = ph::construct<std::pair<stat, int>>(stat::acc, qi::_1)];
			nop =
				qi::lit("nop") >> qi::int_[qi::_val = ph::construct<std::pair<stat, int>>(stat::nop, qi::_1)];
		}
};

static int dummy_acc;
static bool terminatesCorrectly(const std::vector<std::pair<stat, int>> & code, int & acc = dummy_acc) {
	acc = 0;

	int pos = 0;
	std::vector<bool> visited(code.size(), false);

	while(pos < code.size() && !visited[pos]) {
		visited[pos] = true;
		switch(code[pos].first){
			case stat::jmp:
				pos += code[pos].second;
				break;
			case stat::acc:
				acc += code[pos].second;
			case stat::nop:
			default:
				++pos;
		}
	}

	return pos == code.size();
}

int main() {
	std::ifstream file("input.txt");
	if(!file.is_open()) {
		std::cout << "Could not open file!" << std::endl;
		return 1;
	}

	file >> std::noskipws;
	boost::spirit::istream_iterator iter(file);
	boost::spirit::istream_iterator end;
	ChristmasGrammar<decltype(iter)> grammar;

	std::vector<std::pair<stat, int>> code;
	bool success = qi::phrase_parse(iter, end, grammar, qi::space, code);

	file.close();

	if(!success) {
		std::cout << "Unable to parse input" << std::endl;
		return 1;
	}

	{// part 1
		int acc;
		terminatesCorrectly(code, acc);
		std::cout << "result 1: " << acc << std::endl;
	}
	{// part 2
		int acc;
		for(int i = 0; i < code.size(); ++i) {
			if(code[i].first != stat::jmp && code[i].first != stat::nop){
				continue;
			}

			std::vector<std::pair<stat, int>> modifiedCode = code;
			modifiedCode[i] = std::pair<stat, int>(code[i].first == stat::jmp ? stat::nop : stat::jmp, code[i].second);
			if(terminatesCorrectly(modifiedCode, acc)) {
				std::cout << "result 2: " << acc << std::endl;
				break;
			}
		}
	}

	return 0;
}

