#include "ZooPackedStates.h"

const ZooPackedState& ZooPackedStates::closestState(double z) const
{
    unsigned k = 0;
    double dist = std::numeric_limits<double>::infinity();
    for (unsigned j = 0; j < size(); ++j) {
	const double d = std::abs(operator[](j).z() - z);
	if (d < dist) k = j, dist = d;
    }
    return operator[](k);
}

ClassImp(ZooPackedStates);
