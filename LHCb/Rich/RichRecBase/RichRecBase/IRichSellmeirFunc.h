// $Id: IRichSellmeirFunc.h,v 1.4 2004-04-17 09:28:03 jonesc Exp $
#ifndef RICHRECTOOLS_IRICHSELLMEIRFUNC_H
#define RICHRECTOOLS_IRICHSELLMEIRFUNC_H 1

// Event model
class RichRecSegment;

/** @class IRichSellmeirFunc IRichSellmeirFunc.h
 *
 *  Interface for tool to calculate quantities using the Sellmeir
 *  function and related parameters
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

static const InterfaceID IID_IRichSellmeirFunc( "IRichSellmeirFunc", 1, 0 );

class IRichSellmeirFunc : public virtual IAlgTool {

public:

  /// Static interface identification
  static const InterfaceID& interfaceID() {return IID_IRichSellmeirFunc;}

  /// Computes the number of photons emitted in the given energy range for a 
  /// given RichRecSegment under a certain mass hypothesis
  virtual double photonsInEnergyRange( RichRecSegment * segment, 
                                       const Rich::ParticleIDType id,
                                       const double botEn,
                                       const double topEn ) const = 0;

};

#endif // RICHRECTOOLS_IRICHSELLMEIRFUNC_H
