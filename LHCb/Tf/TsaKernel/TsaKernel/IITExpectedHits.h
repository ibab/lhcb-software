#ifndef _IITExpectedHits_H
#define _IITExpectedHits_H

#include <utility>
#include <vector>

#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IITExpectedHits( "IITExpectedHits", 0, 0 );

namespace LHCb{
  class STChannelID;
}

namespace Tf
{
  namespace Tsa
  {
    class Parabola;
    class Line;

    /** @class IITExpectedHits
     *
     *  How many IT hits do we expect in a given detector
     *
     *  @author M.Needham
     *  @date   31/05/2004
     */

    class IITExpectedHits: virtual public IAlgTool 
    {

    public:

      typedef std::pair<LHCb::STChannelID,double> ITPair;

      /// Retrieve interface ID
      static const InterfaceID& interfaceID() { return IID_IITExpectedHits ; }

      virtual StatusCode   collect(const Parabola& parab,
                                   const Line& line,
                                   const LHCb::STChannelID& aChan,
                                   std::vector<IITExpectedHits::ITPair>& hits,
                                   const unsigned int iSector) const = 0;

    };

  }
}

#endif // _IITExpectedHits_H
