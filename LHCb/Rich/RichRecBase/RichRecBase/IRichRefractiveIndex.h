// $Id: IRichRefractiveIndex.h,v 1.1 2003-08-06 09:55:31 jonrob Exp $
#ifndef RICHRECTOOLS_IRICHREFRACTIVEINDEX_H
#define RICHRECTOOLS_IRICHREFRACTIVEINDEX_H 1

// Event model
class RichRecSegment;

/** @class IRichRefractiveIndex IRichRefractiveIndex.h
 *
 *  Interface for tool to calculate the effective refractive index for
 *  a given RichRecSegment
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

static const InterfaceID IID_IRichRefractiveIndex( "IRichRefractiveIndex", 1, 0 );

class IRichRefractiveIndex : public virtual IAlgTool {

public:

  /// Static interface identification
  static const InterfaceID& interfaceID() {return IID_IRichRefractiveIndex;}

  /// Average refractive index for this segments radiator
  virtual double refractiveIndex( RichRecSegment * segment ) = 0;

  /// Refractive index for a given energy
  virtual double refractiveIndex( const Rich::RadiatorType radiator,
                                  double energy ) = 0;

  /// Average refractive index for given radiator
  virtual double refractiveIndex( const Rich::RadiatorType radiator ) = 0;

};

#endif // RICHRECTOOLS_IRICHREFRACTIVEINDEX_H
