// $Id: RichDigiAlgMoni.h,v 1.16 2009-01-20 16:00:55 cattanem Exp $
#ifndef RICHMONITOR_RICHDIGIALGMONI_H
#define RICHMONITOR_RICHDIGIALGMONI_H 1

// STD
#include <sstream>

// base class
#include "RichKernel/RichHistoAlgBase.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Vector3DTypes.h"

// Event model
#include "Event/MCRichDigit.h"
#include "Event/MCRichDeposit.h"
#include "Event/MCRichSummedDeposit.h"
#include "Event/MCRichOpticalPhoton.h"
#include "Event/MCRichHit.h"
#include "Kernel/RichParticleIDType.h"

// Kernel
#include "RichKernel/RichMap.h"
#include "RichKernel/RichHashMap.h"
#include "Kernel/RichDetectorType.h"
#include "Kernel/RichRadiatorType.h"
#include "Kernel/RichParticleIDType.h"

// GSL
#include "gsl/gsl_math.h"

// interfaces
#include "RichKernel/IRichSmartIDTool.h"
#include "MCInterfaces/IRichMCTruthTool.h"

// boost
#include "boost/lexical_cast.hpp"

// RichDet
#include "RichDet/DeRichSystem.h"

// temporary histogramming numbers
#include "RichDetParams.h"

namespace Rich
{
  namespace MC
  {
    namespace Digi
    {

      /** @class AlgMoni RichDigiAlgMoni.h
       *
       *  Monitor for Rich Digitisation algorithm performance
       *
       *  @author Chris Jones   (Christopher.Rob.Jones@cern.ch)
       *  @date   2003-09-08
       */

      class AlgMoni : public HistoAlgBase
      {

      public:

        /// Standard constructor
        AlgMoni( const std::string& name, ISvcLocator* pSvcLocator );

        virtual ~AlgMoni( ); ///< Destructor

        virtual StatusCode initialize();    ///< Algorithm initialization
        virtual StatusCode execute   ();    ///< Algorithm execution

      private: // definitions

        /// key for HPD+MCParticle hits
        typedef std::pair<const LHCb::RichSmartID,const LHCb::MCParticle*>   PartKey;
        /// mapping given hits for each HPD+MCParticle Pair
        typedef std::map< PartKey, std::vector<const LHCb::MCRichDeposit*> > PartMap;

      private: // methods

        // Map to count cherenkov photons for each radiator
        typedef std::pair<const LHCb::MCParticle*,Rich::RadiatorType> PhotPair;
        typedef Rich::Map< PhotPair, int >                            PhotMap;

        // PD occupancies
        typedef Rich::Map<const LHCb::RichSmartID,unsigned int> PDMulti;

        /// Particle mass
        double mass( const LHCb::MCParticle * mcPart );

        /// Returns beta for a given MCParticle
        double mcBeta( const LHCb::MCParticle * mcPart );

        /// Returns the momentum for a given MCParticle
        double momentum( const LHCb::MCParticle * mcPart );

        /// Count the number of photo electrons
        void countNPE( PhotMap & photMap,
                       const LHCb::MCRichHit * hit );

        /// IS this a true cherenkov signal hit ?
        bool trueCKHit(  const LHCb::MCRichHit * hit );

        /// Fill histograms for each HPD silicon wafer background plots
        void fillHPDPlots( const PartMap & pmap,
                           const std::string & plotsDir,
                           const std::string & plotsName );

      private: // data

        /// Pointer to RichSmartID tool
        const Rich::ISmartIDTool * m_smartIDTool;

        /// Pointer to MC truth tool
        const Rich::MC::IMCTruthTool * m_mcTool;

        /// Pointer to RICH system detector element
        const DeRichSystem * m_richSys;

        // job options
        std::string m_mcdigitTES;     ///< Location of MCRichDigits in TES
        std::string m_mcdepTES;       ///< Location of MCRichDeposits in TES
        std::string m_mchitTES;       ///< Location of MCRichHits in TES

        /// Particle masses
        Rich::Map<Rich::ParticleIDType,double> m_particleMass;

        // histo ID
        unsigned int m_ID;

        // max number of Quartz CK HPD event histos to make
        unsigned int m_maxID;

      };

      inline double AlgMoni::mass( const LHCb::MCParticle * mcPart )
      {
        return m_particleMass[ m_mcTool->mcParticleType(mcPart) ];
      }

      inline double AlgMoni::momentum( const LHCb::MCParticle * mcPart )
      {
        return ( mcPart ? mcPart->momentum().P() : 0 );
      }

      inline double AlgMoni::mcBeta( const LHCb::MCParticle * mcPart )
      {
        if ( !mcPart ) return 0;
        const double pTot = momentum( mcPart );
        const double Esquare = pTot*pTot + gsl_pow_2(mass(mcPart));
        return ( Esquare > 0 ? pTot/sqrt(Esquare) : 0 );
      }

      inline bool AlgMoni::trueCKHit(  const LHCb::MCRichHit * hit )
      {
        return hit->isSignal();
      }

    }
  }
}

#endif // RICHMONITOR_RICHDIGIALGMONI_H
