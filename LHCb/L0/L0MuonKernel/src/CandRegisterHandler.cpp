// $Id: CandRegisterHandler.cpp,v 1.2 2007-08-27 09:32:24 jucogan Exp $

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

// void L0Muon::writeCandInRegister(CandRegisterHandler * handler,unsigned int data, int icand, int bankVersion) {
//   switch (bankVersion) {
//   case 0:
//     handler->setCandPT(     ( data>>  0 ) & ( (1<<7 ) -1 ) , icand);
//     handler->setCandCharge( ( data>>  7 ) & ( (1<<1 ) -1 ) , icand);
//     handler->setCandRowM3(  ( data>>  8 ) & ( (1<<2 ) -1 ) , icand);
//     handler->setCandColM3(  ( data>> 10 ) & ( (1<<5 ) -1 ) , icand);
//     handler->setCandOffM2(  ( data>> 15 ) & ( (1<<4 ) -1 ) , icand);
//     handler->setCandOffM1(  ( data>> 19 ) & ( (1<<4 ) -1 ) , icand);
//     handler->setCandPU(     ( data>> 23 ) & ( (1<<2 ) -1 ) , icand);
//     handler->setCandBoard(  ( data>> 25 ) & ( (1<<4 ) -1 ) , icand);
//     handler->setCandQuarter(( data>> 29 ) & ( (1<<2 ) -1 ) , icand);    
//     break;
//   default:                            // benefit from the fact that in the current version, the encoding 
//     handler->setCandWord(data,icand); // of the candidate in the register is the same as the one in the bank. 
//     break;                            // This shall not always be the case. Tempting but fragile ???          
//   }
// }

// unsigned int L0Muon::readCandFromRegister(CandRegisterHandler * handler,int icand, int bankVersion) {
//   unsigned int data=0;
//   switch (bankVersion) {
//   case 0:
//     data += ( ( handler->getCandPT(     icand) & ( (1<<7 ) -1 ) ) <<  0 ) ;  
//     data += ( ( handler->getCandCharge( icand) & ( (1<<1 ) -1 ) ) <<  7 ) ;  
//     data += ( ( handler->getCandRowM3(  icand) & ( (1<<2 ) -1 ) ) <<  8 ) ;  
//     data += ( ( handler->getCandColM3(  icand) & ( (1<<5 ) -1 ) ) << 10 ) ;  
//     data += ( ( handler->getCandOffM2(  icand) & ( (1<<4 ) -1 ) ) << 15 ) ;  
//     data += ( ( handler->getCandOffM1(  icand) & ( (1<<4 ) -1 ) ) << 19 ) ;  
//     data += ( ( handler->getCandPU(     icand) & ( (1<<2 ) -1 ) ) << 23 ) ;  
//     data += ( ( handler->getCandBoard(  icand) & ( (1<<4 ) -1 ) ) << 25 ) ;  
//     data += ( ( handler->getCandQuarter(icand) & ( (1<<2 ) -1 ) ) << 29 ) ;    
//     break;
//   default:                              // benefit from the fact that in the current version, the encoding      
//     data = handler->getCandWord(icand); // of the candidate in the register is the same as the one in the bank.
//     break;                              // This shall not always be the case. Tempting but fragile ???         
//   }
//   return data;
// }

