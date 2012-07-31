// $Id: $
#ifndef PRKERNEL_PRHIT_H 
#define PRKERNEL_PRHIT_H 1

// Include files
#include "Kernel/LHCbID.h"
#include "Kernel/DetectorSegment.h"

/** @class PrHit PrHit.h PrKernel/PrHit.h
 *  Hits to be used in the pattern in the T/TT stations
 *
 *  @author Olivier Callot
 *  @date   2012-03-13
 */
class PrHit {
public: 
  /// Standard constructor
  PrHit( ):
    m_size(0),
    m_charge(0),
    m_segment( ),
    m_w(0.),
    m_coord(0.),
    m_zone(0),
    m_isUsed( false )
  {}; 

  virtual ~PrHit( ) {}; ///< Destructor

  void setHit( const LHCb::LHCbID id,
               const int size,
               const int charge,
               const DetectorSegment& segment,
               const float errX,
               const int zone,
               const int planeCode ) {
    m_id         = id;
    m_size       = size;
    m_charge     = charge;
    m_segment    = segment;
    m_w          = 1./(errX*errX);
    m_coord      = segment.x(0.);
    m_zone       = zone;
    m_planeCode  = planeCode;
    m_isUsed     = false;
    m_isX        = fabs( x(1.)-x(0.) ) < 0.001;
  }
  
  LHCb::LHCbID id()     const { return m_id;     }
  int   size()          const { return m_size; }
  int   charge()        const { return m_charge; }
  float x( float y=0. ) const { return m_segment.x( y ); }
  float z( float y=0. ) const { return m_segment.z( y ); }
  float w()             const { return m_w;      }
  float yMin()          const { return m_segment.yMin();   }
  float yMax()          const { return m_segment.yMax();   }
  float coord()         const { return m_coord;  }
  int   zone()          const { return m_zone;   }
  int   planeCode()     const { return m_planeCode; }
  bool  isUsed()        const { return m_isUsed; }
  float yOnTrack( float y0, float dyDz ) { return m_segment.yOnTrack( y0, dyDz ); }
  float dxDy()          const { return x(1.)-x(0.); }
  bool  isX()           const { return m_isX; }

  void setCoord( float c )   { m_coord = c;     }
  void setUsed( bool flag )  { m_isUsed = flag; }

  struct LowerByCoord {
    bool operator() (const PrHit* lhs, const PrHit* rhs ) const { return lhs->coord() < rhs->coord(); }
  };

  struct LowerByZ {
    bool operator() (const PrHit* lhs, const PrHit* rhs ) const { return lhs->z() < rhs->z(); }
  };

protected:

private:
  LHCb::LHCbID m_id;
  int    m_size;
  int    m_charge;
  DetectorSegment m_segment;
  float  m_w;
  float  m_coord;
  int    m_zone;
  int    m_planeCode;
  bool   m_isUsed;
  bool   m_isX;
};

typedef std::vector<PrHit*> PrHits;

#endif // PRKERNEL_PRHIT_H
