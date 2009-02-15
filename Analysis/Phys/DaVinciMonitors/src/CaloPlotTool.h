// $Id: CaloPlotTool.h,v 1.2 2009-02-15 22:03:51 jonrob Exp $
#ifndef DAVINCIMONITORS_CaloPlotTool_H
#define DAVINCIMONITORS_CaloPlotTool_H 1

// bass class
#include "BasePlotTool.h"

// boost
#include "boost/lexical_cast.hpp"

/** @class CaloPlotTool CaloPlotTool.h
 *
 *  Fills CALO specific PID plots
 *
 *  @author Patrick Koppenburg
 *  @date   2008-12-05
 */
class CaloPlotTool : public BasePlotTool
{

public:

  /// Standard constructor
  CaloPlotTool( const std::string& type,
                const std::string& name,
                const IInterface* parent);

  virtual ~CaloPlotTool( ); ///< Destructor

protected:

  /// Fill final state PID plots - overwrites BasePlotTools method
  StatusCode fillImpl( const LHCb::Particle* p,
                       const std::string& trailer = "" );

};

#endif // DAVINCIMONITORS_CaloPlotTool_H
