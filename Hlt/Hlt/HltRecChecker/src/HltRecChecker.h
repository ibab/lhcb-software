// $Id: HltRecChecker.h,v 1.4 2007-07-13 08:54:09 pkoppenb Exp $
#ifndef HLTRECCHECKER_H 
#define HLTRECCHECKER_H 1

// Include files
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
  

protected:

  void checkQuark();
  
  void checkTracks();
  
  void checkVertices();  

  StatusCode printTrack(const LHCb::Track*);
  

protected:

  std::string m_linkName;

};
#endif // HLTMCMONITOR_H
