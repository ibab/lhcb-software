// $Id: VeloRawClustersMoni.h,v 1.1 2006-05-08 14:11:49 szumlat Exp $
#ifndef VELORAWCLUSTERSMONI_H 
#define VELORAWCLUSTERSMONI_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"

// velo
#include "Event/MCVeloFE.h"
#include "Event/VeloCluster.h"

/** @class VeloRawClustersMoni VeloRawClustersMoni.h
 *  
 *
 *  @author Tomasz Szumlak
 *  @date   2006-03-17
 */

class IMCVeloFEType;
class DeVelo;

class VeloRawClustersMoni : public GaudiTupleAlg{
public: 
  /// Standard constructor
  VeloRawClustersMoni( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~VeloRawClustersMoni( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
  StatusCode getData();
  StatusCode rawVeloClusterMonitor();
  StatusCode clusterType(LHCb::VeloCluster* clu, bool& s, bool& n, bool& o);
  
protected:

private:

  std::string m_clusterCont;
  std::string m_VeloFECont;
  long m_numberOfEvents;
  LHCb::VeloClusters* m_rawClusters;
  LHCb::MCVeloFEs* m_VeloFEs;
  IMCVeloFEType* m_feTypeTool;
  DeVelo* m_veloDet;
  double m_nRawClusters;
  double m_nRawClusters2;
  double m_nOneStr;
  double m_nTwoStr;
  double m_nThreeStr;
  double m_nFourStr;
  double m_nClusS;
  double m_nClusN;
  double m_nClusO;

};
#endif // VELORAWCLUSTERSMONI_H
