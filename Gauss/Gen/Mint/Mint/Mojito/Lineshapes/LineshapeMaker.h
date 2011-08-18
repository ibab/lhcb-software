#ifndef LINESHAPEMAKER_HH
#define LINESHAPEMAKER_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:04 GMT

#include "Mint/Mojito/Lineshapes/ILineshape.h"
#include "Mint/Mojito/DecayTrees/AssociatedDecayTree.h"
#include "Mint/Mojito/DalitzEvents/IDalitzEventAccess.h"

#include <string>

ILineshape* LineshapeMaker(const AssociatedDecayTree* tree
			   , IDalitzEventAccess* events
			   , const std::string& lopt
			   );

#endif
//
