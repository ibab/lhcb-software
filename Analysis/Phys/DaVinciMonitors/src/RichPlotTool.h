// $Id: RichPlotTool.h,v 1.1 2009-02-13 12:28:16 jonrob Exp $
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

  virtual StatusCode initialize();    ///< initialization

protected:

  /// Fill final state PID plots - overwrites BasePlotTools method
  StatusCode fillImpl( const LHCb::Particle* p,
                       const std::string trailer = "" );

private:

  /// Pointer to Job Options Service
  IJobOptionsSvc* joSvc() const;

  /// Get the Rich type enum from the Particle properties object
  const Rich::ParticleIDType pidType( const LHCb::ParticleProperty * prop ) const;

private:

  /// Pointer to job options service
  mutable IJobOptionsSvc * m_jos;

  /// Rich Plot tool
  const Rich::Rec::IPIDPlots * m_plots;

  /// Fill full set of histograms
  bool m_extraHistos;       

};

#endif // RichPlotTool_H
