// $Id: BankUtilities.h,v 1.2 2008-11-07 16:23:38 jucogan Exp $

#ifndef L0MUONKERNEL_BANKUTILITIES_H
#define L0MUONKERNEL_BANKUTILITIES_H     1


#include <iostream>
#include "L0MuonKernel/CandRegisterHandler.h"

namespace L0Muon {

  /** @file  BankUtilities BankUtilities.h L0MuonKernel/BankUtilities.h

  Functions with behaviour that may change from a bank version to an other.

  @author Julien Cogan
  @date 2007 August the 27th
  */

  /// Used to read a candidate from a L0Muon bank and fill a register (CtrFinal::decodeBank* )
  void writeCandInRegister(CandRegisterHandler * handler,unsigned int data, int icand, int bankVersion);

  /// Used to read a candidate from a register and fill a L0Muon bank (CtrFinal::rawBank* )
  unsigned int readCandFromRegister(CandRegisterHandler * handler,int icand, int bankVersion);

} // namespace L0Muon

#endif      // L0MUONKERNEL_BANKUTILITIESH_H
