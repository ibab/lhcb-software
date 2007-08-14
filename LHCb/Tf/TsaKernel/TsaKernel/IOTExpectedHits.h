#ifndef _IOTExpectedHits_H
#define _IOTExpectedHits_H

#include <utility>
#include <vector>

#include "GaudiKernel/IAlgTool.h"

namespace LHCb{
  class OTChannelID;
}

static const InterfaceID IID_IOTExpectedHits( "IOTExpectedHits", 0, 0 );

namespace Tf
{
  namespace Tsa
  {
    class Parabola;
    class Line;

    /** @class IOTExpectedHits
     *
     *  How many OT hits do we expect in a given detector
     *
     *  @author M.Needham
     *  @date   31/05/2004
     */

    class IOTExpectedHits: virtual public IAlgTool {

    public:

      typedef std::pair<LHCb::OTChannelID,double> OTPair;

      /// Retrieve interface ID
      static const InterfaceID& interfaceID() { return IID_IOTExpectedHits ; }

      virtual StatusCode   collect(const Parabola& parab,
                                   const Line& line,
                                   const LHCb::OTChannelID& aChan,
                                   std::vector<IOTExpectedHits::OTPair>& hits,
                                   const unsigned int iSector) const = 0;

    };

  }
}


#endif // _IOTExpectedHits_H

