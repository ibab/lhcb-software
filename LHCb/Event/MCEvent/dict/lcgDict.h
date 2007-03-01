// $Id: lcgDict.h,v 1.6 2007-03-01 09:29:53 mato Exp $
#ifndef MCEVENT_LCGDICT_H 
#define MCEVENT_LCGDICT_H 1

// Additional classes to be added to automatically generated lcgdict

// begin include files
#include "Event/MCTrackInfo.h"
#include "Event/MCParticle.h"

#include "Kernel/IForcedBDecayTool.h"
#include "Kernel/IMCDecayFinder.h"
#include "Kernel/IMCReconstructible.h"
#include "Kernel/IMuonPad2MCTool.h"
#include "Kernel/ISiDepositedCharge.h"
#include "Kernel/IVisPrimVertTool.h"

// end include files

namespace {
  struct _Instantiations {
    // begin instantiations
    std::vector<const LHCb::MCParticle*> _i1;
    // end instantiations
  };
}

#endif // MCEVENT_LCGDICT_H
