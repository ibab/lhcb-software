// $Id: CopyRegisterUnit.h,v 1.4 2005-02-03 19:49:51 atsareg Exp $

#ifndef L0MUONKERNEL_COPYREGISTERUNIT_H
#define L0MUONKERNEL_COPYREGISTERUNIT_H     1

/* class Unit Unit L0MuonKernel/Unit.h

   Class representing a unit of data processing logic 
   of the level-0 muon trigger
   which just copies its input to its output
   
   author  Andrei Tsaregorodtsev
   date  12 January 2002
*/ 

#include "ProcessorKernel/Unit.h"

namespace L0Muon {

class CopyRegisterUnit : public L0Muon::Unit {

public:

  /// Constructor 
  CopyRegisterUnit();

  /// Destructor
  virtual ~CopyRegisterUnit();
  
  /// Copy input into output
  void execute();

};

};  // namespace L0Muon

#endif      // L0MUONKERNEL_COPYREGISTERUNIT_H  
