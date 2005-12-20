// $Id: STOfflinePosition.h,v 1.2 2005-12-20 15:29:32 cattanem Exp $
#ifndef _STOfflinePosition_H
#define _STOfflinePosition_H

#include "GaudiAlg/GaudiTool.h"

// From LHCbKernel
#include "Kernel/ISTClusterPosition.h"

#include <vector>

/** @class STOfflinePosition STOfflinePosition.h
 *
 *  Tool for calculating offline cluster position
 *
 *  @author M.Needham
 *  @date   14/3/2002
 */

class STOfflinePosition: public GaudiTool, virtual public ISTClusterPosition {

public: 
   
  /// constructer
  STOfflinePosition(const std::string& type,
                      const std::string& name,
                      const IInterface* parent);

  /// destructer
  virtual ~STOfflinePosition();

  /// method
  virtual ISTClusterPosition::Measurement estimate(const LHCb::STCluster* aCluster) const;

  virtual ISTClusterPosition::Measurement estimate(const SmartRefVector<LHCb::STDigit>& digits) const;

  virtual double error(const unsigned int nStrips) const;

private:

  double chargeSharingCorr(const double dist) const;

  std::vector<double> m_ErrorVec;
  double m_sharingCorr;
  int m_MaxNtoCorr;

};

#endif // _STOfflinePosition_H



