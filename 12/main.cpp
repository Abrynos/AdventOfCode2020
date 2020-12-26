#include <iostream>
#include <fstream>
#include <cmath>

int main() {
	std::ifstream file("input.txt");
	if(!file.is_open()) {
		std::cout << "Could not open file!" << std::endl;
		return 1;
	}
	
	int hPos = 0;
	int vPos = 0;
	int orientation = 0;

	int whPos = 10;
	int wvPos = 1;
	int shPos = 0;
	int svPos = 0;

	char action;
	int distance;
	while(file >> action >> distance) {
		switch(action) {
			case 'S':
				distance *= -1;
			case 'N':
				vPos += distance;
				wvPos += distance;
				break;

			case 'W':
				distance *= -1;
			case 'E':
				hPos += distance;
				whPos += distance;
				break;

			case 'L':
				distance = 360 - distance;
			case 'R':
				if((distance % 90) != 0) {
					std::cout << "rotation is invalid: " << distance << std::endl;
					return 1;
				}

				for(int i = 0; i < distance / 90; ++i) {
					int tmp;
					tmp = whPos * -1;
					whPos = wvPos;
					wvPos = tmp;
				}

				orientation += distance;
				if(orientation < 0) {
					orientation += 360;
				}
				orientation %= 360;
				break;

			case 'F':
				shPos += whPos * distance;
				svPos += wvPos * distance;

				if((orientation % 90) != 0) {
					std::cout << "angle is invalid: " << orientation << std::endl;
					return 1;
				}

				switch(orientation / 90) {
					case 0:
						hPos += distance;
						break;
					case 1:
						vPos += distance;
						break;
					case 2:
						hPos -= distance;
						break;
					case 3:
						vPos -= distance;
						break;
				}
				break;
		}
	}

	file.close();

	std::cout << "result 1: " << (std::abs(hPos) + std::abs(vPos)) << std::endl;
	std::cout << "result 2: " << (std::abs(shPos) + std::abs(svPos)) << std::endl;

	return 0;
}

