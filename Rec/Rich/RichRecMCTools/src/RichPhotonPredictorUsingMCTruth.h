
//---------------------------------------------------------------------------------------------------
/** @file RichPhotonPredictorUsingMCTruth.h
 *
 *  Header file for RICH reconstruction tool : Rich::Rec::PhotonPredictorUsingMCTruth
 *
 *  CVS Log :-
 *  $Id: RichPhotonPredictorUsingMCTruth.h,v 1.1 2009-03-27 14:15:50 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   18/03/2009
 */
//---------------------------------------------------------------------------------------------------

#ifndef RICHRECMCTOOLS_RichPhotonPredictorUsingMCTruth_H
#define RICHRECMCTOOLS_RichPhotonPredictorUsingMCTruth_H 1

// from Gaudi
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/IRndmGenSvc.h"

// base class
#include "RichRecBase/RichRecToolBase.h"

// Event model
#include "Event/RichRecPixel.h"
#include "Event/RichRecSegment.h"
#include "Event/MCParticle.h"

// Interfaces
#include "RichRecBase/IRichPhotonPredictor.h"
#include "MCInterfaces/IRichRecMCTruthTool.h"

// RichKernel
#include "RichKernel/BoostArray.h"

namespace Rich
{
  namespace Rec
  {
    namespace MC
    {

      //---------------------------------------------------------------------------------------------------
      /** @class PhotonPredictorUsingMCTruth RichPhotonPredictorUsingMCTruth.h
       *
       *  Tool which performs the association between RichRecSegments and
       *  RichRecPixels to form RichRecPhotons.  This particular instance uses
       *  MC truth information to select only true Cherenkov photon combinations.
       *
       *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
       *  @date   18/03/2009
       */
      //---------------------------------------------------------------------------------------------------

      class PhotonPredictorUsingMCTruth : public Rich::Rec::ToolBase,
                                          virtual public IPhotonPredictor
      {

      public: // methods for Gaudi framework

        /// Standard constructor
        PhotonPredictorUsingMCTruth( const std::string& type,
                                     const std::string& name,
                                     const IInterface* parent );

        /// Destructor
        virtual ~PhotonPredictorUsingMCTruth();

        // Initialize method
        StatusCode initialize();

      public: // Public interface methods

        // Is it possible to make a photon candidate using this segment and pixel.
        bool photonPossible( LHCb::RichRecSegment * segment,
                             LHCb::RichRecPixel * pixel ) const;

      private: // private data

        // Pointers to tool instances
        const Rich::Rec::MC::IMCTruthTool * m_mcRecTool;  ///< MC Truth tool

        /// The fraction of signal hits to select
        double m_sigSelFrac;

        /// The fraction of background hits to select
        double m_bkgSelFrac;

        /// Gaussian random distributions
        mutable Rndm::Numbers m_rand;

      };

    }
  }
}

#endif // RICHRECMCTOOLS_RichPhotonPredictorUsingMCTruth_H
