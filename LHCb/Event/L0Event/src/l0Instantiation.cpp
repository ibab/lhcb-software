#include "GaudiKernel/Kernel.h"
#include "L0Event/Level0Calo2x2Sum.h"
#include "L0Event/Level0CaloClus.h"
#include "L0Event/Level0MuonTrack.h"
#include "L0Event/Level0PileUpVeto.h"

// Class IDs for the Level 0 detector start here
const CLID& CLID_Level0                 = 8000;  // 8001 -> 7999 reserved for Level 0 stuff
const CLID& CLID_Level0Calo2x2Sum       = 8001;  // FrontEnd Level 0 calorimeter 2x2 sum
const CLID& CLID_Level0CaloClus         = 8002;  // FrontEnd Level 0 calorimeter cluster candidates
const CLID& CLID_Level0MuonTrack        = 8003;  // FrontEnd Level 0 muon tracks
const CLID& CLID_Level0PileUpVeto       = 8004;  // FrontEnd Level 0 pileup veto decision


/// Create instance of everything
static void testLHCbEventL0Instances() {
  Level0Calo2x2Sum l0calo2x2sum;
  Level0CaloClus   l0caloclus;
  Level0MuonTrack  l0muontrack;
  Level0PileUpveto l0pileupveto;
}
