#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_set>
#include <algorithm>

namespace std {
	template<>
	struct hash<vector<unsigned int>> {
		size_t operator() (const vector<unsigned int> & v) const {
			hash<unsigned int> hasher;
			size_t seed = 0;
			for(unsigned int i : v) {
				seed ^= hasher(i) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
			}
			return seed;
		}
	};

	template<>
	struct hash<pair<vector<unsigned int>, vector<unsigned int>>> {
		size_t operator() (const pair<vector<unsigned int>, vector<unsigned int>> & p) const {
			hash<vector<unsigned int>> hasher;
			size_t seed = 0;
			seed ^= hasher(p.first) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
			seed ^= hasher(p.second) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
			return seed;
		}
	};
}

class player {
	private:
		int id = -1;
		std::vector<unsigned int> cards;
		friend std::istream & operator >> (std::istream & is, player & p);

	public:
		player() { }
		player(const player & p, unsigned int n) {
			id = p.id;
			for(int i = 1; i <= n; ++i) {
				cards.push_back(p.cards[i]);
			}
		}
		int getId() const {
			return id;
		}
		bool lost() const {
			return cards.empty();
		}
		unsigned int peek() const {
			return cards[0];
		}
		void addCard(unsigned int card) {
			cards.push_back(card);
		}
		long countCards() const {
			return cards.size();
		}
		unsigned int pop() {
			unsigned int res = peek();
			cards.erase(cards.begin());
			return res;
		}
		long getScore() const {
			long score = 0;
			for(int i = 1; i <= cards.size(); ++i) {
				score += i * cards[cards.size() - i];
			}

			return score;
		}
		std::vector<unsigned int> getCards() const {
			return cards;
		}
};

std::istream & operator >> (std::istream & is, player & p) {
	is.ignore(7, ' ');// "Player "
	player result;
	if(!(is >> result.id)) {
		return is;
	}

	is.ignore(2, '\n');// ":\n"
	for(std::string line; std::getline(is, line) && !line.empty() && !is.eof();) {
		result.cards.push_back(std::stoi(line));
	}

	p = result;
	return is;
}

static inline long combat(player p0, player p1) {
	while(true) {
		unsigned int c0 = p0.pop();
		unsigned int c1 = p1.pop();

		if(c0 > c1) {
			p0.addCard(c0);
			p0.addCard(c1);
		} else {
			p1.addCard(c1);
			p1.addCard(c0);
		}

		if(p0.lost()) {
			return p1.getScore();
		}
		if(p1.lost()) {
			return p0.getScore();
		}
	}
}

// forward declaration for indirect recursion
static inline player playRecursiveCombat(player p0, player p1);

static inline player playRecursiveCombatRound(const player & p0, const player & p1) {
	unsigned int c0 = p0.peek();
	unsigned int c1 = p1.peek();

	if(c0 < p0.countCards() && c1 < p1.countCards()) {
		return playRecursiveCombat(player(p0, c0), player(p1, c1));
	}

	return c0 > c1 ? p0 : p1;
}

static inline player playRecursiveCombat(player p0, player p1) {
	typedef std::vector<unsigned int> hand;
	std::unordered_set<std::pair<hand, hand>> memory;

	while(!p0.lost() && !p1.lost()) {
		std::pair<hand, hand> hands(p0.getCards(), p1.getCards());
		if(memory.find(hands) != memory.end()) {
			return p0.getId() == 1 ? p0 : p1;
		}

		memory.insert(hands);

		player w = playRecursiveCombatRound(p0, p1);
		if(w.getId() == p0.getId()) {
			p0.addCard(p0.pop());
			p0.addCard(p1.pop());
		} else {
			p1.addCard(p1.pop());
			p1.addCard(p0.pop());
		}
	}

	return p0.lost() ? p1 : p0;
}

int main() {
	std::ifstream file("input.txt");
	if(!file.is_open()) {
		std::cout << "Could not open file!" << std::endl;
		return 1;
	}

	player p0, p1;
	file >> p0 >> p1;

	file.close();

	std::cout << "result 1: " << combat(p0, p1) << std::endl;

	std::cout << "result 2: " << playRecursiveCombat(p0, p1).getScore() << std::endl;

	return 0;
}

