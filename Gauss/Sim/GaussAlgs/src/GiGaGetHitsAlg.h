// $Id: GiGaGetHitsAlg.h,v 1.5 2006-01-18 09:12:46 gcorti Exp $
#ifndef GIGAGETHITSALG_H 
#define GIGAGETHITSALG_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

/** @class GiGaGetHitsAlg GiGaGetHitsAlg.h
 *  
 *
 *  @author Witold Pokorski
 *  @date   2002-08-13
 */

class GiGaGetHitsAlg : public GaudiAlgorithm {
public:
  /// Standard constructor
  GiGaGetHitsAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~GiGaGetHitsAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:
  
  // Print info for Silicon or Outer Tracker MCHits
  virtual void hitsTracker( const std::string det, const std::string location );

  // Print info for RICH detector: MCRichHits, Optical photons, segments and
  // tracks
  virtual void infoRICH( );  

private:

  std::string    m_othits      ;      ///< TES path where to look for OT hits 
  std::string    m_sthits      ;      ///< TES path where to look for ST hits
  std::string    m_velohits    ;      ///< TES path where to look for Velo hits
  std::string    m_puvelohits  ;      ///< TES path where to look for PileUp Veto hits
  std::string    m_muonhits    ;      ///< TES path where to look for Muon hits
  std::string    m_richhits    ;      ///< TES path where to look for RICH hits
  std::string    m_richop      ;      ///< TES path where to look for Optical photons
  std::string    m_richsegments;      ///< TES path where to look for Rich segments
  std::string    m_richtracks  ;      ///< TES path where to look for Rich tracks
  
  typedef std::vector<std::string> Addresses ;
  Addresses      m_caloHits;          ///< TES pathes where to look for Calo hits

  std::vector<std::string> m_caloDet; ///< List of calorimeters detecotors

  std::string   m_extraHits;
  
};
#endif // GIGAGETHITSALG_H
