
//-----------------------------------------------------------------------------
/** @file IRichSnellsLawRefraction.h
 *
 *  Header file for tool interface : Rich::ISnellsLawRefraction
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   03/01/2008
 */
//-----------------------------------------------------------------------------

#ifndef RICHKERNEL_IRichSnellsLawRefraction_H
#define RICHKERNEL_IRichSnellsLawRefraction_H 1

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// from MathCore
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"

// Event Model
namespace LHCb
{
  class RichTrackSegment;
}

/// Static Interface Identification
static const InterfaceID IID_IRichSnellsLawRefraction( "Rich::ISnellsLawRefraction", 1, 0 );

namespace Rich
{

  //-----------------------------------------------------------------------------
  /** @class ISnellsLawRefraction RichKernel/IRichSnellsLawRefraction.h
   *
   *  Interface to a tool which implements snells law refraction at Aerogel / Rich1Gas
   *  boundary.
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   03/01/2008
   */
  //-----------------------------------------------------------------------------

  class ISnellsLawRefraction : public virtual IAlgTool
  {

  public:

    /** static interface identification
     *  @return unique interface identifier
     */
    static const InterfaceID& interfaceID() { return IID_IRichSnellsLawRefraction; }

    /** Correct the direction vector and start point for refraction in going from Aerogel to Rich1Gas.
     *
     *  Uses the effective refractive index seen by the track segment in question.
     *
     *  @param[in]     startPoint The starting point
     *  @param[in,out] dir   The direction to update (WARNING Must be a unit vector)
     *  @param[in]     trSeg The aerogel track segment associated to the photon direction to update
     */
    virtual void aerogelToGas( Gaudi::XYZPoint & startPoint,
                               Gaudi::XYZVector & dir,
                               const LHCb::RichTrackSegment& trSeg ) const = 0;

    /** Correct the direction vector and start point for refraction in going from Aerogel to Rich1Gas
     *
     *  Uses a simple average of the refractive index for the radiator volumes
     *
     *  @param[in]     startPoint The starting point
     *  @param[in,out] dir        The direction to update (WARNING Must be a unit vector)
     *  @param[in]     photonEnergy The energy of the photon (used to get the correct refractive indices)
     */
    virtual void aerogelToGas( Gaudi::XYZPoint & startPoint,
                               Gaudi::XYZVector & dir,
                               const double photonEnergy ) const = 0;

    /** Correct the direction vector for refraction in going from Rich1Gas to aerogel
     *
     *  Uses the effective refractive index seen by the track segment in question.
     *
     *  @param[in,out] dir   The direction to update (WARNING Must be a unit vector)
     *  @param[in]     trSeg The aerogel track segment associated to the photon direction to update
     */
    virtual void gasToAerogel( Gaudi::XYZVector & dir,
                               const LHCb::RichTrackSegment& trSeg ) const = 0;

    /** Correct the direction vector for refraction in going from Rich1Gas to aerogel
     *
     *  Uses a simple average of the refractive index for the radiator volumes
     *
     *  @param[in,out] dir        The direction to update (WARNING Must be a unit vector)
     *  @param[in]     photonEnergy The energy of the photon (used to get the correct refractive indices)
     */
    virtual void gasToAerogel( Gaudi::XYZVector & dir,
                               const double photonEnergy ) const = 0;

  };

}

#endif // RICHKERNEL_IRichSnellsLawRefraction_H
