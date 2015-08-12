
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
#include <algorithm>

// Base class
#include "RichGlobalPIDToolBase.h"
#include "RichRecBase/IRichPID.h"
#include "IRichGlobalPID.h"

// interfaces
#include "RichRecBase/IRichExpectedTrackSignal.h"
#include "RichRecBase/IRichPhotonSignal.h"
#include "IRichGlobalPIDTrackCreator.h"

// RichKernel
#include "RichKernel/RichMap.h"

// boost
#include "boost/format.hpp"
#include "boost/numeric/conversion/bounds.hpp"
#include "boost/limits.hpp"

// Vc
//#include "Vc/Vc"

// Eigen
//#include <Eigen/Geometry>

// VectorClass
//#include "VectorClass/vectorclass.h"

// VDT
#include "vdt/exp.h"
#include "vdt/log.h"

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

      private: // typedefs

        /// Container for changes to be made following an event iterations
        /// Contains a pointer to a track and the its new hypothesis
        typedef Rich::Map<LHCb::RichGlobalPIDTrack*,Rich::ParticleIDType> MinTrList;

        /// Track list entry. Its current best DLL change and a pointer to the trackk
        typedef std::pair<double,LHCb::RichGlobalPIDTrack*> TrackPair;

        /// List of all track list entries
        typedef std::vector<TrackPair> TrackList;

      private: // helper classes

        /// Stores information associated to a RichGlobalPIDTrack
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

        /// Sort a TrackList
        class TrackListSort : std::binary_function<const TrackPair,const TrackPair,bool>
        {
        public:
          /// Sorting operator
          inline bool operator() ( const TrackPair& p1, const TrackPair& p2 ) const
          {
            return p1.first < p2.first;
          }
        };

        /// Lookup table for log(exp(x)-1)
        template< class TYPE >
        class LogExpLookUp
        {
        public:
          /// Default Constructor
          LogExpLookUp() { clear(); }
          /// Initialise
          void init( const TYPE& minX, const TYPE& maxX, const unsigned int nBins )
          {
            // clear the current interpolation data
            clear();
            // reset the cached parameters
            m_minX = minX;
            m_maxX = maxX;
            m_incX = (double)nBins / ( m_maxX - m_minX );
            // refill the data vector
            m_data.reserve( nBins );
            for ( unsigned int i = 0; i < nBins; ++i )
            { m_data.push_back( LogExpLookUp::Data(binLowX(i),binHighX(i)) ); } 
          }
          /** get the log(exp(x)-1) value for a given x from the look up table
           *  @attention No min value range check, as this is done elsewhere */
          inline TYPE lookup( const TYPE& x ) const
          {
            return ( x < m_maxX ? m_data[xIndex(x)].getY(x) : 
                     vdt::fast_logf( vdt::fast_expf(x) - 1.0f ) );
          }
        private:
          /// A single data point
          class Data
          {
          public:
            /// Constructor from bin low/high edges
            Data( const TYPE& lowX, const TYPE& highX) 
              : m_xLow(lowX),         m_xHigh(highX), 
                m_yLow(logexp(lowX)), m_yHigh(logexp(highX)) 
            {
              m_slope = ( m_yLow - m_yHigh ) / ( m_xLow - m_xHigh );
              m_const = m_yLow - ( m_xLow * m_slope );
            }
            /// Get the y value for a given x for this Data point
            inline TYPE getY( const TYPE& x ) const 
            {
              return ( x * m_slope ) + m_const;
            }
          private:
            /// The log(exp(x)-1) function to initialise the map
            inline TYPE logexp( const TYPE& x ) const
            { return vdt::fast_logf( vdt::fast_expf(x) - 1.0f ); }
          private:
            // The (x,y) values for the low and high edge of this bin
            TYPE m_xLow, m_xHigh, m_yLow, m_yHigh;
            /// The slope parameter
            TYPE m_slope;
            /// The constant parameter
            TYPE m_const;
          public:
            /// type for a list of data points
            typedef std::vector<Data> Vector;
          };
        private:
          /// Clear this object
          void clear() 
          { 
            m_data.clear(); 
            m_minX = m_maxX = m_incX = 0;
          }
          /// Get the low x value for a given bin index
          inline TYPE binLowX( const unsigned int i ) const
          {
            return m_minX + (i/m_incX);
          }
          /// GGet the high x value for a given bin index
          inline TYPE binHighX( const unsigned int i ) const
          {
            return binLowX(i) + (1.0/m_incX);
          }
          /** Get the look up index for a given x
           *  Note NO range checking is done. Assumed done beforehand */
          inline unsigned int xIndex( const TYPE& x ) const
          {
            return (unsigned int)((x-m_minX)*m_incX);
          }
        private:
          /// The look up vector of data points
          typename Data::Vector m_data;
          /// The minimum valid x
          TYPE m_minX;
          /// The maximum valid x
          TYPE m_maxX;
          /// 1 / the bin increment
          TYPE m_incX;
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

        /// Implementation of log( e^x -1 ) (Original)
        //float logExpOriginal( const float x ) const;

        /// Implementation of log( e^x -1 ) (Eigen)
        //float logExpEigen( const float x ) const;

        /// Implementation of log( e^x -1 ) (VectorClass)
        //float logExpVectorClass( const float x ) const;

        /// Implementation of log( e^x -1 ) (Private Interpolator)
        inline float logExpInterp( const float x ) const
        {
          return m_logExpLookUp.lookup(x);
        }

        /// Prefered implementation of log( e^x -1 )
        inline float logExp( const float x ) const
        {
          return logExpInterp(x);
        }

        /// log( exp(x) - 1 ) or an approximation for small signals
        inline float sigFunc( const float x ) const
        {
          return ( x > m_minSig ? logExp(x) : m_logMinSig );
        }

        /// Implementation of log( e^x -1 ) - log( e^y -1 )  (VectorClass)
        //float sigFuncDiff( const float x, const float y ) const;

        /// Returns the force change Dll value
        inline double forceChangeDll() const { return m_forceChangeDll; }

        /// Returns the freeze out Dll value
        inline double freezeOutDll() const { return m_freezeOutDll; }

        /// Sets flags to say if the given set of tracks are in Rich1 and/or Rich2
        void updateRichFlags( const MinTrList & minTracks ) const;

        /// Print the current track list
        void printTrackList( const MSG::Level level ) const;

        /// Sort the track list
        inline void sortTrackList() const
        {
          std::stable_sort( m_trackList.begin(), m_trackList.end(), TrackListSort() );
        }

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
        float m_minSig;

        /// Cached value of log( exp(m_minSig) - 1 ) for efficiency
        float m_logMinSig;

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

        /// Look up table for log(exp(x)-1) function
        LogExpLookUp<float> m_logExpLookUp;

      };

    }
  }
}

#endif // RICHGLOBALPID_RichGlobalPIDLikelihoodTool_H
