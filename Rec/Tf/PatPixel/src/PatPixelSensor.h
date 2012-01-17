// $Id: $
#ifndef PATPIXELSENSOR_H
#define PATPIXELSENSOR_H 1

// Include files
#include "PatPixelHit.h"
#include "VeloPixDet/DeVeloPix.h"
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"

class ChipParams {
public:
  ChipParams( Gaudi::XYZPoint p0, Gaudi::XYZVector slx, Gaudi::XYZVector sly ) {
    m_p0 = p0;
    m_vl = slx;
    m_vh = sly;
  };
  
  ~ChipParams() {};
  
  Gaudi::XYZPoint position( double x, double y ) { return m_p0 + x * m_vl + y * m_vh; }

private:
  Gaudi::XYZPoint m_p0;
  Gaudi::XYZVector m_vl;
  Gaudi::XYZVector m_vh;
};

/** @class PatPixelSensor PatPixelSensor.h
 *  Describes one sensor and its data for the Fast hit manager, from FastVelo
 *
 *  @author Olivier Callot
 *  @date   2012-01-05
 */
class PatPixelSensor {
public:
  /// Standard constructor
  PatPixelSensor( DeVeloPixSensor* sensor );

  virtual ~PatPixelSensor( ) {}; ///< Destructor

  unsigned int number()     const { return m_number; }
  int previous()            const { return m_previous;  }
  bool isRight()            const { return m_isRight; }
  double z()                const { return m_z; }
  PatPixelHits&  hits( )          { return m_hits; }

  void setPrevious( int prev ) { m_previous = prev; }

  DeVeloPixSensor* sensor()      { return m_sensor; }

  Gaudi::XYZPoint position( LHCb::VeloPixChannelID id, double dx, double dy ) {
    double low = id.pixel_lp() + dx;
    double hig = id.pixel_hp() + dy;
    return m_chips[id.chip()].position( low, hig ); 
  }

  void reset() { 
    m_hits.clear(); 
  }

  void addHit( PatPixelHit* hit ) {
    m_hits.push_back( hit );
  }

protected:

private:
  unsigned int      m_number;
  int               m_previous;
  bool              m_isRight;
  double            m_z;

  DeVeloPixSensor*  m_sensor;
  std::vector<ChipParams> m_chips;

  PatPixelHits      m_hits;
};
#endif // PATPIXELSENSOR_H
