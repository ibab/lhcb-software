// $Id: STOfflinePosition.h,v 1.7 2009-01-09 16:15:36 jvantilb Exp $
#ifndef STOfflinePosition_H
#define STOfflinePosition_H 1

// Gaudi
#include "GaudiKernel/IIncidentListener.h"
#include "Kernel/STToolBase.h"

// LHCbKernel
#include "Kernel/ISTClusterPosition.h"

/** @class STOfflinePosition STOfflinePosition.h
 *
 *  Tool for calculating offline cluster position
 *
 *  @author M.Needham
 *  @date   14/3/2002
 */

class STOfflinePosition: public ST::ToolBase, 
                         virtual public ISTClusterPosition,
                         virtual public IIncidentListener {

public: 
   
  /// constructer
  STOfflinePosition( const std::string& type,
                     const std::string& name,
                     const IInterface* parent );

  /// destructer
  virtual ~STOfflinePosition();

  /** intialize */
  StatusCode initialize();

  /** Implement the handle method for the Incident service.
  *  This is used to nform the tool of software incidents.
  *
  *  @param incident The incident identifier
  */
  void handle( const Incident& incident );

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

  /// STCluster container, needed to merge splitted clusters
  mutable LHCb::STClusters* m_clusters;

  // job options
  std::vector<double> m_errorVec;  ///< Error parametrized by cluster size
  double m_sharingCorr;            ///< Charge sharing correction factor
  int m_maxNtoCorr;                ///< Maximum size of cluster for S-shape corr
  double m_trim;                   ///< Trimming value to suppress cap. coupling
  bool m_mergeClusters;            ///< Flag to merge split clusters
  std::string m_clusterLocation;   ///< Location STClusters (needed for merging)

};

#endif // STOfflinePosition_H
