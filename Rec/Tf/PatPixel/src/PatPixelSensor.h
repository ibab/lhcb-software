// $Id: $
#ifndef PATPIXELSENSOR_H
#define PATPIXELSENSOR_H 1

// Include files
#include "PatPixelHit.h"
#include "VPDet/DeVP.h"
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "TMath.h"

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

  unsigned int   number()     const { return m_number; }
  int            previous()   const { return m_previous;  }
  bool           isRight()    const { return m_isRight; }
  double         z()          const { return m_z; }
  PatPixelHits&  hits( )            { return m_hits; }

  void setPrevious( int prev )  { m_previous = prev; }

  DeVPSensor* sensor()          { return m_sensor; }    // return detector element from XML

  Gaudi::XYZPoint position( LHCb::VPChannelID id, double dx, double dy ) // X,Y,Z-position for pixel ChannelID with fractional interpolation
  {  std::pair<double,double> offsets(dx,dy);
    double dx_prime = dx*1.0, dy_prime = dy*1.0;
    Double_t P_offset, T_factor;
    Double_t slx, sly;
    Double_t delta_x = fabs(dx-0.5);
    Double_t delta_y = fabs(dy-0.5);
    Gaudi::XYZPoint Point = sensor()->globalXYZ( id.channelID(), offsets );
    if (dx == 0.5 && dy ==0.5) {return Point; }

    if (dx != 0.5)
    { slx = fabs(Point.x()/Point.z());
      P_offset =  0.31172471 +  0.15879833 * TMath::Erf(-6.78928312*slx + 0.73019077);
      T_factor =  0.43531842 +  0.3776611  * TMath::Erf( 6.84465914*slx - 0.75598833);
      dx_prime = 0.5 + (dx-0.5)/delta_x*(P_offset + T_factor *delta_x); }

    if (dy!=0.5)
    { sly = fabs(Point.y()/Point.z());
      P_offset =  0.35829374 - 0.20900493 * TMath::Erf(5.67571733*sly -0.40270243);
      T_factor =  0.29798696 + 0.47414641 * TMath::Erf(5.84419802*sly -0.40472057);
      dy_prime = 0.5 + (dy-0.5)/delta_y*(P_offset + T_factor *delta_y); }

    std::pair<double,double> offsets2(dx_prime,dy_prime);
    return sensor()->globalXYZ( id.channelID(), offsets2 );
  }
    //double low = id.pixel_lp() + dx;
    //double hig = id.pixel_hp() + dy;
    //return m_chips[id.chip()].position( low, hig ); }

  void reset() {  m_hits.clear(); }                               // clear (remove) all stored hits

  void addHit( PatPixelHit* hit ) {  m_hits.push_back( hit ); }   // add a new (pointer to) hit, which is stored in HitManager

  void print(void)
  { printf("PatPixelSensor: %02d%c Z=%+6.1f %d hits\n",
           (m_number>>1)+1, m_isRight?'R':'L', m_z, (int)m_hits.size() );
    for( PatPixelHits::const_iterator itH = m_hits.begin(); itH != m_hits.end(); ++itH)
    { (*itH)->print(); }
  }

protected:

private:
  unsigned int            m_number;
  int                     m_previous;
  bool                    m_isRight;      // Right or Left side of VELO
  double                  m_z;            // Z-position

  DeVPSensor*             m_sensor;       // DetectorElement from XML
  std::vector<ChipParams> m_chips;        // vector of readout chips

  PatPixelHits            m_hits;         // vector of (pointers to) hits, which are stored in PatPixelManager
};
#endif // PATPIXELSENSOR_H
