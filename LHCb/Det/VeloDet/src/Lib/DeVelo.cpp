// $Id: DeVelo.cpp,v 1.1 2001-10-25 06:13:26 ocallot Exp $
//
// ============================================================================
#define  VELODET_DEVELO_CPP 1
// ============================================================================
// from STL
#include <cmath>
#include <algorithm>
// from CLHEP
#include "CLHEP/Units/SystemOfUnits.h"
// from Gaudi
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/TransientStore.h"
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

  StatusCode sc = DetectorElement::initialize();
  ///
  if( sc.isFailure() ) { return sc ; }
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

  MsgStream loging( msgSvc(), "DeVelo" );
  loging << MSG::INFO << "Velo : Radius from " << m_innerRadius/mm 
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
  m_phiInnerTilt    = m_innerTilt - m_phiOrigin;
  m_phiAtBoundary   = m_phiInnerTilt - 
                      asin( m_innerTiltRadius / m_phiBoundRadius );
  m_phiOuterTilt    = m_phiAtBoundary + m_outerTilt;
  m_phiPitchInner   = 2.* m_halfAngle / m_nbPhiInner;
  m_phiPitchOuter   = 2.* m_halfAngle / (2048. - m_nbPhiInner);

  double phi = m_phiOuterTilt + asin( m_outerTiltRadius / m_outerRadius );
  loging << MSG::INFO
      << "Phi inner "    << - m_phiOrigin
      << " at boundary " << m_phiAtBoundary 
      << " and outside " << phi
      << endreq;

 
/**  
 * Get the stations (wafers) with their Z, type and orientation
 */
  HepPoint3D pointLocal( 0, 0, 0);
  HepPoint3D pointGlobal( 0, 0, 0);
  int number;
  int index = 0;
  
  IDetectorElement* stations = *childBegin();

  for( IDetectorElement::IDEContainer::iterator child = stations->childBegin();
       stations->childEnd() != child ; ++child ) {
    number = child - stations->childBegin();
    loging << MSG::INFO << "wafer " << number ;
    
    IGeometryInfo* geoData = (*child)->geometry();
    if ( 0 != geoData ) {
      pointGlobal = geoData->toGlobal( pointLocal );
      loging << " at Z = " << pointGlobal.z() << endreq;
      // *** Create the wafers, 4 per 'station'
      double z =  pointGlobal.z();
      
      for ( int j=0 ; 2 > j ; j++ ) {
        VeloWafer* phiWafer = new VeloWafer();
        phiWafer->setNumber( index++ );
        if ( 0 == number%2 ) {
          phiWafer->setType( 2*j+1 );    // Negative X
        } else {
          phiWafer->setType( 2*j+2 );    // Positive X
        }
        phiWafer->setZ( z - 0.65 );      // R-Phi separation = 1.3 mm
        m_wafer.push_back( phiWafer );
        
        VeloWafer* radWafer = new VeloWafer();
        radWafer->setNumber( index++ );
        radWafer->setType( 0 );
        radWafer->setZ( z + 0.65 );
        m_wafer.push_back( radWafer );
        z = z + 15.0;                    // Right-Left separation 15 mm
      }
    } else {
      loging << " no geometry !" << endreq;
    }
  }
  
  return sc;
};

// ============================================================================
// Get the wafer a point is in.
// ============================================================================
int DeVelo::waferNumber( HepPoint3D& point ) {
  for ( std::vector<VeloWafer*>::const_iterator it = m_wafer.begin() ;
        m_wafer.end() != it; it++ ) {
    if ( 0.250 * mm > fabs( point.z() - (*it)->z() ) ) {
      return (*it)->number();
    }
  }
  return -1;
};

//=============================================================================
// Returns the strip number for the specified wafer
//=============================================================================
double DeVelo::stripNumber( unsigned int waferNumber, 
                            HepPoint3D& point, 
                            double& pitch ) {
  double strip = -1.;
  pitch  = -1.;
  if ( m_wafer.size() <= waferNumber ) {
    return strip;
  }
  double radius = point.perp();
  double phi    = point.phi();
  
  if ( 0 == m_wafer[waferNumber]->type() ) {

    // ** This is an R wafer.    

    if ( halfpi < phi ) {
      phi -= pi;
    } else if ( -halfpi > phi ) {
      phi += pi;
    }
    int zone     = (int) ( (phi+halfpi) / m_quarterAngle );
    
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
        if ( 0 != (zone % 2) ) { 
          strip += m_nbRInner;          // second half of inner strips
        }
      }
      if ( 1 < zone ) {
        strip += 1024.;           // second half of the wafer
      }
    }
    return strip;
  }
  // ** This is a phi wafer. Can be rotated or symmetrized
  if ( 2 >= m_wafer[waferNumber]->type()  ) {
    if ( 0 < phi ) {
      phi = phi - pi;
    } else {
      phi = phi + pi;
    }
  }
  if ( 0 == m_wafer[waferNumber]->type() % 2 ) {
    phi = - phi;
  }
  
  phi += halfpi;   // phi is now in a range close to 0-180 degree

  if ( (m_innerRadius < radius ) && ( m_outerRadius > radius ) ) {
    if ( m_phiBoundRadius > radius ) {
      phi  -=  m_phiInnerTilt - asin( m_innerTiltRadius / radius );
      strip = phi / m_phiPitchInner;
      pitch = m_phiPitchInner * radius;
      if ( (0 > strip) || (m_nbPhiInner < strip) ) {
        strip = -1.;
      }
    } else {
      phi  -=  m_phiOuterTilt - asin( m_outerTiltRadius / radius );
      strip = phi / m_phiPitchOuter + m_nbPhiInner;
      pitch = m_phiPitchOuter * radius;
      if ( (m_nbPhiInner > strip) || (2048. < strip ) ) {
        strip = -1.;
      }
    }
  }
  return strip;
}

//=============================================================================
// Returns the strip number for the specified wafer
//=============================================================================
bool DeVelo::getSpacePoint( unsigned int RWaferNumber, 
                            double       RStripNumber,
                            unsigned int PhiWaferNumber, 
                            double       PhiStripNumber,
                            HepPoint3D& point, 
                            double&  rPitch,
                            double&  phiPitch ) {

  // check that the wafer number are valid

  if ( ( m_wafer.size() <= RWaferNumber   ) ||
       ( m_wafer.size() <= PhiWaferNumber )    ) {
    return false;
  }

  // check that the wafer types are valid
  int phiType =  m_wafer[PhiWaferNumber]->type();
  
  if ( ( 0 != m_wafer[RWaferNumber]->type() ) || ( 0 == phiType ) ) {
    return false;
  }

  // Nearby wafer...

  double zR   = m_wafer[RWaferNumber]->z();
  double zPhi = m_wafer[PhiWaferNumber]->z();
  if ( fabs(zR - zPhi) > 50. ) {
    return false;
  }
  
  // Compute R from strip. Could be tabulated for perfromance, if needed.
  double localR;
  int    phiZone;
  double localStrip;
  
  localStrip = RStripNumber;
  phiZone = 0;
  if ( 1024. <= localStrip ) {
    localStrip -= 1024.;
    phiZone = 3;
  }
  
  if ( m_nbRInner < localStrip ) {
    localStrip -= (int)m_nbRInner;
    phiZone += 1;
    if  ( m_nbRInner < localStrip ) {
      phiZone += 1;
    }
  }
  
  if ( m_nbRFixedPitch > localStrip ) {
    localR = m_innerRadius + localStrip * m_innerPitch;
  } else {
    double dStrip = localStrip-m_nbRFixedPitch;
    localR = m_fixPitchRadius + m_innerPitch * 
      ( exp(dStrip * m_logPitchStep) - 1. ) / m_pitchSlope;
  }
  
  // check some matching in the detector region.
  double zRef = 0.;  // Should be a 'nominal' vertex.
  
  double rAtPhi = localR * ( zPhi - zRef ) / ( zR -zRef );

  // Coherence in the Phi detector region, with some tolerance of 1 strip
  if ( m_phiBoundRadius < rAtPhi - m_innerPitch ) {
    if ( PhiStripNumber < m_nbPhiInner ) {
      return false;
    }
  } else if ( m_phiBoundRadius > rAtPhi + m_innerPitch ) {
    if ( PhiStripNumber > m_nbPhiInner ) {
      return false;
    }
  }
  double phiLocal;
  
  if ( PhiStripNumber < m_nbPhiInner ) {
    phiLocal = (PhiStripNumber * m_phiPitchInner) + 
               m_phiInnerTilt - asin( m_innerTiltRadius / rAtPhi );
  } else {
    phiLocal = ((PhiStripNumber-m_nbPhiInner) * m_phiPitchOuter) + 
               m_phiOuterTilt - asin( m_outerTiltRadius / rAtPhi );
  }
  phiLocal -= halfpi;
  if ( 0 == phiType%2 ) {
    phiLocal = -phiLocal;
  }

  // phi is in the -pi/2, pi/2 range. Test for R compatibility

  if ( 0 == phiZone  ) {
    if ( (-m_halfAngle > phiLocal) || ( -m_quarterAngle < phiLocal ) ){
      return false;
    }
  } else if ( 1 == phiZone ) {
    if ( (-m_quarterAngle > phiLocal) || ( 0 < phiLocal )  ) {
      return false;
    }
  } else if ( 2 == phiZone ) {
    if ( (-m_halfAngle > phiLocal) || ( 0 < phiLocal ) ) {
      return false;
    }
  } else if ( 3 == phiZone ) {
    if ( (0 > phiLocal) || (m_quarterAngle < phiLocal)  ) {
      return false;
    }
  } else if ( 4 == phiZone ) {
    if ( (m_quarterAngle > phiLocal) || (m_halfAngle < phiLocal) ) {
      return false;
    }
  } else if ( 5 == phiZone ) {
    if ( (0 > phiLocal) || (m_halfAngle < phiLocal) ) {
      return false;
    }
  }
  
  if ( 2 >= phiType ) {
    if ( 0 < phiLocal ) {
      phiLocal -= pi;
    } else {
      phiLocal += pi;
    }
  }

  point.setZ( zR );
  point.setX( localR * cos( phiLocal ) );
  point.setY( localR * sin( phiLocal ) );
  // Here we should convert a local point to a global point, using the 
  // toGlobal method of the R wafer detector element... Not now !


  // Compute the pitch. 
  if ( m_fixPitchRadius > localR ) {
    rPitch = m_innerPitch;
  } else {
    rPitch = m_innerPitch +  m_pitchSlope * (localR - m_fixPitchRadius);
  }
  if ( m_phiBoundRadius > localR ) {
    phiPitch = m_phiPitchInner * localR;
  } else {
    phiPitch = m_phiPitchOuter * localR;
  }

  return true;
}

