// $Id: IPlotTool.h,v 1.2 2005-01-05 17:32:29 pkoppenb Exp $
#ifndef KERNEL_IPLOTTOOL_H 
#define KERNEL_IPLOTTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "Event/Particle.h"

static const InterfaceID IID_IPlotTool ( "IPlotTool", 1, 0 );

/** @class IPlotTool IPlotTool.h Kernel/IPlotTool.h
 *  
 *  Interface for tools making plots with particles
 *
 *  @author Patrick KOPPENBURG
 *  @date   2005-01-05
 */
class IPlotTool : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IPlotTool; }

  /// Define plot directory
  virtual StatusCode setPath(const std::string&) = 0;

  /// Fill plots using a ParticleVector
  virtual StatusCode fillPlots(const ParticleVector&) = 0;

  /// Fill plots using a single Particle
  virtual StatusCode fillPlots(const Particle*) = 0;


protected:

private:

};
#endif // KERNEL_IPLOTTOOL_H
