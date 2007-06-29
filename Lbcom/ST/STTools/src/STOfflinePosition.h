// $Id: STOfflinePosition.h,v 1.6 2007-06-29 14:37:25 mneedham Exp $
#ifndef STOfflinePosition_H
#define STOfflinePosition_H 1

// Gaudi
#include "GaudiAlg/GaudiTool.h"

// LHCbKernel
#include "Kernel/ISTClusterPosition.h"

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
  STOfflinePosition( const std::string& type,
                     const std::string& name,
                     const IInterface* parent );

  /// destructer
  virtual ~STOfflinePosition();

  /// method
  virtual ISTClusterPosition::Info estimate(const LHCb::STCluster* 
                                            aCluster) const;

  virtual ISTClusterPosition::Info 
  estimate(const SmartRefVector<LHCb::STDigit>& digits) const;

  virtual double error(const unsigned int nStrips) const;

private:

  double stripFraction( const double stripNum,
                        const unsigned int clusterSize ) const;

  double chargeSharingCorr( const double dist ) const;

  // job options
  std::vector<double> m_errorVec;  ///< Error parametrized by cluster size
  double m_sharingCorr;            ///< Charge sharing correction factor
  int m_maxNtoCorr;                ///< Maximum size of cluster for S-shape corr
  double m_trim;

};

#endif // STOfflinePosition_H
