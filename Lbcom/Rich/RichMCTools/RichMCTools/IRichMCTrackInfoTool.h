// $Id: IRichMCTrackInfoTool.h,v 1.1.1.1 2004-01-29 16:39:24 jonesc Exp $
#ifndef RICHMCTOOLS_IRICHMCTRACKINFOTOOL_H
#define RICHMCTOOLS_IRICHMCTRACKINFOTOOL_H 1

// Event Model
class MCRichOpticalPhoton;
class MCRichSegment;
class MCRichTrack;
class MCRichDigit;
class MCParticle;
class MCRichHit;

/** @class IRichMCTrackInfoTool IRichMCTrackInfoTool.h
 *
 *  Interface to tool providing MC information for RICH tracking objects
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

static const InterfaceID IID_IRichMCTrackInfoTool( "IRichMCTrackInfoTool", 1, 0 );

class IRichMCTrackInfoTool : public virtual IAlgTool {

public:

  /// Static interface identification
  static const InterfaceID& interfaceID() { return IID_IRichMCTrackInfoTool; }

  /// Computes the intersect points with the HPD panel in LHCb global coordinates
  virtual const bool panelIntersectGlobal( const MCRichSegment * segment,
                                           HepPoint3D & hitPoint ) const = 0;

  /// Computes the intersect points with the HPD panel in HPD panel local coordinates
  virtual const bool panelIntersectLocal( const MCRichSegment * segment,
                                          HepPoint3D & hitPoint ) const = 0;

};

#endif // RICHMCTOOLS_IRICHMCTRACKINFOTOOL_H
