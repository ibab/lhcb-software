// $Id: HltRecCheckTracks.h,v 1.3 2008-01-22 11:04:06 hernando Exp $
#ifndef HLTRECCHECKTRACKS_H 
#define HLTRECCHECKTRACKS_H 1

// Include files
#include "HltBase/HltAlgorithm.h"
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
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:
  
  void checkTracks();

protected:

  std::string m_linkName;

protected:

  Hlt::Histo* m_histoGhost;
  Hlt::Histo* m_histoDx;
  Hlt::Histo* m_histoDy;
  Hlt::Histo* m_histoDz;
  Hlt::Histo* m_histoDTx;
  Hlt::Histo* m_histoDTy;
  Hlt::Histo* m_histoPOP;

};
#endif // HLTMCMONITOR_H
