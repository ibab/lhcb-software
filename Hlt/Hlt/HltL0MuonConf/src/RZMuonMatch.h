// $Id: RZMuonMatch.h,v 1.3 2008-07-30 13:42:04 graven Exp $
#ifndef RZMUONMATCH_H 
#define RZMUONMATCH_H 1

// Include files
// from Gaudi
#include "HltBase/HltAlgorithm.h"
#include "HltBase/HltSelectionContainer.h"
#include "HltBase/IMatchTVeloTracks.h"

/** @class RZMuonMatch RZMuonMatch.h
 *  
 *
 *  @author Alessia Satta
 *  @date   2007-02-21
 */
class RZMuonMatch : public HltAlgorithm {
public: 
  /// Standard constructor
  RZMuonMatch( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~RZMuonMatch( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

private:
  Hlt::SelectionContainer3<LHCb::Track,LHCb::Track,LHCb::Track> m_selections;
  IMatchTVeloTracks* m_matchToolPointer;
  
  
};
#endif // RZMUONMATCH_H
