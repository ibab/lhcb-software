// $Id: MagneticFieldSvc.cpp,v 1.28 2008-05-13 11:16:02 smenzeme Exp $

// Include files
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SystemOfUnits.h"
#include <iostream>

#include "MagneticFieldSvc.h"

#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/Point3DTypes.h"

#include <cstdlib>
#include <fstream>

/** @file MagneticFieldSvc.cpp
 *  Implementation of MagneticFieldSvc class
 *
 *  @author Edgar De Oliveira
 *  @date   2002-05-21
 *  Updated and further developped - Adlene Hicheur
 */

DECLARE_SERVICE_FACTORY( MagneticFieldSvc );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MagneticFieldSvc::MagneticFieldSvc( const std::string& name, 
            ISvcLocator* svc ) : Service( name, svc )
{


  m_constFieldVector.push_back( 0. );
  m_constFieldVector.push_back( 0. );
  m_constFieldVector.push_back( 0. );
  
  declareProperty( "FieldMapFile",        m_filename = "FieldMapFileNotSet" );
  declareProperty( "FieldMapFileQ1",m_qfilename[0] = "FieldMapFileQ1NotSet");
  declareProperty( "FieldMapFileQ2",m_qfilename[1] = "FieldMapFileQ2NotSet");
  declareProperty( "FieldMapFileQ3",m_qfilename[2] = "FieldMapFileQ3NotSet");
  declareProperty( "FieldMapFileQ4",m_qfilename[3] = "FieldMapFileQ4NotSet");
  declareProperty( "UseConstantField",    m_useConstField = false );
  declareProperty( "ConstantFieldVector", m_constFieldVector );
  declareProperty( "ScaleFactor",         m_scaleFactor = 1. );
  declareProperty( "UseRealMap", m_useRealMap = false);
  //magnet current in kA, the default is the nominal value
  declareProperty( "MagnetCurrent", m_current = 5.85);
  declareProperty( "CondPath", m_condpath = "CondPathNotSet");
  
  
}
//=============================================================================
// Standard destructor
//=============================================================================
MagneticFieldSvc::~MagneticFieldSvc()
{
}

//=============================================================================
// Initialize
//=============================================================================
StatusCode MagneticFieldSvc::initialize()
{
  MsgStream log(msgSvc(), name());
  StatusCode status = Service::initialize();
  if( status.isFailure() ) return status;
  
  if( m_useConstField ) {
    log << MSG::WARNING << "using constant magnetic field with field vector "
        << m_constFieldVector << " (Tesla)" << endmsg;
    return StatusCode::SUCCESS;
  }
  //retrieve current from conditions, then compute the scale factor:
  // m_scaleFactor = retrieved_current/m_current;
  
  if( fabs(m_scaleFactor-1.) > 1e-6 ) {
    log << MSG::WARNING << "Field map will be scaled by a factor = "
        << m_scaleFactor << endmsg;
  }
  
  if (m_useRealMap) { 
    log << MSG::INFO << "*** Real Field parameterization will be used *** " << endreq;
    status = parseRealFiles();

  }
  else {

   status = parseFile(); 
  }
  

  if ( status.isSuccess() ) {
      log << MSG::DEBUG << "Magnetic field parsed successfully" << endreq;

    for (int iC = 0; iC< 3; ++iC ){
      m_min_FL[iC] = 0.0;
      m_max_FL[iC] = m_min_FL[iC]+( m_Nxyz[iC]-1 )* m_Dxyz[iC];
    } // iC
    
    return status;
  }
  else {
    log << MSG::DEBUG << "Magnetic field parse failed" << endreq;
    return StatusCode::FAILURE;
  }
}

//=============================================================================
// QueryInterface
//=============================================================================
StatusCode MagneticFieldSvc::queryInterface( const InterfaceID& riid, 
                                             void** ppvInterface      ) 
{
  StatusCode sc = StatusCode::FAILURE;
  if ( ppvInterface ) {
    *ppvInterface = 0;
    
    if ( riid == IID_IMagneticFieldSvc ) {
      *ppvInterface = static_cast<IMagneticFieldSvc*>(this);
      sc = StatusCode::SUCCESS;
      addRef();
    }
    else
      sc = Service::queryInterface( riid, ppvInterface );    
  }
  return sc;
}

// ---------------------------------------------------------------------------
// Routine: parseFile
// Purpose: Parses the file and fill a magnetic field vector
// ---------------------------------------------------------------------------
StatusCode MagneticFieldSvc::parseFile() {
  StatusCode sc = StatusCode::FAILURE;
  
  MsgStream log( msgSvc(), name() );
  char line[ 255 ];
  std::ifstream infile( m_filename.c_str() );
  
  if ( infile ) {
	  sc = StatusCode::SUCCESS;
    log << MSG::INFO << "Opened magnetic field file : " << m_filename
	      << endreq;
    
    // Skip the header till PARAMETER
    do{
	    infile.getline( line, 255 );
	  } while( line[0] != 'P' );
    
    // Get the PARAMETER
    std::string sPar[2];
    char* token = strtok( line, " " );
    int ip = 0;
    do{
      if ( token ) { sPar[ip] = token; token = strtok( NULL, " " );} 
      else continue;
      ip++;
    } while ( token != NULL );
    long int npar = atoi( sPar[1].c_str() );

    // Skip the header till GEOMETRY
    do{
	    infile.getline( line, 255 );
	  } while( line[0] != 'G' );
    
    // Skip any comment before GEOMETRY 
    do{
	    infile.getline( line, 255 );
	  } while( line[0] != '#' );
    
    // Get the GEOMETRY
    infile.getline( line, 255 );
    std::string sGeom[7];
    token = strtok( line, " " );
    int ig = 0;
    do{
      if ( token ) { sGeom[ig] = token; token = strtok( NULL, " " );} 
      else continue; 
      ig++;  
    } while (token != NULL);

    // Grid dimensions are given in cm in CDF file. Convert to CLHEP units
    m_Dxyz[0] = atof( sGeom[0].c_str() ) * Gaudi::Units::cm;
    m_Dxyz[1] = atof( sGeom[1].c_str() ) * Gaudi::Units::cm;
    m_Dxyz[2] = atof( sGeom[2].c_str() ) * Gaudi::Units::cm;
    m_Nxyz[0] = atoi( sGeom[3].c_str() );
    m_Nxyz[1] = atoi( sGeom[4].c_str() );
    m_Nxyz[2] = atoi( sGeom[5].c_str() );
    m_zOffSet = atof( sGeom[6].c_str() ) * Gaudi::Units::cm;
    
    m_Q.reserve(npar - 7);
    // Number of lines with data to be read
    long int nlines = ( npar - 7 ) / 3;
    
    // Check number of lines with data read in the loop
    long int ncheck = 0;
    
    // Skip comments and fill a vector of magnetic components for the
    // x, y and z positions given in GEOMETRY
    
   	while( infile ) {
      // parse each line of the file, 
      // comment lines begin with '#' in the cdf file
	    infile.getline( line, 255 );
	    if ( line[0] == '#' ) continue;
	    std::string sFx, sFy, sFz; 
	    char* token = strtok( line, " " );
	    if ( token ) { sFx = token; token = strtok( NULL, " " );} else continue;
  	  if ( token ) { sFy = token; token = strtok( NULL, " " );} else continue;
  	  if ( token ) { sFz = token; token = strtok( NULL, " " );} else continue;
	    if ( token != NULL ) continue;
      
      // Field values are given in gauss in CDF file. Convert to CLHEP units
      double fx = atof( sFx.c_str() ) * Gaudi::Units::gauss * m_scaleFactor;
      double fy = atof( sFy.c_str() ) * Gaudi::Units::gauss * m_scaleFactor;
      double fz = atof( sFz.c_str() ) * Gaudi::Units::gauss * m_scaleFactor;
      
      // Add the magnetic field components of each point to 
      // sequentialy in a vector 
      m_Q.push_back( fx );
      m_Q.push_back( fy );
      m_Q.push_back( fz );
      // counts after reading and filling to match the number of lines
      ncheck++; 
	  }
    infile.close();
    if ( nlines != ncheck ) {
      log << MSG::ERROR << " Number of points in field map does not match" 
          << endreq;
      return StatusCode::FAILURE;
    }
  }
  else {
  	log << MSG::ERROR << "Unable to open magnetic field file : " 
        << m_filename << endreq;
  }
  
  return sc;
}


StatusCode MagneticFieldSvc::parseRealFiles() {
  StatusCode sc = StatusCode::FAILURE;
  
  MsgStream log( msgSvc(), name() );
  char line[ 255 ];

for(int ifile=0;ifile<4;ifile++) {
  std::ifstream infile( m_qfilename[ifile].c_str() );
  
  if ( infile ) {
	  sc = StatusCode::SUCCESS;
    log << MSG::INFO << "Opened magnetic field file : " << m_qfilename[ifile]
	      << endreq;
    
    // Skip the header till PARAMETER
    do{
	    infile.getline( line, 255 );
	  } while( line[0] != 'P' );
    
    // Get the PARAMETER
    std::string sPar[2];
    char* token = strtok( line, " " );
    int ip = 0;
    do{
      if ( token ) { sPar[ip] = token; token = strtok( NULL, " " );} 
      else continue;
      ip++;
    } while ( token != NULL );
    long int npar = atoi( sPar[1].c_str() );

    // Skip the header till GEOMETRY
    do{
	    infile.getline( line, 255 );
	  } while( line[0] != 'G' );
    
    // Skip any comment before GEOMETRY 
    do{
	    infile.getline( line, 255 );
	  } while( line[0] != '#' );
    
    // Get the GEOMETRY
    infile.getline( line, 255 );
    std::string sGeom[7];
    token = strtok( line, " " );
    int ig = 0;
    do{
      if ( token ) { sGeom[ig] = token; token = strtok( NULL, " " );} 
      else continue; 
      ig++;  
    } while (token != NULL);

    // Grid dimensions are given in cm in CDF file. Convert to CLHEP units
    m_Dxyz[0] = atof( sGeom[0].c_str() ) * Gaudi::Units::cm;
    m_Dxyz[1] = atof( sGeom[1].c_str() ) * Gaudi::Units::cm;
    m_Dxyz[2] = atof( sGeom[2].c_str() ) * Gaudi::Units::cm;
    m_Nxyz[0] = atoi( sGeom[3].c_str() );
    m_Nxyz[1] = atoi( sGeom[4].c_str() );
    m_Nxyz[2] = atoi( sGeom[5].c_str() );
    m_zOffSet = atof( sGeom[6].c_str() ) * Gaudi::Units::cm;
    
    m_Q_quadr[ifile].reserve(npar - 7);
    // Number of lines with data to be read
    long int nlines = ( npar - 7 ) / 3;
    
    // Check number of lines with data read in the loop
    long int ncheck = 0;
    
    // Skip comments and fill a vector of magnetic components for the
    // x, y and z positions given in GEOMETRY
    
   	while( infile ) {
      // parse each line of the file, 
      // comment lines begin with '#' in the cdf file
	    infile.getline( line, 255 );
	    if ( line[0] == '#' ) continue;
	    std::string sFx, sFy, sFz; 
	    char* token = strtok( line, " " );
	    if ( token ) { sFx = token; token = strtok( NULL, " " );} else continue;
  	  if ( token ) { sFy = token; token = strtok( NULL, " " );} else continue;
  	  if ( token ) { sFz = token; token = strtok( NULL, " " );} else continue;
	    if ( token != NULL ) continue;
      
      // Field values are given in gauss in CDF file. Convert to CLHEP units
      double fx = atof( sFx.c_str() ) * Gaudi::Units::gauss * m_scaleFactor;
      double fy = atof( sFy.c_str() ) * Gaudi::Units::gauss * m_scaleFactor;
      double fz = atof( sFz.c_str() ) * Gaudi::Units::gauss * m_scaleFactor;
      
      // Add the magnetic field components of each point to 
      // sequentialy in a vector 
      m_Q_quadr[ifile].push_back( fx );
      m_Q_quadr[ifile].push_back( fy );
      m_Q_quadr[ifile].push_back( fz );
      // counts after reading and filling to match the number of lines
      ncheck++; 
	  }
    infile.close();
    if ( nlines != ncheck ) {
      log << MSG::ERROR << " Number of points in field map does not match" 
          << endreq;
      return StatusCode::FAILURE;
    }
  }
  else {
  	log << MSG::ERROR << "Unable to open magnetic field file : " 
        << m_qfilename[ifile] << endreq;
  }
}
 
  return sc;
}


//=============================================================================
// FieldVector: find the magnetic field value at a given point in space
//=============================================================================
StatusCode MagneticFieldSvc::fieldVector(const Gaudi::XYZPoint&  r,
                                               Gaudi::XYZVector& bf ) const {


  if( m_useConstField ) {
    bf.SetXYZ( m_constFieldVector[0]*Gaudi::Units::tesla,
               m_constFieldVector[1]*Gaudi::Units::tesla,
               m_constFieldVector[2]*Gaudi::Units::tesla );
    return StatusCode::SUCCESS;
  }
  
  bf.SetXYZ( 0.0, 0.0, 0.0 );

  ///  Linear interpolated field
  double z = r.z() - m_zOffSet;
  if( !(z >= m_min_FL[2] && z < m_max_FL[2]) )  return StatusCode::SUCCESS;
  double x = fabs( r.x() );  
  if( !(x >= m_min_FL[0] && x < m_max_FL[0]) )  return StatusCode::SUCCESS;
  double y = fabs( r.y() );
  if( !(y >= m_min_FL[1] && y < m_max_FL[1]) )  return StatusCode::SUCCESS;
  int i = int( x/m_Dxyz[0]);
  int j = int( y/m_Dxyz[1] );
  int k = int( z/m_Dxyz[2] );

  
  int ijk000 = 3*( m_Nxyz[0]*( m_Nxyz[1]*k     + j )     + i );
  int ijk001 = 3*( m_Nxyz[0]*( m_Nxyz[1]*(k+1) + j )     + i );
  int ijk010 = 3*( m_Nxyz[0]*( m_Nxyz[1]*k     + j + 1 ) + i );
  int ijk011 = 3*( m_Nxyz[0]*( m_Nxyz[1]*(k+1) + j + 1)  + i );
  int ijk100 = 3*( m_Nxyz[0]*( m_Nxyz[1]*k     + j)      + i + 1 );
  int ijk101 = 3*( m_Nxyz[0]*( m_Nxyz[1]*(k+1) + j)      + i + 1 );
  int ijk110 = 3*( m_Nxyz[0]*( m_Nxyz[1]*k     + j + 1)  + i + 1 );
  int ijk111 = 3*( m_Nxyz[0]*( m_Nxyz[1]*(k+1) + j + 1 ) + i + 1 );
  // auxiliary variables defined at the vertices of the cube that
  // contains the (x, y, z) point where the field is interpolated

  double cx000,cx001,cx010,cx011,cx100,cx101,cx110,cx111,cy000,cy001,cy010,cy011,cy100,cy101,cy110,cy111,cz000,cz001,cz010,cz011,cz100,cz101,cz110,cz111;

  if(!m_useRealMap) {
    
    cx000 = m_Q[ ijk000 ];
    cx001 = m_Q[ ijk001 ];
    cx010 = m_Q[ ijk010 ];
    cx011 = m_Q[ ijk011 ];
    cx100 = m_Q[ ijk100 ];
    cx101 = m_Q[ ijk101 ];
    cx110 = m_Q[ ijk110 ];
    cx111 = m_Q[ ijk111 ];
    cy000 = m_Q[ ijk000+1 ];
    cy001 = m_Q[ ijk001+1 ];
    cy010 = m_Q[ ijk010+1 ];
    cy011 = m_Q[ ijk011+1 ];
    cy100 = m_Q[ ijk100+1 ];
    cy101 = m_Q[ ijk101+1 ];
    cy110 = m_Q[ ijk110+1 ];
    cy111 = m_Q[ ijk111+1 ];
    cz000 = m_Q[ ijk000+2 ];
    cz001 = m_Q[ ijk001+2 ];
    cz010 = m_Q[ ijk010+2 ];
    cz011 = m_Q[ ijk011+2 ];
    cz100 = m_Q[ ijk100+2 ];
    cz101 = m_Q[ ijk101+2 ];
    cz110 = m_Q[ ijk110+2 ];
    cz111 = m_Q[ ijk111+2 ];
    
  } else {
    
    int iquadr=999;
    
    if(r.x() >=0 )
      if (r.y() >=0 ) iquadr=0;
      else iquadr=2;
    else 
      if (r.y() >=0 ) iquadr=1;
      else iquadr=3;
    
    cx000 = (m_Q_quadr[iquadr])[ ijk000 ];
    cx001 = (m_Q_quadr[iquadr])[ ijk001 ];
    cx010 = (m_Q_quadr[iquadr])[ ijk010 ];
    cx011 = (m_Q_quadr[iquadr])[ ijk011 ];
    cx100 = (m_Q_quadr[iquadr])[ ijk100 ];
    cx101 = (m_Q_quadr[iquadr])[ ijk101 ];
    cx110 = (m_Q_quadr[iquadr])[ ijk110 ];
    cx111 = (m_Q_quadr[iquadr])[ ijk111 ];
    cy000 = (m_Q_quadr[iquadr])[ ijk000+1 ];
    cy001 = (m_Q_quadr[iquadr])[ ijk001+1 ];
    cy010 = (m_Q_quadr[iquadr])[ ijk010+1 ];
    cy011 = (m_Q_quadr[iquadr])[ ijk011+1 ];
    cy100 = (m_Q_quadr[iquadr])[ ijk100+1 ];
    cy101 = (m_Q_quadr[iquadr])[ ijk101+1 ];
    cy110 = (m_Q_quadr[iquadr])[ ijk110+1 ];
    cy111 = (m_Q_quadr[iquadr])[ ijk111+1 ];
    cz000 = (m_Q_quadr[iquadr])[ ijk000+2 ];
    cz001 = (m_Q_quadr[iquadr])[ ijk001+2 ];
    cz010 = (m_Q_quadr[iquadr])[ ijk010+2 ];
    cz011 = (m_Q_quadr[iquadr])[ ijk011+2 ];
    cz100 = (m_Q_quadr[iquadr])[ ijk100+2 ];
    cz101 = (m_Q_quadr[iquadr])[ ijk101+2 ];
    cz110 = (m_Q_quadr[iquadr])[ ijk110+2 ];
    cz111 = (m_Q_quadr[iquadr])[ ijk111+2 ];
    
  }
  
  double hx1 = ( x-i*m_Dxyz[0] )/m_Dxyz[0];
  double hy1 = ( y-j*m_Dxyz[1] )/m_Dxyz[1];
  double hz1 = ( z-k*m_Dxyz[2] )/m_Dxyz[2];
  double hx0 = 1.0-hx1;
  double hy0 = 1.0-hy1;
  double hz0 = 1.0-hz1;

  double h000 = hx0*hy0*hz0;
  if( fabs(h000) > 0.0 &&
      cx000 > 9.0e5 && cy000 > 9.0e5 && cz000 > 9.0e5) return StatusCode::SUCCESS;
 
  double h001 = hx0*hy0*hz1;
  if( fabs(h001) > 0.0 && 
      cx001 > 9.0e5 && cy001 > 9.0e5 && cz001 > 9.0e5) return StatusCode::SUCCESS;

  double h010 = hx0*hy1*hz0;
  if( fabs(h010) > 0.0 && 
      cx010 > 9.0e5 && cy010 > 9.0e5 && cz010 > 9.0e5) return StatusCode::SUCCESS;

  double h011 = hx0*hy1*hz1;
  if( fabs(h011) > 0.0 && 
      cx011 > 9.0e5 && cy011 > 9.0e5 && cz011 > 9.0e5) return StatusCode::SUCCESS;

  double h100 = hx1*hy0*hz0;
  if( fabs(h100) > 0.0 && 
      cx100 > 9.0e5 && cy100 > 9.0e5 && cz100 > 9.0e5) return StatusCode::SUCCESS;
 
  double h101 = hx1*hy0*hz1;
  if( fabs(h101) > 0.0 && 
      cx101 > 9.0e5 && cy101 > 9.0e5 && cz101 > 9.0e5) return StatusCode::SUCCESS;
 
  double h110 = hx1*hy1*hz0;
  if( fabs(h110) > 0.0 && 
      cx110 > 9.0e5 && cy110 > 9.0e5 && cz110 > 9.0e5) return StatusCode::SUCCESS;

  double h111 = hx1*hy1*hz1;
  if( fabs(h111) > 0.0 && 
      cx111 > 9.0e5 && cy111 > 9.0e5 && cz111 > 9.0e5) return StatusCode::SUCCESS;

  bf.SetX ( cx000*h000 + cx001*h001 + cx010*h010 + cx011*h011 +
            cx100*h100 + cx101*h101 + cx110*h110 + cx111*h111);
  bf.SetY ( cy000*h000 + cy001*h001 + cy010*h010 + cy011*h011 +
            cy100*h100 + cy101*h101 + cy110*h110 + cy111*h111 );
  bf.SetZ ( cz000*h000 + cz001*h001 + cz010*h010 + cz011*h011 +
            cz100*h100 + cz101*h101 + cz110*h110 + cz111*h111 );

  if( r.x() < 0. && r.y() >= 0. ){
    bf.SetX( -bf.x() );
  }
  else if(  r.x() < 0. &&  r.y()  < 0. ){
    bf.SetZ( -bf.z() );
  }
  else if( r.x() >= 0. && r.y() < 0. ){    
    bf.SetX( -bf.x() );
    bf.SetZ( -bf.z() );
  } 
  return StatusCode::SUCCESS;      
}
