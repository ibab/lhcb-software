// LbPythia.
#include "LbPythia/Pythia.h"

// Local.
#include "LHAupGenXicc.h"

//-----------------------------------------------------------------------------
//  Implementation file for class: LHAupGenXicc
//
//  2015-05-01 : Philip Ilten
//-----------------------------------------------------------------------------

//=============================================================================
// Default constructor.
//=============================================================================
Pythia8::LHAupGenXicc::LHAupGenXicc(HardProduction *hard) {m_hard = hard;}

//=============================================================================
// Fill the HEPRUP common block.
//=============================================================================
bool Pythia8::LHAupGenXicc::fillHepRup() {
  if (!m_hard || !m_hard->m_pythia) return false;
  return ((m_hard->hardInitializeGenerator()).isSuccess() && 
	  (m_hard->m_pythia->initializeGenerator()).isSuccess());
}

//=============================================================================
// Fill the HEPEUP common block.
//=============================================================================
bool Pythia8::LHAupGenXicc::fillHepEup() {
  if (!m_hard || !m_hard->m_pythia || !m_hard->m_pythia8) return false;
  hepeup_.idprup = 1001;
  ::Pythia::PyUpev();
  
  // Correct the initial state gluon directions.
  Pythia8::hepeup_.istup[0] = -1;
  Pythia8::hepeup_.istup[1] = -1;
  
  // Change the energy/masses to match the defined Pythia 8 masses.
  double diff = 0;
  for (int ip = 2; ip < hepeup_.nup; ++ip) {
    double m0(m_hard->m_pythia8->m_pythia->particleData.m0(hepeup_.idup[ip])), 
      px(hepeup_.pup[ip][0]), py(hepeup_.pup[ip][1]), pz(hepeup_.pup[ip][2]),
      e(sqrt(px*px + py*py + pz*pz + m0*m0));
    diff += e - hepeup_.pup[ip][3];
    hepeup_.pup[ip][3] = e;
    hepeup_.pup[ip][4] = m0;
  }
  
  // Absorb the energy imbalance in the incoming partons.
  if (diff == 0) return true;
  diff /= 2;
  for (int ip = 0; ip < hepeup_.nup && ip < 2; ++ip) {
    double e(hepeup_.pup[ip][3] + diff), efrac(e/hepeup_.pup[ip][3]);
    hepeup_.pup[ip][0] *= efrac;
    hepeup_.pup[ip][1] *= efrac;
    hepeup_.pup[ip][2] *= efrac;
    hepeup_.pup[ip][3]  = e;
  }
  return true;
}

//=============================================================================
// The END.
//=============================================================================
