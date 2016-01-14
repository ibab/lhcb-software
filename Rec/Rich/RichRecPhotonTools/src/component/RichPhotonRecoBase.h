
//-----------------------------------------------------------------------------
/** @file RichPhotonRecoBase.h
 *
 *  Header file for tool : Rich::Rec::PhotonRecoBase
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @author Antonis Papanestis
 *  @date   2003-11-14
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RichPhotonRecoBase_H
#define RICHRECTOOLS_RichPhotonRecoBase_H 1

// Base class
#include "RichRecBase/RichRecToolBase.h"

// interfaces
#include "RichRecBase/IRichPhotonReconstruction.h"
#include "RichRecBase/IRichPhotonEmissionPoint.h"

// RichKernel
#include "Kernel/RichSide.h"
#include "Kernel/RichSmartID.h"

namespace Rich
{
  namespace Rec
  {

    //-----------------------------------------------------------------------------
    /** @class PhotonRecoBase RichPhotonRecoBase.h
     *
     *  Base class for geometrical photon reconstruction tools
     *
     *  @author Chris Jones         Christopher.Rob.Jones@cern.ch
     *  @date   2003-11-14
     */
    //-----------------------------------------------------------------------------

    class PhotonRecoBase : public Rich::Rec::ToolBase,
                           virtual public IPhotonReconstruction
    {

    public: // Methods for Gaudi Framework

      /// Standard constructor
      PhotonRecoBase( const std::string& type,
                      const std::string& name,
                      const IInterface* parent);

      virtual ~PhotonRecoBase( ); ///< Destructor

      // Initialization of the tool after creation
      virtual StatusCode initialize();

    public: // methods (and doxygen comments) inherited from interface

      // Reconstructs the geometrical photon candidate for a given RichTrackSegment
      // and RichSmartID channel identifier
      virtual StatusCode reconstructPhoton ( const LHCb::RichRecSegment * segment,
                                             const LHCb::RichRecPixel * pixel,
                                             LHCb::RichGeomPhoton& gPhoton ) const = 0;

    protected: // methods

      /// Check if the coordinate hits a and b are on the same detector side
      inline bool sameSide( const Rich::RadiatorType rad,
                            const Gaudi::XYZPoint & a,
                            const Gaudi::XYZPoint & b ) const
      {
        return ( Rich::Rich2Gas == rad ? 
                 a.x() * b.x() > 0 : 
                 a.y() * b.y() > 0 );
      }

      /// Correction for CK theta
      inline double ckThetaCorrection( const Rich::RadiatorType rad ) const
      {
        return m_ckJOCorrs[rad] + m_ckBiasCorrs[rad];
      }

      /// Access the emission point tool
      inline const IPhotonEmissionPoint * emissPoint() const { return m_emissPoint; }

    protected: // data

      /// Check for photons that cross between the different RICH 'sides'
      std::vector<bool> m_checkPhotCrossSides;

      /** Job-Option Corrections applied to the reconstructed theta vales.
       *  By default 0. */
      std::vector<double> m_ckJOCorrs;

      /** Cherenkov theta bias corrections, specific for each photon
       *  reconstruction method. */
      std::vector<double> m_ckBiasCorrs;

    private: // data

      /// Estimated emission point tool
      const IPhotonEmissionPoint * m_emissPoint = nullptr;

    };

  }
}

#endif // RICHRECTOOLS_RichPhotonRecoBase_H
