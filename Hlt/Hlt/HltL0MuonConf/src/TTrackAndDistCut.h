// $Id: TTrackAndDistCut.h,v 1.2 2008-01-22 09:58:06 hernando Exp $
#ifndef TTRACKANDDISTCUT_H 
#define TTRACKANDDISTCUT_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "HltBase/HltAlgorithm.h"
#include "Event/L0MuonCandidate.h"
#include "MuonDet/IMuonPosTool.h"

/** @class TTrackAndDistCut TTrackAndDistCut.h
 *  
 *
 *  @author Alessia Satta
 *  @date   2007-02-21
 */
class TTrackAndDistCut : public HltAlgorithm {
public: 
  /// Standard constructor
  TTrackAndDistCut( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TTrackAndDistCut( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
  
protected:

private:
  IMuonPosTool*      m_iPosTool;
  float m_maxdist;
//  bool m_skipFilter;
  double  calcDLL(LHCb::Track* track, LHCb::MuonTileID & tileM4, LHCb::MuonTileID & tileM5);

};
#endif // TTRACKANDDISTCUT_H
