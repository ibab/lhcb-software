// $Id: GetMCRichHitsAlg.h,v 1.2 2005-12-22 16:42:43 jonrob Exp $
#ifndef SENSDET_GetMCRichHitsAlg_H
#define SENSDET_GetMCRichHitsAlg_H 1

// base class
#include "GetMCRichInfoBase.h"

/** @class GetMCRichHitsAlg GetMCRichHitsAlg.h
 *
 *  Algorithm to create MCRichHit objects from Gauss G4 information
 *  
 *  @author Sajan EASO
 *  @date   2005-12-06
 */

class GetMCRichHitsAlg : public GetMCRichInfoBase
{

public:

  /// Standard constructor
  GetMCRichHitsAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~GetMCRichHitsAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:

  std::string m_richHitsLocation;   ///< Name of TES path for MCRichHits

  // now the variables used for the local monitoring. This may eventually
  // go into GaussMonitor. SE Nov 2005.
  /// Count number of events processed
  unsigned long int m_nEvts;
  /// overall count hits in each RICH detector
  std::vector< unsigned long int > m_hitTally;
  /// overall hit count in each radiator
  std::vector< unsigned long int > m_radHits;
  /// overall number of invalid radiator flag hits per event
  std::vector< unsigned long int > m_invalidRadHits;
  /// number of hits with invalid RICH flag
  unsigned long int m_invalidRichHits;
  /// overall charged track hit count in each radiator
  std::vector< unsigned long int > m_ctkHits;
  /// overall scattered hit count in each radiator
  std::vector< unsigned long int > m_scatHits;
  /// overall background hit count in each radiator
  std::vector< unsigned long int > m_bkgHits;
  /// overall MCParticle-less hit count in each radiator
  std::vector< unsigned long int > m_nomcpHits;

};

#endif // SENSDET_GetMCRichHitsAlg_H
