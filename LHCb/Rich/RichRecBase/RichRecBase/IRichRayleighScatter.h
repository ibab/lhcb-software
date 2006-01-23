
//-----------------------------------------------------------------------------
/** @file IRichRayleighScatter.h
 *
 *  Header file for RICH reconstruction tool interface : IRichRayleighScatter
 *
 *  CVS Log :-
 *  $Id: IRichRayleighScatter.h,v 1.5 2006-01-23 14:08:55 jonrob Exp $
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
static const InterfaceID IID_IRichRayleighScatter( "IRichRayleighScatter", 1, 0 );

//-----------------------------------------------------------------------------
/** @class IRichRayleighScatter IRichRayleighScatter.h
 *
 *  Interface for tool to calculate quantities related to Rayleigh scattering
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

class IRichRayleighScatter : public virtual IAlgTool
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

#endif // RICHRECTOOLS_IRICHRAYLEIGHSCATTER_H
