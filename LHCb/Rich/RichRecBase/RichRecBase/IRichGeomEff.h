
//-----------------------------------------------------------------------------
/** @file IRichGeomEff.h
 *
 *  Header file for RICH reconstruction tool interface : Rich::Rec::IGeomEff
 *
 *  CVS Log :-
 *  $Id: IRichGeomEff.h,v 1.6 2007-02-01 17:26:21 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_IRICHGEOMEFF_H
#define RICHRECTOOLS_IRICHGEOMEFF_H 1

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// Event model
namespace LHCb
{
  class RichRecSegment;
}

/// Static Interface Identification
static const InterfaceID IID_IRichGeomEff( "Rich::Rec::IGeomEff", 1, 0 );

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

  /** @namespace Rich::Rec
   *
   *  General namespace for RICH reconstruction software
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   08/07/2004
   */
  namespace Rec
  {

    //-----------------------------------------------------------------------------
    /** @class IGeomEff IRichGeomEff.h
     *
     *  Interface for tool to calculate the geometrical efficiencies for a given
     *  RichRecSegment and mass hypotheses. The geometrical efficiencies are
     *  defined as the fraction of the Cherenkov ring that falls on the active HPD
     *  area, and thus is detectable.
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   15/03/2002
     */
    //-----------------------------------------------------------------------------

    class IGeomEff : public virtual IAlgTool
    {

    public:

      /** static interface identification
       *  @return unique interface identifier
       */
      static const InterfaceID& interfaceID() { return IID_IRichGeomEff; }

      /** Calculate the signal geometrical efficiency for the given
       *  RichRecSegment and mass hypothesis
       *
       *  @param segment Pointer to the RichRecSegment
       *  @param id      The mass hypothesis
       *
       *  @return the geometrical efficiency
       */
      virtual double geomEfficiency ( LHCb::RichRecSegment * segment,
                                      const Rich::ParticleIDType id ) const = 0;

      /** Calculate the scattered geometrical efficiency for the given
       *  RichRecSegment and mass hypothesis
       *
       *  @param segment Pointer to the RichRecSegment
       *  @param id      The mass hypothesis
       *
       *  @return the geometrical efficiency
       */
      virtual double geomEfficiencyScat ( LHCb::RichRecSegment * segment,
                                          const Rich::ParticleIDType id ) const = 0;

    };

  }
}

#endif // RICHRECTOOLS_IRICHGEOMEFF_H
