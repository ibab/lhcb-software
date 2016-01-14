
//-----------------------------------------------------------------------------
/** @file RichPIDQC.h
 *
 *  Header file for RICH reconstruction monitoring algorithm : Rich::Rec::MC::PIDQC
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   2002-06-13
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECQC_RICHPIDQC_H
#define RICHRECQC_RICHPIDQC_H 1

// base class
#include "RichRecBase/RichRecHistoAlgBase.h"

// RichRecBase
#include "RichRecBase/RichTrackID.h"

// Event model
#include "Event/RichPID.h"
#include "Event/Track.h"
#include "Event/MCParticle.h"

// RichKernel
#include "RichKernel/RichMap.h"

// temporary histogramming numbers
#include "RichRecBase/RichDetParams.h"

// interfaces
#include "RichRecBase/IRichTrackSelector.h"
#include "MCInterfaces/IRichRecMCTruthTool.h"
#include "MCInterfaces/IMCParticleSelector.h"
#include "RichRecBase/IRichPIDPlots.h"

// Histogramming
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"

// Boost
#include "boost/lexical_cast.hpp"
#include "boost/format.hpp"

namespace Rich
{
  namespace Rec
  {
    namespace MC
    {

      //-----------------------------------------------------------------------------
      /** @class PIDQC RichPIDQC.h
       *
       *  Quality control monitor for RichPIDs
       *
       *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
       *  @date   2002-06-13
       */
      //-----------------------------------------------------------------------------

      class PIDQC : public Rich::Rec::HistoAlgBase
      {

      private:

        /** @class TkTally RichPIDQC.h
         * Class to store track count tallies
         */
        class TkTally
        {
        public:
          TkTally() : nTracks(0), multiplicity(0) { }
        public:
          /// Operator +=
          inline TkTally& operator+=(const TkTally& c)
          {
            this->nTracks      += c.nTracks;
            this->multiplicity += c.multiplicity;
            return *this;
          } 
          /// Overload output to ostream
          friend inline std::ostream& operator << ( std::ostream& ost, 
                                                    const TkTally& id )
          { return ost << "[ nTracks=" << id.nTracks 
                       << " multiplicity=" << id.multiplicity << " ]"; }
        public:
          unsigned int nTracks;
          unsigned int multiplicity;
        };

      public:

        /// Standard constructor
        PIDQC( const std::string& name, ISvcLocator* pSvcLocator );

        virtual ~PIDQC( ); ///< Destructor

        virtual StatusCode initialize();    // Algorithm initialization
        virtual StatusCode execute   ();    // Algorithm execution
        virtual StatusCode finalize  ();    // Algorithm finalization

      private: // methods

        /// Loads the PID data from configured location
        StatusCode loadPIDData();

        /// Count all Tracks in given location passing the selection criteria
        TkTally countTracks( const std::string & location );

        /// Print out the given PID
        void print( MsgStream & msg, 
                    LHCb::RichPID * iPID,
                    const Rich::ParticleIDType pid,
                    const Rich::ParticleIDType mcpid ) const;

        /// Get the track momentum
        inline double trackP( LHCb::RichPID * pid ) const
        {
          const LHCb::State* state = &(pid->track())->firstState();
          return ( state ? state->p()/Gaudi::Units::GeV : 0 );
        }

        /// Get the MCParticle Selector tool
        inline const IMCParticleSelector * mcPselector()
        {
          return m_mcPselector;
        }

        /// Select Tracks
        bool selectTracks( const LHCb::Track * track );

        /// Access the PID plots tool for the given MC type
        const Rich::Rec::IPIDPlots * plotsTool( const Rich::ParticleIDType mcpid );  

      private: // data

        // Vector of pointers to RichPIDs
        std::vector<ContainedObject*> m_richPIDs;

        // job options
        std::string m_pidTDS;          ///< Location of target RichPIDs in TDS
        unsigned int m_minMultCut;     ///< Minimum track multiplicity
        unsigned int m_maxMultCut;     ///< Maximum track multiplicity
        bool m_truth;                  ///< MCTruth available
        unsigned int m_bins;           ///< Number of bins
        bool m_finalPrintOut;          ///< Perform final prinout of PID tables
        bool m_ignoreRecoThres;        ///< Flag to turn on/off the setting of Reco-PIDs as "below threshold"
        bool m_ignoreMCThres;          ///< Flag to turn on/off the setting of MC-PIDs as "below threshold"
        bool m_mcPsel;                 ///< Flag to turn on/off the selection of 'nice' MCParticles

        const ITrackSelector * m_trSelector = nullptr;  ///< Track selector

        const Rich::Rec::MC::IMCTruthTool * m_mcTruth = nullptr;    ///< MC Truth tool

        const IMCParticleSelector * m_mcPselector = nullptr; ///< MCParticle selector

        // Summary information
        double m_sumTab[6][6];
        int m_nEvents[2];
        int m_nTracks[2];

        typedef Rich::Map<Rich::Rec::Track::Type,std::pair<unsigned int,unsigned int> > TkCount;
        /// Count the number of PID objects by track type
        TkCount m_trackCount;

        typedef Rich::Map<std::string,std::pair<unsigned int,unsigned int> > PIDsByType;
        /// Count the number of PID objects by PID type
        PIDsByType m_pidPerTypeCount;

        float m_dllKaonCut;
        float m_dllPionCut;

        /// Radiators to require are present
        std::vector<bool> m_requiredRads;

        /// format for numbers
        std::string m_sF;

        /// PID plots tool for each PID type
        typedef Rich::Map< const Rich::ParticleIDType, 
                           const Rich::Rec::IPIDPlots* > PIDMap;
        /// PID plots tool for each PID type
        PIDMap m_plotTools;

        /// Inclusive plots tool
        const Rich::Rec::IPIDPlots * m_allPlotTool;

        /// Plots configuration object
        Rich::Rec::IPIDPlots::Configuration m_plotsConfig;

        /// Expert plots
        bool m_expertPlots;

      private:

        /// Utility class for radiators used
        class Radiators
        {
        public:
          Radiators( bool aero = false, bool r1gas = false, bool r2gas = false )
            : hasAero(aero), hasR1Gas(r1gas), hasR2Gas(r2gas) { }
          inline std::string radiators() const
          {
            std::string rads = "'";
            if ( hasAero )  rads += "Aerogel";
            if ( hasR1Gas ) { if(rads!="'") rads+='+'; rads += "Rich1Gas"; }
            if ( hasR2Gas ) { if(rads!="'") rads+="+"; rads += "Rich2Gas"; }
            if ( rads == "'" ) rads += "NoRadiators";
            rads += "'";
            rads.resize(30,' ');
            return rads;
          }
          inline int value() const
          {
            return ((int)hasAero)*100 + ((int)hasR1Gas)*10 + ((int)hasR2Gas);
          }
          inline bool operator<( const Radiators& rads ) const
          {
            return rads.value() < this->value();
          }
        private:
          bool hasAero; ///< Has aerogel info
          bool hasR1Gas; ///< Has Rich1 Gas
          bool hasR2Gas; ///< Has Rich1 Gas
        };
        typedef Rich::Map<Radiators,unsigned long long> RadCount;
        /// Tally of tracks which each type of radiator
        RadCount m_radCount;

      };

    }
  }
}

#endif // RICHRECQC_RICHPIDQC_H
