// $Id: IRichMCTrackInfoTool.h,v 1.3 2004-07-15 15:36:53 jonrob Exp $
#ifndef RICHKERNEL_IRICHMCTRACKINFOTOOL_H
#define RICHKERNEL_IRICHMCTRACKINFOTOOL_H 1

// Event Model
class MCRichOpticalPhoton;
class MCRichSegment;
class MCRichTrack;
class MCRichDigit;
class MCParticle;
class MCRichHit;

/** @class IRichMCTrackInfoTool IRichMCTrackInfoTool.h
 *
 *  Interface to tool providing tracking extrapolation information from
 *  RICH Monte Carlo data objects
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

static const InterfaceID IID_IRichMCTrackInfoTool( "IRichMCTrackInfoTool", 1, 0 );

class IRichMCTrackInfoTool : public virtual IAlgTool {

public:

  /** static interface identification
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() { return IID_IRichMCTrackInfoTool; }

  /** Takes the direction information from a MCRichSegment and ray traces it through the
   *  appropriate RICH optical system and computes the intersect points with the HPD 
   *  panel in LHCb global coordinates.
   *
   *  @param segment  Pointer to an MCRichSegment
   *  @param hitPoint Ray traced hit position on HPD panel in global LHCb coordinates
   *
   *  @return boolean indicating if the intersection was successful
   *  @retval true   intersection was successful, returned hitPoint is valid
   *  @retval false  intersection was unsuccessful, returned hitPoint is not valid
   */
  virtual const bool 
  panelIntersectGlobal ( const MCRichSegment * segment,
                         HepPoint3D & hitPoint ) const = 0;

  /** Takes the direction information from a MCRichSegment and ray traces it through the
   *  appropriate RICH optical system and computes the intersect points with the HPD 
   *  panel in local HPD panel coordinates.
   *
   *  @param segment  Pointer to an MCRichSegment
   *  @param hitPoint Ray traced hit position on HPD panel in local HPD panel coordinates
   *
   *  @return boolean indicating if the intersection was successful
   *  @retval true   intersection was successful, returned hitPoint is valid
   *  @retval false  intersection was unsuccessful, returned hitPoint is not valid
   */
  virtual const bool 
  panelIntersectLocal ( const MCRichSegment * segment, 
                        HepPoint3D & hitPoint ) const = 0;

};

#endif // RICHKERNEL_IRICHMCTRACKINFOTOOL_H
