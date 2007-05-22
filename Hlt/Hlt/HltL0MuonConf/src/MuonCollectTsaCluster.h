// $Id: MuonCollectTsaCluster.h,v 1.1.1.1 2007-05-22 10:20:31 asatta Exp $
#ifndef MUONCOLLECTTSACLUSTER_H 
#define MUONCOLLECTTSACLUSTER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiAlg/GaudiTupleAlg.h"


#include "Event/L0DUReport.h"
#include "Event/L0MuonCandidate.h"
#include "Event/RecHeader.h"

//#include "MuonDet/IMuonPosTool.h"



/** @class MuonCollectTsaCluster MuonCollectTsaCluster.h
 *  
 *
 *  @author Johannes Albrecht
 *  @date   2007-01-02
 */

// forward declaration
class IPrepareMuonSeed;
class ITsaCollector;
//play with a seeding tool
class ITrackConfirmTool;

class MuonCollectTsaCluster : public GaudiTupleAlg {
public: 
  /// Standard constructor
  MuonCollectTsaCluster( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MuonCollectTsaCluster( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
  
  int m_nsigma;
  bool m_debugInfo;  

protected:
  
private:
  
  LHCb::L0DUReport* m_l0;
  LHCb::L0MuonCandidates* m_inputL0Muons;

  IPrepareMuonSeed* m_prepareMuonSeed;

  // Tsa Collector
  ITsaCollector*    m_tsacollector;

  //new seeding tool
  ITrackConfirmTool* m_TsaConfirmTool;

};
#endif // NEWL0CONFIRM_H
