#include "random.h"

#include <algorithm>
#include <array>
#include <random>
#include <ctime>

// Random number generation algorithms

namespace cultEvoResolution {
	static std::mt19937 RAND_ENGINE(static_cast<unsigned int> (time(NULL)));

	RandomNumberGenerator create_random_number_generator(const std::vector<int> &seed) {
		if (seed.empty()) {
			std::array<int, RandomNumberGenerator::state_size> seed_data;
			std::random_device device;
			for (auto &x : seed_data)
				x = device();
			std::seed_seq seed_sequence(std::begin(seed_data), std::end(seed_data));
			return RandomNumberGenerator(seed_sequence);
		}
		else {
			std::seed_seq seed_sequence(seed.begin(), seed.end());
			return RandomNumberGenerator(seed_sequence);
		}
	}

	static std::mt19937 getRandomNumberGenerator() {
		std::array<int, std::mt19937::state_size> seed_data;
		std::random_device device;
		std::generate_n(seed_data.data(), seed_data.size(), std::ref(device));
		std::seed_seq seed_sequence(std::begin(seed_data), std::end(seed_data));
		return std::mt19937(seed_sequence);
	}

	/////
	double getRandomUniform(RandomNumberGenerator &rng) {
		static std::uniform_real_distribution<> distribution;
		return distribution(rng);
	}

	double getRandomNormal(const double mean, const double sd, RandomNumberGenerator &rng) {
		std::normal_distribution<double> distribution(mean, sd);
		return distribution(rng);
	}

	int getRandom(const int N, RandomNumberGenerator &rng) {
		return 0 + rng() % N;
	}

}
