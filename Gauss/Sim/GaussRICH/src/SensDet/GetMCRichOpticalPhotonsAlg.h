// $Id: GetMCRichOpticalPhotonsAlg.h,v 1.2 2006-03-01 09:31:26 jonrob Exp $
#ifndef SENSDET_GETMCRICHOPTICALPHOTONSALG_H
#define SENSDET_GETMCRICHOPTICALPHOTONSALG_H 1

// base class
#include "GetMCRichInfoBase.h"

// rich kernel
#include "RichKernel/RichMap.h"

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

  std::string  m_richHitsLocation;     ///< Name of TES path for MCRichHits

  // now the variables used for the local monitoring. This may eventually
  // go into GaussMonitor. SE Nov 2005.
  /// Count number of events processed
  unsigned long int m_nEvts;

  /// map for photon counting
  typedef Rich::Map< const Rich::RadiatorType, unsigned long int > RadMap;
  /// count overall number of photons in each RICH radiator medium
  RadMap m_hitTally;

};

#endif // SENSDET_GETMCRICHOPTICALPHOTONSALG_H
