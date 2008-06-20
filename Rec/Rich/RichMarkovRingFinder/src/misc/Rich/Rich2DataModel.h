
#ifndef LESTER_Rich2DataModel_H
#define LESTER_Rich2DataModel_H

// includes
#include <iostream>
#include <string>
#include <map>
#include "Rich/EventDescription.fwd"
#include "Rich/Data.fwd"
#include "Rich/CircleParams.fwd"
#include "Rich/Hit.fwd"
#include "Rich/Small2Vector.fwd"
#include "Rich/RichDataModel.h"

namespace Lester 
{

  // declaration
  class Rich2DataModel : public RichDataModel
  {

  public:

    /// Default constructor
    Rich2DataModel();

    /// Setup the parameters for each concrete implementation
    void initialise();

  };

}

#endif

