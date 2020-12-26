#include <iostream>
#include <fstream>
#include <unordered_map>
#include <limits>
#include <algorithm>

template<typename T>
class node {
	public:
		T value;
		node<T>* next;

		node() = delete;
		node(const T & t, node<T>* n = nullptr) : value { t }, next { n } { }
};

template<typename T, std::enable_if_t<std::is_default_constructible<std::hash<T>>::value, bool> = true>
class hashlist {
	private:
		typedef std::unordered_map<T, node<T>*> index;
		index idx;
		node<T>* first = nullptr;
		node<T>* last = nullptr;

		void resetLast() {
			if(first == nullptr) {
				return;
			}

			if(last->next == first) {
				return;
			}

			// best case: it's the first one we find - worst case: it's the last one we find
			// following the node-pointers it would always be the last one we find
			for(typename index::iterator it = idx.begin(), end = idx.end(); it != end; ++it) {
				if(it->second->next == first) {
					last = it->second;
					break;
				}
			}
		}
	public:
		~hashlist() {
			resetLast();
			last->next = nullptr;
			last = nullptr;
			while(first != nullptr) {
				node<T>* n = first;
				first = first->next;
				delete n;
			}
			idx.clear();
		}

		node<T>* getFirst() const {
			return first;
		}

		node<T>* append(const T & value) {
			resetLast();

			node<T>* n = new node<T>(value, first);
			if(first == nullptr) {
				first = last = n;
			}
			last->next = n;
			last = n;
			idx[value] = n;
			return n;
		}

		node<T>* operator [](const T & value) {
			typename index::iterator it = idx.find(value);
			if(it == idx.end()) {
				return nullptr;
			}

			return it->second;
		}
};

static inline int getDestinationValue(int value, int min, int max) {
	--value;
	if(value < min) {
		value = max;
	}
	return value;
}

template<int steps>
static inline void doSteps(hashlist<int> & list, int min, int max) {
	node<int>* current = list.getFirst();
	for(int i = 0; i < steps; ++i) {
		node<int>* first = current->next;
		node<int>* last = first->next->next;
		current->next = last->next;

		int forbiddenNexts[3] { first->value, first->next->value, last->value };

		int destinationValue = getDestinationValue(current->value, min, max);
		node<int>* destination = list[destinationValue];
		while(destination == nullptr || destinationValue == forbiddenNexts[0] || destinationValue == forbiddenNexts[1] || destinationValue == forbiddenNexts[2]) {
			destinationValue = getDestinationValue(destinationValue, min, max);
			destination = list[destinationValue];
		}

		last->next = destination->next;
		destination->next = first;
		current = current->next;
	}
}

static inline std::string doPart1(hashlist<int> & list, int min, int max) {
	doSteps<100>(list, min, max);

	std::string result("");
	for(node<int>* n = list[1]->next; n->value != 1; n = n->next) {
		result = result.append(std::to_string(n->value));
	}

	return result;
}

static inline long doPart2(hashlist<int> & list, int min, int max) {
	doSteps<10000000>(list, min, max);

	node<int>* n = list[1]->next;
	long result = n->value;
	n = n->next;
	result *= n->value;

	return result;
}

int main() {
	std::ifstream file("input.txt");
	if(!file.is_open()) {
		std::cout << "Could not open file!" << std::endl;
		return 1;
	}

	// can't be bothered to implement assignment operators and copy constructors
	hashlist<int> list0;
	hashlist<int> list1;
	int min = std::numeric_limits<int>::max();
	int max = std::numeric_limits<int>::min();
	for(char ch; file >> ch;) {
		int value = ch - '0';
		list0.append(value);
		list1.append(value);
		if(value < min) {
			min = value;
		}
		if(value > max) {
			max = value;
		}
	}

	file.close();

	std::cout << "result 1: " << doPart1(list0, min, max) << std::endl;

#define ELEMENTS 1000000
	for(int i = max + 1; i <= ELEMENTS; ++i) {
		list1.append(i);
	}
	max = ELEMENTS;
#undef ELEMENTS
	std::cout << "result 2: " << doPart2(list1, min, max) << std::endl;

	return 0;
}

