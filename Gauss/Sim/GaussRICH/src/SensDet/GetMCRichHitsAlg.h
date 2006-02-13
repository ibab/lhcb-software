// $Id: GetMCRichHitsAlg.h,v 1.3 2006-02-13 16:30:43 jonrob Exp $
#ifndef SENSDET_GetMCRichHitsAlg_H
#define SENSDET_GetMCRichHitsAlg_H 1

// STD
#include <sstream>

// base class
#include "GetMCRichInfoBase.h"

// RichDet
#include "RichDet/DeRich1.h"
#include "RichDet/DeRich2.h"

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

  /// overall gas quartz window CK hit count in each radiator
  std::vector< unsigned long int > m_gasQzHits;
  /// overall HPD quartz window CK hit count in each radiator
  std::vector< unsigned long int > m_hpdQzHits;
  /// overall nitrogen CK hit count in each radiator
  std::vector< unsigned long int > m_nitroHits;
  /// overall aerogel filter CK hit count in each radiator
  std::vector< unsigned long int > m_aeroFilterHits;

  /// overall MCParticle-less hit count in each radiator
  std::vector< unsigned long int > m_nomcpHits;

  /// Pointers to RICH detector elements
  std::vector<const DeRich *> m_richDets;

  /// Locations of RICH detector objects in TES
  std::vector<std::string> m_richDetsLoc;

};

#endif // SENSDET_GetMCRichHitsAlg_H
