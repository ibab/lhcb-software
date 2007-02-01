
/** @file RichMCTrackInfoTool.h
 *
 *  Header file for tool : Rich::MC::TrackInfoTool
 *
 *  CVS Log :-
 *  $Id: RichMCTrackInfoTool.h,v 1.12 2007-02-01 17:50:13 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

#ifndef RICHMCTOOLS_RICHMCTRACKINFOTOOL_H
#define RICHMCTOOLS_RICHMCTRACKINFOTOOL_H 1

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

//-----------------------------------------------------------------------------
/** @namespace Rich
 *
 *  General namespace for RICH software
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
//-----------------------------------------------------------------------------
namespace Rich
{

  //-----------------------------------------------------------------------------
  /** @namespace MC
   *
   *  General namespace for RICH MC related software
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   05/12/2006
   */
  //-----------------------------------------------------------------------------
  namespace MC
  {

    /** @class MCTrackInfoTool RichMCTrackInfoTool.h
     *
     *  Tool providing tracking extrapolation information from
     *  RICH Monte Carlo data objects
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   15/03/2002
     */

    class MCTrackInfoTool : public Rich::ToolBase,
                            virtual public IMCTrackInfoTool
    {

    public: // Methods for Gaudi Framework

      /// Standard constructor
      MCTrackInfoTool( const std::string& type,
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

      const IRayTracing * m_rayTrace;     ///< Pointer to ray tracing tool
      const ISmartIDTool * m_smartIDTool; ///< Pointer to RichSmartID tool

      /// Ray-tracing configuration object
      LHCb::RichTraceMode m_traceMode;

    };

  }
}

#endif // RICHMCTOOLS_RICHMCTRACKINFOTOOL_H
