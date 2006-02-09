// $Id: VeloClusterMoni.h,v 1.1 2006-02-09 12:20:41 szumlat Exp $
#ifndef VELOCLUSTERMONI_H 
#define VELOCLUSTERMONI_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"

// velo
#include "Event/MCVeloFE.h"
#include "Event/InternalVeloCluster.h"
/** @class VeloClusterMoni VeloClusterMoni.h
 *  
 *
 *  @author Tomasz Szumlak
 *  @date   2005-11-30
 */

class IMCVeloFEType;

class VeloClusterMoni : public GaudiTupleAlg {
public: 
  /// Standard constructor
  VeloClusterMoni( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~VeloClusterMoni( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
  StatusCode getData();
  StatusCode veloClusterMonitor();
  StatusCode clusterType(LHCb::InternalVeloCluster* clu,
                         bool& s, bool& n, bool& o);
  
protected:

private:

  std::string m_clusterCont;
  std::string m_feCont;
  bool m_printInfo;
  LHCb::InternalVeloClusters* m_veloClusters;
  LHCb::MCVeloFEs* m_veloFEs;
  IMCVeloFEType* m_feTypeTool;
  double m_nVeloClusters;
  double m_nVeloClusters2;
  double m_nVeloClustersS;
  double m_nVeloClustersN;
  double m_nVeloClustersO;
  int m_numberOfEvents;
  
};
#endif // VELOCLUSTERMONI_H
