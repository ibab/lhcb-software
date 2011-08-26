// $Id: CombinedPidPlotTool.h,v 1.2 2009-02-15 22:03:51 jonrob Exp $
#ifndef DAVINCIMONITORS_CombinedPidPlotTool_H
#define DAVINCIMONITORS_CombinedPidPlotTool_H 1

// base class
#include "BasePlotTool.h"

// boost
#include "boost/lexical_cast.hpp"

/** @class CombinedPidPlotTool CombinedPidPlotTool.h
 *
 *  Fills Combined PID plots
 *
 *  @author Patrick Koppenburg
 *  @date   2008-12-05
 */
class CombinedPidPlotTool : public BasePlotTool
{

public:

  /// Standard constructor
  CombinedPidPlotTool( const std::string& type,
                       const std::string& name,
                       const IInterface* parent);

  virtual ~CombinedPidPlotTool( ); ///< Destructor

protected:

  /// Fill final state PID plots - overwrites BasePlotTools method
  StatusCode fillImpl( const LHCb::Particle* p,
                       const std::string& trailer = "" );

};

#endif // DAVINCIMONITORS_CombinedPidPlotTool_H
