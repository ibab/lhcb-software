// $Id: SiGeantDepositedCharge.h,v 1.4 2007-06-01 11:57:59 cattanem Exp $
#ifndef SiGeantDepositedCharge_H
#define SiGeantDepositedCharge_H 1

// Gaudi
#include "GaudiAlg/GaudiTool.h"

// Interface
#include "MCInterfaces/ISiDepositedCharge.h"

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

private:

  // Job option
  double m_scalingFactor; ///< Scaling factor for the conversion into electrons
 
};

#endif // SiGeantDepositedCharge_H







