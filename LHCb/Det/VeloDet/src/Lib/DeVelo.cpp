// $Id: DeVelo.cpp,v 1.19 2002-06-24 08:17:56 ocallot Exp $
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
#include "GaudiKernel/Bootstrap.h"

#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/IJobOptionsSvc.h"
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

  //========== Trick from Pere to set the output level =================
  PropertyMgr* pmgr = new PropertyMgr();
  int outputLevel = -1;
  pmgr->declareProperty( "OutputLevel", outputLevel );
  IJobOptionsSvc* jobSvc;
  ISvcLocator* svcLoc = Gaudi::svcLocator();
  StatusCode sc = svcLoc->service("JobOptionsSvc", jobSvc );
  jobSvc->setMyProperties( "DeVelo", pmgr );
  if ( -1 != outputLevel ) msgSvc()->setOutputLevel( "DeVelo", outputLevel );
  //====================================================================

  MsgStream loging( msgSvc(), "DeVelo" );

  sc = DetectorElement::initialize();
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

  it = std::find( pars.begin() , pars.end () , std::string("SiThick") );
  if( pars.end() != it ) {
    m_siliconThickness =  userParameterAsDouble( *it ) ;
    pars.erase( it );
  }

  if( !pars.empty() ) {
    // some "extra" parameters.
    // should be an error??
  }
  ///

  loging << MSG::DEBUG    //==== Shoudl be changed for production !!!
         << "Velo : Radius from " << m_innerRadius/mm 
         << " to " << m_outerRadius/mm 
         << " thick " << m_siliconThickness / micrometer << " microns"
         << endreq;
 
  // Auxilliary variables for R strip computation

  m_pitchSlope = (m_outerPitch - m_innerPitch ) /  
                 (m_outerRadius - m_fixPitchRadius);
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
  m_phiPitchOuter   = 2.* m_halfAngle / (nbStrips() - m_nbPhiInner);

  double phi = m_phiOuterTilt - asin( m_outerTiltRadius / m_outerRadius );

  loging << "Phi (degree) inner "    << m_phiOrigin / degree
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
  
  //  IDetectorElement* stations = *childBegin();
  IDetectorElement* stations = this;
  unsigned int firstInStation = 0 ;

  loging << "PuVeto " ;

  for( IDetectorElement::IDEContainer::iterator child = stations->childBegin();
       stations->childEnd() != child ; ++child ) {
    number = child - stations->childBegin();

    if ( 0 == number%2) { firstInStation = m_sensor.size(); }

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
          if ( typeStr == "VetoL" ) {
            type = 1;
          } else if ( typeStr == "VetoR" ) {
            type = 0;
          } else {
            if ( inVeto ) {
              inVeto = false;
              loging << endreq;
              index  = 0;
            }
            if ( typeStr == "RRigh" ) {
              type = 0;
            } else if ( typeStr == "RLeft" ) {
              type = 1;
            } else if ( typeStr == "PhiUR" ) {
              type = 2;
            } else if ( typeStr == "PhiUL" ) {
              type = 3;
            } else if ( typeStr == "PhiDR" ) {
              type = 4;
            } else if ( typeStr == "PhiDL" ) {
              type = 5;
            }
          }
        }

        if ( 0 > type ) {
          loging << MSG::ERROR << "Unknown type " << typeStr
                 << " for sensor " << index
                 << " at z = " << z
                 << endreq;
          sc = StatusCode::FAILURE;
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

        sprintf( line, "S%2d ", sensor->number() );
        loging << line << typeStr;
        sprintf( line, "(%d) z=%7.2f ", sensor->type(), sensor->z() );
        loging << line;

      } else {
        loging << " no geometry !" << endreq;
      }
    }

    if ( !inVeto && (0 != number%2) ) {

      // build the list of Phi associated to R, in principle both PHI of the 
      // station due to the dog-leg shape.

      for ( unsigned int sR = firstInStation ; m_sensor.size() > sR ; sR++ ) {
        if ( isRSensor( sR )  ) {
          loging << " R:" << sR << "-Phi";
          for ( unsigned int sP = firstInStation; m_sensor.size() > sP; sP++) {
            if ( !isRSensor( sP )  ) {
              m_sensor[sR]->associate( sP );
              loging << sP << " ";
            }
          }
        }
      }
      loging << endreq;
    }

  }

  // Build the list of radius for the R strips. In fact the boundaries.

  m_rStrip.clear();
  
  double radius = m_innerRadius;
  m_rStrip.push_back( radius );
  
  while( m_outerRadius > radius ){
    radius += rPitch( radius );
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
          int ifst = 0;
          int ilst = m_rStrip.size()-1;
          while ( 1 < ilst-ifst ) {
            int imed = (ifst+ilst)/2;
            if ( m_rStrip[imed] < radius ) {
              ifst = imed;
            } else {
              ilst = imed;
            }
          }
          pitch = m_rStrip[ilst]- m_rStrip[ifst];
          strip = ifst + ( radius - m_rStrip[ifst] ) / pitch;
          
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
  // ** This is a phi sensor. Use symmetry to handle the second stereo...

  if ( 3 < type ) { phi = - phi; }
  
  phi += halfpi;   // phi is now in a range close to 0-180 degree

  if ( (m_innerRadius < radius ) && ( m_outerRadius > radius ) ) {
    phi = phi - phiOffset( radius );
    if ( m_phiBoundRadius > radius ) {
      strip = phi / m_phiPitchInner;
      pitch = m_phiPitchInner * radius;
      if ( 0 > strip) {
        strip = -2.; 
      } else if (m_nbPhiInner < strip) {
        strip = -2.; 
      }
    } else {
      strip = phi / m_phiPitchOuter + m_nbPhiInner;
      pitch = m_phiPitchOuter * radius;
      if ( m_nbPhiInner > strip) {
        strip = -2.; 
      } else if ( nbStrips() < strip ) {
        strip = -2.; 
      }
    }
  }
  return strip;
}

//=============================================================================
// Returns the (local) radius of the specified strip, and also its zone.
//=============================================================================
double DeVelo::rOfStrip( double strip, int& rZone ) {
  double localStrip = strip;
  rZone = 0;
  if ( 1024. <= localStrip ) {
    localStrip -= 1024.;
    rZone = 3;
  }
  
  if ( m_nbRInner <= localStrip ) {     // two zones in the central part
    localStrip = localStrip - m_nbRInner;
    rZone += 1;
    if  ( m_nbRInner <= localStrip ) {
      rZone += 1;
    }
  }

  int    rBin  = (int) localStrip;
  double delta = localStrip - (double)rBin;
  return  m_rStrip[rBin] * (1-delta) + delta * m_rStrip[rBin+1];
}

//=========================================================================
//  Return true if the two rZones are matching. This depends on the
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

  double phiLocal;

  if ( strip < m_nbPhiInner ) {
    phiLocal = (strip * m_phiPitchInner) + phiOffset( radius );
  } else {
    phiLocal = ((strip-m_nbPhiInner) * m_phiPitchOuter) + phiOffset( radius );
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
  int rType   = m_sensor[RSensorNumber]->type();
  int phiType = m_sensor[PhiSensorNumber]->type();
  
  if ( ( 1 < rType ) || ( 2 > phiType ) ) {
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
  int    rZone;
  double localR = rOfStrip( RStripNumber, rZone );
  
  // check some matching in the detector region.
  double rAtPhi = localR * ( zPhi - m_zVertex ) / ( zR - m_zVertex );
  double tolPhiBoundary = 5. * m_innerPitch;

  // If the local computed slope is too big, keep R constant...
  if ( 0.4 < localR / fabs( zR - m_zVertex ) ) { 
    rAtPhi = localR; 
    tolPhiBoundary = .5 * fabs( zPhi-zR );   // Don't know the angle -> 45 deg
  }

  if ( m_innerRadius > rAtPhi ) { return false; }
  if ( m_outerRadius < rAtPhi ) { return false; }

  // Coherence in the Phi detector region, with some tolerance

  if ( m_phiBoundRadius + tolPhiBoundary < rAtPhi ) {
    if ( PhiStripNumber < m_nbPhiInner ) {
      return false;
    }
  } else if ( m_phiBoundRadius - tolPhiBoundary > rAtPhi ) {
    if ( PhiStripNumber > m_nbPhiInner ) {
      return false;
    }
  }

  double phiLocal = phiOfStrip( PhiStripNumber, rAtPhi, PhiSensorNumber );

  double phiMin = phiLocal + 0.02;    // Tolerance for tests
  double phiMax = phiLocal - 0.02;    // tolerance for tests

  // For unusual pairing, rotate Phi ranges to match the R zone...
  if ( ( phiType + rType )%2 != 0 ) {
    if ( phiLocal < 0 ) {
      phiMin += pi;
      phiMax += pi;
    } else {
      phiMin -= pi;
      phiMax -= pi;
    }
  }
  
  // phi is in the +- m_halfAngle range. Test for R compatibility

  if ( 0 == rZone  ) {
    if ( (-m_halfAngle > phiMin) || ( -m_quarterAngle < phiMax ) ){
      return false;
    }
  } else if ( 1 == rZone ) {
    if ( (-m_quarterAngle > phiMin) || ( 0 < phiMax )  ) {
      return false;
    }
  } else if ( 2 == rZone ) {
    if ( (-m_halfAngle > phiMin) || ( 0 < phiMax ) ) {
      return false;
    }
  } else if ( 3 == rZone ) {
    if ( (0 > phiMin) || (m_quarterAngle < phiMax)  ) {
      return false;
    }
  } else if ( 4 == rZone ) {
    if ( (m_quarterAngle > phiMin) || (m_halfAngle < phiMax) ) {
      return false;
    }
  } else if ( 5 == rZone ) {
    if ( (0 > phiMin) || (m_halfAngle < phiMax) ) {
      return false;
    }
  }
  
  //=== put back in the proper space location 
  if ( 0 == phiType%2 ) { phiLocal += pi;  }

  point.set( localR * cos(phiLocal),  localR * sin(phiLocal), zR );

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

  double distance;
  if ( strip < m_nbPhiInner ) {
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

  double phiLocal;
  if ( strip < m_nbPhiInner ) {
    phiLocal = (strip * m_phiPitchInner) + m_phiInnerTilt;
  } else {
    phiLocal = ((strip-m_nbPhiInner) * m_phiPitchOuter) + m_phiOuterTilt;
  }
  phiLocal -= halfpi;

  int phiType =  m_sensor[sensorNb]->type();

  if ( 3 <  phiType   ) { phiLocal = -phiLocal; }
  if ( 0 == phiType%2 ) { phiLocal += pi;       }
  return phiLocal;
}

//=========================================================================
//  Return the 'numAway' channelIDfrom the specified one. 
//=========================================================================
VeloChannelID DeVelo::neighbour ( const VeloChannelID& chan, 
                                  int numAway, 
                                  bool& valid) {
  int strip = chan.strip();
  int newStrip = strip + numAway;
  valid = true;
  
  if ( isRSensor( chan.sensor() ) ) {
    if ( 0 < numAway ) {
      if ( (m_nbRInner >  strip) && 
           (m_nbRInner <= newStrip) ) {
        newStrip += (int)m_nbRInner;
      } else if ( ( 1024 >  strip ) && 
                  ( 1024 <= newStrip ) ) {
        valid = false;
      } else if ( (m_nbRInner+1024 >  strip) && 
                  (m_nbRInner+1024 <= newStrip) ) {
        newStrip += (int)m_nbRInner;
      } else if ( 2048 <= newStrip ) {
        valid = false;
      }
    } else {
      if ( 0 > newStrip ) {
        valid = false;
      } else if ( (m_nbRInner <= strip) && 
                  (m_nbRInner >  newStrip) ) {
        valid = false;
      } else if ( ( 1024 <= strip ) && 
                  ( 1024 >  newStrip ) ) {
        valid = false;
      } else if ( (m_nbRInner+1024 <= strip) && 
                  (m_nbRInner+1024 >  newStrip) ) {
        valid = false;
      }
    }
  } else {
    if ( 0 < numAway ) {
      if ( ( m_nbPhiInner > strip ) && ( m_nbPhiInner <= newStrip ) ) {
        valid = false;
      } else if ( nbStrips() <= newStrip ) {
        valid = false;
      }
    } else {
      if ( 0 > newStrip ) {
        valid = false;
      } else if ( (m_nbPhiInner <= strip) && ( m_nbPhiInner > newStrip ) ) {
        valid = false;
      }
    }
  }
  if ( valid ) {
    return VeloChannelID( chan.sensor(), newStrip );
  }
  return VeloChannelID( chan.sensor(), 0 );
}

//=========================================================================
//  Returns the distance between these two channels.
//=========================================================================
int DeVelo::neighbour ( const VeloChannelID& entryChan,
                        const VeloChannelID& exitChan,
                        bool& valid ) {
  valid = true;
  if ( entryChan.sensor() != exitChan.sensor() ) {
    valid = false;
    return -1;
  }
  int str1 = entryChan.strip();
  int str2 = exitChan.strip();
  int dist = str2 - str1;
  if ( str1 > str2 ) {
    str2 = entryChan.strip();
    str1 = exitChan.strip();
  }
  
  if ( isRSensor( entryChan.sensor() ) ) {
    if ( (1024 > str1) && (1024 <= str2) ) valid = false;
    str1 = str1 % 1024;
    str2 = str2 % 1024;
    if ( (m_nbRInner > str1) && (m_nbRInner <= str2 ) ) {
      if ( 2*m_nbRInner <= str2  ) {
        if ( 0 < dist ) {
          dist -= (int) m_nbRInner;
        } else {
          dist += (int) m_nbRInner;
        }
      } else {
        valid = false;
      }
    }
  } else {
    if ( ( m_nbPhiInner > str1 ) && ( m_nbPhiInner <= str2 ) ) {
      valid = false;
    }
  }
  if ( valid ) return dist;
  return -9999;  
}

//=========================================================================
//  Return an index of the strip.
//=========================================================================
int DeVelo::stripArrayIndex ( int sensorId, int stripId ) {
  if ( m_sensor.size() > (unsigned int) sensorId ) {
    return stripId;
  } else {
    return -1;
  }
}

//=========================================================================
//  
//=========================================================================
int DeVelo::stripNumber ( int sensorId, int stripIndex ) {
  if ( m_sensor.size() > (unsigned int) sensorId ) {
    return stripIndex;
  } else {
    return -1;
  }
}

//=========================================================================
//  
//=========================================================================
VeloChannelID DeVelo::channelID (const HepPoint3D& point, 
                                 double& fraction, 
                                 double& pitch,
                                 bool& valid ) {
  valid = true;
  int sensor    = sensorNumber( point );
  double strip  = stripNumber( sensor, point, pitch );
  if ( 0 <= strip ) {
    int iStrip = (int) strip;
    fraction = strip - (double) iStrip;
    return VeloChannelID( sensor, iStrip );
  } else {
    valid = false;
    return VeloChannelID( 0, 0 );
  }  
}
//=========================================================================
