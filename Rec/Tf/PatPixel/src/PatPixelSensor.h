// $Id: $
#ifndef PATPIXELSENSOR_H
#define PATPIXELSENSOR_H 1

// Include files
#include "PatPixelHit.h"
#include "VPDet/DeVP.h"
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"

class ChipParams   // geometrical parameters for a single readout chip: there are 3 chips per sensor and 12 chips per module
{
public:
  ChipParams( Gaudi::XYZPoint p0, Gaudi::XYZVector slx, Gaudi::XYZVector sly )
  { m_p0 = p0;
    m_vl = slx;
    m_vh = sly; };
  
  ~ChipParams() {};
  
  Gaudi::XYZPoint position( double x, double y ) { return m_p0 + x*m_vl + y*m_vh; } // return X,Y,Z-position for given pixel X.Y-coordinates

private:
  Gaudi::XYZPoint  m_p0; // reference point: X,Y,Z position of pixel (0,0)
  Gaudi::XYZVector m_vl; // pixel pitch vector along the low-resolution axis
  Gaudi::XYZVector m_vh; // pixel pitch vector along the high-resolution axis
};                       // high/low-resolution was made for alongated pixels, for square pixels both resolutions are same: 55 microns

/** @class PatPixelSensor PatPixelSensor.h
 *  Describes one sensor and its data for the Fast hit manager, from FastVelo
 *
 *  @author Olivier Callot
 *  @date   2012-01-05
 */
class PatPixelSensor
{
public:
  PatPixelSensor( DeVPSensor* sensor );  // Constructor from DetectorElement

  virtual ~PatPixelSensor( ) {}; ///< Destructor

  unsigned int number()     const { return m_number; }
  int previous()            const { return m_previous;  }
  bool isRight()            const { return m_isRight; }
  double z()                const { return m_z; }
  PatPixelHits&  hits( )          { return m_hits; }

  void setPrevious( int prev )  { m_previous = prev; }

  DeVPSensor* sensor()      { return m_sensor; }

  Gaudi::XYZPoint position( LHCb::VPChannelID id, double dx, double dy ) // X,Y,Z-position for pixel ChannelID with fractional interpolation
  { double low = id.pixel_lp() + dx;
    double hig = id.pixel_hp() + dy;
    return m_chips[id.chip()].position( low, hig ); }

  void reset() {  m_hits.clear(); }                               // clear (remove) all stored hits

  void addHit( PatPixelHit* hit ) {  m_hits.push_back( hit ); }   // add a new hit

protected:

private:
  unsigned int      m_number;
  int               m_previous;
  bool              m_isRight;      // Right or Left side of VELO
  double            m_z;            // Z-position

  DeVPSensor*  m_sensor;            // DetectorElement from XML
  std::vector<ChipParams> m_chips;  // vector of readout chips

  PatPixelHits      m_hits;         // vector of hits
};
#endif // PATPIXELSENSOR_H
