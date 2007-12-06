// $Id: StateElectronEnergyCorrectionTool.h,v 1.3 2007-12-06 14:53:55 wouter Exp $
#ifndef TRACKTOOLS_STATEELECTRONENERGYCORRECTIONTOOL_H 
#define TRACKTOOLS_STATEELECTRONENERGYCORRECTIONTOOL_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from TrackInterfaces
#include "TrackInterfaces/IStateCorrectionTool.h"

// from TrackEvent
#include "Event/State.h"

/** @class StateElectronEnergyCorrectionTool
 *  
 *  This state correction tool applies a dE/dx energy loss correction
 *  to electrons
 *
 *  @author Eduardo Rodrigues
 *  @date   2006-08-18
 *  (Original code taken from the master extrapolator)
 */
class StateElectronEnergyCorrectionTool : public GaudiTool,
                                          virtual public IStateCorrectionTool {
public:
  /// Correct a State for electron dE/dx energy losses
  void correctState( LHCb::State& state,
                     const Material* material,
                     double wallThickness = 0,
                     bool upstream = true ) const ;
  
  /// Standard constructor
  StateElectronEnergyCorrectionTool( const std::string& type, 
                                     const std::string& name,
                                     const IInterface* parent);
  
  virtual ~StateElectronEnergyCorrectionTool( ); ///< Destructor
  
  StatusCode initialize();
  
protected:
  
private:
  // Job options
  double m_maxRadLength;   ///< maximum radiation length (in mm)

};
#endif // TRACKTOOLS_STATEELECTRONENERGYCORRECTIONTOOL_H
