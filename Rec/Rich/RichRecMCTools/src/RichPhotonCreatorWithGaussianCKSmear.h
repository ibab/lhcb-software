
//-------------------------------------------------------------------------
/** @file RichPhotonCreatorWithGaussianCKSmear.h
 *
 *  Header file for RICH reconstruction tool :
 *  Rich::Rec::MC::PhotonCreatorWithGaussianCKSmear
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   20/05/2005
 */
//------------------------------------------------------------------------

#ifndef RICHRECMCTOOLS_RICHPHOTONCREATORWITHGAUSSIANCKSMEAR_H
#define RICHRECMCTOOLS_RICHPHOTONCREATORWITHGAUSSIANCKSMEAR_H 1

// from Gaudi
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/IJobOptionsSvc.h"

// base class
#include "RichRecBase/RichPhotonCreatorBase.h"

// Interfaces
#include "RichRecBase/IRichPhotonCreator.h"
#include "MCInterfaces/IRichRecMCTruthTool.h"

// RichKernel
#include "RichKernel/BoostArray.h"

// Event model
#include "Event/MCRichOpticalPhoton.h"

// boost
#include "boost/assign/list_of.hpp"

namespace Rich
{
  namespace Rec
  {
    namespace MC
    {

      //--------------------------------------------------------------------------------------
      /** @class PhotonCreatorWithGaussianCKSmear RichPhotonCreatorWithGaussianCKSmear.h
       *
       *  Tool which first delegates the photon creator to another tool, but then applies
       *  a Gaussian smear to the true Cherenkov photons for each Radiator.
       *  The about of smearing is seperately configurable for each radiator.
       *
       *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
       *  @date   20/05/2005
       */
      //--------------------------------------------------------------------------------------

      class PhotonCreatorWithGaussianCKSmear : public PhotonCreatorBase
      {

      public: // methods for Gaudi framework

        /// Standard constructor
        PhotonCreatorWithGaussianCKSmear( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent );

        /// Destructor
        virtual ~PhotonCreatorWithGaussianCKSmear(){}

        // Initialize method
        StatusCode initialize();

        // Finalize method
        StatusCode finalize();

      protected: // methods

        /// Initialise for a new event
        virtual void InitNewEvent();

      private: // private methods

        /// Form a Photon candidate from a Segment and a pixel.
        virtual LHCb::RichRecPhoton * buildPhoton( LHCb::RichRecSegment * segment,
                                                   LHCb::RichRecPixel * pixel,
                                                   const Rich::Rec::PhotonKey key ) const;

        /// Access RICH MC reconstruction tool on demand
        inline const Rich::Rec::MC::IMCTruthTool * richMCRecTool() const
        {
          if ( !m_mcRecTool ) { acquireTool( "RichRecMCTruthTool", m_mcRecTool ); }
          return m_mcRecTool;
        }
        
      private: // private data

        // Pointers to tool instances
        mutable const Rich::Rec::MC::IMCTruthTool * m_mcRecTool;  ///< Rich Reconstruction MC Truth tool
        const IPhotonCreator * m_delPhotCr;   ///< Delegated photon creator

        /// Apply smearing to all photons, or only to true photons
        bool m_applySmearingToAll;

        /// Which radiators to apply the smearing to
        std::vector<bool> m_smearRad;

        /// Smearing value to apply to each radiators photons
        std::vector<double> m_smearWid;

        /// Gaussian random distributions
        mutable boost::array< Rndm::Numbers, Rich::NRadiatorTypes > m_rand;

        /// photon smearing done map
        mutable Rich::HashMap<Rich::Rec::PhotonKey::Int64_t,bool> m_photSmearDone;

        /// count of smeared photons
        mutable std::vector<unsigned long long> m_smearCount;

      };

    }
  }
}

#endif // RICHRECMCTOOLS_RICHPHOTONCREATORWITHGAUSSIANCKSMEAR_H
