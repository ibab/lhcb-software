
//-----------------------------------------------------------------------------
/** @file IRichCherenkovResolution.h
 *
 *  Header file for RICH reconstruction tool interface : IRichCherenkovResolution
 *
 *  CVS Log :-
 *  $Id: IRichCherenkovResolution.h,v 1.7 2006-03-12 16:13:47 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_IRICHCHERENKOVRESOLUTION_H
#define RICHRECTOOLS_IRICHCHERENKOVRESOLUTION_H 1

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// Event model
namespace LHCb
{
  class RichRecSegment;
}

/// Static Interface Identification
static const InterfaceID IID_IRichCherenkovResolution( "IRichCherenkovResolution", 1, 0 );

//-----------------------------------------------------------------------------
/** @class IRichCherenkovResolution IRichCherenkovResolution.h
 *
 *  Interface for tool providing the expected Cherenkov angle resolution for
 *  given RichRecSegment and mass hypothesis combinations.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

class IRichCherenkovResolution : public virtual IAlgTool
{

public:

  /** static interface identification
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() {return IID_IRichCherenkovResolution;}

  /** Calculates the Cherenkov theta resolution for given RichRecSegment
   *  and particle mass hypothesis
   *
   *  @param segment Pointer to the RichRecSegment
   *  @param id Mass hypothesis
   *
   *  @return Cherenkov angle theta resolution
   */
  virtual double ckThetaResolution( LHCb::RichRecSegment * segment,
                                    const Rich::ParticleIDType id = Rich::Pion ) const = 0;

};

#endif // RICHRECTOOLS_IRICHCHERENKOVRESOLUTION_H
