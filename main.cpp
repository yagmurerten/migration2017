////////////////////////////////////////////////////////////////////////////////
/// Migration & Acculturation version 2017 /////////////////////////////////////
/// by E.Yagmur Erten //////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#include "individual.h"
#include "random.h"

#include <climits>
#include <cmath>
#include <cstdio>
#include <ctime>
#include <fstream>
#include <iostream>
#include <omp.h>
#include <string>
#include <vector>

using namespace cultEvoResolution;

// the function that selects interaction partners from the population
static void interact(std::vector<Individual> &population, const int populationSize, RandomNumberGenerator &rng) {
	int n = getRandom(populationSize, rng);
	int m;
	do {
		m = getRandom(populationSize, rng);
	} while (m == n);
	Individual &focal = population[n];
	Individual &partner = population[m];
	Individual::changeTrait(focal, partner, rng);
}

// the function that selects one random individual from the population and 
// replaces it with an immigrant of type I
static void migration(std::vector<Individual> &population, const int populationSize, 
	const Individual &immigrant, RandomNumberGenerator &rng) {
	int n = getRandom(populationSize, rng);
	Individual &migrateAway = population[n];
	migrateAway = immigrant;
}

// checks how many individuals of type I there are
static int check(const std::vector<Individual> &population, const int populationSize) {
	int total = 0;
	for (int i = 0; i < populationSize; ++i) {
		total += population[i].getCulture(); //adds 1 if the individuals is an immigrant
	}
	return total;
}

// for this version we assume the tendencies of the individuals to interact with
// the same type are the same and does not affect the dynamics; they can
// interact but these interactions do not cause any change
double interactionTendencySame_A = 0.5;
double interactionTendencySame_B = 0.5;

// the parameters for our simulation
// migration rate ranges between 0.001 and 0.1
// changing and interaction tendencies range between 0.001 and 1.0
// note that changing tendency is 1 - cultural conservatism
class Parameters {
public:
	Parameters(
			const double change_tendency_A,
			const double change_tendency_B,
			const double migrationRate,
			const double interactionRate,
			const double interactionTendencyOther):
		_change_tendency_A(change_tendency_A),
		_change_tendency_B(change_tendency_B),
		_migrationRate(migrationRate),
		_interactionRate(interactionRate),
		_interactionTendencyOther(interactionTendencyOther){}

	const double _change_tendency_A;
	const double _change_tendency_B;
	const double _migrationRate;
	const double _interactionRate;
	const double _interactionTendencyOther;
};

static void parameterCombinations(std::vector<Parameters> &parameters) {
	const std::vector<double> valuesA{1.0};
	const std::vector<double> valuesB{0.0, 0.25, 0.5, 0.75, 1.0};
	const std::vector<double> values2a{ 0.01, 0.1, 0.25 };
	const std::vector<double> values2b{ 0.75, 0.9, 0.99 };
	const std::vector<double> values3{0.1,0.5};
	parameters.reserve(90);
			for (double change_tendency_A : valuesA)
				for (double change_tendency_B : valuesB)
					for (double migration: values2a)
						for(double interaction: values2b)
							for (double interactionTendencyOther : values3)
                				parameters.push_back(Parameters(change_tendency_A,
								change_tendency_B, migration, interaction, interactionTendencyOther));
}

int main() {
	const int populationSize = 1000;

	const std::vector<int> seed;
	RandomNumberGenerator rng = create_random_number_generator(seed);

	std::vector<Parameters> parameters;
	parameterCombinations(parameters);
    
    // 12 parallel runs
#pragma omp parallel for num_threads(12)
	for (int i = 0; i < parameters.size(); ++i) {
		std::cout << "\ncomb " << i << "\n";
		const Parameters &item = parameters[i];
		double MAX = 1e6/ (item._migrationRate + item._interactionRate); //endtime, i.e. every simulation has 1e6 events 
			double changeTendencyA = item._change_tendency_A;
			double changeTendencyB = item._change_tendency_B;
			const double migrationRate = item._migrationRate;
			const double interactionRate = item._interactionRate;
			double interactionTendencyOther_A = item._interactionTendencyOther;
			double interactionTendencyOther_B = item._interactionTendencyOther;
			std::string outputName = std::to_string(interactionTendencyOther_A) + "-1.csv";
			std::ofstream myOutput(outputName, std::ios_base::app);
			std::string outputName2 = std::to_string(interactionTendencyOther_A) + "-2.csv";
			std::ofstream myOutput2(outputName2, std::ios_base::app);
			for (int i = 0; i < 10; ++i) {
				int count = 0;
				std::vector<Individual> population;
				population.reserve(populationSize);
				const Individual individualA(Individual::Culture::A, changeTendencyA, interactionTendencySame_A, interactionTendencyOther_A);
				const Individual individualB(Individual::Culture::B, changeTendencyB, interactionTendencySame_B, interactionTendencyOther_B);
				for (int j = 0; j < populationSize; ++j) population.push_back(individualB);
				int total = check(population, populationSize);
				double ChangeTendDif = changeTendencyA - changeTendencyB;
				while (count < MAX) {
					++count;
					// in this part, there is now a change of the code to allow both immigration and interaction to occur with independent probs
					double probabilityMigrate = getRandomUniform(rng);
					double probabilityInteract = getRandomUniform(rng);
					double whichonefirst = getRandomUniform(rng); //we randomize the order of both events in each time step
					
					if (whichonefirst < 0.5)
					{
						if (probabilityMigrate <= migrationRate) {
							migration(population, populationSize, individualA, rng);
						}
						if (probabilityInteract <= interactionRate) {
							interact(population, populationSize, rng);
						}
					}
					else
					{
						if (probabilityInteract <= interactionRate) {
							interact(population, populationSize, rng);
						}
						if (probabilityMigrate <= migrationRate) {
							migration(population, populationSize, individualA, rng);
						}
					}
				}
				total = check(population, populationSize);
				double Freq = (static_cast<double>(1000-total))/1000;
#pragma omp critical
				{
                    myOutput << migrationRate << "," << interactionRate << "," << changeTendencyA << "," << changeTendencyB << "," << ChangeTendDif << "," << Freq << std::endl;
                    // we are interested in the cases where Delta c > 0, as we expect type I to fixate when Delta c < 0
					if (changeTendencyA >= changeTendencyB)
                        myOutput2 << migrationRate << "," << interactionRate << "," << changeTendencyA << "," << changeTendencyB << "," << ChangeTendDif << "," << Freq << std::endl;
				}
		}
	}
    return 0;
}
