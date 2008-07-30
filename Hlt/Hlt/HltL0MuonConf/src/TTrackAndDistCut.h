// $Id: TTrackAndDistCut.h,v 1.3 2008-07-30 13:42:04 graven Exp $
#ifndef TTRACKANDDISTCUT_H 
#define TTRACKANDDISTCUT_H 1

// Include files
// from Gaudi
#include "HltBase/HltAlgorithm.h"
#include "HltBase/HltSelectionContainer.h"
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
  

private:
  Hlt::SelectionContainer2<LHCb::Track,LHCb::Track> m_selections;
  IMuonPosTool*      m_iPosTool;
  double m_maxdist;
  double  calcDLL(LHCb::Track* track, LHCb::MuonTileID & tileM4, LHCb::MuonTileID & tileM5);

};
#endif // TTRACKANDDISTCUT_H
