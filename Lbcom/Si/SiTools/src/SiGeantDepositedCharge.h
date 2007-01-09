// $Id: SiGeantDepositedCharge.h,v 1.2 2007-01-09 14:57:21 jvantilb Exp $
#ifndef SiGeantDepositedCharge_H
#define SiGeantDepositedCharge_H 1

// Gaudi
#include "GaudiAlg/GaudiTool.h"

// Interface from MCEvent
#include "Kernel/ISiDepositedCharge.h"

/** @class SiGeantDepositedCharge SiGeantDepositedCharge.h
 *
 *  Trivial class to turn dE/dx from Geant into e-
 *
 *  @author M.Needham
 *  @date   13/3/2002
 */

class SiGeantDepositedCharge : public GaudiTool, 
                               virtual public ISiDepositedCharge {

public: 

  /** Constructer */
  SiGeantDepositedCharge(const std::string& type, const std::string& name,
                         const IInterface* parent);

  /** destructer */
  virtual ~SiGeantDepositedCharge();

  /** calculate deposited charge (in electrons)
  * @param hit 
  * @return deposited charge 
  */
  double charge(const LHCb::MCHit* aHit) const;

private:

  // Job option
  double m_scalingFactor; ///< Scaling factor for the conversion into electrons
 
};

#endif // SiGeantDepositedCharge_H







