#include <iostream>
#include <fstream>
#include <sstream>
#include <bitset>
#include <map>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/qi.hpp>

namespace qi = boost::spirit::qi;
namespace ph = boost::phoenix;

#define VALUE_LENGTH 36
typedef std::bitset<VALUE_LENGTH> value;
typedef std::bitset<36> address;

struct Comparator {
	bool operator() (const address & a1, const address & a2) const {
		return a1.to_ulong() < a2.to_ulong();
	}
};

static value orMask;
static value andMask;
static std::map<address, value, Comparator> memory;

static address floatingMask;
static std::map<address, value, Comparator> memory2;

static void clearMasks() {
	orMask = 0;
	andMask = 0;

	floatingMask = 0;
}

static void updateMasks(char ch) {
	orMask <<= 1;
	andMask <<= 1;

	if(ch == '1') {
		orMask |= 1;
	}

	if(ch != '0') {
		andMask |= 1;
	}

	floatingMask <<= 1;
	if(ch == 'X') {
		floatingMask |= 1;
	}
}

static void setMemValues(address addy, value val, int i = 0) {
	if(i >= addy.size()) {
		memory2[addy] = val;
		return;
	}

	if(floatingMask[i] == 0) {
		setMemValues(addy, val, i + 1);
		return;
	}

	for(int j = 0; j < 2; ++j) {
		addy[i] = j;
		setMemValues(addy, val, i + 1);
	}
}

static void setValue(address addy, value val) {
	memory[addy] = (val | orMask) & andMask;

	setMemValues(addy | orMask, val);
}

template<typename Iterator>
struct Interpreter : qi::grammar<Iterator, qi::space_type> {
	private:
		using rule = qi::rule<Iterator, qi::space_type>;

		rule program, stat, maskset;
		qi::rule<Iterator, qi::locals<address>, qi::space_type> memset;

	public:
		Interpreter() : Interpreter::base_type(program) {
			program =
				*stat;
			stat =
				maskset
				| memset;
			memset =
				qi::lit("mem") >> "[" >> qi::long_long[qi::_a = qi::_1] >> "]" >> "=" >> qi::long_long[ph::bind(&setValue, qi::_a, qi::_1)];
			maskset =
				qi::lit("mask")[ph::bind(&clearMasks)] >> "=" >> qi::repeat(VALUE_LENGTH)[
					qi::char_("01X")[ph::bind(&updateMasks, qi::_1)]
				];
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

	Interpreter<decltype(iter)> grammar;
	bool success = qi::phrase_parse(iter, end, grammar, qi::space);

	file.close();
	if(!success) {
		std::cout << "Unable to parse input" << std::endl;
		return 1;
	}

	unsigned long long result = 0;
	for(const std::pair<address, value> & p : memory) {
		result += p.second.to_ulong();
	}

	std::cout << "result 1: " << result << std::endl;

	result = 0;
	for(const std::pair<address, value> & p : memory2) {
		result += p.second.to_ulong();
	}

	std::cout << "result 2: " << result << std::endl;

	return 0;
}

