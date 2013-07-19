#include "GaussCherenkov/RichPmtPSF.h"


RichPmtPSF::RichPmtPSF()
  : m_pmtPointSpreadFunctionVect(4),
    m_pmtPSFPhoEnergyVect(4),
    m_pmtPSFRadialPosVect(4)
 { ; }
// the 4 above is just a dummy value.

RichPmtPSF::~RichPmtPSF() { ; }

void RichPmtPSF::resetPSFVect() {

  m_pmtPointSpreadFunctionVect.clear();
  m_pmtPSFPhoEnergyVect.clear();
  m_pmtPSFRadialPosVect.clear();

}


