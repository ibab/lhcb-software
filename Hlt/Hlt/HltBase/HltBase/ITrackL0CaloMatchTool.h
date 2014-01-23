// $Id: ITrackL0CaloMatchTool.h,v 1.4 2008-06-23 07:25:09 graven Exp $
#ifndef HLTBASE_ITRACKL0CALOMATCHTOOL_H
#define HLTBASE_ITRACKL0CALOMATCHTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// forward declarations
namespace LHCb {
class L0CaloCandidate;
class Track;
}

static const InterfaceID IID_ITrackL0CaloMatchTool( "ITrackL0CaloMatchTool", 1, 0 );

/** @class ITrackL0CaloMatchTool ITrackL0CaloMatchTool.h
 *HltBase/ITrackL0CaloMatchTool.h
 *
 *
 *  @author Jose Angel Hernando Morata
 *  @date   2007-01-11
 */
class ITrackL0CaloMatchTool : virtual public IAlgTool {
  public:
    // Return the interface ID
    static const InterfaceID &interfaceID() { return IID_ITrackL0CaloMatchTool; }

    virtual double match( const LHCb::Track &track,
                          const LHCb::L0CaloCandidate &calo ) = 0;
};
#endif // HLTBASE_ITRACKL0CALOMATCHTOOL_H
