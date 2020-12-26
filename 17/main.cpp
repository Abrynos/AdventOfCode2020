#include <iostream>
#include <fstream>
#include <unordered_set>
#include <limits>
#include <stdexcept>
#include <vector>

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

template<unsigned int dimensions, unsigned int stayActiveMin = 2, unsigned int stayActiveMax = 3, unsigned int becomeActiveMin = 3, unsigned int becomeActiveMax = 3, std::enable_if_t<(dimensions > 0), bool> = true>
class CubeDimension {
	public:
		typedef std::vector<int> coordinate;
	private:
		typedef std::unordered_set<coordinate> state;

		state actives;
		coordinate min;
		coordinate max;

		int countActivesInArea(coordinate coord, const int position = 0) const {
			if(position == dimensions) {
				return (actives.find(coord) != actives.end())
					? 1
					: 0;
			}

			int start = coord[position] - 1;
			int end = coord[position] + 1;
			const int nPos = position + 1;

			int result = 0;
			for(int i = start; i <= end; ++i) {
				coord[position] = i;
				result += countActivesInArea(coord, nPos);
			}

			return result;
		}

		void simulate(state & newActives, coordinate position, int index = 0) const {
			if(index == dimensions) {
				int nearby = countActivesInArea(position);
				if(actives.find(position) != actives.end()) {
					// if the current cell is active the nearby count is inflated by one, as the recursive function does not ignore the position to be searched around - luckily we can optimize this at compile-time
					if(isInRange<stayActiveMin + 1, stayActiveMax + 1>(nearby)) {
						newActives.insert(position);
					}
				} else if(isInRange<becomeActiveMin, becomeActiveMax>(nearby)) {
					newActives.insert(position);
				}

				return;
			}

			int start = min[index] - 1;
			int end = max[index] + 1;
			int nIdx = index + 1;

			for(int i = start; i <= end; ++i) {
				position[index] = i;
				simulate(newActives, position, nIdx);
			}
		}

		void resetLimits() {
			min = coordinate(dimensions, std::numeric_limits<int>::max());
			max = coordinate(dimensions, std::numeric_limits<int>::min());

			for(const coordinate & coord : actives) {
				for(int i = 0; i < dimensions; ++i) {
					if(coord[i] < min[i]) {
						min[i] = coord[i];
					}

					if(coord[i] > max[i]) {
						max[i] = coord[i];
					}
				}
			}
		}

	public:
		int countActives() const {
			return actives.size();
		}

		CubeDimension(state initiallyActives = {}) : actives { initiallyActives } {
			for(const coordinate & coord : actives) {
				if(coord.size() != dimensions) {
					throw std::invalid_argument("active coordinate dimension");
				}
			}

			resetLimits();
		}

		void simulate() {
			state newActives;
			simulate(newActives, coordinate(dimensions));
			actives = newActives;
			resetLimits();
		}
};

int main() {
	std::ifstream file("input.txt");
	if(!file.is_open()) {
		std::cout << "Could not open file!" << std::endl;
		return 1;
	}

	std::unordered_set<CubeDimension<3>::coordinate> data;
	std::unordered_set<CubeDimension<4>::coordinate> bigData;

	int x = 0;
	for(std::string line; std::getline(file, line); ++x) {
		for(int y = 0; y < line.size(); ++y) {
			if(line[y] == '#') {
				data.insert({x, y, 0});
				bigData.insert({x, y, 0, 0});
			}
		}
	}

	file.close();

	CubeDimension<3> simpleDimension(data);
	for(int i = 0; i < 6; ++i) {
		simpleDimension.simulate();
	}

	std::cout << "result 1: " << simpleDimension.countActives() << std::endl;

	CubeDimension<4> bigDimension(bigData);
	for(int i = 0; i < 6; ++i) {
		bigDimension.simulate();
	}

	std::cout << "result 2: " << bigDimension.countActives() << std::endl;

	return 0;
}

