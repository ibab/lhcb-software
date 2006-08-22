// $Id: MuonCandidate.cpp,v 1.3 2006-08-22 13:29:34 jucogan Exp $

#include <cmath>
#include <vector>
#include "L0MuonKernel/MuonCandidate.h"

#include <iostream>

L0Muon::MuonCandidate::MuonCandidate() {
  m_addM3  =0;
  m_offM2  =0;
  m_offM1  =0;
  m_pu     =0;
  m_quarter=0;
  m_board  =0;
  m_pT     =0;
  m_charge =0;
}

L0Muon::MuonCandidate::MuonCandidate(const MuonCandidate& cand) {
  m_addM3   = cand.addM3();
  m_offM2   = cand.offM2();
  m_offM1   = cand.offM1();
  m_pu      = cand.pu();
  m_quarter = cand.quarter();
  m_board   = cand.board();
  m_pT      = cand.pT();
  m_charge  = cand.charge();
}


std::string L0Muon::MuonCandidate::dump()
{
  std::string cand="";
  std::string sbuf;
  char buf[16];

  cand += "|----- CAND -------|\n";
  sprintf(buf,"%4d",m_quarter );
  sbuf = buf;
  cand +=   "|   quarter = " + sbuf +" |\n";
  sprintf(buf,"%4d",m_board   );
  sbuf = buf;
  cand +=   "|   board   = " + sbuf +" |\n";
  sprintf(buf,"%4d",m_pu      );
  sbuf = buf;
  cand +=   "|   pu      = " + sbuf +" |\n";
  sprintf(buf,"%4d",m_addM3   );
  sbuf = buf;
  cand +=   "|   addM3   = " + sbuf +" |\n";
  sprintf(buf,"%4d",m_offM2   );
  sbuf = buf;
  cand +=   "|   offM2   = " + sbuf +" |\n";
  sprintf(buf,"%4d",m_offM1   );
  sbuf = buf;
  cand +=   "|   offM1   = " + sbuf +" |\n";
  sprintf(buf,"%4d",m_pT      );
  sbuf = buf;
  cand +=   "|   pt      = " + sbuf +" |\n";
  sprintf(buf,"%4d",m_charge  );
  sbuf = buf;
  cand +=   "|   charge  = " + sbuf +" |\n";
  cand += "|------------------|\n";
  
  return cand;
  
}
