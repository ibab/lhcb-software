#ifndef VPGAUSSMONI_H 
#define VPGAUSSMONI_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"
#include "Event/MCHit.h"

/** @class VPGaussMoni VPGaussMoni.h
 *  
 *
 *  @author Victor Coco based on VeloGaussMoni
 *  @date   2009-06-05
 */

class DeVP;
class DeVPXType;
class DeVPYType;
class DeVPSquareType;

class VPGaussMoni : public GaudiTupleAlg {
public: 
  /// Standard constructor
  VPGaussMoni( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~VPGaussMoni( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  StatusCode checkTests();
  StatusCode getData();
  StatusCode vpMCHitMonitor();
  StatusCode basicMonitor();

private:

  std::string m_vpDetLocation;
  DeVP* m_vpDet;
  LHCb::MCHits* m_vpMCHits;
  int m_print;
  bool m_printInfo;
  bool m_detailedMonitor;
  bool m_testMCHit;
  double m_nMCH;
  double m_nMCH2;
  int m_nEvent;
  /// Location of Velo MCHits 
  std::string m_vpMCHitsLocation ;
};
#endif // VPGAUSSMONI_H
