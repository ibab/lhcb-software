// $Id: CopyRegisterUnit.h,v 1.2 2004-03-11 15:57:55 ltocco Exp $

#ifndef L0MUONKERNEL_COPYREGISTERUNIT_H
#define L0MUONKERNEL_COPYREGISTERUNIT_H     1

/* class Unit Unit L0MuonKernel/Unit.h

   Class representing a unit of data processing logic 
   of the L0Muon Trigger for hardware simulations
   which just copies its input to its output
   
   author  Andrei Tsaregorodtsev
   date  12 January 2002
*/ 

#include "L0MuonKernel/Unit.h"

namespace L0Muon {

class CopyRegisterUnit : public L0Muon::Unit {

public:
  CopyRegisterUnit();
  virtual ~CopyRegisterUnit();
  
  void execute();

};

};  // namespace L0Muon

#endif      // L0MUONKERNEL_COPYREGISTERUNIT_H  
