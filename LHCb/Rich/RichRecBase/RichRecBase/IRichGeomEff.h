// $Id: IRichGeomEff.h,v 1.2 2004-02-02 14:23:02 jonesc Exp $
#ifndef RICHRECTOOLS_IRICHGEOMEFF_H
#define RICHRECTOOLS_IRICHGEOMEFF_H 1

// Event model
class RichRecSegment;

/** @class IRichGeomEff IRichGeomEff.h
 *
 *  Interface for tool to calculate the geometrical efficiency for a given
 *  RichRecSegment and mass hypothesis
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

static const InterfaceID IID_IRichGeomEff( "IRichGeomEff", 1, 0 );

class IRichGeomEff : public virtual IAlgTool {

public:

  /// static interface identification
  static const InterfaceID& interfaceID() { return IID_IRichGeomEff; }

  /// Obtain signal geometrical efficiency for the given RichRecSegment and mass hypothesis
  virtual double geomEfficiency ( RichRecSegment * segment,
                                  const Rich::ParticleIDType id ) const = 0;

  /// Obtain scattered geometrical efficiency for the given RichRecSegment and mass hypothesis
  virtual double geomEfficiencyScat ( RichRecSegment * segment,
                                      const Rich::ParticleIDType id ) const = 0;

};

#endif // RICHRECTOOLS_IRICHGEOMEFF_H
