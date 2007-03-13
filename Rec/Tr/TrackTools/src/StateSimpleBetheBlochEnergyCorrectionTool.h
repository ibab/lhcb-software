// $Id: StateSimpleBetheBlochEnergyCorrectionTool.h,v 1.2 2007-03-13 14:32:31 cattanem Exp $
#ifndef TRACKTOOLS_STATESIMPLEBETHEBLOCHENERGYCORRECTIONTOOL_H 
#define TRACKTOOLS_STATESIMPLEBETHEBLOCHENERGYCORRECTIONTOOL_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from TrackInterfaces
#include "TrackInterfaces/IStateCorrectionTool.h"

// from TrackEvent
#include "Event/State.h"

/** @class StateSimpleBetheBlochEnergyCorrectionTool
 *  
 *  This state correction tool applies a dE/dx energy loss correction
 *  with a simplified version of the Bethe-Bloch equation.
 *
 *  @author Eduardo Rodrigues
 *  @date   2006-08-18
 *  (Original code taken from the master extrapolator)
 */
class StateSimpleBetheBlochEnergyCorrectionTool : public GaudiTool,
                                                  virtual public IStateCorrectionTool {
public:
  /// Correct a State for dE/dx energy losses with a simplified Bethe-Bloch equiaton
  void correctState( LHCb::State& state,
                     const Material* material,
                     double wallThickness = 0,
                     bool upstream = true );
  
  /// Standard constructor
  StateSimpleBetheBlochEnergyCorrectionTool( const std::string& type, 
                                             const std::string& name,
                                             const IInterface* parent);
  
  virtual ~StateSimpleBetheBlochEnergyCorrectionTool( ); ///< Destructor
  
  StatusCode initialize();
  
protected:
  
private:
  // Job options
  double m_energyLossCorr; ///< tunable energy loss correction
  double m_maxEnergyLoss;  ///< maximum energy loss in dE/dx correction
  
};
#endif // TRACKTOOLS_STATESIMPLEBETHEBLOChENERGYCORRECTIONTOOL_H
