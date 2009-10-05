#ifndef VELOPIXGAUSSMONI_H 
#define VELOPIXGAUSSMONI_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"
#include "Event/MCHit.h"

/** @class VeloPixGaussMoni VeloPixGaussMoni.h
 *  
 *
 *  @author Victor Coco based on VeloGaussMoni
 *  @date   2009-06-05
 */

class DeVeloPix;
class DeVeloPixXType;
class DeVeloPixYType;
class DeVeloPixSquareType;

class VeloPixGaussMoni : public GaudiTupleAlg {
public: 
  /// Standard constructor
  VeloPixGaussMoni( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~VeloPixGaussMoni( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  StatusCode checkTests();
  StatusCode getData();
  StatusCode velopixMCHitMonitor();
  StatusCode basicMonitor();

private:

  std::string m_velopixDetLocation;
  DeVeloPix* m_velopixDet;
  LHCb::MCHits* m_velopixMCHits;
  int m_print;
  bool m_printInfo;
  bool m_detailedMonitor;
  bool m_testMCHit;
  double m_nMCH;
  double m_nMCH2;
  int m_nEvent;
  /// Location of Velo MCHits 
  std::string m_velopixMCHitsLocation ;
};
#endif // VELOPIXGAUSSMONI_H
