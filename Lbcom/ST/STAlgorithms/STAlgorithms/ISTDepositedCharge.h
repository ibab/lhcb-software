// $Id: ISTDepositedCharge.h,v 1.1.1.1 2005-12-19 15:43:16 mneedham Exp $
#ifndef _ISTDepositedCharge_H
#define _ISTDepositedCharge_H

#include "GaudiKernel/IAlgTool.h"

namespace LHCb{
 class MCHit;
};

/** @class ISTDepositedCharge ISTDepositedCharge.h ITAlgorithms/ISTDepositedCharge.h
 *
 *  Interface Class for estimating response of amplifer in ST Channel
 *
 *  @author M.Needham
 *  @date   13/3/2002
 */

// Declaration of the interface ID ( interface id, major version, minor version) 
static const InterfaceID IID_ISTDepositedCharge("ISTDepositedCharge", 0 , 0); 

class ISTDepositedCharge : virtual public IAlgTool {

public: 

  /// Static access to interface id
  static const InterfaceID& interfaceID() { return IID_ISTDepositedCharge; }
   
  /// calc noise
  virtual double charge(const LHCb::MCHit* aHit) const=0;

};

#endif // _ISTDepositedCharge_H




