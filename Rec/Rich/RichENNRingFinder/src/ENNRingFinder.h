
//-----------------------------------------------------------------------------
/** @file ENNRingFinder.h
 *
 *  Header file for ENN ring finder
 *
 *  CVS Log :-
 *  $Id: ENNRingFinder.h,v 1.9 2009-05-23 10:45:07 jonrob Exp $
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
        
        /** Default Constructor
         *  @param debug Flag to enable debug printout
         */
        Finder( const bool debug = false ) : m_debug(debug) { }

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
          friend inline std::ostream & operator << ( std::ostream & os, const Hit & hit )
          { 
            return os << "[ Hit " << hit.key << " (x,y)= (" << hit.x << "," << hit.y << ")"
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
          inline double purity() const { return ( NHits>0 ? (double)NOwn/(double)NHits : 0 ); }
        public:
          /// Overloaded output to ostream
          friend inline std::ostream & operator << ( std::ostream & os, const Ring & ring )
          { 
            return os << "[ Ring (x,y,r) " << ring.x << "," << ring.y << "," << ring.r
                      << " NHits=" << ring.NHits
                      << " NOwn="  << ring.NOwn
                      << " Chi2="  << ring.chi2
                      << " Purity=" << ring.purity()
                      << " ]"; 
          }
        public:
          bool on;   ///< is the ring selected?
          double x;  ///< ring centre point y
          double y;  ///< ring centre point y
          double r;  ///< ring radius
          double chi2; ///< chi2
          Hit::PtnVector Hits; ///< pointers to ring hits
          // variables for the selection procedure:
          int NHits; ///< number of ring hits
          int NOwn;  ///< number of its own hits
          bool skip; ///< skip the ring during selection
        };

      public:

        /** Do the ring finding with the given parameters
         *  @param HitSigma hit sigma
         *  @param MinRIngHits Minimum number of hits on a ring
         *  @param RMin Minimum ring radius
         *  @param RMax Maximum ring radius
         */
        void FindRings( const double HitSigma = 1.,
                        const int MinRingHits = 5,
                        const double RMin     = 2.,
                        const double RMax     = 6. );

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

        Hit::Vector  m_Hits;   ///< The input hits
        Ring::Vector m_Rings;  ///< The found rings
        bool m_debug;          ///< Turn on debug printout

      };

    }
  }
}

#endif // RichENNRingFinder_ENNRingFinder_H
