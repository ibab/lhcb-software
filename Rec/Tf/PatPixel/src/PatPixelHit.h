// $Id: $
#ifndef PATPIXELHIT_H
#define PATPIXELHIT_H 1

// Include files
#include "Kernel/LHCbID.h"
#include "GaudiKernel/Point3DTypes.h"

/** @class PatPixelHit PatPixelHit.h
 *  This defines the Pixel Velo hits to be used in the pattern.
 *
 *  @author Olivier Callot
 *  @date   2012-01-05
 */

class PatPixelHit {
public:
  /// Standard constructor
  PatPixelHit( ): 
    m_x(0.), 
    m_y(0.), 
    m_z(0.), 
    m_r(0.),
    m_wx(0.),
    m_wy(0.),
    m_sensor(0),
    m_isUsed( false )
  { };

  virtual ~PatPixelHit( ) {}; ///< Destructor

  void setHit( const LHCb::LHCbID id,
               const Gaudi::XYZPoint& point,
               const double dx,
               const double dy,
               const unsigned int sensor )
  { m_id        = id;
    m_x         = point.x();
    m_y         = point.y();
    m_z         = point.z();
    m_r         = sqrt(m_x*m_x + m_y*m_y);
    m_wx        = 1.0/(dx*dx);
    m_wy        = 1.0/(dy*dy);
    m_sensor    = sensor;
    m_isUsed    = false; }

  LHCb::LHCbID id()     const { return m_id;     }
  double x()            const { return m_x;      }
  double y()            const { return m_y;      }
  double z()            const { return m_z;      }
  double r()            const { return m_r;      }
  double wx()           const { return m_wx;     }
  double wy()           const { return m_wx;     }
  int    sensor()       const { return m_sensor; }
  bool   isUsed()       const { return m_isUsed; }
  void   setUsed( bool flag ) { m_isUsed = flag; }

  double distance( double x, double y ) const         // distance in X-Y to given X-Y point
  { double dx = x - m_x;
    double dy = y - m_y;
    return sqrt( dx*dx + dy*dy ); }

  double chi2( double x, double y ) const            // distance-square / sigma-square
  { double dx = x - m_x;
    double dy = y - m_y;
    return dx*dx*m_wx + dy*dy*m_wy; }

  // for sorting the vectors of pointers to hits
  struct DecreasingByZ { bool operator() (const PatPixelHit* lhs, const PatPixelHit* rhs) const { return lhs->z() > rhs->z(); } };
  struct IncreasingByZ { bool operator() (const PatPixelHit* lhs, const PatPixelHit* rhs) const { return lhs->z() < rhs->z(); } };
  struct LowerByX      { bool operator() (const PatPixelHit* lhs, const PatPixelHit* rhs) const { return lhs->x() < rhs->x(); } };
  struct LowerByY      { bool operator() (const PatPixelHit* lhs, const PatPixelHit* rhs) const { return lhs->y() < rhs->y(); } };
  struct HigherByR     { bool operator() (const PatPixelHit* lhs, const PatPixelHit* rhs) const { return lhs->r() > rhs->r(); } };

  void print(void) const
  { printf(" %02d%c:%+6.1f:%4.1f:%+6.1f [%+5.1f,%+5.1f] %c\n",
           (m_sensor>>1)+1, (m_sensor&1)?'R':'L', m_z, m_r, (180/M_PI)*atan2(m_y, m_x), m_x, m_y, m_isUsed?'*':' ' ); }

  void print(double x, double y) const
  { printf(" %02d%c:%+6.1f:%4.1f:%+6.1f [%+5.1f,%+5.1f] [%+5.2f,%+5.2f] %4.1f %c\n",
           (m_sensor>>1)+1, (m_sensor&1)?'R':'L', m_z, m_r, (180/M_PI)*atan2(m_y, m_x), m_x, m_y, x-m_x, y-m_y, chi2(x,y), m_isUsed?'*':' ' ); }

protected:

private:
  LHCb::LHCbID m_id;
  double m_x;             // X-position
  double m_y;             // Y-position
  double m_z;             // Z-position
  double m_r;             // X-Y radius
  double m_wx;            // weight: 1/squared error in X
  double m_wy;            // weight: 1/squared error in Y
  unsigned int m_sensor;  // sensor number
  bool   m_isUsed;        // already used by a (associated to) track ?
};

typedef std::vector<PatPixelHit*> PatPixelHits;  // vector of hits

#endif // PATPIXELHIT_H
