
//-----------------------------------------------------------------------------
/** @file IRichGeomEff.h
 *
 *  Header file for RICH reconstruction tool interface : IRichGeomEff
 *
 *  CVS Log :-
 *  $Id: IRichGeomEff.h,v 1.3 2004-07-26 18:00:57 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_IRICHGEOMEFF_H
#define RICHRECTOOLS_IRICHGEOMEFF_H 1

// Event model
class RichRecSegment;

/// Static Interface Identification
static const InterfaceID IID_IRichGeomEff( "IRichGeomEff", 1, 0 );

/** @class IRichGeomEff IRichGeomEff.h
 *
 *  Interface for tool to calculate the geometrical efficiencies for a given
 *  RichRecSegment and mass hypotheses. The geometrical efficiencies are
 *  defined as the fraction of the Cherenkov ring that falls on the active HPD
 *  area, and thus is detectable.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

class IRichGeomEff : public virtual IAlgTool {

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
  virtual double geomEfficiency ( RichRecSegment * segment,
                                  const Rich::ParticleIDType id ) const = 0;

  /** Calculate the scattered geometrical efficiency for the given
   *  RichRecSegment and mass hypothesis
   *
   *  @param segment Pointer to the RichRecSegment
   *  @param id      The mass hypothesis
   *
   *  @return the geometrical efficiency
   */
  virtual double geomEfficiencyScat ( RichRecSegment * segment,
                                      const Rich::ParticleIDType id ) const = 0;

};

#endif // RICHRECTOOLS_IRICHGEOMEFF_H
