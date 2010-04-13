#ifndef CHEATEDVELOPIXPAT_H 
#define CHEATEDVELOPIXPAT_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiAlg/ISequencerTimerTool.h"
#include "TrackInterfaces/IVeloPixClusterPosition.h"
#include "VeloPixDet/DeVeloPix.h"
#include "Event/VeloPixLiteCluster.h"
#include "TrackInterfaces/ITrackFitter.h"
#include "GaudiAlg/GaudiTupleAlg.h"

/** @class CheatedVeloPixPat CheatedVeloPixPat.h
 *  Create VeloPix LHCb::Track cheating the pattern recognition, knowing which
 *  LHCbID belong to which MCParticle
 *
 *  @author Laurence Carson / Victor Coco
 *  @date   2010-02-25
 */
  



class CheatedVeloPixPat : public GaudiTupleAlg {
public: 
  /// Standard constructor
  CheatedVeloPixPat( const std::string& name, ISvcLocator* pSvcLocator );
  
  virtual ~CheatedVeloPixPat( ); ///< Destructor
  
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
  
protected:
  
    
private:
  DeVeloPix* m_veloPix;
  IVeloPixClusterPosition* m_positiontool ;
  ITrackFitter* m_veloPixFitter ;  
  std::string m_clusterLocation ;
  std::string m_outputTracksLocation;
  unsigned int m_minIDs;
  bool m_UseLinearFit;
  LHCb::VeloPixLiteCluster::VeloPixLiteClusters* m_clusters;
  
}; 

#endif // CHEATEDVELOPIXPAT_H
