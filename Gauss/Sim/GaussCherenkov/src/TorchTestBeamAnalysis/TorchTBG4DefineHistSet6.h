// $Id: $
#ifndef TORCHTESTBEAMANALYSIS_TORCHTBG4DEFINEHISTSET6_H 
#define TORCHTESTBEAMANALYSIS_TORCHTBG4DEFINEHISTSET6_H 1

// Include files
#include <string>
#include <cmath>
#include <vector>
#include "GaudiKernel/ISvcLocator.h"

// Forward declarations
class IHistogram1D;
class IHistogram2D;


/** @class TorchTBG4DefineHistSet6 TorchTBG4DefineHistSet6.h TorchTestBeamAnalysis/TorchTBG4DefineHistSet6.h
 *  
 *
 *  @author Sajan Easo
 *  @date   2012-06-01
 */
class TorchTBG4DefineHistSet6 {
public: 
  /// Standard constructor
  TorchTBG4DefineHistSet6( ); 

  virtual ~TorchTBG4DefineHistSet6( ); ///< Destructor

protected:

private:

  void BookTorchTBCherenkovG4HistogramSet6();
  
  IHistogram1D*  m_histoNumTotHitInTestBeam;
  IHistogram1D*  m_histoNumTotHitMcp0;
  IHistogram1D*  m_histoNumTotHitMcp1;
  
  IHistogram1D*  m_histoHitZLocationMcp0;
  
  std::string m_TorchTBHistoPathSet6;
 
};
#endif // TORCHTESTBEAMANALYSIS_TORCHTBG4DEFINEHISTSET6_H
