// $Id: PidPlotTool.h,v 1.2 2008-12-08 18:12:13 pkoppenb Exp $
#ifndef PIDPLOTTOOL_H 
#define PIDPLOTTOOL_H 1

// Include files
// from Gaudi
#include "BasePlotTool.h"

/** @class PidPlotTool PidPlotTool.h
 *  
 *  Fills PID plots
 *
 *  @author Patrick Koppenburg
 *  @date   2008-12-05
 */
class PidPlotTool : public BasePlotTool {
public: 
  /// Standard constructor
  PidPlotTool( const std::string& type, 
               const std::string& name,
               const IInterface* parent);

  virtual ~PidPlotTool( ); ///< Destructor

  /// initialisation
  StatusCode initialize();

protected:

private:
  /// Fill mother mass plot
  StatusCode fillMother(const LHCb::Particle*,const std::string trailer = "");
  /// Fill final state PID plots
  StatusCode fillFinal(const LHCb::Particle*,const std::string trailer = "");
  /// Fill PID plot pair
  void fillPID(double val, double p, std::string part, 
               const LHCb::ParticleProperty* pp, std::string trailer);
  
};
#endif // PIDPLOTTOOL_H
