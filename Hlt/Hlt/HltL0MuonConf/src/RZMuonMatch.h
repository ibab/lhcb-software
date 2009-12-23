// $Id: RZMuonMatch.h,v 1.4 2009-12-23 15:05:58 graven Exp $
#ifndef RZMUONMATCH_H 
#define RZMUONMATCH_H 1

// Include files
// from Gaudi
#include "HltBase/HltSelectionContainer.h"
#include "HltBase/HltAlgorithm.h"
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
