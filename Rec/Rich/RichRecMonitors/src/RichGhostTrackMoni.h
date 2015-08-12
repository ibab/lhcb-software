
//---------------------------------------------------------------------------
/** @file RichGhostTrackMoni.h
 *
 *  Header file for algorithm class : Rich::Rec::MC::GhostTrackMoni
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//---------------------------------------------------------------------------

#ifndef RICHRECMONITOR_RICHTRACKGEOMMONI_H
#define RICHRECMONITOR_RICHTRACKGEOMMONI_H 1

// base class
#include "RichRecBase/RichRecTupleAlgBase.h"

// Event
#include "Event/RichRecTrack.h"

// RichKernel
#include "RichKernel/RichMap.h"

// Interfaces
#include "RichRecBase/IRichTrackSelector.h"
#include "MCInterfaces/IRichRecMCTruthTool.h"
#include "RichRecBase/IRichExpectedTrackSignal.h"
#include "RichRecBase/IRichCherenkovAngle.h"
#include "RichRecBase/IRichCherenkovResolution.h"

// GSL
#include "gsl/gsl_math.h"

namespace Rich
{
  namespace Rec
  {
    namespace MC
    {

      //---------------------------------------------------------------------------
      /** @class GhostTrackMoni RichGhostTrackMoni.h
       *
       *  Monitors the properties of ghost tracks
       *
       *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
       *  @date   05/04/2002
       */
      //---------------------------------------------------------------------------

      class GhostTrackMoni : public Rich::Rec::TupleAlgBase
      {

      public:

        /// Standard constructor
        GhostTrackMoni( const std::string& name,
                        ISvcLocator* pSvcLocator );

        virtual ~GhostTrackMoni( ); ///< Destructor

        virtual StatusCode initialize();    // Algorithm initialization
        virtual StatusCode execute   ();    // Algorithm execution
        virtual StatusCode finalize  ();    // Algorithm finalization

      private: // classes

        /// Simple class to collect numbers for the ntupling
        class CountVects
        {
        public:
          typedef std::vector<double>                  Data;
          typedef std::map< Rich::RadiatorType, Data > Map;
          CountVects()
          {
            m_map[Rich::Aerogel]  = Data(Rich::NParticleTypes,-1);
            m_map[Rich::Rich1Gas] = Data(Rich::NParticleTypes,-1);
            m_map[Rich::Rich2Gas] = Data(Rich::NParticleTypes,-1);
          }
          inline Data & data( const Rich::RadiatorType rad ) { return m_map[rad]; }
        private:
          Map m_map;
        };

      private: // data

        /// Track selector
        const ITrackSelector * m_trSelector;

        /// Pointer to RichRecMCTruthTool interface
        const Rich::Rec::MC::IMCTruthTool* m_richRecMCTruth;

        /// Pointer to RichExpectedTrackSignal tool
        const IExpectedTrackSignal * m_tkSignal;

        /// Rich Cherenkov angle calculator tool
        const ICherenkovAngle * m_ckAngle;

        /// Cherenkov angle resolution tool
        const ICherenkovResolution * m_ckAngleRes;

        /// Number of sigmas to select photons, for each radiator
        std::vector<double> m_nSigma;

      };

    }
  }
}

#endif // RICHRECMONITOR_RICHTRACKGEOMMONI_H
