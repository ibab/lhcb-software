// $Id: SpaceMuonMatch.h,v 1.7 2008-07-30 13:42:04 graven Exp $
#ifndef SPACEMUONMATCH_H 
#define SPACEMUONMATCH_H 1

// Include files
// from Gaudi
#include "HltBase/HltAlgorithm.h"
#include "HltBase/HltSelectionContainer.h"
#include "HltBase/IMatchTVeloTracks.h"

/** @class SpaceMuonMatch SpaceMuonMatch.h
 *  
 *
 *  @author Alessia Satta
 *  @date   2007-02-21
 */
class SpaceMuonMatch : public HltAlgorithm {
public: 
  /// Standard constructor
  SpaceMuonMatch( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~SpaceMuonMatch( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
  
    
protected:


private:
  std::string m_outputMuonTracksName;   ///< output container name 

  Hlt::SelectionContainer3<LHCb::Track,LHCb::Track,LHCb::Track> m_selections;
 IMatchTVeloTracks* m_matchToolPointer;


};
#endif // SPACEMUONMATCH_H
