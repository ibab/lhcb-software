
//-----------------------------------------------------------------------------
/** @file RichStereoProjection.h
 *
 *  Header file for tool : Rich::Rec::StereoProjection
 *
 *  CVS Log :-
 *
 *  @author Luigi Delbuono  delbuono@in2p3.fr
 *  @date   20/06/2007
 */
//-----------------------------------------------------------------------------

// RichStereoProjection
#ifndef RICHRECTOOLS_RICHSTEREOPROJECTION_H
#define RICHRECTOOLS_RICHSTEREOPROJECTION_H 1

// from Gaudi
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/IParticlePropertySvc.h"

// base classes
#include "RichRecBase/RichRecToolBase.h"

// Event model
#include "Event/RichRecSegment.h"
#include "Event/RichRecRing.h"

// interfaces
#include "RichRecBase/IRichStereoProjection.h"
#include "RichKernel/IRichParticleProperties.h"

// GSL
#include "gsl/gsl_math.h"

namespace Rich
{
  namespace Rec
  {

    //--------------------------------------------------------------------------------------
    /** @class StereoProjection RichStereoProjection.h
     *
     *  Tool to perform a stereographic projection for the RICH photons
     *
     *  @author Luigi Delbuono  delbuono@in2p3.fr
     *  @date   20/06/2007
     */
    //--------------------------------------------------------------------------------------

    class StereoProjection : public Rich::Rec::ToolBase,
                             virtual public IStereoProjection
    {

    public: // Methods for Gaudi Framework

      /// Standard constructor
      StereoProjection( const std::string& type,
                        const std::string& name,
                        const IInterface* parent );

      /// Destructor
      virtual ~StereoProjection() {};

      // Initialize method
      StatusCode initialize();

    public: // methods (and doxygen comments) inherited from public interface

      // Project (RichRec) photons pertaining to one segment in the stereo
      // plane perpendicular to the segemnt direction
      int Project( const LHCb::RichRecSegment *segment, LHCb::RichRecRing &recRing ) const;

      //Project a given vector of (RichRec) photons associated to one segment
      int Project( LHCb::RichRecPhoton::ConstVector & richRecPhotons, LHCb::RichRecRing &recRing ) const;

      // Compute the average Cerenkov photons emission point in the radiator (for a segement)
      int avgEmissionPnt(const LHCb::RichRecSegment *segment, Gaudi::XYZPoint &avgPoint) const;

      // Compute the average Cerenkov photons emission point in the radiator (for a vector of recphotons)
      int avgEmissionPnt(LHCb::RichRecPhoton::ConstVector & richRecPhotons, Gaudi::XYZPoint &avgPoint) const;

      // Returns the rotation matrix to the stereo frame (Z along track direction)
      int toUnitSphere(const LHCb::RichRecSegment *segment, Gaudi::Rotation3D &rot) const;

    private: // Private data

      /// Pointer to properties tool
      const IParticleProperties *m_richPartProp;

    };
    // End of class

  }
}

#endif // RICHRECTOOLS_RICHSTEREOPROJECTION_H
