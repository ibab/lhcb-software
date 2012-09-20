#ifndef CHEATEDVPPAT_H 
#define CHEATEDVPPAT_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiAlg/ISequencerTimerTool.h"
#include "TrackInterfaces/IVPClusterPosition.h"
#include "VPDet/DeVP.h"
#include "Event/VPLiteCluster.h"
#include "TrackInterfaces/ITrackFitter.h"
#include "GaudiAlg/GaudiTupleAlg.h"

/** @class CheatedVPPat CheatedVPPat.h
 *  Create VP LHCb::Track cheating the pattern recognition, knowing which
 *  LHCbID belong to which MCParticle
 *
 *  @author Laurence Carson / Victor Coco
 *  @date   2010-02-25
 */
  



class CheatedVPPat : public GaudiTupleAlg {
public: 
  /// Standard constructor
  CheatedVPPat( const std::string& name, ISvcLocator* pSvcLocator );
  
  virtual ~CheatedVPPat( ); ///< Destructor
  
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
  
protected:
  
    
private:
  DeVP* m_vP;
  IVPClusterPosition* m_positiontool ;
  ITrackFitter* m_vPFitter ;  
  std::string m_clusterLocation ;
  std::string m_outputTracksLocation;
  unsigned int m_minIDs;
  bool m_UseLinearFit;
  std::string m_vPFitterName;
  LHCb::VPLiteCluster::VPLiteClusters* m_clusters;
 bool m_stateAtBeam;  
}; 

#endif // CHEATEDVPPAT_H
