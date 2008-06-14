
#ifndef LESTER_EVENT_DESCRIPTION_H
#define LESTER_EVENT_DESCRIPTION_H 1

#include <iostream>
#include <vector>

#include "CircleParams.h"
#include "GenericRingFinder/GenericResults.h"
#include "GenericRingFinder/GenericInput.h"

#include <boost/shared_ptr.hpp>

namespace Lester
{

  class EventDescription
  {

  public:

    typedef std::vector<CircleParams> Circs;
    Circs circs;

  public:

    /// Default constructor
    EventDescription() { }

    /// Constructor from results
    EventDescription( const GenRingF::GenericResults & results )
    {
      for ( GenRingF::GenericResults::GenericRings::const_iterator it = results.rings.begin();
            it!=results.rings.end(); ++it )
      {
        circs.push_back(CircleParams(Small2Vector(it->x(), it->y()), it->radius()));
      }
    }

    /** The fill method fills the "GenRingF::GenericResults" structure with
     *  the rings which were found during the fitting procedure.
     */
    void fill ( GenRingF::GenericResults & results ) const;

  public:

    /// Print this object to ostream
    std::ostream & printMeTo(std::ostream & os) const;

    /// Overload ostream operator <<
    friend inline std::ostream & operator<< ( std::ostream & os,
                                              const Lester::EventDescription & rp)
    {
      return rp.printMeTo(os);
    }

  };

}

#endif
