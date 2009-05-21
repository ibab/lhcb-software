
#ifndef _ENNRingFinder_
#define _ENNRingFinder_

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
       *  Trackless ring finder using a ENN method
       *
       *  See http://www.gsi.de/documents/DOC-2005-Sep-83-1.pdf for details
       */
      //-----------------------------------------------------------------------
      class Finder
      {

      public:
        
        Finder( bool debug = false ) : m_debug(debug) { }

      public:

        //-----------------------------------------------------------------------
        /** @class Hit ENNRingFinder.h
         *
         * Hit object for ring Finder
         */
        //-----------------------------------------------------------------------
        class Hit
        {
        public:
          typedef std::vector<Hit> Vector;
          typedef std::vector<Hit*> PtnVector;
        public:
          Hit( const int _key  = -1,
               const double _x = 0,
               const double _y = 0 )
            : x(_x), y(_y), busy(0),
              lx(0), ly(0), lr2(0),
              S0(0), S1(0), S2(0), S3(0), S4(0),
              C(0), Cx(0), Cy(0),
              on_ring(false), key(_key) { }
        public:
          inline bool operator < ( const Hit & h ) const
          { return ( this->x < h.x ); }
        public:
          /// Overloaded output to ostream
          friend inline std::ostream & operator << ( std::ostream & os, const Hit & hit )
          { return os << "[ Hit " << hit.key << " (x,y)=" << hit.x << "," << hit.y << " ]"; }
        public:
          double x, y; // coordinates
          int busy; // quality of the best ring with this hit
          // variables for local search:
          double lx, ly, lr2; // local coordinates
          double S0, S1, S2, S3, S4; // coefficients for calculation of E
          double C, Cx, Cy; // coefficients for the parameter space
          bool on_ring; // is the hit close to the current ring
          int key;
        };

        //-----------------------------------------------------------------------
        /** @class Ring ENNRingFinder.h
         *
         * Ring object for ring Finder
         */
        //-----------------------------------------------------------------------
        class Ring
        {
        public:
          typedef std::vector<Ring> Vector;
          typedef std::vector<Ring*> PtnVector;
        public:
          Ring() : on(false),
                   x(0), y(0), r(0),
                   chi2(0),
                   NHits(0),
                   NOwn(0),
                   skip(false) { }
        public:
          inline double radius() const { return r; }
        public:
          /// Overloaded output to ostream
          friend inline std::ostream & operator << ( std::ostream & os, const Ring & ring )
          { return os << "[ Ring (x,y,r) " << ring.x << "," << ring.y << "," << ring.r << " ]"; }
        public:
          bool on; // is the ring selected?
          double x, y, r; // parameters
          double chi2; // chi2
          Hit::PtnVector Hits; // pointers to ring hits
          // variables for the selection procedure:
          int NHits; // number of ring hits
          int NOwn; // number of its own hits
          bool skip; // skip the ring during selection
        };

      public:

        void FindRings( const double HitSigma = 1.,
                        const int MinRingHits = 5,
                        const double RMin = 2.,
                        const double RMax = 6. );

        inline Hit::Vector  & hits()  { return m_Hits;  }

        inline Ring::Vector & rings() { return m_Rings; }

        /// Clean up for a new event
        void clear();

      private:

        Hit::Vector  m_Hits;
        Ring::Vector m_Rings;
        bool m_debug;

      };

    }
  }
}

#endif
