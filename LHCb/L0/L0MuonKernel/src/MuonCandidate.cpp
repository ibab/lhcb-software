// $Id: MuonCandidate.cpp,v 1.1 2005-12-15 15:53:27 jucogan Exp $

#include <cmath>
#include <vector>
#include "L0MuonKernel/MuonCandidate.h"

L0Muon::MuonCandidate::MuonCandidate() {
  m_addM3 =0;
  m_offM2 =0;
  m_offM1 =0;
  m_pu    =0;
  m_board =0;
  m_pT    =0;
  m_charge=0;
}
