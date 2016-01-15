
//-----------------------------------------------------------------------------
/** @file RichAdaptivePhotonReco.h
 *
 *  Header file for tool : Rich::Rec::AdaptivePhotonReco
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @author Antonis Papanestis
 *  @date   2003-11-14
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECPHOTONTOOLS_RichAdaptivePhotonReco_H
#define RICHRECPHOTONTOOLS_RichAdaptivePhotonReco_H 1

// STL
#include <vector>

// Base class
#include "RichPhotonRecoBase.h"

// Interfaces
#include "RichRecBase/IRichPhotonReconstruction.h"
#include "RichRecBase/IRichCherenkovAngle.h"

namespace Rich
{
  namespace Rec
  {

    //-----------------------------------------------------------------------------
    /** @class AdaptivePhotonReco RichAdaptivePhotonReco.h
     *
     *  Photon reconstruction tool that delegates the recosntruction to other
     *  tools based on the track properties.
     *     *
     *  @author Chris Jones         Christopher.Rob.Jones@cern.ch
     *  @date   2016-01-11
     */
    //-----------------------------------------------------------------------------

    class AdaptivePhotonReco : public PhotonRecoBase,
                               virtual public IPhotonReconstruction
    {

    public: // Methods for Gaudi Framework

      /// Standard constructor
      AdaptivePhotonReco( const std::string& type,
                          const std::string& name,
                          const IInterface* parent);
      
      virtual ~AdaptivePhotonReco( ); ///< Destructor

      // Initialization of the tool after creation
      virtual StatusCode initialize();

    public: // methods (and doxygen comments) inherited from interface

      // Reconstructs the geometrical photon candidate for a given RichTrackSegment
      // and RichSmartID channel identifier
      StatusCode reconstructPhoton ( const LHCb::RichRecSegment * segment,
                                     const LHCb::RichRecPixel * pixel,
                                     LHCb::RichGeomPhoton& gPhoton ) const;

    private: // methods

      /// Access maximum CK theta value
      inline double maxCKThetaForFast( const Rich::RadiatorType rad ) const
      {
        if ( m_maxCKThetaForFast[rad] < 0 )
        {
          m_maxCKThetaForFast[rad] = ( m_ckAngle->nominalSaturatedCherenkovTheta(rad) *
                                       m_maxFracCKtheta[rad] );
        }
        return m_maxCKThetaForFast[rad]; 
      }

    private: // data

      /// Max fraction of saturated kaon CK theta angle to use fast reco tool
      std::vector<double> m_maxFracCKtheta;

      /// Cherenkov angle tool
      const ICherenkovAngle * m_ckAngle = nullptr;

      /// Slow Photon reco tool (highest precision)
      const IPhotonReconstruction * m_recoSlow = nullptr;

      /// Fast Photon reco tool
      const IPhotonReconstruction * m_recoFast = nullptr;

      /// Cache (for speed) the maximum CK theta value for the fast tool.
      mutable std::vector<double> m_maxCKThetaForFast;

    };

  }
}

#endif // RICHRECPHOTONTOOLS_RichAdaptivePhotonReco_H
