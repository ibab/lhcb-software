#ifndef TSAKERNEL_ITSASEEDTRACKCNVTOOL_H 
#define TSAKERNEL_ITSASEEDTRACKCNVTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_ITsaSeedTrackCnvTool ( "ITsaSeedTrackCnvTool", 1, 0 );

/** @class ITsaSeedTrackCnvTool ITsaSeedTrackCnvTool.h TsaKernel/ITsaSeedTrackCnvTool.h
 *  
 *  Interface to tool which converts TsaSeedTracks to LHCb::Tracks 
 *
 *  @author Johannes Albrecht
 *  @date   2007-10-31
 */
namespace LHCb{
  class Track;
}

namespace Tf
{
  namespace Tsa
  {


    class SeedTrack;

    class ITsaSeedTrackCnvTool : virtual public IAlgTool {
    public: 

      // Return the interface ID
      static const InterfaceID& interfaceID() { return IID_ITsaSeedTrackCnvTool; }

      virtual StatusCode initialize() = 0;
  
      virtual LHCb::Track* convert(const SeedTrack* aTrack) const = 0;

    protected:

    private:

    };
  }
}

#endif // TSAKERNEL_ITSASEEDTRACKCNVTOOL_H
