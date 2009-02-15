// $Id: MuonPlotTool.h,v 1.2 2009-02-15 22:03:51 jonrob Exp $
#ifndef DAVINCIMONITORS_MuonPlotTool_H
#define DAVINCIMONITORS_MuonPlotTool_H 1

// bass class
#include "BasePlotTool.h"

/** @class MuonPlotTool MuonPlotTool.h
 *
 *  Fills MUON specific PID plots
 *
 *  @author Patrick Koppenburg
 *  @date   2008-12-05
 */
class MuonPlotTool : public BasePlotTool
{

public:

  /// Standard constructor
  MuonPlotTool( const std::string& type,
                const std::string& name,
                const IInterface* parent);

  virtual ~MuonPlotTool( ); ///< Destructor

protected:

  /// Fill final state PID plots - overwrites BasePlotTools method
  StatusCode fillImpl( const LHCb::Particle* p,
                       const std::string& trailer = "" );

};

#endif // DAVINCIMONITORS_MuonPlotTool_H
