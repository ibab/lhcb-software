
//-----------------------------------------------------------------------------
/** @file IRichRayleighScatter.h
 *
 *  Header file for RICH reconstruction tool interface : Rich::Rec::IRayleighScatter
 *
 *  CVS Log :-
 *  $Id: IRichRayleighScatter.h,v 1.6 2007-02-01 17:26:21 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_IRICHRAYLEIGHSCATTER_H
#define RICHRECTOOLS_IRICHRAYLEIGHSCATTER_H 1

// Event model
namespace LHCb
{
  class RichRecSegment;
}

/// Static Interface Identification
static const InterfaceID IID_IRichRayleighScatter( "Rich::Rec::IRayleighScatter", 1, 0 );

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
    /** @class IRayleighScatter IRichRayleighScatter.h
     *
     *  Interface for tool to calculate quantities related to Rayleigh scattering
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   15/03/2002
     */
    //-----------------------------------------------------------------------------

    class IRayleighScatter : public virtual IAlgTool
    {

    public:

      /** static interface identification
       *  @return unique interface identifier
       */
      static const InterfaceID& interfaceID() { return IID_IRichRayleighScatter; }

      /** Computes the Rayleigh scattering probability for given RichRecSegment
       *  and photon energy
       *
       *  @param segment Pointer to the RichRecSegment
       *  @param energy  The photon energy
       *
       *  @return The Rayleigh scattering probability
       */
      virtual double photonScatteredProb( const LHCb::RichRecSegment * segment,
                                          const double energy ) const = 0;

    };

  }
}

#endif // RICHRECTOOLS_IRICHRAYLEIGHSCATTER_H
