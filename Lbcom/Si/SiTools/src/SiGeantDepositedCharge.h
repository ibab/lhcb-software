// $Id: SiGeantDepositedCharge.h,v 1.1.1.1 2006-05-16 08:33:54 mneedham Exp $
#ifndef _SiGeantDepositedCharge_H
#define _SiGeantDepositedCharge_H


#include "Kernel/ISiDepositedCharge.h"

#include "GaudiAlg/GaudiTool.h"

/** @class SiGeantDepositedCharge SiGeantDepositedCharge.h
 *
 *  Trivial class to turn dE/dx from Geant into e-
 *
 *  @author M.Needham
 *  @date   13/3/2002
 */


class SiGeantDepositedCharge : public GaudiTool, virtual public ISiDepositedCharge {

public: 

  /** Constructer */
  SiGeantDepositedCharge(const std::string& type, 
              const std::string& name,
              const IInterface* parent);

  /** destructer */
  virtual ~SiGeantDepositedCharge();

  /** calculate deposited charge (in electrons)
  * @param hit 
  * @return deposited charge 
  */
  double charge(const LHCb::MCHit* aHit) const;

private:

  double m_scalingFactor;
 
};

#endif // _SiGeantDepositedCharge_H







