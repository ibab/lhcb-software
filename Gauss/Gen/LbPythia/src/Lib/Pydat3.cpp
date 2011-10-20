// $Id: Pydat3.cpp,v 1.1.1.2 2009-08-05 09:43:37 ngueissa Exp $
// access pythia common Pydat3
#include "LbPythia/Pydat3.h"

#include <cstdlib>

// set pointer to zero at start
Pydat3::PYDAT3* Pydat3::s_pydat3 =0;

// Constructor
Pydat3::Pydat3() : m_dummy( 0 ) , m_realdummy( 0. ) { }

// Destructor
Pydat3::~Pydat3() { }

// access mdcy in common
int& Pydat3::mdcy(int kc, int i) {
  init(); // check COMMON is initialized
  if( kc < 1 || kc > lenMdcy() || i  < 1 || i  > depthMdcy()) {
    m_dummy = -999;
    return m_dummy;
  }
  return s_pydat3->mdcy[i-1][kc-1];
}

// access mdme in common
int& Pydat3::mdme(int idc, int i) {
  init(); // check COMMON is initialized
  if( idc < 1 || idc > lenMdme() || i  < 1 || i  > depthMdme()) {
    m_dummy = -999;
    return m_dummy;
  }
  return s_pydat3->mdme[i-1][idc-1];
}

// access brat in common
double& Pydat3::brat(int idc) {
  init(); // check COMMON is initialized
  if (idc < 1 || idc > lenBrat()) {
    m_realdummy = -999.;
    return m_realdummy;
  }
  return s_pydat3->brat[idc-1]; 
}

// access kfdp in common
int& Pydat3::kfdp(int idc, int kf) {
  init(); // check COMMON is initialized
  if( idc < 1 || idc > lenKfdp() || kf  < 1 || kf  > depthKfdp()) {
    m_dummy = -999;
    return m_dummy;
  }
  return s_pydat3->kfdp[kf-1][idc-1];
}

