// $Id: RichMCTrackInfoTool.h,v 1.1.1.1 2004-01-29 16:39:23 jonesc Exp $
#ifndef RICHMCTOOLS_RICHMCTRACKINFOTOOL_H
#define RICHMCTOOLS_RICHMCTRACKINFOTOOL_H 1

// from Gaudi
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ToolFactory.h"

// base class
#include "RichUtils/RichToolBase.h"

// Event model
#include "Event/MCParticle.h"
#include "Event/MCTruth.h"
#include "Event/RichDigit.h"
#include "Event/MCRichDigit.h"
#include "Event/MCRichOpticalPhoton.h"
#include "Event/MCRichSegment.h"
#include "Event/MCRichTrack.h"

// Interfaces
#include "RichMCTools/IRichMCTrackInfoTool.h"
#include "RichDetTools/IRichDetInterface.h"
#include "RichDetTools/IRichSmartIDTool.h"

// CLHEP
#include "CLHEP/Units/PhysicalConstants.h"

/** @class RichMCTrackInfoTool RichMCTrackInfoTool.h
 *
 *  Tool providing MC information for RICH tracking objects
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

class RichMCTrackInfoTool : public RichToolBase,
                            virtual public IRichMCTrackInfoTool {

public:

  /// Standard constructor
  RichMCTrackInfoTool( const std::string& type,
                       const std::string& name,
                       const IInterface* parent );

  /// Initialize method
  StatusCode initialize();

  /// Finalize method
  StatusCode finalize();

  /// Computes the intersect points with the HPD panel in LHCb global coordinates
  const bool panelIntersectGlobal( const MCRichSegment * segment,
                                   HepPoint3D & hitPoint ) const;

  /// Computes the intersect points with the HPD panel in HPD panel local coordinates
  const bool panelIntersectLocal( const MCRichSegment * segment,
                                  HepPoint3D & hitPoint ) const;

private: // private methods


private: // private data

  IRichDetInterface * m_richDetInt;   ///< Pointer to the RichDetInterface tool
  IRichSmartIDTool * m_smartIDTool;   ///< Pointer to RichSmartID tool

};

#endif // RICHMCTOOLS_RICHMCTRACKINFOTOOL_H
