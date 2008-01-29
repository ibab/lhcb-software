// $Id: GetMCRichHitsAlg.h,v 1.7 2008-01-29 12:25:52 jonrob Exp $
#ifndef SENSDET_GetMCRichHitsAlg_H
#define SENSDET_GetMCRichHitsAlg_H 1

// STD
#include <sstream>

// base class
#include "GetMCRichInfoBase.h"

// RichDet
#include "RichDet/DeRich.h"

// RichKernel
#include "RichKernel/RichMap.h"
#include "RichG4HpdReflectionFlag.h"

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

  /// Count number of events processed
  unsigned long int m_nEvts;

  /// map for counting detector tallies
  typedef Rich::Map< const Rich::DetectorType, unsigned long int > DMap;
  /// map for counting radiator tallies
  typedef Rich::Map< const Rich::RadiatorType, unsigned long int > RMap;

  /// overall count hits in each RICH detector
  DMap m_hitTally;

  /// overall hit count in each radiator
  RMap m_radHits;
  /// overall number of invalid radiator flag hits per event
  DMap m_invalidRadHits;
  /// number of hits with invalid RICH flag
  unsigned long int m_invalidRichHits;
  /// overall charged track hit count in each radiator
  RMap m_ctkHits;
  /// overall scattered hit count in each radiator
  RMap m_scatHits;

  /// overall gas quartz window CK hit count in each radiator
  DMap m_gasQzHits;
  /// overall HPD quartz window CK hit count in each radiator
  DMap m_hpdQzHits;
  /// overall nitrogen CK hit count in each radiator
  DMap m_nitroHits;
  /// overall aerogel filter CK hit count in each radiator
  DMap m_aeroFilterHits;

  /// Back-scattering from Si hits
  DMap m_siBackScatt;

  /// overall hits from all types of HPD reflections
  DMap m_hpdReflHits;
  /// overall hits from photons reflected by the QW/PC interface in the HPD
  DMap m_hpdReflHitslQWPC;
  /// overall hits from photons reflected by the Chromium in the HPD
  DMap m_hpdReflHitslChr;
  /// overall hits from photons reflected by the Air/QW interface in the HPD
  DMap m_hpdReflHitsAirQW;
  /// overall hits from photons reflected by the Air/PC interface in the HPD
  DMap m_hpdReflHitsAirPC;
  /// overall hits from photons reflected by the silicon in the HP
  DMap m_hpdReflHitsSi;
  /// overall hits from photons reflected by the kovar in the HP
  DMap m_hpdReflHitsKovar;
  /// overall hits from photons reflected by the kapton in the HP
  DMap m_hpdReflHitsKapton;
  /// overall hits from photons reflected by the PC/QW interface in the HPD
  DMap m_hpdReflHitsPCQW;

  /// overall MCParticle-less hit count in each radiator
  RMap m_nomcpHits;

  /// Number of hits in each aerogel tile
  typedef Rich::Map<int,unsigned int> AeroTileMap;
  AeroTileMap m_aeroTileHits;

  /// Pointers to RICH detector elements
  std::vector<const DeRich *> m_richDets;

};

#endif // SENSDET_GetMCRichHitsAlg_H
