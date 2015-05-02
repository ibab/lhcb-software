#ifndef LBGENXICC_LHAUPGENXICC_H 
#define LBGENXICC_LHAUPGENXICC_H 1

// Parton shower packages.
#include "LbPythia8/LHAFortran.h"
#include "LbHard/HardProduction.h"

/**
 * Pythia 8 Les Houches Accord (LHA) user process tool for GenXicc.
 * 
 * Generates events via Pythia with the PYUPEV subroutine and then
 * passes these to Pythia 8 from the HEPEUP and HEPRUP common
 * blocks. Fixes the IDPRUP and ISTUP for the process as well as
 * ensuring consistent masses with Pythia 8.
 * 
 * @class  LHAupGenXicc
 * @file   GenXiccProduction.h 
 * @author Philip Ilten
 * @date   2015-05-01
 */
namespace Pythia8 {
  class LHAupGenXicc : public LHAupFortran {
  public:  

    /// Standard constructor.
    LHAupGenXicc(HardProduction *hard);

    /// Fills the HEPRUP common block.
    bool fillHepRup();
    
    /**
     * Fills the HEPEUP common block.
     *
     * From the derived LHAupFortran class the default idProcIn for
     * setEvent is 0 to which IDPRUP is then set. However, for GenXicc
     * IDPRUP must be set to 1001. Additionally, the ISTUP (particle
     * status) is not always correctly set for the incoming gluons,
     * and so this is also changed manually. See arXiv:hep-ph/0109068
     * for the HEPEUP and HEPRUP standards.
     *
     * GenXicc requires a specific mass scheme, and consequently, it
     * is not recommended to change masses within GenXicc. This causes
     * a mis-match between the defined Pythia 8 masses and the
     * generated hard process, so the hard process is corrected so the
     * masses match. This is done by absorbing any mass differences in
     * the energies of the outgoing particles, and then
     * re-distributing these to the two initial partons to ensure
     * momentum conservation.
     */
    bool fillHepEup();
    
    // Members.
    HardProduction *m_hard; ///< The hard production tool.
  };
}

#endif // LBGENXICC_LHAUPGENXICC_H
