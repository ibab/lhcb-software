// $Id: ISiDepositedCharge.h,v 1.2 2007-02-26 10:34:05 cattanem Exp $
#ifndef _ISiDepositedCharge_H
#define _ISiDepositedCharge_H

#include "GaudiKernel/IAlgTool.h"

namespace LHCb{
 class MCHit;
};

/** @class ISiDepositedCharge ISiDepositedCharge.h Kernel/ISiDepositedCharge.h
 *
 *  Interface Class for estimating deposited charge in Silicon
 *
 *  @author M.Needham
 *  @date   13/3/2002
 */

// Declaration of the interface ID ( interface id, major version, minor version) 
static const InterfaceID IID_ISiDepositedCharge("ISiDepositedCharge", 0 , 0); 

class ISiDepositedCharge : virtual public IAlgTool {

public: 

  /** Static access to interface id */
  static const InterfaceID& interfaceID() { return IID_ISiDepositedCharge; }
   
  /** calculate deposited charge (in electrons)
  * @param  aHit hit 
  * @return deposited charge 
  */
  virtual double charge(const LHCb::MCHit* aHit) const=0;

};

#endif // _ISiDepositedCharge_H




