// $Id: GetMCCkvTracksAlg.h,v 1.3 2009-03-26 21:49:47 robbep Exp $
#ifndef SENSDET_GetMCCkvTracksAlg_H
#define SENSDET_GetMCCkvTracksAlg_H 1

// base class
#include "GaussCherenkov/GetMCCkvInfoBase.h"

/** @class GetMCCkvTracksAlg GetMCCkvTracksAlg.h
 *
 *  Algorithm to create MCRichTrack objects from Gauss G4 information
 *  
 *  @author Sajan EASO
 *  @date   2005-12-06
 *  Modif SE 2014-08-30
 */

class GetMCCkvTracksAlg : public GetMCCkvInfoBase
{

public:

  /// Standard constructor
  GetMCCkvTracksAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~GetMCCkvTracksAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:

  // now the variables used for the local monitoring. This may eventually
  // go into GaussMonitor. SE Nov 2005.
  /// Count number of events processed
  unsigned long int m_nEvts;
  /// Count tracks
  unsigned long int m_hitTally;
  /// Location of mc particles
  std::string m_mcParticles ;
  /// Location of rich segments
  std::string m_mcRichSegments ;
};

#endif // SENSDET_GetMCCkvTracksAlg_H
