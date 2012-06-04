// $Id: $
#ifndef TORCHTESTBEAMANALYSIS_TORCHTBG4FILLHISTOSET6_H 
#define TORCHTESTBEAMANALYSIS_TORCHTBG4FILLHISTOSET6_H 1

// Include files
#include "G4Event.hh"
#include "CkvG4Hit.h"
#include <vector>
#include "GaudiKernel/Point3DTypes.h"
//#include "GaudiKernel/Plane3DTypes.h"
//#include "GaudiKernel/Vector3DTypes.h"
//#include "GaudiKernel/Transform3DTypes.h"


/** @class TorchTBG4FillHistoSet6 TorchTBG4FillHistoSet6.h TorchTestBeamAnalysis/TorchTBG4FillHistoSet6.h
 *  
 *
 *  @author Sajan Easo
 *  @date   2012-06-01
 */
class TorchTBG4FillHistoSet6 {
public: 
  /// Standard constructor
  TorchTBG4FillHistoSet6( ); 

  virtual ~TorchTBG4FillHistoSet6( ); ///< Destructor

  void InitTorchTBHistoFillSet6();
  void FillSet6TorchTBMcpOccupancy();
  void FillSet6TorchTBHitCoord(CkvG4Hit* adHit );
    
protected:

private:

  int m_NumHitInCurEvTorchTB;
  int m_NumHitInMcp0TorchTB;
  int m_NumHitInMcp1TorchTB;
  
};
#endif // TORCHTESTBEAMANALYSIS_TORCHTBG4FILLHISTOSET6_H
