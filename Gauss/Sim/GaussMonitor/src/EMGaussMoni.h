// $Id: EMGaussMoni.h,v 1.5 2009-03-26 22:02:12 robbep Exp $
#ifndef EMGAUSSMONI_H 
#define EMGAUSSMONI_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"
#include "Event/MCHit.h"

#include "GaudiKernel/IRndmGen.h"
#include "GaudiKernel/SmartIF.h"
// velo
#include "VeloDet/DeVelo.h"

/** @class EMGaussMoni EMGaussMoni.h
 * 	modified VeloGaussMoni renamed to avoid conflicts
 *	Peter Griffith & Nigel Watson
 *
 *  @author Tomasz Szumlak Chris Parkes
 *  *  @date   2005-12-13
 *
 */

//class DeVelo;
class DeVeloRType;
class DeVeloPhiType;
class IRndmGen;
class IRndmGenSvc;


class EMGaussMoni : public GaudiTupleAlg {
public: 
  /// Standard constructor
  EMGaussMoni( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~EMGaussMoni( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  StatusCode checkTests();
  StatusCode getData();
  StatusCode veloMCHitMonitor();
  StatusCode veloPileUpMCHitMonitor();
  StatusCode basicMonitor();

private:

  std::string m_veloDetLocation;
  DeVelo* m_veloDet;
  LHCb::MCHits* m_veloMCHits;
  LHCb::MCHits* m_veloPileUpMCHits;
  int m_print;
  bool m_printInfo;
  bool m_detailedMonitor;
  bool m_bremmMonitor;
  bool m_testMCHit;
  bool m_testPileUpMCHit;
  double m_nMCH;
  double m_nMCH2;
  double m_nPUMCH;
  double m_nPUMCH2;
  int m_nEvent;
  /// Location of Velo MCHits 
  std::string m_veloMCHitsLocation ;
  /// Location of PuVeto MCHits
  std::string m_puVetoMCHitsLocation ; 
  //
  SmartIF<IRndmGen>  m_gaussGen;

};
#endif // EMGAUSSMONI_H
