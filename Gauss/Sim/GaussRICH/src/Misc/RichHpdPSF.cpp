#include "GaussRICH/RichHpdPSF.h"


RichHpdPSF::RichHpdPSF()
  : m_hpdPointSpreadFunctionVect(4),
    m_hpdPSFPhoEnergyVect(4),
    m_hpdPSFRadialPosVect(4),
    m_hitSmearValueVect(3)
 { ; }
// the 4 above is just a dummy value.

RichHpdPSF::~RichHpdPSF() { ; }

void RichHpdPSF::resetPSFVect() {

  m_hpdPointSpreadFunctionVect.clear();
  m_hpdPSFPhoEnergyVect.clear();
  m_hpdPSFRadialPosVect.clear();
  m_hitSmearValueVect.clear();
  
}


