// $Id: CandRegisterHandler.cpp,v 1.3 2008-07-11 15:29:08 jucogan Exp $

#include <cmath>
#include <vector>
#include "L0MuonKernel/CandRegisterHandler.h"

L0Muon::CandRegisterHandler::CandRegisterHandler() {
}

L0Muon::CandRegisterHandler::CandRegisterHandler(Register * candRegister) {
  m_candRegister = candRegister;  
}				 

void L0Muon::CandRegisterHandler::setMuonCandidate(L0Muon::PMuonCandidate pcand, int icand) {
  setCandOffM1(pcand->offM1(),icand);
  setCandOffM2(pcand->offM2(),icand);
  setCandColM3(pcand->colM3(),icand);
  setCandRowM3(pcand->rowM3(),icand);
  setCandPT(pcand->pT(),icand);
  setCandCharge(pcand->charge(),icand);
  setCandBoard(pcand->board(),icand);
  setCandPU(pcand->pu(),icand);
  setCandQuarter(pcand->quarter(),icand);  
}

L0Muon::PMuonCandidate  L0Muon::CandRegisterHandler::getMuonCandidate(int icand) {
  PMuonCandidate pcand(new MuonCandidate());
  
  pcand->setOffM1(getCandOffM1(icand));
  pcand->setOffM2(getCandOffM2(icand));
  pcand->setColM3(getCandColM3(icand));
  pcand->setRowM3(getCandRowM3(icand));
  pcand->setPT(getCandPT(icand));
  pcand->setCharge(getCandCharge(icand));
  pcand->setBoard(getCandBoard(icand));
  pcand->setPu(getCandPU(icand));
  pcand->setQuarter(getCandQuarter(icand));

  return pcand;  
}

void L0Muon::CandRegisterHandler::dump(int icand, std::string tab) {      
  if (icand==0 || icand==1) {        
    std::cout <<tab<<"icand="<<icand;
    if ( isEmpty(icand) ){
      std::cout << " X ";
    } else {
      std::cout << " Quarter= " <<getCandQuarter(icand);
      std::cout << " Pu= "      <<getCandPU(icand);
      std::cout << " Board= "   <<getCandBoard(icand);
      if (getCandCharge(icand)>0)
        std::cout << " + ";
      else
        std::cout << " - ";
      std::cout << " Pt= "      <<getCandPT(icand);
      std::cout << " AddM3= "   <<getCandAddM3(icand);
      std::cout << " OffM2= "   <<getCandOffM2(icand);
      std::cout << " OffM1= "   <<getCandOffM1(icand);
    }
    std::cout <<"\n";
  } else {
    dump(0,tab);
    dump(1,tab);
    std::cout <<tab<< "status= "<<getStatus()<<"\n";
  }
  
}

int L0Muon::CandRegisterHandler::numberOfCandidates(bool useStatus) {
  int ncand=0;
  if (!useStatus) {
    for (int icand=0; icand<2; icand++){
      if (!isEmpty(icand)) ncand++;
    }
  } else {
    int status = getStatus();
    ncand=status&0x3;
    ncand = ncand>2 ? 2 : ncand;
  }
  
  return ncand;
}  

