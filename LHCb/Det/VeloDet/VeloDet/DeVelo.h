// $Id: DeVelo.h,v 1.10 2002-07-09 20:45:09 parkesb Exp $
#ifndef       VELODET_DEVELO_H
#define       VELODET_DEVELO_H 1
// ============================================================================
/// from STL
#include <iostream>
#include <vector>

/// from CLHEP
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Units/PhysicalConstants.h"

/// GaudiKernel
#include "GaudiKernel/MsgStream.h"

/// from Det/DetDesc
#include "DetDesc/DetectorElement.h"
#include "DetDesc/IGeometryInfo.h"

/// forwad declarations
class MsgStream;
#include "Kernel/VeloChannelID.h"

/**  
 * Auxilliary class to handle each sensor.
 */

class VeloSensor {
  
public:
  VeloSensor() {};
  ~VeloSensor() {};

  double z()                    const { return m_z;      }
  void   setZ( double z )             { m_z = z;         }
  
  int    type()                 const { return m_type;   }
  void   setType( int type )          { m_type = type;   }
  
  int    number()               const { return m_number; }
  void   setNumber( int num )         { m_number = num;  }

  std::vector<int> phiAssociated() const { return m_associated; };
  void   associate( int num )         { m_associated.push_back( num ); };
  
  IGeometryInfo* geometry( )               const { return m_geometry; }
  void   setGeometry( IGeometryInfo* geo )       { m_geometry = geo; }


private:
  int m_number;   ///< station number
  /** The detector type describes if the detector is right or left (low bit)
   *  and which type : R is 0/1, Phi with one stereo is 2/3, phi with the 
   *  other stereo is 4/5
   */
  int m_type;     ///< detector type
  double m_z;     ///< Z position of silicium center
  std::vector<int> m_associated; ///< list of associated phi sensors.
  IGeometryInfo*   m_geometry;  ///< Pointer to the sensor geometry.
};

/** @class DeVelo DeVelo.h "VeloDet/DeVelo.h" 
 *
 *  Velo detector element class.
 *
 *  @author Olivier Callot Olivier.Callot@cern.ch
 */

static const CLID& CLID_DeVelo = 8100; 

class DeVelo: public DetectorElement {

public:
  
  /// Constructors
  DeVelo( const std::string& name    = "" );
  
  /// (dirtual) Destructor
  virtual ~DeVelo() ;
  
  /// object identification
  static  const CLID& classID ()       { return CLID_DeVelo ; } 
  /// object identification
  virtual const CLID& clID    () const ;
  
  /// initialization method 
  virtual StatusCode initialize(); 

  /// return the sensor number for a point
  int sensorNumber( const HepPoint3D& point );

  /// return the PU sensor number for a point
  int puSensorNumber( const HepPoint3D& point );

  /// return the number of sensors
  int nbSensor()  const { return m_sensor.size() ; };
  
  /// return the number of sensors
  int nbPuSensor()  const { return m_puSensor.size(); };
  
  /// return the (floating) strip number for the point in this sensor;
  double stripNumber( unsigned int sensorNumber, 
                      const HepPoint3D& point, 
                      double& pitch );

  /// return the (floating) strip number for the point in this sensor;
  double puStripNumber( unsigned int sensorNumber, 
                        const HepPoint3D& point, 
                        double& pitch );

  /// return the space point and sigma for a given pair of strips.
  bool getSpacePoint( unsigned int RSensorNumber, 
                      double       RStripNumber,
                      unsigned int PhiSensorNumber, 
                      double       PhiStripNumber,
                      HepPoint3D& point, 
                      double&  rPitch,
                      double&  phiPitch );

  /// return true if this sensor measures the R coordinate
  bool isRSensor( unsigned int num ) { 
    if ( m_sensor.size() > num ) {
      if ( (0 ==  m_sensor[num]->type()) || 
           (1 ==  m_sensor[num]->type()) )  {
        return true;
      }
    }
    return false;
  }

  /// Returns the vector of Phi sensor number one can match with the 
  /// specified R sensor number
  std::vector<int> phiSensorAssociated( unsigned int rNum ) {
    if ( m_sensor.size() > rNum ) {
      if ( (0 ==  m_sensor[rNum]->type()) || 
           (1 ==  m_sensor[rNum]->type()) )  {
        return m_sensor[rNum]->phiAssociated();
      }
    }
    return std::vector<int>();
  }
  
  /// Returns the z position of this Velo sensor
  double zSensor( unsigned int num ) { 
    if ( m_sensor.size() > num ) {
      return m_sensor[num]->z(); 
    } else{
      return -9999.;
    }
  }
  
  /// Returns the Z position of this PuVeto sensor
  double zPuSensor( unsigned int num ) { 
    if ( m_puSensor.size() > num ) {
      return m_puSensor[num]->z(); 
    } else{
      return -9999.;
    }
  }

  /// returns the local radius of the strip
  double rOfStrip( double strip, int& rZone );

  /// Returns the (local) radius of the specified strip
  double rOfStrip( int strip ) {
    int localStrip = strip %1024;   // Two halves in R are equal.
    if ( m_nbRInner <= localStrip ) { localStrip -= (int)m_nbRInner; }
    return m_rStrip[localStrip];
  }
  
  /// return true if the two rZones can match
  bool matchingZones( int zone1, int zone2 );

  /// returns the phi of the strip at the specified radius for this sensor.
  double phiOfStrip( double strip, double radius, int sensor );

  /// returns the signed distance from origin to phi strip number
  double originToPhiDistance( double strip, int sensor );
  
  /// returns the phi angle of the strip itself. 'phiOfStrip' returns the 
  /// phi of the point at the specified radius on the strip. 
  /// 'phiDirectionOfStrip' is the direction of the strip
  double phiDirectionOfStrip( double strip, int sensor );

  /// returns the R pitch at the given radius
  double rPitch( double radius ) {
    if ( m_fixPitchRadius > radius ) {
      return m_innerPitch;
    } else {
      return m_innerPitch + m_pitchSlope * ( radius -  m_fixPitchRadius);
    }
  }

  /// returns the Phi pitch (in mm) at the given radius
  double phiPitch( double radius ) {
    if ( m_phiBoundRadius > radius ) {
      return m_phiPitchInner * radius;
    } else {
      return m_phiPitchOuter * radius;
    }
  }
  
  /// returns the Phi pitch (in radian) for a given strip
  double phiPitch( int strip ) {
    if ( strip%2048 < m_nbPhiInner ) {
      return m_phiPitchInner;
    } else {
      return m_phiPitchOuter;
    }
  }
  

  /**
   * Set accessor to member m_zVertex, used to compute phi by extrapolating
   * between R and Phi sensors.
   * @param zVertex the new value for m_zVertex
   */
  void setZVertex (double zVertex) { m_zVertex = zVertex; }

  /**
   * Get accessor to member m_zVertex, used to compute Phi by extraposlating 
   * between R and Phi sensors.
   * @return the current value of m_zVertex
   */
  double zVertex () { return m_zVertex; }

  /// return the maximum sensitive radius of an R wafer
  double rMax() { return m_outerRadius; };
  
  /// returns the silicon thickness
  double siliconThickness ( unsigned int num ) {
    if ( m_sensor.size() > num ) {
      return m_siliconThickness;
    }
    return -1.;
  }
  /// returns the number of strips per sensor.
  int nbStrips() const { return 2048; }
  
  /// return the neighbour at 'numAway' channels.
  VeloChannelID neighbour( const VeloChannelID& chan, 
                           int numAway, 
                           bool& valid );

  /// returns the distance in strip of two channels, and if this is valid.
  int neighbour( const VeloChannelID& entryChan,
                 const VeloChannelID& exitChan,
                 bool& valid );
  
  /// Indices to get access to an array of properties...
  /// get an index to use in an array (0->max) from the sensor number
  int sensorArrayIndex(int sensorId) { return sensorId; };

  /// given an array index what is the corresponding sensor number
  int sensorNumber(int sensorIndex) { return sensorIndex; };

  /// get an index to use in an array (0->max) from the strip number
  int stripArrayIndex(int sensorId, int stripId);

  /// given an array index what is the corresponding strip number
  int stripNumber(int sensorId, int stripIndex);
 
  /// returns the capacitance of the strip.
  double stripCapacitance(int sensorId, int stripIndex);

  /// return the channelID number + fraction of channel for the point 
  /// in this sensor;
  VeloChannelID channelID(const HepPoint3D& point, 
                          double& fraction, 
                          double& pitch,
                          bool& valid);


protected: 

  /// return the (floating) strip number for the point in a sensor of this type
  double stripNumberByType( int type,
                            const HepPoint3D& point,
                            double& pitch );
  
  double phiOffset( double radius )  {
    if (  m_phiBoundRadius > radius ) {
      return m_phiInnerTilt - asin( m_innerTiltRadius / radius );
    } else {
      return m_phiOuterTilt - asin( m_outerTiltRadius / radius ) ;
    }
  }
  
private:
  
  double m_innerRadius;
  double m_outerRadius;
  double m_siliconThickness;
  // R detector
  double m_fixPitchRadius;
  double m_middleRRadius;
  double m_innerPitch;
  double m_outerPitch;
  double m_nbRInner;
  
  // Phi detector
  double m_phiBoundRadius;
  double m_innerTilt;
  double m_outerTilt;
  double m_phiOrigin;
  double m_nbPhiInner;
  
  // auxilliary variables

  double m_pitchSlope;
  double m_halfAngle;
  double m_quarterAngle;
    
  double m_innerTiltRadius;
  double m_outerTiltRadius;
  double m_phiAtBoundary;
  double m_phiPitchInner;
  double m_phiPitchOuter;
  double m_phiInnerTilt;
  double m_phiOuterTilt;

  double m_zVertex;
  // Local storage for geometry computation
  std::vector<VeloSensor*> m_sensor;
  std::vector<VeloSensor*> m_puSensor;
  std::vector<double>      m_rStrip;           ///< list of strip radii
};

// ============================================================================
#endif  //    VELODET_DEVELO_H
// ============================================================================
