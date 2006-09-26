// $Id: HltRecCheckTracks.h,v 1.2 2006-09-26 14:30:06 cattanem Exp $
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

  HltHisto m_histoGhost;
  HltHisto m_histoDx;
  HltHisto m_histoDy;
  HltHisto m_histoDz;
  HltHisto m_histoDTx;
  HltHisto m_histoDTy;
  HltHisto m_histoPOP;

};
#endif // HLTMCMONITOR_H
