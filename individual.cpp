#include "individual.h"
#include "random.h"

namespace cultEvoResolution {

	//constructor of the individual
	Individual::Individual(const Culture culturalTrait, double changeTendency, double interactionTendencySame
		, double interactionTendencyOther) :
		_culturalTrait(culturalTrait), _changeTendency(changeTendency), _interactionTendencySame(interactionTendencySame),
		_interactionTendencyOther(interactionTendencyOther){};

	// individuals interact if their interaction tendency surpasses an
	// interaction threshold determined randomly in this version of simulation,
	// the same type individuals can interact but this does not result in any
	// change. Also we assume the same tendency to interact with each other,
	// i.e. it is not possible to have a situation where when type is more
	// willing to interact than the other
	bool Individual::didInteract(const Individual &partner1, const Individual &partner2, RandomNumberGenerator &rng) {
		const double interactionThreshold = getRandomUniform(rng); //a random interaction threshold
		if (partner1._culturalTrait != partner2._culturalTrait)
			return partner1._interactionTendencyOther >= interactionThreshold;
		else
			return partner1._interactionTendencySame*partner2._interactionTendencySame >= interactionThreshold;
	}

	//the function that determines whether the individuals change their traits
	//or not in case of interaction
	void Individual::changeTrait(Individual &focal, Individual &partner, RandomNumberGenerator &rng) {
		if (didInteract(focal, partner, rng) == true &&
			focal._culturalTrait != partner._culturalTrait) {
			double changeThreshold = getRandomUniform(rng);
			if (focal._changeTendency > changeThreshold) {
				focal = partner;
			}
		}
	}

	//the function to return the culture of a given individual
	int Individual::getCulture() const {
		return _culturalTrait;
	}

}