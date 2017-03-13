#ifndef INDIVIDUAL_H
#define INDIVIDUAL_H

#include "random.h"

namespace cultEvoResolution {

	class Individual {
	public:
		enum Culture { A, B };
		Individual(Culture culturalTrait, double changeTendency, double interactionTendencySame,
			double interactionTendencyOther);
		static void changeTrait(Individual &partner1, Individual &partner2, RandomNumberGenerator &rng);
		int getCulture() const;
	protected:
		static bool didInteract(const Individual &partner1, const Individual &partner2, RandomNumberGenerator &rng);
		Culture _culturalTrait;
	private:
		double _changeTendency;
		double _interactionTendencySame;
		double _interactionTendencyOther;
	};
}
#endif //INDIVIDUAL_H
