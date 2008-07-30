// $Id: HltRecCheckTracks.h,v 1.4 2008-07-30 13:38:48 graven Exp $
#ifndef HLTRECCHECKTRACKS_H 
#define HLTRECCHECKTRACKS_H 1

// Include files
#include "HltBase/HltAlgorithm.h"
#include "HltBase/HltSelectionContainer.h"
#include "Event/MCParticle.h"

/** @class HltRecCheckTracks HltRecCheckTracks.h
 *  
 *
 *  @author Jose A. Hernando
 *  @date   2006-05-24
 */
class HltRecCheckTracks : public HltAlgorithm {
public: 
  /// Standard constructor
  HltRecCheckTracks( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltRecCheckTracks( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

private:
  
  Hlt::SelectionContainer2<LHCb::Track,LHCb::Track> m_selections;
  std::string m_linkName;

  Hlt::Histo* m_histoGhost;
  Hlt::Histo* m_histoDx;
  Hlt::Histo* m_histoDy;
  Hlt::Histo* m_histoDz;
  Hlt::Histo* m_histoDTx;
  Hlt::Histo* m_histoDTy;
  Hlt::Histo* m_histoPOP;

};
#endif // HLTMCMONITOR_H
