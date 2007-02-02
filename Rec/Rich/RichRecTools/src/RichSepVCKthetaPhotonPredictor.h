
//-----------------------------------------------------------------------------
/** @file RichSepVCKthetaPhotonPredictor.h
 *
 *  Header file for tool : Rich::Rec::SepVCKthetaPhotonPredictor
 *
 *  CVS Log :-
 *  $Id: RichSepVCKthetaPhotonPredictor.h,v 1.5 2007-02-02 10:10:41 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   01/06/2005
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RICHPHOTONPREDICTOR_H
#define RICHRECTOOLS_RICHPHOTONPREDICTOR_H 1

// base class
#include "RichRecBase/RichRecToolBase.h"

// Event
#include "Event/RichRecPixel.h"
#include "Event/RichRecSegment.h"

// Interfaces
#include "RichRecBase/IRichPhotonPredictor.h"
#include "RichRecBase/IRichRecGeomTool.h"
#include "RichRecBase/IRichCherenkovAngle.h"
#include "RichRecBase/IRichCherenkovResolution.h"

// RichKernel
#include "RichKernel/RichPoissonEffFunctor.h"

// GSL
#include "gsl/gsl_math.h"

//-----------------------------------------------------------------------------
/** @namespace Rich
 *
 *  General namespace for RICH software
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
//-----------------------------------------------------------------------------
namespace Rich
{

  /** @namespace Rec
   *
   *  General namespace for RICH reconstruction software
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   08/07/2004
   */
  namespace Rec
  {

    //-----------------------------------------------------------------------------
    /** @class SepVCKthetaPhotonPredictor RichSepVCKthetaPhotonPredictor.h
     *
     *  Tool which performs the association between RichRecTracks and
     *  RichRecPixels to form RichRecPhotons.
     *
     *  This particular implementation uses a cut range per radiator,
     *  on the seperation between the pixel and ray-traced track impact point,
     *  that scales with the expected Cherenkov angle. The hit is required to be
     *  in range for at least one possible meass hypothesis.
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   01/06/2005
     */
    //-----------------------------------------------------------------------------

    class SepVCKthetaPhotonPredictor : public RichRecToolBase,
                                       virtual public IPhotonPredictor
    {

    public: // Methods for Gaudi Framework

      /// Standard constructor
      SepVCKthetaPhotonPredictor( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent );

      /// Destructor
      virtual ~SepVCKthetaPhotonPredictor(){}

      // Initialize method
      StatusCode initialize();

      // Finalize method
      StatusCode finalize();

    public: // methods (and doxygen comments) inherited from public interface

      // Is it possible to make a photon candidate using this segment and pixel.
      bool photonPossible( LHCb::RichRecSegment * segment,
                           LHCb::RichRecPixel * pixel ) const;

    private: // private data

      /// Geometry tool
      const IGeomTool * m_geomTool;

      /// Pointer to Cherenkov angle tool
      const ICherenkovAngle * m_ckAngle;

      /// Pointer to Cherenkov angle resolution tool
      const ICherenkovResolution * m_ckRes;

      std::vector<double> m_minROI;  ///< Min hit radius of interest around track centres
      std::vector<double> m_maxROI;  ///< Max hit radius of interest around track centres
      std::vector<double> m_maxROI2; ///< Square of m_maxROI
      std::vector<double> m_minROI2; ///< Square of m_minROI
      std::vector<double> m_ckThetaMax; ///< Scaling parameter - Max CK theta point
      std::vector<double> m_sep2GMax;   ///< Scaling parameter - Max separation point
      std::vector<double> m_tolF;       ///< Region of tolerance in seperation
      std::vector<double> m_scale;     ///< Internal cached parameter for speed

      // debug counting numbers
      mutable std::vector<unsigned int> m_Nselected; ///< Number of selected combinations for each radiator
      mutable std::vector<unsigned int> m_Nreject;   ///< Number of rejected combinations for each radiator

    };

  }
}

#endif // RICHRECTOOLS_RICHPHOTONPREDICTOR_H
