// $Id: IRichRayleighScatter.h,v 1.2 2004-02-02 14:23:03 jonesc Exp $
#ifndef RICHRECTOOLS_IRICHRAYLEIGHSCATTER_H
#define RICHRECTOOLS_IRICHRAYLEIGHSCATTER_H 1

// Event model
class RichRecSegment;

/** @class IRichRayleighScatter IRichRayleighScatter.h
 *
 *  Interface for tool to calculate quantities related to Rayleigh scattering
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

static const InterfaceID IID_IRichRayleighScatter( "IRichRayleighScatter", 1, 0 );

class IRichRayleighScatter : public virtual IAlgTool {

public:

  /// Static interface identification
  static const InterfaceID& interfaceID() {return IID_IRichRayleighScatter;}

  /// Rayleigh scattering probability for given pathlength (segment) and photyon energy
  virtual double photonScatteredProb( const RichRecSegment * segment,
                                      const double energy ) const = 0;

};

#endif // RICHRECTOOLS_IRICHRAYLEIGHSCATTER_H
