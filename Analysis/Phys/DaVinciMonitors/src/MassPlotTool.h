// $Id: MassPlotTool.h,v 1.3 2009-05-14 11:25:57 jonrob Exp $
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

private:

  /** Typical reconstruction resolution. Added in quadrature to particle width
   *  and used to determine the histogram minimum and maximum values */
  double m_recoRes;

  /// Size of the mass plot range in +- # sigma
  double m_nSigma;

};

#endif // MassPlotTool_H
