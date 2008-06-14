
#ifndef LESTER_RichSampler_H
#define LESTER_RichSampler_H 1

// includes
#include <iostream>

#include "GenericRingFinder/GenericRingFinder.h"
#include "GenericRingFinder/GenericResults.fwd"

#include "Rich/EventDescription.fwd"
#include "Rich/ThreePointCircleProposerB.fwd"
#include "Rich/Data.fwd"
#include "Rich/RichInferrer.h"

#include "boost/shared_ptr.hpp"

namespace Lester
{

  // declaration
  template < class DATAMODEL >
  class RichSampler : public GenRingF::GenericRingFinder
  {

  private:

    /// The data model
    boost::shared_ptr<DATAMODEL> m_ntrm;

  public:

    /// Constructor
    RichSampler( ) : m_ntrm ( new DATAMODEL() ) { }

    /// Run the ring finding on the given input data
    virtual boost::shared_ptr<GenRingF::GenericResults> 
    fit ( const GenRingF::GenericInput & input ) throw (CouldNotFit);
    
  private:
  
    /// Do the main sampling work
    void doTheWork( Lester::EventDescription & currentPoint,
                    double & currentLogProb,
                    Lester::ThreePointCircleProposerB<DATAMODEL> & p,
                    const DATAMODEL & ntrm,
                    const Lester::Data & data);

  public:

    /// Print this object to ostream
    std::ostream & printMeTo(std::ostream & os) const;
    
    /// Overload ostream operator <<
    friend std::ostream & operator<<(std::ostream & os, const RichSampler<DATAMODEL> & obj)
    {
      return obj.printMeTo(os);
    }
  
  };

}

#endif

