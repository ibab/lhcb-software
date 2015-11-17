// $Id: GetMCCkvHitsAlg.h,v 1.11 2009-07-17 13:46:12 jonrob Exp $
#ifndef SENSDET_GetMCCkvHitsAlg_H
#define SENSDET_GetMCCkvHitsAlg_H 1

// STD
#include <sstream>

// base class
#include "GaussCherenkov/GetMCCkvInfoBase.h"

// RichDet
#include "RichDet/DeRich.h"

// RichKernel
#include "RichKernel/RichMap.h"
#include "GaussRICH/RichG4HpdReflectionFlag.h"

/** @class GetMCCkvHitsAlg GetMCCkvHitsAlg.h
 *
 *  Algorithm to create MCRichHit objects from Gauss G4 information
 *
 *  @author Sajan EASO
 *  @date   2011-03-06
 */

class GetMCCkvHitsAlg : public GetMCCkvInfoBase
{

public:

  /// Standard constructor
  GetMCCkvHitsAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~GetMCCkvHitsAlg( ); ///< Destructor

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
  /// overall count hits in each RICH detector
  DMap m_signalTally;

  /// overall hit count in each radiator
  RMap m_radHits;
  /// overall number of invalid radiator flag hits per event
  DMap m_invalidRadHits;
  /// number of hits with invalid RICH flag
  unsigned long int m_invalidRichHits;
  /// overall charged track hit count in each detector
  DMap m_ctkHits;
  /// overall scattered hit count in each radiator
  RMap m_scatHits;

  /// overall gas quartz window CK hit count in each radiator
  DMap m_gasQzHits;
  /// overall PMT quartz window CK hit count in each radiator
  DMap m_hpdQzHits;
  /// overall nitrogen CK hit count in each radiator
  DMap m_nitroHits;
  /// overall aerogel filter CK hit count in each radiator
  DMap m_aeroFilterHits;

  /// Back-scattering from Si hits
  DMap m_siBackScatt;

  ///  Scintillation hits
  DMap m_scintillationHits;
  
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

  /// MCParticles location
  std::string m_mcParticles ;

private:

  /// Print a monitoring statistic
  void printStat( std::string name, DMap & a );
  
  /// Print a monitoring statistic
  void printStat( std::string name, RMap & a );

  //create  smartid 
  LHCb::RichSmartID assembleMCPmtRichSmartID(const CkvG4Hit * aHit, bool isAGrandPmtHitFlag);
  
  int m_NumPmtInAModule;
  
};

#endif // SENSDET_GetMCCkvHitsAlg_H
