// $Id: PidPlotTool.h,v 1.4 2009-02-13 12:28:16 jonrob Exp $
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
class PidPlotTool : public BasePlotTool 
{

public:

  /// Standard constructor
  PidPlotTool( const std::string& type,
               const std::string& name,
               const IInterface* parent);

  virtual ~PidPlotTool( ); ///< Destructor

  /// initialisation
  StatusCode initialize();

protected:

  /// Fill final state PID plots - overwrites BasePlotTools method
  StatusCode fillImpl( const LHCb::Particle* p,
                       const std::string trailer = "" );

private:
  
  /// Fill PID plot pair
  void fillPID ( double val, double p, 
                 const std::string & part,
                 const LHCb::ParticleProperty* pp, 
                 const std::string & trailer );

};

#endif // PIDPLOTTOOL_H
