// $Id: ICaloSeedTool.h,v 1.4 2010-01-06 08:46:59 albrecht Exp $
#ifndef HLTBASE_ICALOSEEDTOOL_H
#define HLTBASE_ICALOSEEDTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_ICaloSeedTool( "ICaloSeedTool", 1, 0 );

namespace LHCb {
class L0CaloCandidate;
class Track;
}

/** @class ICaloSeedTool ICaloSeedTool.h HltBase/ICaloSeedTool.h
 *
 *
 *  @author Johannes Albrecht
 *  @date   2007-07-03
 */
class ICaloSeedTool : virtual public IAlgTool {
  public:
    // Return the interface ID
    static const InterfaceID &interfaceID() { return IID_ICaloSeedTool; }

    virtual StatusCode makeTrack( const LHCb::L0CaloCandidate &eL0Cand,
                                  LHCb::Track &seedTrack ) = 0;
};
#endif // HLTBASE_ICALOSEEDTOOL_H
