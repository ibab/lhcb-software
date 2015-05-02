// LbPythia.
#include "LbPythia/Pythia.h"

// Local.
#include "LHAupBcVegPy.h"

//-----------------------------------------------------------------------------
//  Implementation file for class: LHAupBcVegPy
//
//  2015-05-01 : Philip Ilten
//-----------------------------------------------------------------------------

//=============================================================================
// Default constructor.
//=============================================================================
Pythia8::LHAupBcVegPy::LHAupBcVegPy(HardProduction *hard) {m_hard = hard;}

//=============================================================================
// Fill the HEPRUP common block.
//=============================================================================
bool Pythia8::LHAupBcVegPy::fillHepRup() {
  if (!m_hard || !m_hard->m_pythia) return false;
  return ((m_hard->hardInitializeGenerator()).isSuccess() && 
	  (m_hard->m_pythia->initializeGenerator()).isSuccess());
}

//=============================================================================
// Fill the HEPEUP common block.
//=============================================================================
bool Pythia8::LHAupBcVegPy::fillHepEup() {
  if (!m_hard || !m_hard->m_pythia) return false;
  hepeup_.idprup = 1001;
  ::Pythia::PyUpev();
  hepeup_.istup[0] = -1;
  hepeup_.istup[1] = -1;
  return true;
}

//=============================================================================
// The END.
//=============================================================================
