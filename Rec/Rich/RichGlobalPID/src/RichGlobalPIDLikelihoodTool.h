
//--------------------------------------------------------------------------
/** @file RichGlobalPIDLikelihoodTool.h
 *
 *  Header file for RICH Global PID tool : Rich::Rec::GlobalPID::LikelihoodTool
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2008-03-01
 */
//--------------------------------------------------------------------------

#ifndef RICHGLOBALPID_RichGlobalPIDLikelihoodTool_H
#define RICHGLOBALPID_RichGlobalPIDLikelihoodTool_H 1

// STD
#include <sstream>
#include <cmath>

// Base class
#include "RichGlobalPIDToolBase.h"
#include "RichRecBase/IRichPID.h"
#include "IRichGlobalPID.h"

// interfaces
#include "RichRecBase/IRichExpectedTrackSignal.h"
#include "RichRecBase/IRichPhotonSignal.h"
#include "IRichGlobalPIDTrackCreator.h"

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// RichKernel
#include "RichKernel/RichMap.h"

#include "RichDet/Rich1DTabFunc.h"

// boost
#include "boost/format.hpp"

namespace Rich
{
  namespace Rec
  {
    namespace GlobalPID
    {

      /** @class LikelihoodTool RichGlobalPIDLikelihoodTool.h
       *
       *  Tool to create RichGlobalPIDTrack track objects for the RICH
       *  Global PID algorithm
       *
       *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
       *  @date   2008-03-01
       */
      class LikelihoodTool : public Rich::Rec::GlobalPID::ToolBase,
                             virtual public Rich::Rec::IRichPID,
                             virtual public Rich::Rec::GlobalPID::IRichGlobalPID
      {

      public:

        /// Standard constructor
        LikelihoodTool( const std::string& type,
                        const std::string& name,
                        const IInterface* parent );

        /// Destructor
        virtual ~LikelihoodTool( );

        // Initialize method
        virtual StatusCode initialize();

        // Initialize method
        virtual StatusCode finalize();

      public: // IRichPID

        // Access the RichPID associated to the given Track
        virtual const LHCb::RichPID * pid( const LHCb::Track * track ) const;

        // Access the RichPID data objects for the given Tracks
        virtual void pids( const LHCb::Track::ConstVector & tracks,
                           LHCb::RichPID::ConstVector & richpids ) const;

      public: // IRichGlobalPID

        // Access the RichGlobalPID associated to the given Track
        virtual LHCb::RichGlobalPID * pid( LHCb::RichGlobalPIDTrack * track ) const;

        // Create the RichGlobalPID data objects for the given Tracks
        virtual void pids( const LHCb::RichGlobalPIDTrack::Vector & tracks ) const;

      private: // helper classes

        // private typedefs

        /// Container for changes to be made following an event iterations
        /// Contains a pointer to a track and the its new hypothesis
        typedef Rich::Map<LHCb::RichGlobalPIDTrack*,Rich::ParticleIDType> MinTrList;

        /// Track list entry. Its current best DLL change and a pointer to the trackk
        typedef std::pair<double,LHCb::RichGlobalPIDTrack*> TrackPair;

        /// List of all track list entries
        typedef std::vector<TrackPair> TrackList;

        class InitTrackInfo
        {
        public:
          /// Container
          typedef std::vector<InitTrackInfo> Vector;
          /// Constructor
          InitTrackInfo( LHCb::RichGlobalPIDTrack * track = NULL,
                         const Rich::ParticleIDType h = Rich::Pion,
                         const double mindll = 0 )
            : pidTrack(track), hypo(h), minDLL(mindll) { }
        public:
          LHCb::RichGlobalPIDTrack * pidTrack; ///< Pointer to the track
          Rich::ParticleIDType hypo;     ///< Track hypothesis
          double minDLL;                 ///< The DLL value
        };

      private: // Private methods

        /** Starting with all tracks pion, calculate logLikelihood. Then for
         * each track in turn, holding all others to pion, calculate new
         * logLikelihood for each particle code. If less than with all pion,
         * set new minimum track hypothesis.
         * @return Number of tracks that changed mass hypothesis
         */
        unsigned int initBestLogLikelihood() const;

        /** Starting with all tracks with best hypotheses as set by
         * initBestLogLikelihood(), for each track in turn get
         * logLikelihood for each particle code, and return the track and
         * particle code which gave the optimal log likelihood.
         * @return Number of tracks that changed mass hypothesis
         */
        void findBestLogLikelihood( MinTrList & minTracks ) const;

        /// Do the event iterations
        unsigned int doIterations() const;

        /// Calculates logLikelihood for event with a given set of track hypotheses.
        /// Performs full loop over all tracks and hypotheses
        double logLikelihood() const;

        /// Computes the change in the logLikelihood produced by changing given
        /// track to the given hypothesis
        double deltaLogLikelihood( LHCb::RichRecTrack * track,
                                   const Rich::ParticleIDType newHypo ) const;

      private:

        /// Returns log( exp(signal) - 1 ) using a full calculation
        inline double logExpFull( const double x ) const
        {
          return std::log( std::exp(x) - 1.0 );
        }

        /// Fast calculation using approximate exponential function
        inline double logExpApprox( const double x ) const
        {
          return std::log( x / std::pow((1.0-(x/6.0)),3) );
        }

        /// Power Approximation of log(exp(x)-1) (Mathematica)
        inline double logExp( const double x ) const
        {
          double res(0);
          if      ( x > 1.0 ) { res = std::log( std::exp(x) - 1.0 ); }
          else if ( x > 0.1 ) 
          {
            const double xx    = x*x;
            const double xxx   = xx*x;
            const double xxxx  = xxx*x;
            const double xxxxx = xxxx*x; 
            res = (-5.751779337152293 - 261.58791552313113*x - 
                   1610.1902353909695*xx - 291.61172549536417*xxx + 
                   3733.957211885683*xxxx + 1224.2104172168554*xxxxx)/
              (1.0 + 79.52981108433892*x + 953.4570349071099*xx + 
               2638.609797400796*xxx + 1506.9612115322623*xxxx - 
               27.33558114045007*xxxxx);
          }
          else if ( x > 0.01 )
          { 
            const double xx    = x*x;
            const double xxx   = xx*x;
            const double xxxx  = xxx*x;
            const double xxxxx = xxxx*x; 
            res = (-7.845788509794026 - 3428.7804135353526*x - 
                   228752.20145929293*xx - 3.082032088759535e6*xxx - 
                   3.836270197409883e6*xxxx + 1.2251900378118051e7*xxxxx)/
              (1.0 + 638.7306815040638*x + 60430.91709817034*xx + 
               1.315432074531156e6*xxx + 6.373056770682967e6*xxxx + 
               3.3914176474223877e6*xxxxx);
          }
          else
          { 
            const double xx    = x*x;
            const double xxx   = xx*x;
            const double xxxx  = xxx*x;
            const double xxxxx = xxxx*x; 
            res = (-10.160864268729455 - 49897.23275778952*x - 
                   3.855669108991894e7*xx - 6.777802095268419e9*xxx - 
                   2.421987003565588e11*xxxx - 3.5610129242332263e11*xxxxx)/
              (1.0 + 6487.897657865318*x + 6.294785881144457e6*xx + 
               1.4333658673633337e9*xxx + 7.670700007081306e10*xxxx + 
               6.06654149712832e11*xxxxx);
          }
          return res;
        }

        /// Returns log( exp(signal) - 1 ) or an approximation for small signals
        inline double sigFunc( const double s ) const
        {
          //plot1D( s, "Signal Values", 0, 10, 300 );
          //plot1D( std::log10(s), "Log10 Signal Values", -6, 6, 300 );
          return ( s > m_minSig ? logExp(s) : m_logMinSig );
        }

        /// Returns the force change Dll value
        inline double forceChangeDll() const
        {
          return m_forceChangeDll;
        }
        
        /// Returns the freeze out Dll value
        inline double freezeOutDll() const
        {
          return m_freezeOutDll;
        }
        
        /// Sets flags to say if the given set of tracks are in Rich1 and/or Rich2
        void updateRichFlags( const MinTrList & minTracks ) const;

      private:  // Private data members

        // tool pointers
        const ITrackCreator * m_gtkCreator;      ///< RichGlobalPIDTrack creator
        const IExpectedTrackSignal * m_tkSignal; ///< Track signal tool
        const IPhotonSignal * m_photonSig;       ///< Photon signal tool

        /// Pointer to the track container being considered
        mutable const LHCb::RichGlobalPIDTrack::Vector * m_gpidTracksToPID;

        /// Track DLL value to freeze track out from future iterations
        double m_freezeOutDll;

        /// Track DLL value for a forced change
        double m_forceChangeDll;

        /// Threshold for likelihood maximisation
        double m_epsilon;

        /// Maximum number of track iterations
        unsigned int m_maxEventIterations;

        /// Minimum signal value for full calculation of log(exp(signal)-1)
        double m_minSig;

        /// Cached value of log(exp(m_minSig) - 1) for efficiency
        double m_logMinSig;

        /// List of tracks ordered by change in likelihood
        mutable TrackList m_trackList;

        /// Current best event log likelihood value
        mutable double m_currentBestLL;

        /// Flag to turn on final DLL and hypothesis check
        bool m_doFinalDllCheck;

        /// Flag to turn on RICH check in LL minimisation
        bool m_richCheck;

        /// Maximum number of tracks to change in a single event iteration
        unsigned int m_maxTkChanges;

        /// Maximum number of iteration retries
        unsigned int m_maxItRetries;

        // working flags and variables
        mutable bool m_inR1, m_inR2;

      };

    }
  }
}

#endif // RICHGLOBALPID_RichGlobalPIDLikelihoodTool_H
