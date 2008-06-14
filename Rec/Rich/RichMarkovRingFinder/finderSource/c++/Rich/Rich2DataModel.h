
#ifndef LESTER_Rich2DataModel_H
#define LESTER_Rich2DataModel_H

// fwd dec
#include "Rich2DataModel.fwd"

// includes
#include <iostream>
#include <string>
#include <map>
#include "EventDescription.fwd"
#include "Data.fwd"
#include "CircleParams.fwd"
#include "Hit.fwd"
#include "Small2Vector.fwd"
#include "RichDataModel.h"

namespace Lester 
{

  // declaration
  class Rich2DataModel : public RichDataModel
  {

  public:

    /// Default constructor
    Rich2DataModel();

  };

}

#endif

