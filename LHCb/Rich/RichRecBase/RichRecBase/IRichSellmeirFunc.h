// $Id: IRichSellmeirFunc.h,v 1.1 2003-08-06 09:55:31 jonrob Exp $
#ifndef RICHRECTOOLS_IRICHSELLMEIRFUNC_H
#define RICHRECTOOLS_IRICHSELLMEIRFUNC_H 1

// Event model
class RichRecSegment;

/** @class IRichSellmeirFunc IRichSellmeirFunc.h
 *
 *  Interface for tool to calculate the Sellmeir parameters
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

static const InterfaceID IID_IRichSellmeirFunc( "IRichSellmeirFunc", 1, 0 );

class IRichSellmeirFunc : public virtual IAlgTool {

public:

  /// Static interface identification
  static const InterfaceID& interfaceID() {return IID_IRichSellmeirFunc;}

  virtual double paraWt ( Rich::RadiatorType rad, double topEn ) = 0;

  virtual double paraWb ( Rich::RadiatorType rad, double botEn ) = 0;

  /// Computes the number of photons emitted in the given energy range for a 
  /// given RichRecSegment under a certain mass hypothesis
  virtual double photonsInEnergyRange( RichRecSegment * segment, 
                                       const Rich::ParticleIDType id,
                                       double botEn,
                                       double topEn ) = 0;

};

#endif // RICHRECTOOLS_IRICHSELLMEIRFUNC_H
