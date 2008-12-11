
/** @file IRichMCTrackInfoTool.h
 *
 *  Header file for tool interface : Rich::IMCTrackInfoTool
 *
 *  CVS Log :-
 *  $Id: IRichMCTrackInfoTool.h,v 1.2 2008-12-11 13:57:00 cattanem Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

#ifndef RICHKERNEL_IRICHMCTRACKINFOTOOL_H
#define RICHKERNEL_IRICHMCTRACKINFOTOOL_H 1

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/Point3DTypes.h"

// Event Model
namespace LHCb
{
  class MCRichSegment;
}

/// Static Interface Identification
static const InterfaceID IID_IRichMCTrackInfoTool( "Rich::IMCTrackInfoTool", 1, 0 );

namespace Rich
{
  namespace MC
  {

    /** @class IMCTrackInfoTool IRichMCTrackInfoTool.h
     *
     *  Interface to tool providing tracking extrapolation information from
     *  RICH Monte Carlo data objects
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   15/03/2002
     */

    class IMCTrackInfoTool : public virtual IAlgTool
    {

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
      virtual bool
      panelIntersectGlobal ( const LHCb::MCRichSegment * segment,
                             Gaudi::XYZPoint & hitPoint ) const = 0;

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
      virtual bool
      panelIntersectLocal ( const LHCb::MCRichSegment * segment,
                            Gaudi::XYZPoint & hitPoint ) const = 0;

    };

  }
}

#endif // RICHKERNEL_IRICHMCTRACKINFOTOOL_H
