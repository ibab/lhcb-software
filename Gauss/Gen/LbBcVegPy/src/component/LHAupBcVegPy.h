#ifndef LBBCVEGPY_LHAUPBCVEGPY_H 
#define LBBCVEGPY_LHAUPBCVEGPY_H 1

// Parton shower packages.
#include "LbPythia8/LHAFortran.h"
#include "LbHard/HardProduction.h"

/**
 * Pythia 8 Les Houches Accord (LHA) user process tool for BcVegPy.
 * 
 * Generates events via Pythia with the PYUPEV subroutine and then
 * passes these to Pythia 8 from the HEPEUP and HEPRUP common
 * blocks. Fixes the IDPRUP and ISTUP for the process.
 * 
 * @class  LHAupBcVegPy
 * @file   LHAupBcVegPy.h 
 * @author Philip Ilten
 * @date   2015-05-01
 */
namespace Pythia8 {
  class LHAupBcVegPy : public LHAupFortran {
  public:  

    /// Standard constructor.
    LHAupBcVegPy(HardProduction *hard, int meson = 541);

    /// Fill the HEPRUP common block.
    bool fillHepRup();
    
    /**
     * Fill the HEPEUP common block.
     *
     * From the derived LHAupFortran class the default idProcIn for
     * setEvent is 0 to which IDPRUP is then set. However, for BcVegPy
     * IDPRUP must be set to 1001. Additionally, the ISTUP (particle
     * status) is not always correctly set for the incoming gluons,
     * and so this is also changed manually. See arXiv:hep-ph/0109068
     * for the HEPEUP and HEPRUP standards.
     */
    bool fillHepEup();
    
    // Members.
    HardProduction *m_hard; ///< The hard production tool.
    int m_meson;            ///< The PDG ID of the meson state to produce.
  };
}

#endif // LBBCVEGPY_LHAUPBCVEGPY_H
