
//-----------------------------------------------------------------------------
/** @file IRichPhotonPredictor.h
 *
 *  Header file for RICH reconstruction tool interface : IRichPhotonPredictor
 *
 *  CVS Log :-
 *  $Id: IRichPhotonPredictor.h,v 1.8 2006-03-12 16:13:47 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_IRICHPHOTONPREDICTOR_H
#define RICHRECTOOLS_IRICHPHOTONPREDICTOR_H 1

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// Event
namespace LHCb
{
  class RichRecPixel;
  class RichRecSegment;
}

/// Static Interface Identification
static const InterfaceID IID_IRichPhotonPredictor( "IRichPhotonPredictor" , 1 , 0 );

//-----------------------------------------------------------------------------
/** @class IRichPhotonPredictor IRichPhotonPredictor.h
 *
 *  Interface for a simple tool to perform a relatively fast determination of whether
 *  a valid RichRecPhoton candidate can be produced using a given RichRecSegment
 *  and RichRecPixel. The means by which this is determined depends on the particular
 *  instance of the tool used.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

class IRichPhotonPredictor : public virtual IAlgTool
{

public:

  /** static interface identification
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() { return IID_IRichPhotonPredictor; }

  /** Determines if it is possible to make a reconstructed photon candidate
   *  using a particular segment and pixel combination.
   *
   *  @param segment Pointer to the RichRecSegment
   *  @param pixel   Pointer to the RichRecPixel
   *
   *  @return boolean indicating if a photon is possible
   *  @retval true  It is geometrically possible for the given pixel to be the
   *                result of Cherenkov radiation from the given segment
   *  @retval false It is not possible for the pixel to be CHerenkov radiation
   *                from the segment
   */
  virtual bool photonPossible( LHCb::RichRecSegment * segment,
                               LHCb::RichRecPixel * pixel ) const = 0;

};

#endif // RICHRECTOOLS_IRICHPHOTONPREDICTOR_H
