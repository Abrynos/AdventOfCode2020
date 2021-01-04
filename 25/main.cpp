#include <iostream>
#include <fstream>

typedef size_t encryptionkey;

template<unsigned long divisor>
struct DoorAttacker {
	private:
		static inline unsigned int findLoopSize(const encryptionkey key, const encryptionkey subject) {
			int i = 0;
			for(encryptionkey value = 1; value != key; value = (value * subject) % divisor, ++i);

			return i;
		}

		static inline encryptionkey apply(const encryptionkey subject, const unsigned int loopSize) {
			encryptionkey key = 1;
			for(int i = 0; i < loopSize; ++i) {
				key = (key * subject) % divisor;
			}

			return key;
		}

	public:
		static inline encryptionkey crack(const encryptionkey publicKey0, const encryptionkey publicKey1, const int subject) {
			int loopSize = findLoopSize(publicKey0, subject);
			return apply(publicKey1, loopSize);
		}
};

int main() {
	std::ifstream file("input.txt");
	if(!file.is_open()) {
		std::cout << "Could not open file!" << std::endl;
		return 1;
	}

	encryptionkey k0;
	file >> k0;

	encryptionkey k1;
	file >> k1;

	file.close();

	encryptionkey result0 = attacker<20201227>::crack(k0, k1, 7);

	std::cout << "result 1: " << result0 << std::endl;

	return 0;
}

