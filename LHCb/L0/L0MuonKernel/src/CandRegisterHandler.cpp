// $Id: CandRegisterHandler.cpp,v 1.1 2005-12-15 15:53:27 jucogan Exp $

#include <cmath>
#include <vector>
#include "L0MuonKernel/CandRegisterHandler.h"

L0Muon::CandRegisterHandler::CandRegisterHandler() {
}

L0Muon::CandRegisterHandler::CandRegisterHandler(Register * candRegister) {
  m_candRegister = candRegister;  
}				 

void L0Muon::CandRegisterHandler::setMuonCandidate(L0Muon::PMuonCandidate pcand, int icand)
{
  setCandAddM3(pcand->addM3(),icand);
  setCandOffM2(pcand->offM2(),icand);
  setCandOffM1(pcand->offM1(),icand);
  setCandBoard(pcand->board(),icand);
  setCandPU(pcand->pu(),icand);
  setCandPT(pcand->pT(),icand);
  setCandCharge(pcand->charge(),icand);

  
}

L0Muon::PMuonCandidate  L0Muon::CandRegisterHandler::getMuonCandidate(int icand)
{
  PMuonCandidate pcand(new MuonCandidate());
  
  pcand->setAddM3(getCandAddM3(icand));
  pcand->setOffM2(getCandOffM2(icand));
  pcand->setOffM1(getCandOffM1(icand));
  pcand->setBoard(getCandBoard(icand));
  pcand->setPu(getCandPU(icand));
  pcand->setPT(getCandPT(icand));
  pcand->setCharge(getCandCharge(icand));

  return pcand;
  
}



