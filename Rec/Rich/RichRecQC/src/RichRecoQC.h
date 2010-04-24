
//---------------------------------------------------------------------------------
/** @file RichRecoQC.h
 *
 *  Header file for RICH reconstruction monitoring algorithm : Rich::Rec::MC::RecoQC
 *
 *  CVS Log :-
 *  $Id: RichRecoQC.h,v 1.28 2009-07-27 20:35:28 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   2002-07-02
 */
//---------------------------------------------------------------------------------

#ifndef RICHRECQC_RICHRECOQC_H
#define RICHRECQC_RICHRECOQC_H 1

// base class
#include "RichRecBase/RichRecHistoAlgBase.h"

// Interfaces
#include "MCInterfaces/IRichRecMCTruthTool.h"

// temporary histogramming numbers
#include "RichRecBase/RichDetParams.h"

// interfaces
#include "RichRecBase/IRichTrackSelector.h"
#include "RichKernel/IRichParticleProperties.h"
#include "RichRecBase/IRichCherenkovAngle.h"
#include "RichRecBase/IRichCherenkovResolution.h"
#include "RichRecBase/IRichIsolatedTrack.h"
#include "RichRecBase/IRichStereoFitter.h"

// RichKernel
#include "RichKernel/RichStatDivFunctor.h"

// boost
#include "boost/assign/list_of.hpp"

namespace Rich
{
  namespace Rec
  {
    namespace MC
    {

      //---------------------------------------------------------------------------------
      /** @class RecoQC RichRecoQC.h
       *
       *  Quality control monitor for Rich Reconstruction
       *
       *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
       *  @date   2002-07-02
       */
      //---------------------------------------------------------------------------------

      class RecoQC : public RichRecHistoAlgBase
      {

      public:

        /// Standard constructor
        RecoQC( const std::string& name, ISvcLocator* pSvcLocator );

        virtual ~RecoQC( ); ///< Destructor

        virtual StatusCode initialize();    // Algorithm initialization
        virtual StatusCode execute   ();    // Algorithm execution
        virtual StatusCode finalize  ();    // Algorithm finalization

      protected:

        /// Pre-Book all (non-MC) histograms
        virtual StatusCode prebookHistograms();

      private: // methods

        /// access RichRecMCTruthTool tool on demand
        const Rich::Rec::MC::IMCTruthTool * richRecMCTool() const
        {
          if ( !m_richRecMCTruth ) { acquireTool( "RichRecMCTruthTool", m_richRecMCTruth ); }
          return m_richRecMCTruth;
        }

      private: // data

        // Pointers to tool instances
        const IParticleProperties * m_richPartProp; ///< Rich Particle properties
        const ICherenkovAngle * m_ckAngle;  ///< Pointer to RichCherenkovAngle tool
        const ICherenkovResolution * m_ckRes; ///< Cherenkov angle resolution tool
        mutable const Rich::Rec::MC::IMCTruthTool* m_richRecMCTruth;  ///< Pointer to RichRecMCTruthTool interface
        const ITrackSelector * m_trSelector;  ///< Track selector
        const IIsolatedTrack * m_isoTrack;

        /// Pointer to the Stereographic refitting tool
        const IStereoFitter * m_fitter;

        // job options selection cuts
        std::vector<double> m_minBeta; ///< minimum beta value for tracks
        std::vector<double> m_maxBeta; ///< maximum beta value for tracks

        std::vector<unsigned int> m_truePhotCount; ///< Total number of true cherenkov photons per radiator
        std::vector<unsigned int> m_nSegs;         ///< Total number of track segments per radiator

        std::vector<double> m_ckThetaMax; ///< Max theta limit for histos for each rad
        std::vector<double> m_ckThetaMin; ///< Min theta limit for histos for each rad

        std::vector<bool> m_rads; ///< Which radiators to monitor

        std::vector<unsigned int> m_minRadSegs; ///< Minimum segments per radiator
        std::vector<unsigned int> m_maxRadSegs; ///< Maximum segments per radiator

        /** Flag to turn on the rejection of aerogel photons if they share the same
         *  pixel with a Rich1Gas photon */
        bool m_checkAeroGas;

        /// Histogram ranges for CK resolution plots
        std::vector<double> m_ckResRange;

      };

    }
  }
}

#endif // RICHRECQC_RICHRECOQC_H
