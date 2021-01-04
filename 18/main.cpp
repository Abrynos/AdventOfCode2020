#include <iostream>
#include <fstream>

#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/qi.hpp>

namespace qi = boost::spirit::qi;
namespace ph = boost::phoenix;

template<typename Iterator>
struct Calculator : qi::grammar<Iterator, long, qi::space_type> {
	private:
		template<typename result>
		using rule = qi::rule<Iterator, result, qi::space_type>;

		rule<long> expr;
		rule<long> fact;

	public:
		Calculator() : Calculator::base_type(expr) {
			expr =
				fact[qi::_val = qi::_1]
				>> *(
						('+' >> fact[qi::_val += qi::_1])
				   		| ('*' >> fact[qi::_val *= qi::_1])
					);
			fact =
				qi::long_[qi::_val = qi::_1]
				| '(' >> expr[qi::_val = qi::_1] >> ')';
		}
};

template<typename Iterator>
struct AdvancedCalculator : qi::grammar<Iterator, long, qi::space_type> {
	private:
		template<typename result>
		using rule = qi::rule<Iterator, result, qi::space_type>;

		rule<long> expr;
		rule<long> term;
		rule<long> fact;

	public:
		AdvancedCalculator() : AdvancedCalculator::base_type(expr) {
			expr =
				term[qi::_val = qi::_1]
				>> *('*' >> term[qi::_val *= qi::_1]);
			term =
				fact[qi::_val = qi::_1]
				>> *('+' >> fact[qi::_val += qi::_1]);
			fact =
				qi::long_[qi::_val = qi::_1]
				| ('(' >> expr[qi::_val = qi::_1] >> ')');
		}
};

template<typename T>
static inline long evaluate(std::string::iterator begin, const std::string::iterator end, const T & grammar) {
	long value;
	if (!qi::phrase_parse(begin, end, grammar, qi::space, value)) {
		std::cout << "ERROR: failed to parse input" << std::endl;
	}

	return value;
}

int main() {
	std::ifstream file("input.txt");
	if(!file.is_open()) {
		std::cout << "Could not open file!" << std::endl;
		return 1;
	}

	long result0 = 0;
	long result1 = 0;
	Calculator<std::string::iterator> calc;
	AdvancedCalculator<std::string::iterator> adCalc;
	for(std::string line; std::getline(file, line);) {
		result0 += evaluate(line.begin(), line.end(), calc);
		result1 += evaluate(line.begin(), line.end(), adCalc);
	}

	file.close();

	std::cout << "result 1: " << result0 << std::endl;
	std::cout << "result 2: " << result1 << std::endl;

	return 0;
}

