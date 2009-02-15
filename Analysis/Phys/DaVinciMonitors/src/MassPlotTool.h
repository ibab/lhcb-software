// $Id: MassPlotTool.h,v 1.2 2009-02-15 22:03:51 jonrob Exp $
#ifndef MassPlotTool_H
#define MassPlotTool_H 1

// Include files
// from Gaudi
#include "BasePlotTool.h"

/** @class MassPlotTool MassPlotTool.h
 *
 *  Fills Mass plots
 *
 *  @author Patrick Koppenburg
 *  @date   2008-12-05
 */
class MassPlotTool : public BasePlotTool
{

public:

  /// Standard constructor
  MassPlotTool( const std::string& type,
                const std::string& name,
                const IInterface* parent );

  virtual ~MassPlotTool( ); ///< Destructor

protected:

  /// Fill final state PID plots - overwrites BasePlotTools method
  StatusCode fillImpl( const LHCb::Particle* p,
                       const std::string& trailer = "" );

};

#endif // MassPlotTool_H
