#ifndef RANDOM_H
#define RANDOM_H

//some random functions that will be used in other functions

#include <cstdint>
#include <random>
#include <vector>

namespace cultEvoResolution {

	typedef std::mt19937 RandomNumberGenerator;

	// Create a random number generator initialized with a seed. If seed is empty
	// initialize with a random device.
	RandomNumberGenerator create_random_number_generator(const std::vector<int> &seed);

	double getRandomUniform(RandomNumberGenerator &rng);

	int getRandom(int N, RandomNumberGenerator &rng);

	double getRandomNormal(double mean, double sd, RandomNumberGenerator &rng);
}


#endif //RANDOM_H
