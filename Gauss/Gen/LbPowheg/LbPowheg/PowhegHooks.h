#include "powheg.h"
using namespace Pythia8;

// Transverse momentum of emissions and vetoing (modified from main71.cc).
class PowhegHooks : public UserHooks {
  
public:  

   // Constructor and destructor.
  PowhegHooks();
  ~PowhegHooks();

  // Use VetoMIStep to analyse the incoming LHEF event and
  // extract the veto scale
  bool canVetoMIStep();
  int  numberVetoMIStep();
  bool doVetoMIStep(int, const Event &e);

  // For subsequent ISR/FSR emissions, find the pT of the shower
  // emission and veto as necessary
  bool canVetoISREmission();
  bool doVetoISREmission(int, const Event &e);
  bool canVetoFSREmission();
  bool doVetoFSREmission(int, const Event &e);

  // Functions to return information
  double getPTpowheg();
  double getPTshower();
  int    getNISRveto();
  int    getNFSRveto();
  bool   getNoRad();

private:

  double pTveto, pTpowheg, pTshower;
  int    nISRveto, nFSRveto;
  bool   noRad, firstNoRad;
};
