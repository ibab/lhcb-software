// $Id: GetMCRichTracksAlg.h,v 1.2 2006-03-01 09:31:26 jonrob Exp $
#ifndef SENSDET_GetMCRichTracksAlg_H
#define SENSDET_GetMCRichTracksAlg_H 1

// base class
#include "GetMCRichInfoBase.h"

/** @class GetMCRichTracksAlg GetMCRichTracksAlg.h
 *
 *  Algorithm to create MCRichTrack objects from Gauss G4 information
 *  
 *  @author Sajan EASO
 *  @date   2005-12-06
 */

class GetMCRichTracksAlg : public GetMCRichInfoBase
{

public:

  /// Standard constructor
  GetMCRichTracksAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~GetMCRichTracksAlg( ); ///< Destructor

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

};

#endif // SENSDET_GetMCRichTracksAlg_H
