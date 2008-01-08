// $Id: STOnlinePosition.h,v 1.7 2008-01-08 10:21:49 mneedham Exp $
#ifndef STOnlinePosition_H
#define STOnlinePosition_H 1

// Gaudi
#include "GaudiAlg/GaudiTool.h"

// From LHCbKernel
#include "Kernel/ISTClusterPosition.h"

/** @class STOnlinePosition STOnlinePosition.h
 *
 *  Tool for calculating offline cluster position
 *
 *  @author M.Needham
 *  @date   14/3/2002
 */

class STOnlinePosition: public GaudiTool, virtual public ISTClusterPosition {

public: 
   
  /// constructer
  STOnlinePosition( const std::string& type,
                    const std::string& name,
                    const IInterface* parent);

  /// destructer
  virtual ~STOnlinePosition();

  /// method
  virtual ISTClusterPosition::Info estimate(const LHCb::STCluster* 
                                            aCluster) const;

  virtual ISTClusterPosition::Info 
  estimate(const SmartRefVector<LHCb::STDigit>& digits) const;

  virtual double error(const unsigned int nStrips) const;

private:

 
  // job options
  std::vector<double> m_errorVec; ///< Error parametrized by cluster size

};

#endif // STOnlinePosition_H



