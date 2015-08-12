
/** @file RichAlignmentMonitor.h
 *
 *  Header file for algorithm class : Rich::Rec::MC::AlignmentMonitor
 *
 *  @author Antonis Papanestis   a.papanestis@rl.ac.uk
 *  @date   2004-02-19
 */

#ifndef RICHRECMONITOR_RICHALIGNMENTMONITOR_H
#define RICHRECMONITOR_RICHALIGNMENTMONITOR_H 1

// STL
#include <sstream>

// base class
#include "RichRecBase/RichRecTupleAlgBase.h"

// from Gaudi
#include "GaudiKernel/SystemOfUnits.h"

// Event
#include "Event/RichRecStatus.h"
#include "Event/MCRichOpticalPhoton.h"

// Histogramming
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "AIDA/IProfile1D.h"

// Interfaces
#include "MCInterfaces/IRichRecMCTruthTool.h"
#include "RichKernel/IRichParticleProperties.h"
#include "RichRecBase/IRichCherenkovAngle.h"
#include "RichRecBase/IRichTrackSelector.h"
#include "RichRecBase/IRichIsolatedTrack.h"

// RichDet
#include "RichDet/DeRichSphMirror.h"

// Kernel
#include "RichKernel/BoostArray.h"
#include "RichKernel/RichHPDIdentifier.h"

// boost
#include <boost/foreach.hpp>

namespace Rich
{
  namespace Rec
  {
    namespace MC
    {

      /** @class AlignmentMonitor RichAlignmentMonitor.h
       *
       *  Provides monitoring histograms for the alignment of the RICH mirrors.
       *
       *  @author Antonis Papanestis   a.papanestis@rl.ac.uk
       *  @date   2004-02-19
       */

      class AlignmentMonitor : public TupleAlgBase
      {

      public:

        /// Mirror number definition
        typedef unsigned int MirrorNumber;

        /// Standard constructor
        AlignmentMonitor( const std::string& name, ISvcLocator* pSvcLocator );

        virtual ~AlignmentMonitor( ); ///< Destructor

        virtual StatusCode initialize();    // Algorithm initialization
        virtual StatusCode execute   ();    // Algorithm execution
        virtual StatusCode finalize  ();    // Algorithm finalization

      private: // methods

        /**Check whether to produce a histogram for this HPD
           @return The HPD identifier
        */
        int makePlotForHPD ( const LHCb::RichSmartID smartID ) const;

      private: // data

        int m_radTemp;    ///< which rich detector to monitor
        Rich::RadiatorType m_radiator;

        int m_maxUsedTracks;

        // set to know to stop all MC Truth
        bool m_useMCTruth;

        // to avoid bias towards small angles use only a photons in the expected range
        double m_deltaThetaRange;
        double m_deltaThetaHistoRange;

        // particle type when fixed
        int m_particleType;
        Rich::ParticleIDType m_pType;

        // keep totals of particle types
        std::vector<int> m_pTypes;

        // histogram output level
        int m_histoOutputLevel;

        // tools
        const ITrackSelector* m_trSelector;        ///< Track selector
        /// Pointer to RichRecMCTruthTool interface
        const Rich::Rec::MC::IMCTruthTool* m_richRecMCTruth;
        const IParticleProperties* m_richPartProp; ///< Rich Particle properties
        const ICherenkovAngle* m_ckAngle;  ///< Pointer to RichCherenkovAngle tool
        const IIsolatedTrack * m_isoTrack; ///< Pointer to isolated track tool

        // Vector of histogram numbers to prebook (helps with merging files).
        // Since the arrival of the python-based configuration, it is
        // a vector of strings.
        // Anatoly Solomin 2008-11-01.
        std::vector<std::string> m_preBookHistos;

        // list of HPDs for histograms
        std::vector<int> m_HPDList;
        /// make plots for all HPDs ignoring the list
        bool m_plotAllHPDs;

        /// when writing NTuples limit the number of photons per HPD
        int m_photonLimitPerHPD;
        /// store how many photons per HPD
        std::vector<int> m_photonsInHPD;

        // vector with the two side histograms (up/down , A/C side)
        std::vector<AIDA::IHistogram2D*> m_sideHistos;
        std::vector<AIDA::IHistogram2D*> m_quarterHistos;
        std::vector<AIDA::IHistogram2D*> m_sideIsolatedHistos;

        std::vector<int> m_hpdVetoList;

      };

    }
  }
}

#endif // RICHRECMONITOR_RICHALIGNMENTMONITOR_H
