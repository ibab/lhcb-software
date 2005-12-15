// $Id: L0MuonStatus.h,v 1.4 2005-12-15 15:53:26 jucogan Exp $


#ifndef L0MUONKERNEL_L0MUONSTATUS_H
#define L0MUONKERNEL_L0MUONSTATUS_H     1

/** @class Unit Unit.h L0MuonKernel/Unit.h

   Class representing a unit of data processing logic 
   of the level-0 muon trigger for hardware simulations
   
*/ 

namespace L0MuonStatus {

  //  enum Status {OK=1, EMPTY, ERROR, CRATE_EMPTY, 
  // CRATE_ERROR, PU_EMPTY, PU_ERROR, PU_OVERFLOW}; // L0Muon candidate status codes
  enum Status {NONE=0, ONE, TWO, PU_OVERFLOW}; // L0Muon candidate status codes

};  // namespace L0Muon

#endif      // L0MUONKERNEL_L0MUONSTATUS_H  
