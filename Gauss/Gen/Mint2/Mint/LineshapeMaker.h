#ifndef LINESHAPEMAKER_HH
#define LINESHAPEMAKER_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:04 GMT

#include "Mint/ILineshape.h"
#include "Mint/AssociatedDecayTree.h"

#include <string>
#include <vector>

ILineshape* LineshapeMaker(const AssociatedDecayTree* tree
			   , const std::string& lopt
			   , const std::vector<double>& numOptions 
			   = std::vector<double>()
			   );

#endif
//
