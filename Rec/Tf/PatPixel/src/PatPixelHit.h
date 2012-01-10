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
    m_wx(0.),
    m_wy(0.),
    m_sensor(0),
    m_isUsed( false )
  {
  };

  virtual ~PatPixelHit( ) {}; ///< Destructor

  void setHit( const LHCb::LHCbID id,
               const Gaudi::XYZPoint& point,
               const double dx,
               const double dy,
               const unsigned int sensor ) {
    m_id        = id;
    m_x         = point.x();
    m_y         = point.y();
    m_z         = point.z();
    m_wx        = 1./dx/dx;
    m_wy        = 1./dy/dy;
    m_sensor    = sensor;
    m_isUsed    = false;    
  }

  LHCb::LHCbID id()     const { return m_id;     }
  double x()            const { return m_x;      }
  double y()            const { return m_y;      }
  double z()            const { return m_z;      }
  double wx()           const { return m_wx;     }
  double wy()           const { return m_wx;     }
  int    sensor()       const { return m_sensor; }
  bool   isUsed()       const { return m_isUsed; }
  void   setUsed( bool flag ) { m_isUsed = flag; }

  double distance( double x, double y ) const { 
    double dx = x - m_x;
    double dy = y - m_y;
    return sqrt( dx*dx + dy * dy );
  }

  double chi2( double x, double y ) const {
    double dx = x - m_x;
    double dy = y - m_y;
    return dx * dx * m_wx + dy * dy * m_wy;
  }

  struct DecreasingByZ  {
    bool operator() (const PatPixelHit* lhs, const PatPixelHit* rhs) const { return lhs->z() > rhs->z(); }
  };

  struct IncreasingByZ  {
    bool operator() (const PatPixelHit* lhs, const PatPixelHit* rhs) const { return lhs->z() < rhs->z(); }
  };

  struct LowerByX  {
    bool operator() (const PatPixelHit* lhs, const PatPixelHit* rhs) const { return lhs->x() < rhs->x(); }
  };

  struct LowerByY  {
    bool operator() (const PatPixelHit* lhs, const PatPixelHit* rhs) const { return lhs->y() < rhs->y(); }
  };

protected:

private:
  LHCb::LHCbID m_id;
  double m_x;
  double m_y;
  double m_z;
  double m_wx;
  double m_wy;
  unsigned int m_sensor;
  bool   m_isUsed;
};

typedef std::vector<PatPixelHit*> PatPixelHits;

#endif // PATPIXELHIT_H
