// $Id: MakeMuonTracks.h,v 1.1 2010-06-04 13:02:57 svecchi Exp $
#ifndef MAKEMUONTRACKS_H 
#define MAKEMUONTRACKS_H 1

// Include files
#include <string>

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// from MuonNNet
#include "MuonInterfaces/IMuonTrackRec.h"
#include "MuonDet/DeMuonDetector.h"

class ITrackMomentumEstimate;

/** @class MakeMuonTracks MakeMuonTracks.h
 *  Simple algorithm to make "LHCb" tracks from MuonNNet tracks
 *  and to copy them to some location.
 *
 *  @author Stefania Vecchi (alias Jan Amoraal)
 *  @date   2009-12-03
 */

class MakeMuonTracks : public GaudiAlgorithm {

public: 
  /// Standard constructor
  MakeMuonTracks( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MakeMuonTracks( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  //virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private: 

  std::string     m_trackOutputLoc ;
  std::string     m_muonRecToolName;
  IMuonTrackRec*  m_muonRecTool    ;
  bool            m_skipBigClusters;
  bool            m_Bfield         ;  
  unsigned int    m_MaxNTiles      ;
  ITrackMomentumEstimate* m_fCalcMomentum; // momentum tool
  DeMuonDetector  *m_muonDetector  ;
  
};
#endif // MAKEMUONTRACKS_H
