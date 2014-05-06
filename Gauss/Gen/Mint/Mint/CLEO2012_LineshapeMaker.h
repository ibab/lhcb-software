#ifndef CLEO2012_LINESHAPEMAKER_HH
#define CLEO2012_LINESHAPEMAKER_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:04 GMT

#include "Mint/ILineshape.h"
#include "Mint/AssociatedDecayTree.h"
#include "Mint/IDalitzEventAccess.h"

#include <string>

ILineshape* CLEO2012_LineshapeMaker(const AssociatedDecayTree* tree
				    , IDalitzEventAccess* events
				    , const std::string& lopt
				    );

#endif
//
