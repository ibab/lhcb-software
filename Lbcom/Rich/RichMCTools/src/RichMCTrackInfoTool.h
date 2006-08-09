
/** @file RichMCTrackInfoTool.h
 *
 *  Header file for tool : RichMCTrackInfoTool
 *
 *  CVS Log :-
 *  $Id: RichMCTrackInfoTool.h,v 1.9 2006-08-09 11:00:12 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

#ifndef RICHMCTOOLS_RICHMCTRACKINFOTOOL_H
#define RICHMCTOOLS_RICHMCTRACKINFOTOOL_H 1

// from Gaudi
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ToolFactory.h"

// base class
#include "RichKernel/RichToolBase.h"

// Event model
#include "Event/MCParticle.h"
#include "Event/MCTruth.h"
#include "Event/MCRichSegment.h"

// Interfaces
#include "RichKernel/IRichMCTrackInfoTool.h"
#include "RichKernel/IRichRayTracing.h"
#include "RichKernel/IRichSmartIDTool.h"

// Constants
#include "Kernel/PhysicalConstants.h"

/** @class RichMCTrackInfoTool RichMCTrackInfoTool.h
 *
 *  Tool providing tracking extrapolation information from
 *  RICH Monte Carlo data objects
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

class RichMCTrackInfoTool : public RichToolBase,
                            virtual public IRichMCTrackInfoTool {

public: // Methods for Gaudi Framework

  /// Standard constructor
  RichMCTrackInfoTool( const std::string& type,
                       const std::string& name,
                       const IInterface* parent );


  // Initialization of the tool after creation
  StatusCode initialize();

  // Finalization of the tool before deletion
  StatusCode finalize();

public: // methods (and doxygen comments) inherited from interface

  // Takes the direction information from a MCRichSegment and ray traces it through the
  // appropriate RICH optical system and computes the intersect points with the HPD
  // panel in LHCb global coordinates
  const bool panelIntersectGlobal ( const LHCb::MCRichSegment * segment,
                                    Gaudi::XYZPoint & hitPoint ) const;

  // Takes the direction information from a MCRichSegment and ray traces it through the
  // appropriate RICH optical system and computes the intersect points with the HPD
  // panel in local HPD panel coordinates
  const bool panelIntersectLocal ( const LHCb::MCRichSegment * segment, 
                                   Gaudi::XYZPoint & hitPoint ) const;

private: // private data

  const IRichRayTracing * m_rayTrace;     ///< Pointer to ray tracing tool
  const IRichSmartIDTool * m_smartIDTool; ///< Pointer to RichSmartID tool

  /// Ray-tracing configuration object
  LHCb::RichTraceMode m_traceMode;

};

#endif // RICHMCTOOLS_RICHMCTRACKINFOTOOL_H
