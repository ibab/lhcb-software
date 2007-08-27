// $Id: BankUtilities.cpp,v 1.1 2007-08-27 09:32:24 jucogan Exp $

#include "L0MuonKernel/BankUtilities.h"

void L0Muon::writeCandInRegister(CandRegisterHandler * handler,unsigned int data, int icand, int bankVersion) {
  switch (bankVersion) {
  case 0:
    handler->setCandPT(     ( data>>  0 ) & ( (1<<7 ) -1 ) , icand);
    handler->setCandCharge( ( data>>  7 ) & ( (1<<1 ) -1 ) , icand);
    handler->setCandRowM3(  ( data>>  8 ) & ( (1<<2 ) -1 ) , icand);
    handler->setCandColM3(  ( data>> 10 ) & ( (1<<5 ) -1 ) , icand);
    handler->setCandOffM2(  ( data>> 15 ) & ( (1<<4 ) -1 ) , icand);
    handler->setCandOffM1(  ( data>> 19 ) & ( (1<<4 ) -1 ) , icand);
    handler->setCandPU(     ( data>> 23 ) & ( (1<<2 ) -1 ) , icand);
    handler->setCandBoard(  ( data>> 25 ) & ( (1<<4 ) -1 ) , icand);
    handler->setCandQuarter(( data>> 29 ) & ( (1<<2 ) -1 ) , icand);    
    break;
  default:                            
    handler->setCandOffM1(  ( data>>  0 ) & ( (1<<4 ) -1 ) , icand);
    handler->setCandOffM2(  ( data>>  4 ) & ( (1<<4 ) -1 ) , icand);
    handler->setCandColM3(  ( data>>  8 ) & ( (1<<5 ) -1 ) , icand);
    handler->setCandRowM3(  ( data>> 13 ) & ( (1<<2 ) -1 ) , icand);
    handler->setCandPT(     ( data>> 16 ) & ( (1<<7 ) -1 ) , icand);
    handler->setCandCharge( ( data>> 23 ) & ( (1<<1 ) -1 ) , icand);
    handler->setCandBoard(  ( data>> 24 ) & ( (1<<4 ) -1 ) , icand);
    handler->setCandPU(     ( data>> 28 ) & ( (1<<2 ) -1 ) , icand);
    handler->setCandQuarter(( data>> 30 ) & ( (1<<2 ) -1 ) , icand);    
    break;                            
  }
}

unsigned int L0Muon::readCandFromRegister(CandRegisterHandler * handler,int icand, int bankVersion) {
  unsigned int data=0;
  switch (bankVersion) {
  case 0:
    data += ( ( handler->getCandPT(     icand) & ( (1<<7 ) -1 ) ) <<  0 ) ;  
    data += ( ( handler->getCandCharge( icand) & ( (1<<1 ) -1 ) ) <<  7 ) ;  
    data += ( ( handler->getCandRowM3(  icand) & ( (1<<2 ) -1 ) ) <<  8 ) ;  
    data += ( ( handler->getCandColM3(  icand) & ( (1<<5 ) -1 ) ) << 10 ) ;  
    data += ( ( handler->getCandOffM2(  icand) & ( (1<<4 ) -1 ) ) << 15 ) ;  
    data += ( ( handler->getCandOffM1(  icand) & ( (1<<4 ) -1 ) ) << 19 ) ;  
    data += ( ( handler->getCandPU(     icand) & ( (1<<2 ) -1 ) ) << 23 ) ;  
    data += ( ( handler->getCandBoard(  icand) & ( (1<<4 ) -1 ) ) << 25 ) ;  
    data += ( ( handler->getCandQuarter(icand) & ( (1<<2 ) -1 ) ) << 29 ) ;    
    break;
  default:                               
    data += ( ( handler->getCandOffM1(  icand) & ( (1<<4 ) -1 ) ) <<  0 ) ;  
    data += ( ( handler->getCandOffM2(  icand) & ( (1<<4 ) -1 ) ) <<  4 ) ;  
    data += ( ( handler->getCandColM3(  icand) & ( (1<<5 ) -1 ) ) <<  8 ) ;  
    data += ( ( handler->getCandRowM3(  icand) & ( (1<<2 ) -1 ) ) << 13 ) ;  
    data += ( ( handler->getCandPT(     icand) & ( (1<<7 ) -1 ) ) << 16 ) ;  
    data += ( ( handler->getCandCharge( icand) & ( (1<<1 ) -1 ) ) << 23 ) ;  
    data += ( ( handler->getCandBoard(  icand) & ( (1<<4 ) -1 ) ) << 24 ) ;  
    data += ( ( handler->getCandPU(     icand) & ( (1<<2 ) -1 ) ) << 28 ) ;  
    data += ( ( handler->getCandQuarter(icand) & ( (1<<2 ) -1 ) ) << 30 ) ; 
    break;                              
  }
  return data;
}

