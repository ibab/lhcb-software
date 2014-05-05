#ifndef LINESHAPEMAKER_HH
#define LINESHAPEMAKER_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:04 GMT

#include "ILineshape.h"
#include "AssociatedDecayTree.h"
#include "IDalitzEventAccess.h"

#include <string>

ILineshape* LineshapeMaker(const AssociatedDecayTree* tree
			   , IDalitzEventAccess* events
			   , const std::string& lopt
			   );

#endif
//
