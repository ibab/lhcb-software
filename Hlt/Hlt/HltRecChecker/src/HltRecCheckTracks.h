// $Id: HltRecCheckTracks.h,v 1.5 2008-12-30 21:25:38 graven Exp $
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

  AIDA::IHistogram1D* m_histoGhost;
  AIDA::IHistogram1D* m_histoDx;
  AIDA::IHistogram1D* m_histoDy;
  AIDA::IHistogram1D* m_histoDz;
  AIDA::IHistogram1D* m_histoDTx;
  AIDA::IHistogram1D* m_histoDTy;
  AIDA::IHistogram1D* m_histoPOP;

};
#endif // HLTMCMONITOR_H
