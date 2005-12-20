// $Id: STGeantDepositedCharge.h,v 1.2 2005-12-20 15:50:25 cattanem Exp $
#ifndef _STGeantDepositedCharge_H
#define _STGeantDepositedCharge_H


#include "ISTDepositedCharge.h"

#include "GaudiKernel/IRndmGen.h"
#include "GaudiAlg/GaudiTool.h"

/** @class STGeantDepositedCharge STGeantDepositedCharge.h
 *
 *  Class for estimating noise in IT Channel
 *
 *  @author M.Needham
 *  @date   13/3/2002
 */


class STGeantDepositedCharge : public GaudiTool, virtual public ISTDepositedCharge {

public: 

  /// Constructer
  STGeantDepositedCharge(const std::string& type, 
              const std::string& name,
              const IInterface* parent);

  /// destructer
  virtual ~STGeantDepositedCharge();


  /// calc noise
  double charge(const LHCb::MCHit* aHit) const;

private:

  double m_scalingFactor;
 
};

#endif // _ITDepositedCharge_H







