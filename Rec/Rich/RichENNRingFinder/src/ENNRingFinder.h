
//-----------------------------------------------------------------------------
/** @file ENNRingFinder.h
 *
 *  Header file for ENN ring finder
 *
 *  CVS Log :-
 *  $Id: ENNRingFinder.h,v 1.12 2009-06-03 08:52:59 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   22/05/2009
 */
//-----------------------------------------------------------------------------

#ifndef RichENNRingFinder_ENNRingFinder_H
#define RichENNRingFinder_ENNRingFinder_H 1

#include <vector>
#include <ostream>

namespace Rich
{
  namespace Rec
  {
    namespace ENNRingFinder
    {

      //-----------------------------------------------------------------------
      /** @class Finder ENNRingFinder.h
       *
       *  Trackless ring finder using an Elastic Neural Network method
       *
       *  See http://www.gsi.de/documents/DOC-2005-Sep-83-1.pdf for details
       */
      //-----------------------------------------------------------------------
      class Finder
      {

      public:

        //-----------------------------------------------------------------------
        /** @class Hit ENNRingFinder.h
         *
         * Ultility Hit class for ring finder
         */
        //-----------------------------------------------------------------------
        class Hit
        {
        public:
          typedef std::vector<Hit> Vector;
          typedef std::vector<Hit*> PtnVector;
        public:
          /// Constructor
          Hit( const unsigned int _key  = -1,
               const double _x = 0,
               const double _y = 0 )
            : x(_x), y(_y), busy(0),
              lx(0), ly(0), lr2(0),
              S0(0), S1(0), S2(0), S3(0), S4(0),
              C(0), Cx(0), Cy(0),
              on_ring(false), key(_key),
              nAssRings(0)
          { }
        public:
          /// Sorting operator
          inline bool operator < ( const Hit & h ) const
          { return ( this->x < h.x ); }
        public:
          /// Overloaded output to ostream
          friend inline std::ostream & operator << ( std::ostream & os, 
                                                     const Hit & hit )
          { 
            return os << "[ Hit " << hit.key 
                      << " (x,y)= (" << hit.x << "," << hit.y << ")"
                      << " nAssRings=" << hit.nAssRings << " ]"; 
          }
        public:
          double x; ///< x coordinate
          double y; ///< y coordinate
          int busy; ///< quality of the best ring with this hit
          // variables for local search:
          double lx, ly, lr2; // local coordinates
          double S0, S1, S2, S3, S4; // coefficients for calculation of E
          double C, Cx, Cy;          // coefficients for the parameter space
          bool on_ring;           ///< is the hit close to the current ring
          unsigned int key;       ///< hit key
          unsigned int nAssRings; ///< Number of rings associated with
        };

        //-----------------------------------------------------------------------
        /** @class Ring ENNRingFinder.h
         *
         *  Utility Ring class for ring finder
         */
        //-----------------------------------------------------------------------
        class Ring
        {
        public:
          typedef std::vector<Ring> Vector;
          typedef std::vector<Ring*> PtnVector;
        public:
          /// Constructor
          Ring( const double _x     = 0,
                const double _y     = 0,
                const double _r     = 0,
                const int    _nHits = 0 ) 
            : on(false),
              x(_x), y(_y), r(_r),
              chi2(0),
              NHits(_nHits),
              NOwn(0),
              skip(false) { }
        public:
          /// The ring radius
          inline double radius() const { return r; }
          /// Ring purity (fraction of associated hits owned by this ring)
          inline double purity() const
          { return ( NHits>0 ? (double)NOwn/(double)NHits : 0 ); }
        public:
          /// Overloaded output to ostream
          friend inline std::ostream & operator << ( std::ostream & os, 
                                                     const Ring & ring )
          { 
            return os << "[ Ring (x,y,r) " 
                      << ring.x << "," << ring.y << "," << ring.r
                      << " NHits=" << ring.NHits
                      << " NOwn="  << ring.NOwn
                      << " Chi2="  << ring.chi2
                      << " Purity=" << ring.purity()
                      << " ]"; 
          }
        public:
          bool on;     ///< is the ring selected?
          double x;    ///< ring centre point y
          double y;    ///< ring centre point y
          double r;    ///< ring radius
          double chi2; ///< chi2
          Hit::PtnVector Hits; ///< pointers to ring hits
          // variables for the selection procedure:
          int NHits; ///< number of ring hits
          int NOwn;  ///< number of its own hits
          bool skip; ///< skip the ring during selection
        };

        //-----------------------------------------------------------------------
        /** @class Config ENNRingFinder.h
         *
         *  Utility Configuration class for ring finder
         */
        //-----------------------------------------------------------------------
        class Config
        {
        public:
          /// Default constructor
          Config( const double _HitSigma = 5.0,
                  const int _minRingHits = 8,
                  const double _rMin     = 85.0,
                  const double _rMax     = 150.0,
                  const double _rejFact  = 0.5,
                  const bool _debug      = false ) 
            : m_HitSigma        ( _HitSigma  ),
              m_MinRingHits     ( _minRingHits   ),
              m_RMin            ( _rMin ),
              m_RMax            ( _rMax  ),
              m_RejectionFactor ( _rejFact ),
              m_debugMode       ( _debug ),
              // cached values
              m_R2Min           ( _rMin*_rMin ),
              m_R2Max           ( _rMax*_rMax ),
              m_HitSize         ( _HitSigma / 2.0 ),
              m_AreaSize        (  2.0 * ( _rMax + _HitSigma ) )
          {
            m_AreaSize2 = m_AreaSize * m_AreaSize;
          }
        public:
          /// Overloaded output to ostream
          friend inline std::ostream & operator << ( std::ostream & os, 
                                                     const Config & config )
          { 
            return os << "[ HitSigma=" << config.hitSigma()
                      << " MinRingHits=" << config.minRingHits()
                      << " RMin=" << config.rMin()
                      << " RMax=" << config.rMax()
                      << " RejectionFactor=" << config.rejectionFact()
                      << " ]";
          }
        public:
          inline double hitSigma()      const { return m_HitSigma; }
          inline int minRingHits()      const { return m_MinRingHits; }
          inline double rMin()          const { return m_RMin; }
          inline double rMax()          const { return m_RMax; }
          inline double rejectionFact() const { return m_RejectionFactor; }
          inline bool debugMode()       const { return m_debugMode; }
          // cached values
          inline double r2Min()         const { return m_R2Min; }
          inline double r2Max()         const { return m_R2Max; }
          inline double hitSize()       const { return m_HitSize; }
          inline double areaSize()      const { return m_AreaSize; }
          inline double areaSize2()     const { return m_AreaSize2; }
        private:
          double m_HitSigma;          ///< Hit sigma
          int m_MinRingHits;          ///< Minimum number of hits on a ring
          double m_RMin;              ///< Minimum ring radius
          double m_RMax;              ///< Maximum ring radius
          double m_RejectionFactor;   ///< Factor for background rejection
          bool m_debugMode;           ///< debug mode
          // cached values
          double m_R2Min;             ///< Minimum ring radius squared
          double m_R2Max;             ///< Maximum ring radius squared
          double m_HitSize;           ///< Hit Size
          double m_AreaSize;          ///< Search area
          double m_AreaSize2;         ///< Search area squared
        };

      public:
        
        /** Default Constructor
         *  @param debug Flag to enable debug printout
         */
        Finder( const Config & _config ) : m_config(_config) 
        { 
          if ( config().debugMode() ) { std::cout << config() << std::endl; }
        }

      public:

        /// Access the configuration object
        const Config & config() const { return m_config; }

        /// Do the ring finding 
        void FindRings( );

        /// Return the probability the given Hit is associated to the given ring
        double hitProbability( const Ring & ring, const Hit & hit );

        /// read/write access to the input hits
        inline Hit::Vector  & hits()  { return m_Hits;  }

        /// read/write access to the found rings
        inline Ring::Vector & rings() { return m_Rings; }

        /// read-only access to the input hits
        inline const Hit::Vector  & hits()  const { return m_Hits;  }

        /// read-only access to the found rings
        inline const Ring::Vector & rings() const { return m_Rings; }

        /// Clean up for a new event
        void clear();

      private:

        Config m_config;       ///< Configuration object
        Hit::Vector  m_Hits;   ///< The input hits
        Ring::Vector m_Rings;  ///< The found rings

      };

      inline void Finder::clear()
      {
        hits().clear();
        rings().clear();
      }

    }
  }
}

#endif // RichENNRingFinder_ENNRingFinder_H
