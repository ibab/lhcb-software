// $Id: MomentumPlotTool.h,v 1.2 2009-02-15 22:03:51 jonrob Exp $
#ifndef DAVINCIMONITORS_MomentumPlotTool_H
#define DAVINCIMONITORS_MomentumPlotTool_H 1

// base class
#include "BasePlotTool.h"

// boost
#include "boost/lexical_cast.hpp"

/** @class MomentumPlotTool MomentumPlotTool.h
 *
 *  Fills momentum plots
 *
 *  @author Patrick Koppenburg
 *  @date   2008-12-05
 */
class MomentumPlotTool : public BasePlotTool
{

public:

  /// Standard constructor
  MomentumPlotTool( const std::string& type,
                    const std::string& name,
                    const IInterface* parent);

  virtual ~MomentumPlotTool( ); ///< Destructor

protected:

  /// Fill final state PID plots - overwrites BasePlotTools method
  StatusCode fillImpl( const LHCb::Particle* p,
                       const std::string& trailer = "" );

};

#endif // DAVINCIMONITORS_MomentumPlotTool_H
