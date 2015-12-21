// $Id: SiDepositedCharge.h,v 1.6 2007-12-11 10:14:25 mneedham Exp $
#ifndef SiDepositedCharge_H
#define SiDepositedCharge_H 1

// Gaudi
#include "SiDepositedChargeBase.h"


// Interface
#include "MCInterfaces/ISiDepositedCharge.h"

/** @class SiDepositedCharge SiDepositedCharge.h
 *
 *  Landau convolved with a Gaussian - see <a href="http://cdsweb.cern.ch/record/690291">LHCb 2003-160</a>
 *
 *  @author M.Needham
 *  @date   13/3/2002
 */

class SiDepositedCharge : public SiDepositedChargeBase, virtual public ISiDepositedCharge {

public:

  /** Constructor */
  SiDepositedCharge( const std::string& type, const std::string& name,
                     const IInterface* parent );

  /** destructor */
  ~SiDepositedCharge() override = default;

  /** initialize */
  StatusCode initialize() override;

  /** calculate deposited charge (in electrons)
  * @param  aHit hit
  * @return deposited charge
  */
  double charge(const LHCb::MCHit* aHit) const;

private:

  SmartIF<IRndmGen> m_LandauDist;

};

#endif // SiDepositedCharge_H
