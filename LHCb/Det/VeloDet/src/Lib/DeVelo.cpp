// $Id: DeVelo.cpp,v 1.12 2002-04-24 09:51:24 ocallot Exp $
//
// ============================================================================
#define  VELODET_DEVELO_CPP 1
// ============================================================================
// from STL
#include <stdio.h>
#include <cmath>
#include <algorithm>
// from CLHEP
#include "CLHEP/Units/SystemOfUnits.h"
// from Gaudi
#include "GaudiKernel/SmartDataPtr.h"
// DetDesc
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/ILVolume.h"
//
// from Det/VeloDet
#include "VeloDet/DeVelo.h"

/** @file DeVelo.cpp
 *
 *  Implementation of class :  DeVelo
 *
 *  @author Olivier Callot Olivier.Callot@cern.ch
 */


// **  Standard Constructors

DeVelo::DeVelo( const std::string& name )
  :  DetectorElement     ( name       )
{ };

//
// Standard Destructor
DeVelo::~DeVelo()
{ };

// ============================================================================
// object identification
// ============================================================================
const CLID& DeVelo::clID () const { return DeVelo::classID() ; }


// ============================================================================
// intialization method
// ============================================================================
StatusCode DeVelo::initialize() {
  MsgStream loging( msgSvc(), "DeVelo" );
  
  StatusCode sc = DetectorElement::initialize();
  ///
  if( sc.isFailure() ) { 
    loging << MSG::ERROR << "Failure to initialize DetectorElement" << endreq;
    return sc ; 
  }
  ///
  typedef std::vector<std::string> Parameters;
  typedef Parameters::iterator     Iterator;
  ///
  Parameters pars( userParameters() );
  Iterator it;
  
  ///
  it = std::find( pars.begin() , pars.end () , std::string("InnerRadius") );
  if( pars.end() != it ) {
    m_innerRadius =  userParameterAsDouble( *it ) ;
    pars.erase( it );
  }
  it = std::find( pars.begin() , pars.end () , std::string("OuterRadius") );
  if( pars.end() != it ) {
    m_outerRadius =  userParameterAsDouble( *it ) ;
    pars.erase( it );
  }

  // R detector parameters

  it = std::find( pars.begin() , pars.end () , std::string("FixPitchRad") );
  if( pars.end() != it ) {
    m_fixPitchRadius =  userParameterAsDouble( *it ) ;
    pars.erase( it );
  }
  it = std::find( pars.begin() , pars.end () , std::string("MiddleRRad") );
  if( pars.end() != it ) {
    m_middleRRadius =  userParameterAsDouble( *it ) ;
    pars.erase( it );
  }
  it = std::find( pars.begin() , pars.end () , std::string("InnerPitch") );
  if( pars.end() != it ) {
    m_innerPitch =  userParameterAsDouble( *it ) ;
    pars.erase( it );
  }
  it = std::find( pars.begin() , pars.end () , std::string("OuterPitch") );
  if( pars.end() != it ) {
    m_outerPitch =  userParameterAsDouble( *it ) ;
    pars.erase( it );
  }
  it = std::find( pars.begin() , pars.end () , std::string("NbRInner") );
  if( pars.end() != it ) {
    m_nbRInner =  userParameterAsDouble( *it ) ;
    pars.erase( it );
  }

  // Phi Detector Parameters

  it = std::find( pars.begin() , pars.end () , std::string("PhiBoundRad") );
  if( pars.end() != it ) {
    m_phiBoundRadius =  userParameterAsDouble( *it ) ;
    pars.erase( it );
  }
  it = std::find( pars.begin() , pars.end () , std::string("InnerTilt") );
  if( pars.end() != it ) {
    m_innerTilt =  userParameterAsDouble( *it ) ;
    pars.erase( it );
  }
  it = std::find( pars.begin() , pars.end () , std::string("OuterTilt") );
  if( pars.end() != it ) {
    m_outerTilt =  userParameterAsDouble( *it ) ;
    pars.erase( it );
  }
  it = std::find( pars.begin() , pars.end () , std::string("PhiOrigin") );
  if( pars.end() != it ) {
    m_phiOrigin =  userParameterAsDouble( *it ) ;
    pars.erase( it );
  }
  it = std::find( pars.begin() , pars.end () , std::string("NbPhiInner") );
  if( pars.end() != it ) {
    m_nbPhiInner =  userParameterAsDouble( *it ) ;
    pars.erase( it );
  }

  if( !pars.empty() ) {
    // some "extra" parameters.
    // should be an error??
  }
  ///

  loging << MSG::DEBUG << "Velo : Radius from " << m_innerRadius/mm 
         << " to " << m_outerRadius/mm << endreq;
 
  // Auxilliary variables for R strip computation

  m_pitchSlope = (m_outerPitch - m_innerPitch ) /  
                 (m_outerRadius - m_fixPitchRadius);
  m_logPitchStep = log( 1. + m_pitchSlope );
  m_nbRFixedPitch = (m_fixPitchRadius - m_innerRadius) / m_innerPitch;
  m_halfAngle     = 91.0 * degree;
  m_quarterAngle  = .5 * m_halfAngle;
  
  // For phi computation  

  m_innerTiltRadius = m_innerRadius * sin( m_innerTilt );
  m_outerTiltRadius = m_phiBoundRadius * sin( m_outerTilt );
  m_phiInnerTilt    = m_innerTilt + m_phiOrigin;
  m_phiAtBoundary   = m_phiInnerTilt - 
                      asin( m_innerTiltRadius / m_phiBoundRadius );
  m_phiOuterTilt    = m_phiAtBoundary + m_outerTilt;
  m_phiPitchInner   = 2.* m_halfAngle / m_nbPhiInner;
  m_phiPitchOuter   = 2.* m_halfAngle / (2048. - m_nbPhiInner);

  double phi = m_phiOuterTilt - asin( m_outerTiltRadius / m_outerRadius );
  loging << MSG::INFO
      << "Phi (degree) inner "    << m_phiOrigin / degree
      << " at boundary " << m_phiAtBoundary / degree
      << " and outside " << phi / degree
      << endreq;

 
/**  
 * Get the stations (sensors) with their Z, type and orientation
 */
  HepPoint3D pointLocal( 0, 0, 0);
  HepPoint3D pointGlobal( 0, 0, 0);
  int number;
  int index = 0;
  bool inVeto = true;
  char line[80];
  
  IDetectorElement* stations = *childBegin();

  for( IDetectorElement::IDEContainer::iterator child = stations->childBegin();
       stations->childEnd() != child ; ++child ) {
    number = child - stations->childBegin();

    unsigned int firstInStation = m_sensor.size();

    for(IDetectorElement::IDEContainer::iterator sens = (*child)->childBegin();
        (*child)->childEnd() != sens ; ++sens ) {

      IGeometryInfo* geoData = (*sens)->geometry();
      if ( 0 != geoData ) {
        pointGlobal = geoData->toGlobal( pointLocal );
        double z =  pointGlobal.z();

        int type = -1;
        std::string typeStr = "???";
        Parameters pars( (*sens)->userParameters() );

        it = std::find( pars.begin() , pars.end () , std::string("Type") );
        if( pars.end() != it ) {
          typeStr = (*sens)->userParameterAsString( *it ) ;
          if ( typeStr == " VetoL" ) {
            type = 0;
          } else if ( typeStr == " VetoR" ) {
            type = 1;
          } else {
            if ( inVeto ) {
              inVeto = false;
              index  = 0;
            }
            if ( typeStr == " RLeft" ) {
              type = 0;
            } else if ( typeStr == " RRigh" ) {
              type = 1;
            } else if ( typeStr == " Phi1L" ) {
              type = 2;
            } else if ( typeStr == " Phi1R" ) {
              type = 3;
            } else if ( typeStr == " Phi2L" ) {
              type = 4;
            } else if ( typeStr == " Phi2R" ) {
              type = 5;
            }
          }
        }

        VeloSensor* sensor = new VeloSensor();
        sensor->setNumber( index++ );
        sensor->setType( type );
        sensor->setZ( z );
        sensor->setGeometry( geoData );
        if ( inVeto ) {
          m_puSensor.push_back( sensor );
        } else {
          m_sensor.push_back( sensor );
        }

        sprintf( line, "Sensor %2d ", sensor->number() );
        loging << MSG::INFO << line << typeStr;
        sprintf( line, "(%d) z=%7.2f ", sensor->type(), sensor->z() );
        loging << line;

      } else {
        loging << " no geometry !" << endreq;
      }
    }
    loging << endreq;

    // build the list of Phi associated to R, in principle both PHI of the 
    // station due to the dog-leg shape.
    // But for now on (phi and R are half circles), use only the closest
    for ( unsigned int sR = firstInStation ; m_sensor.size() > sR ; sR++ ) {
      if ( isRSensor( sR )  ) {
        double bestZ = 10000.;
        int    bestP ;
        for ( unsigned int sP = firstInStation; m_sensor.size() > sP; sP++ ) {
          if ( !isRSensor( sP )  ) {
            if ( bestZ > fabs( m_sensor[sR]->z() - m_sensor[sP]->z() )) {
              bestZ = fabs( m_sensor[sR]->z() - m_sensor[sP]->z() );
              bestP = sP;
            }
          }
        }
        m_sensor[sR]->associate( bestP );
      }
    }

  }

  //=======================================================================
  // Build the list of radius for the R strips. Only for 1024 strips...
  //=======================================================================
  m_rStrip.clear();
  double radius;
  double center = 0.5;
  int dum;
  
  for ( int iR=0 ; 1024 > iR ; iR++ ) {
    radius = rOfStrip( center, dum );
    m_rStrip.push_back( radius );
  }

  m_zVertex = 0;   // default value.

  return sc;
};

// ============================================================================
// Get the sensor a point is in.
// ============================================================================
int DeVelo::sensorNumber( const HepPoint3D& point ) {
  std::vector<VeloSensor*>::const_iterator it ;
  for ( it = m_sensor.begin() ; m_sensor.end() != it; it++ ) {
    if ( 0.250 * mm > fabs( point.z() - (*it)->z() ) ) {
      return (*it)->number();
    }
  }
  
  return -1;
};

// ============================================================================
// Get the sensor a point is in.
// ============================================================================
int DeVelo::puSensorNumber( const HepPoint3D& point ) {
  std::vector<VeloSensor*>::const_iterator it ;
  for ( it = m_puSensor.begin() ; m_puSensor.end() != it; it++ ) {
    if ( 0.250 * mm > fabs( point.z() - (*it)->z() ) ) {
      return (*it)->number();
    }
  }
  return -1;
};

//=============================================================================
// Returns the strip number for the specified sensor
//=============================================================================
double DeVelo::stripNumber( unsigned int sensorNumber, 
                            const HepPoint3D& point, 
                            double& pitch ) {
  int type;
  if ( m_sensor.size() <= sensorNumber ) {
    return -1;
  }
  type = m_sensor[sensorNumber]->type();
  return stripNumberByType( type, point, pitch ) ;
}

   
//=============================================================================
// Returns the strip number for the specified sensor
//=============================================================================
double DeVelo::puStripNumber( unsigned int sensorNumber, 
                              const HepPoint3D& point, 
                              double& pitch ) {
  int type;
  if ( m_puSensor.size() <= sensorNumber ) {
    return -1;
  }
  type = m_puSensor[sensorNumber]->type();
  return stripNumberByType( type, point, pitch ) ;
  
}
//=============================================================================
// Returns the strip number for the specified sensor
//=============================================================================
double DeVelo::stripNumberByType( int type,
                                  const HepPoint3D& point, 
                                  double& pitch ) {
  double strip = -1.;
  pitch  = -1.;

  double radius = point.perp();
  double phi    = point.phi();

  // Rotate the point if in the left part. Is in +- m_halfAngle

  if ( 0 == (type%2) ) {
    if ( 0 < phi ) {
      phi = phi - pi;
    } else {
      phi = phi + pi;
    }
  }
  
  if ( 1 >= type ) {

    // ** This is an R sensor.

    if ( -m_halfAngle < phi ) {
      int zone  = (int) ( ( phi + m_halfAngle ) / m_quarterAngle );
      if ( 4 > zone ) {
        if ( (m_innerRadius < radius ) && ( m_outerRadius > radius ) ) {
          if ( m_fixPitchRadius > radius ) {
            strip = ( radius - m_innerRadius ) / m_innerPitch;
            pitch = m_innerPitch;
          } else {
            strip = log( 1. + m_pitchSlope * (radius - m_fixPitchRadius) / 
                         m_innerPitch ) / m_logPitchStep + m_nbRFixedPitch;
            pitch = m_innerPitch +  m_pitchSlope * (radius - m_fixPitchRadius);
          }
          if ( m_nbRInner < strip ) {
            strip += m_nbRInner;            // get space for second inner part
          } else {
            if ( 1 == (zone%2) ) { 
              strip += m_nbRInner;          // second half of inner strips
            }
          }
          if ( 1 < zone ) {
            strip += 1024.;           // second half of the sensor
          }
        }
      }
    }
    return strip;
  }
  // ** This is a phi sensor. Can symmetrized to handle th eother stereo...

  if ( 3 < type ) {
    phi = - phi;
  }
  
  phi += halfpi;   // phi is now in a range close to 0-180 degree

  if ( (m_innerRadius < radius ) && ( m_outerRadius > radius ) ) {
    if ( m_phiBoundRadius > radius ) {
      phi   = phi - ( m_phiInnerTilt - asin( m_innerTiltRadius / radius ) );
      strip = phi / m_phiPitchInner;
      pitch = m_phiPitchInner * radius;
      if ( 0 > strip) {
        strip = 2048 + m_nbPhiInner + strip;
      } else if (m_nbPhiInner < strip) {
        strip = 2048 + strip - m_nbPhiInner;
      }
    } else {
      phi   = phi - ( m_phiOuterTilt - asin( m_outerTiltRadius / radius ) );
      strip = phi / m_phiPitchOuter + m_nbPhiInner;
      pitch = m_phiPitchOuter * radius;
      if ( m_nbPhiInner > strip) {
        strip = 4096 - m_nbPhiInner + strip;
      } else if (2048 < strip ) {
        strip = m_nbPhiInner + strip;
      }
    }
  }
  return strip;
}

//=============================================================================
// Returns the (local) radius of the specified strip, and also its zone.
//=============================================================================
double DeVelo::rOfStrip( double strip, int& phiZone ) {
  double localStrip = strip;
  phiZone = 0;
  if ( 1024. <= localStrip ) {
    localStrip -= 1024.;
    phiZone = 3;
  }
  
  if ( m_nbRInner <= localStrip ) {     // two zones in the central part
    localStrip -= (int)m_nbRInner;
    phiZone += 1;
    if  ( m_nbRInner <= localStrip ) {
      phiZone += 1;
    }
  }
  double localR;
  if ( m_nbRFixedPitch > localStrip ) {
    localR = m_innerRadius + localStrip * m_innerPitch;
  } else {
    double dStrip = localStrip-m_nbRFixedPitch;
    localR = m_fixPitchRadius + m_innerPitch * 
      ( exp(dStrip * m_logPitchStep) - 1. ) / m_pitchSlope;
  }
  return  localR;
}
  
//=========================================================================
//  Return true if the two phiZones are matching. This depends on the
//  R sensor geometry
//=========================================================================
bool DeVelo::matchingZones ( int zone1, int zone2 ) {
  switch ( zone1 ){
  case 0 : 
    return ( (0 == zone2) || (2 == zone2) );
  case 1 : 
    return ( (1 == zone2) || (2 == zone2) );
  case 2 : 
    return (3 >  zone2) ;
  case 3 : 
    return ( (3 == zone2) || (5 == zone2) );
  case 4 : 
    return ( (4 == zone2) || (5 == zone2) );
  case 5 : 
    return ( 2 < zone2) ;
  }
  return false;
}
//=========================================================================
//  
//=========================================================================
double DeVelo::phiOfStrip ( double strip, double radius, int sensorNb ) {

  // This version codes unphysical strips as SICBMC generates hits in semi-
  // circular sensor, not taking into accound the real Phi sensor shape. The 
  // non-official strips are coded as 2048 plus the strip number of the 
  // sensor which covers this region in the next half station.

  double stripLocal = strip;
  if ( 2048. <= strip ) {
    stripLocal -= 2048.;
  }
  
  double phiLocal;
  if ( stripLocal < m_nbPhiInner ) {
    phiLocal = (stripLocal * m_phiPitchInner) + 
               m_phiInnerTilt - asin( m_innerTiltRadius / radius );
    if ( 2048. <= strip ) {  
      phiLocal = phiLocal-2*m_halfAngle; 
    }
  } else {
    phiLocal = ((stripLocal-m_nbPhiInner) * m_phiPitchOuter) + 
               m_phiOuterTilt - asin( m_outerTiltRadius / radius );
    if ( 2048. <= strip ) {  
      phiLocal = phiLocal-2*m_halfAngle; 
    }
  }
  phiLocal -= halfpi;

  if ( 3 < ( m_sensor[sensorNb]->type() ) ) {
    phiLocal = -phiLocal;
  }
  return phiLocal;
}
//=============================================================================
// Returns the strip number for the specified sensor
//=============================================================================
bool DeVelo::getSpacePoint( unsigned int RSensorNumber, 
                            double       RStripNumber,
                            unsigned int PhiSensorNumber, 
                            double       PhiStripNumber,
                            HepPoint3D& point, 
                            double&  rPitch,
                            double&  phiPitch ) {
  point.set( 0., 0., 0. );
  rPitch   = 0.;
  phiPitch = 0.;

  // check that the sensor number are valid

  if ( ( m_sensor.size() <= RSensorNumber   ) ||
       ( m_sensor.size() <= PhiSensorNumber )    ) {
    return false;
  }

  // check that the sensor types are valid
  int phiType =  m_sensor[PhiSensorNumber]->type();
  
  if ( ( 1 < m_sensor[RSensorNumber]->type() ) || ( 2 > phiType ) ) {
    return false;
  }

  // Nearby sensor...

  double zR   = m_sensor[RSensorNumber]->z();
  double zPhi = m_sensor[PhiSensorNumber]->z();
  if ( fabs(zR - zPhi) > 50. ) {
    return false;
  }

  bool status = true;
  
  // Compute R from strip. Could be tabulated for performance, if needed.
  int    phiZone;
  double localR = rOfStrip( RStripNumber, phiZone );
  
  // check some matching in the detector region.
  double rAtPhi = localR * ( zPhi - m_zVertex ) / ( zR - m_zVertex );
  double tolPhiBoundary = 5*m_innerPitch;

  // If the local computed slope is too big, keep R constant...
  if ( 0.4 < localR / fabs( zR - m_zVertex ) ) { 
    rAtPhi = localR; 
    tolPhiBoundary = .5 * fabs( zPhi-zR );   // Don't know the angle -> 45 deg
  }

  double myPhiStrip = PhiStripNumber;
  if ( 2048. <= myPhiStrip ) myPhiStrip -= 2048.;
 
  // Coherence in the Phi detector region, with some tolerance of 1 strip

  if ( m_phiBoundRadius + tolPhiBoundary < rAtPhi ) {
    if ( myPhiStrip < m_nbPhiInner ) {
      return false;
    }
  } else if ( m_phiBoundRadius - tolPhiBoundary > rAtPhi ) {
    if ( myPhiStrip > m_nbPhiInner ) {
      return false;
    }
  }

  double phiLocal = phiOfStrip( PhiStripNumber, rAtPhi, PhiSensorNumber );
  double phiMin = phiLocal + 0.02;    // Tolerance for tests
  double phiMax = phiLocal - 0.02;    // tolerance for tests
  
  // phi is in the +- m_halfAngle range. Test for R compatibility

  if ( 0 == phiZone  ) {
    if ( (-m_halfAngle > phiMin) || ( -m_quarterAngle < phiMax ) ){
      return false;
    }
  } else if ( 1 == phiZone ) {
    if ( (-m_quarterAngle > phiMin) || ( 0 < phiMax )  ) {
      return false;
    }
  } else if ( 2 == phiZone ) {
    if ( (-m_halfAngle > phiMin) || ( 0 < phiMax ) ) {
      return false;
    }
  } else if ( 3 == phiZone ) {
    if ( (0 > phiMin) || (m_quarterAngle < phiMax)  ) {
      return false;
    }
  } else if ( 4 == phiZone ) {
    if ( (m_quarterAngle > phiMin) || (m_halfAngle < phiMax) ) {
      return false;
    }
  } else if ( 5 == phiZone ) {
    if ( (0 > phiMin) || (m_halfAngle < phiMax) ) {
      return false;
    }
  }
  
  if ( 0 == phiType%2 ) {
    phiLocal += pi;
  }

  point.setZ( zR );
  point.setX( localR * cos( phiLocal ) );
  point.setY( localR * sin( phiLocal ) );
  // Here we should convert a local point to a global point, using the 
  // toGlobal method of the R sensor detector element... Not now !


  // Compute the pitches. 

  rPitch   = this->rPitch( localR );
  phiPitch = this->phiPitch( localR );
  
  return status;
}

//=========================================================================
//  Compute the distance of the origin to the strip
//=========================================================================
double DeVelo::originToPhiDistance ( double strip, int sensorNb ) {

  double stripLocal = strip;
  if ( 2048. <= strip ) {
    stripLocal -= 2048.;
  }
  
  double distance;
  if ( stripLocal < m_nbPhiInner ) {
    distance = m_innerTiltRadius ;
  } else {
    distance = m_outerTiltRadius ;
  }

  int phiType =  m_sensor[sensorNb]->type();
  if ( 4 > phiType    ) {  distance = -distance; }
  return distance;
}

//=========================================================================
//  Returns the angle of the strip with the x axis
//=========================================================================
double DeVelo::phiDirectionOfStrip ( double strip, int sensorNb ) {

  double stripLocal = strip;
  if ( 2048. <= strip ) {
    stripLocal -= 2048.;
  }
  
  double phiLocal;
  if ( stripLocal < m_nbPhiInner ) {
    phiLocal = (stripLocal * m_phiPitchInner) + m_phiInnerTilt;
    if ( 2048. <= strip ) {  
      phiLocal = phiLocal-2*m_halfAngle; 
    }
  } else {
    phiLocal = ((stripLocal-m_nbPhiInner) * m_phiPitchOuter) + m_phiOuterTilt;
    if ( 2048. <= strip ) {  
      phiLocal = phiLocal-2*m_halfAngle; 
    }
  }
  phiLocal -= halfpi;

  int phiType =  m_sensor[sensorNb]->type();

  if ( 3 <  phiType   ) { phiLocal = -phiLocal; }
  if ( 0 == phiType%2 ) { phiLocal += pi;       }
  return phiLocal;
}
