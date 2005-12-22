// $Id: GetMCRichOpticalPhotonsAlg.h,v 1.1 2005-12-22 16:42:43 jonrob Exp $
#ifndef SENSDET_GETMCRICHOPTICALPHOTONSALG_H
#define SENSDET_GETMCRICHOPTICALPHOTONSALG_H 1

// base class
#include "GetMCRichInfoBase.h"

/** @class GetMCRichOpticalPhotonsAlg GetMCRichOpticalPhotonsAlg.h
 *
 *  Algorithm to create MCRichOpticalPhoton objects from Gauss G4 information
 *
 *  @author Sajan EASO
 *  @date   2005-12-06
 */

class GetMCRichOpticalPhotonsAlg : public GetMCRichInfoBase
{

public:

  /// Standard constructor
  GetMCRichOpticalPhotonsAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~GetMCRichOpticalPhotonsAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:

  std::string        m_richHitsLocation;     ///< Name of TES path for MCRichHits
  std::string        m_richPhotonsLocation;     ///< Name of TES path for MCRichOpticalPPhotons

  // now the variables used for the local monitoring. This may eventually
  // go into GaussMonitor. SE Nov 2005.
  /// Count number of events processed
  unsigned long int m_nEvts;
  /// overall count hits in each RICH detector
  std::vector< unsigned long int > m_hitTally;

};

#endif // SENSDET_GETMCRICHOPTICALPHOTONSALG_H
