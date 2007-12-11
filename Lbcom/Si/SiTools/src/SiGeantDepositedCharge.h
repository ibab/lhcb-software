// $Id: SiGeantDepositedCharge.h,v 1.5 2007-12-11 10:14:25 mneedham Exp $
#ifndef SiGeantDepositedCharge_H
#define SiGeantDepositedCharge_H 1

#include "SiDepositedChargeBase.h"


// Interface
#include "MCInterfaces/ISiDepositedCharge.h"

/** @class SiGeantDepositedCharge SiGeantDepositedCharge.h
 *
 *  Trivial class to turn dE/dx from Geant into e-
 * Allows for scaling + a smearing to simulate atomic binding
 *
 *  @author M.Needham
 *  @date   13/3/2002
 */

class SiGeantDepositedCharge : public SiDepositedChargeBase, 
                               virtual public ISiDepositedCharge {

public: 

  /** Constructor */
  SiGeantDepositedCharge(const std::string& type, const std::string& name,
                         const IInterface* parent);

  /** destructor */
  virtual ~SiGeantDepositedCharge();

  /** calculate deposited charge (in electrons)
  * @param  aHit hit 
  * @return deposited charge 
  */
  double charge(const LHCb::MCHit* aHit) const;
    
};

#endif // SiGeantDepositedCharge_H







