// $Id: HltRecChecker.h,v 1.5 2008-07-30 13:38:48 graven Exp $
#ifndef HLTRECCHECKER_H 
#define HLTRECCHECKER_H 1

// Include files
#include "HltBase/HltAlgorithm.h"
#include "HltBase/HltSelectionContainer.h"
#include "Event/MCParticle.h"

/** @class HltRecChecker HltRecChecker.h
 *  
 *
 *  @author Hugo Ruiz Perez
 *  @author Jose A. Hernando
 *  @date   2006-05-24
 */
class HltRecChecker : public HltAlgorithm {
public: 
  /// Standard constructor
  HltRecChecker( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltRecChecker( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
  

private:

  void checkQuark();
  
  void checkTracks(const Hlt::TrackSelection&);
  
  void checkVertices(const Hlt::VertexSelection&);  

  StatusCode printTrack(const LHCb::Track*);

  Hlt::SelectionContainer2<LHCb::Track,LHCb::Track> m_selections;

  std::string m_linkName;

};
#endif // HLTMCMONITOR_H
