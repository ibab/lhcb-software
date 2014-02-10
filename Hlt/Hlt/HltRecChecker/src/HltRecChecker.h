// $Id: HltRecChecker.h,v 1.6 2009-12-23 15:03:36 graven Exp $
#ifndef HLTRECCHECKER_H 
#define HLTRECCHECKER_H 1

// Include files
#include "HltBase/HltSelectionContainer.h"
#include "HltBase/HltAlgorithm.h"
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

  Hlt::SelectionContainer<LHCb::Track,LHCb::Track> m_selections;

  std::string m_linkName;

};
#endif // HLTMCMONITOR_H
