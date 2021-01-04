#include <iostream>
#include <fstream>
#include <unordered_set>
#include <unordered_map>
#include <utility>
#include <algorithm>
#include <limits>
#include <stdexcept>
#include <vector>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/qi.hpp>

/////////////////////////////////////////////////////////////////////////////////
// This is basically a copy of day 17 - Conway dimension
/////////////////////////////////////////////////////////////////////////////////
// I just adapted my loops to search a 6-neighbor coordinate system instead of a
// 4 neighbor one. Do not attempt this at home. The code is pretty ugly
/////////////////////////////////////////////////////////////////////////////////
// I also tried this before without copying day 17, but the colossal fuckup I
// made had a run-time of over 20 minutes for 100 iterations. I really should
// not try to re-invent the wheel after finding such a nice solution earlier
/////////////////////////////////////////////////////////////////////////////////

namespace qi = boost::spirit::qi;
namespace ph = boost::phoenix;

template<typename T>
static inline T abs(T t) {
	if(t < 0) {
		return -t;
	}

	return t;
}

// hashing for int-vector
namespace std {
    template <>
    struct hash<std::vector<int>> {
        size_t operator()(const vector<int>& v) const {
			std::hash<int> hasher;
        	size_t seed = 0;
        	for (int i : v) {
            	seed ^= hasher(i) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        	}
        	return seed;
        }
    };
}

// compile time choosing between (x >= min && x <= max) and (x == min) if min and max are the same
template <int min, int max, std::enable_if_t<min != max, bool> = true>
static inline bool isInRange(int x) {
	return x >= min && x <= max;
}

template <int min, int max, std::enable_if_t<min == max, bool> = true>
static inline bool isInRange(int x) {
	return x == min;
}

template<unsigned int stayActiveMin = 1, unsigned int stayActiveMax = 2, unsigned int becomeActiveMin = 2, unsigned int becomeActiveMax = 2>
class FloorTileSimulator {
	public:
		typedef std::vector<int> coordinate;
	private:
		typedef std::unordered_set<coordinate> state;

		state actives;
		coordinate min;
		coordinate max;

		int countBlacksInArea(const coordinate & coord) const {
			int count = 0;
			for(int y = coord[1] - 1; y <= coord[1] + 1; ++y) {
				int factor = (((y - coord[1]) % 2) != 0) ? 1 : 2;
				for(int x = coord[0] - factor; x <= coord[0] + factor; x += 2) {
					if(actives.find({ x, y }) != actives.end()) {
						++count;
					}
				}
			}
			return count;
		}

		inline void adjustMinMax(const coordinate & coord, coordinate & nmin, coordinate & nmax) const {
			for(int i = 0; i < 2; ++i) {
				if(coord[i] < nmin[i]) {
					nmin[i] = coord[i];
				}
				if(coord[i] > nmax[i]) {
					nmax[i] = coord[i];
				}
			}
		}

		inline void finalizeMinmax(coordinate & nmin, coordinate & nmax) const {
			if(((nmin[0] % 2) != 0) != ((nmin[1] % 2) != 0)) {
				--nmin[0];
			}
			if(((nmax[0] % 2) != 0) != ((nmax[1] % 2) != 0)) {
				++nmax[0];
			}
		}

		inline void simulate(state & newActives, coordinate & nmin, coordinate & nmax) const {
			bool isMinOffset = (min[1] % 2) != 0;
			for(int y = min[1] - 1; y <= max[1] + 1; ++y) {
				int factor = (isMinOffset != ((y % 2) != 0)) ? 1 : 0;
				for(int x = min[0] - factor - 2; x <= max[0] + factor + 2; x += 2) {
					coordinate position({ x, y });
					int nearby = countBlacksInArea(position);
					if(actives.find(position) != actives.end()) {
						// if the current cell is active the nearby count is inflated by one, as the area-search function does not ignore the position to be searched around - luckily we can optimize this at compile-time
						if(isInRange<stayActiveMin + 1, stayActiveMax + 1>(nearby)) {
							newActives.insert(position);
							adjustMinMax(position, nmin, nmax);
						}
					} else if(isInRange<becomeActiveMin, becomeActiveMax>(nearby)) {
						newActives.insert(position);
						adjustMinMax(position, nmin, nmax);
					}
				}
			}

			finalizeMinmax(nmin, nmax);
		}

	public:
		int countBlacks() const {
			return actives.size();
		}

		FloorTileSimulator(state initiallyActives = {}) : actives { initiallyActives } {
			min = coordinate(2, std::numeric_limits<int>::max());
			max = coordinate(2, std::numeric_limits<int>::min());

			for(const coordinate & coord : actives) {
				if(coord.size() != 2) {
					throw std::invalid_argument("invalid dimension");
				}

				adjustMinMax(coord, min, max);
			}

			finalizeMinmax(min, max);
		}

		void simulate() {
			coordinate nmin(2, std::numeric_limits<int>::max());
			coordinate nmax(2, std::numeric_limits<int>::min());
			state newActives;

			simulate(newActives, nmin, nmax);

			actives = newActives;
			min = nmin;
			max = nmax;
		}
};

enum direction {
	e,
	w,
	se,
	sw,
	ne,
	nw
};

template<direction d>
struct dir {
};

// I'm not into copy-paste development - having the same logic sevral times inside my grammar with only slight changes is too repetitive. Generate that shit for me!
template<direction d>
struct rulegenerator {
	template<typename Iterator>
		static qi::rule<Iterator, std::pair<int, int>, qi::space_type> rule() {
			return qi::lit(dir<d>::str)[qi::_val = dir<d>::posi];
		}
};

#define DIR(d, x, y)\
	template<>\
	struct dir<direction::d> {\
		static std::string str;\
		static std::pair<int, int> posi;\
	};\
	std::pair<int, int> dir<direction::d>::posi = std::pair<int, int>(x, y);\
	std::string dir<direction::d>::str = "" #d;

// Following rules have to apply to map our 2-dimensional 6-neighbor space into a 2-dimensional 4-neighbor space:
//   e + w = (0,0)
//   se + nw = (0,0)
//   sw + ne = (0,0)
//   se + ne = e
//   sw + nw = w
// commutativity of +-operation applies
// This leaves us with following move actions in our 2-dimensional 4-neighbor space
// As I do not plan to use a two-dimensional array or such nonsense but rather a set of coordinates it does not matter that 50% of all possible values will never be visited
DIR(e, 2, 0)
DIR(w, -2, 0)
DIR(se, 1, -1)
DIR(sw, -1, -1)
DIR(ne, 1, 1)
DIR(nw, -1, 1)

#undef DIR

static inline void merge(std::pair<int, int> & p0, const std::pair<int, int> & p1) {
	p0.first += p1.first;
	p0.second += p1.second;
}

template<typename Iterator>
struct PositionFinder : qi::grammar<Iterator, std::pair<int, int>, qi::space_type> {
	private:
		qi::rule<Iterator, std::pair<int, int>, qi::space_type> coordinate, direction, e, se, sw, w, nw, ne;

	public:
		PositionFinder() : PositionFinder::base_type(coordinate) {
			coordinate =
				direction[qi::_val = qi::_1]
				>> *(
					direction[ph::bind(&merge, qi::_val, qi::_1)]
				);
			direction =
				e
				| se
				| sw
				| w
				| nw
				| ne;

			e  = rulegenerator<direction::e>::rule<Iterator>();
			w  = rulegenerator<direction::w>::rule<Iterator>();

			se = rulegenerator<direction::se>::rule<Iterator>();
			sw = rulegenerator<direction::sw>::rule<Iterator>();

			ne = rulegenerator<direction::ne>::rule<Iterator>();
			nw = rulegenerator<direction::nw>::rule<Iterator>();
		}
};

template<typename R, typename G>
static inline std::pair<int, int> evaluate(std::string::iterator begin, const std::string::iterator end, const G & grammar) {
	R value;
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

	std::unordered_set<FloorTileSimulator<>::coordinate> coords;

	PositionFinder<std::string::iterator> finder;
	for(std::string line; std::getline(file, line);) {
		std::pair<int, int> pos = evaluate<std::pair<int, int>>(line.begin(), line.end(), finder);

		FloorTileSimulator<>::coordinate coord({ pos.first, pos.second });
		std::unordered_set<FloorTileSimulator<>::coordinate>::iterator it2 = coords.find(coord);
		if(it2 != coords.end()) {
			coords.erase(it2);
		} else {
			coords.insert(coord);
		}
	}

	file.close();

	std::cout << "result 1: " << coords.size() << std::endl;

	FloorTileSimulator<> simulator(coords);
	for(int i = 0; i < 100; ++i) {
		simulator.simulate();
	}

	std::cout << "result 2: " << simulator.countBlacks() << std::endl;

	return 0;
}

