// $Id: MuonCandidate.cpp,v 1.2 2006-03-06 09:56:00 jucogan Exp $

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
