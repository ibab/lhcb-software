#ifndef CHEATEDVELOPIXPAT_H 
#define CHEATEDVELOPIXPAT_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiAlg/ISequencerTimerTool.h"
#include "TrackInterfaces/IVeloPixClusterPosition.h"
#include "VeloPixDet/DeVeloPix.h"
#include "Event/VeloPixLiteCluster.h"
#include "MCInterfaces/ILHCbIDsToMCHits.h"
#include "TrackInterfaces/ITrackFitter.h"
#include "GaudiAlg/GaudiTupleAlg.h"

/** @class CheatedVeloPixPat CheatedVeloPixPat.h
 *  Create VeloPix LHCb::Track cheating the pattern recognition, knowing which
 *  LHCbID belong to which MCParticle
 *
 *  @author Laurence Carson / Victor Coco
 *  @date   2010-02-25
 */
  

// Forward declarations
//class DeVeloPix;
//class VeloPixLiteCluster;


class CheatedVeloPixPat : public GaudiTupleAlg {
public: 
  /// Standard constructor
  CheatedVeloPixPat( const std::string& name, ISvcLocator* pSvcLocator );
  
  virtual ~CheatedVeloPixPat( ); ///< Destructor
  
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
  ILHCbIDsToMCHits* linkTool() const;
  
protected:
  
    
private:
  DeVeloPix* m_veloPix;
  IVeloPixClusterPosition* m_positiontool ;
  ITrackFitter* m_veloPixFitter ;  
  std::string m_clusterLocation ;
  std::string m_outputTracksLocation;
  unsigned int m_minIDs;
  LHCb::VeloPixLiteCluster::VeloPixLiteClusters* m_clusters;
    ILHCbIDsToMCHits* m_linkTool;
  
}; 

inline ILHCbIDsToMCHits* CheatedVeloPixPat::linkTool() const{
  return m_linkTool;
}

#endif // CHEATEDVELOPIXPAT_H
