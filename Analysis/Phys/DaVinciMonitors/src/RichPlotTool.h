// $Id: RichPlotTool.h,v 1.6 2010-04-26 16:06:34 jonrob Exp $
#ifndef RichPlotTool_H
#define RichPlotTool_H 1

// STL
#include <sstream>

// Gaudi
#include "GaudiKernel/IJobOptionsSvc.h"

// local
#include "BasePlotTool.h"

// Rich
#include "RichRecBase/IRichPIDPlots.h"
#include "RichKernel/RichHashMap.h"

/** @class RichPlotTool RichPlotTool.h
 *
 *  Fills RICH specific plots
 *
 *  @author Chris Jones
 *  @date   17/02/2009
 */
class RichPlotTool : public BasePlotTool
{

public:

  /// Standard constructor
  RichPlotTool( const std::string& type,
                const std::string& name,
                const IInterface* parent );

  virtual ~RichPlotTool( ); ///< Destructor

  /// Initialise
  StatusCode initialize();

protected:

  /// Fill final state PID plots - overwrites BasePlotTools method
  StatusCode fillImpl( const LHCb::Particle* p,
                       const std::string& trailer = "" );

private:

  /// Pointer to Job Options Service
  IJobOptionsSvc* joSvc() const;

  /// Get the Rich type enum from the Particle properties object
  Rich::ParticleIDType pidType( const LHCb::ParticleProperty * prop ) const;

  /// Returns the PID tool instance for the given Particle Name
  const Rich::Rec::IPIDPlots * pidTool( const std::string & name ) const;

private:

  /// Mapping between trailer and PID tool
  typedef Rich::HashMap< std::string, const Rich::Rec::IPIDPlots * > PIDToolMap;

private:

  /// Pointer to job options service
  mutable IJobOptionsSvc * m_jos;

  /// Mapping between particle names and PID plot tools
  mutable PIDToolMap m_pidTools;

  /// Fill full set of histograms
  bool m_extraHistos;     

  /// Plot Tool Configuration
  Rich::Rec::IPIDPlots::Configuration m_pidConfig;

};

#endif // RichPlotTool_H
