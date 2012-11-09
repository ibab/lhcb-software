// $Id: $
#ifndef CHERENKOVANALYSIS_CHERENKOVG4HISTOFILLSET5_H 
#define CHERENKOVANALYSIS_CHERENKOVG4HISTOFILLSET5_H 1

// Include files
#include "G4Event.hh"
#include "GaussCherenkov/CkvG4Hit.h"
#include <vector>
#include "GaudiKernel/Point3DTypes.h"
//#include "GaudiKernel/Plane3DTypes.h"
//#include "GaudiKernel/Vector3DTypes.h"
//#include "GaudiKernel/Transform3DTypes.h"


/** @class CherenkovG4HistoFillSet5 CherenkovG4HistoFillSet5.h CherenkovAnalysis/CherenkovG4HistoFillSet5.h
 *  
 *
 *  @author Sajan Easo
 *  @date   2011-04-15
 */
class CherenkovG4HistoFillSet5 {
public: 
  /// Standard constructor
  CherenkovG4HistoFillSet5( ); 

  virtual ~CherenkovG4HistoFillSet5( ); ///< Destructor
  void InitCherenkovG4HistoFillSet5();
  void FillRichG4HistoSet5NumH();
  void FillRichG4HistoSet5Coord(CkvG4Hit* adHit, Gaudi::XYZPoint aHitCoordInPmtPlane);
  
protected:

private:
  int m_TotNumHitR1InCurEv;
  int m_TotNumHitR2InCurEv;
  int m_TotNumHitR2NoScintInCurEv;
  
};
#endif // CHERENKOVANALYSIS_CHERENKOVG4HISTOFILLSET5_H
