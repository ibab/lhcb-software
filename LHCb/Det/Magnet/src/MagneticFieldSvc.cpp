// $Id: MagneticFieldSvc.cpp,v 1.3 2002-05-21 17:10:17 cattanem Exp $

// Include files
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"

#include "MagneticFieldSvc.h"

#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Units/PhysicalConstants.h"

#include <cstdlib>
#include <fstream.h>

/** @class MagneticFieldSvc MagneticFieldSvc.h
 *  A service for finding the magnetic field vector at a given
 *  point in space. Based on original implementation by I.Last and
 *  G.Gracia via Gaudi_gufld (modified from SICBMC)
 *
 *  @author Edgar De Oliveira
 *  @date   2002-05-21
 */

// Instantiation of a static factory class used by clients to create
// instances of this service
static SvcFactory<MagneticFieldSvc> s_factory;
const ISvcFactory& MagneticFieldSvcFactory = s_factory;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MagneticFieldSvc::MagneticFieldSvc( const std::string& name, 
            ISvcLocator* svc ) : Service( name, svc )
{
  if(getenv("LHCBDBASE") != NULL) {
    m_filename = std::string(getenv( "LHCBDBASE" )) + 
      std::string( "/cdf/field043.cdf");
  }
  else {
    m_filename = std::string( "field043.cdf" );
  }
  declareProperty( "FieldMapFile", m_filename ); 
}
//=============================================================================
// Standard destructor
//=============================================================================
MagneticFieldSvc::~MagneticFieldSvc()
{
}

//=============================================================================
// Return the service
//=============================================================================
const IID& MagneticFieldSvc::type() const 
{ 
  return IID_IMagneticFieldSvc; 
}

//=============================================================================
// Initialize
//=============================================================================
StatusCode MagneticFieldSvc::initialize()
{
  MsgStream log(msgSvc(), name());
  StatusCode status = Service::initialize();

  status = parseFile();
  if ( status.isSuccess() ) {
      log << MSG::DEBUG << "Magnetic field parsed successfully" << endreq;
      return status;
  }
  else {
    log << MSG::DEBUG << "Magnetic field parse failled" << endreq;
    return StatusCode::FAILURE;
  }
}

//=============================================================================
// Finalize
//=============================================================================
StatusCode MagneticFieldSvc::finalize()
{
  MsgStream log( msgSvc(), name() );
  StatusCode status = Service::finalize();
  
  if ( status.isSuccess() )
    log << MSG::INFO << "Service finalized successfully" << endreq;
  return status;
}

//=============================================================================
// QueryInterface
//=============================================================================
StatusCode MagneticFieldSvc::queryInterface( const IID& riid, 
               void** ppvInterface ) 
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
    m_Dxyz[0] = atof( sGeom[0].c_str() );
    m_Dxyz[1] = atof( sGeom[1].c_str() );
    m_Dxyz[2] = atof( sGeom[2].c_str() );
    m_Nxyz[0] = atoi( sGeom[3].c_str() );
    m_Nxyz[1] = atoi( sGeom[4].c_str() );
    m_Nxyz[2] = atoi( sGeom[5].c_str() );
    m_zOffSet = atof( sGeom[6].c_str() );
    
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
      
      // Keep cdf magnetic field values (gauss)
      double fx = atof( sFx.c_str() );
      double fy = atof( sFy.c_str() );
      double fz = atof( sFz.c_str() );
      
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
  else
  	log << MSG::ERROR << "Unable to open magnetic field file : " 
        << m_filename << endreq;
  return sc;
}

//=============================================================================
// FieldVector: find the magnetic field value at a given point in space
//=============================================================================
StatusCode MagneticFieldSvc::fieldVector(const HepPoint3D& r, 
                                         HepVector3D& b) const
{
  StatusCode sc = StatusCode::SUCCESS;

  HepPoint3D pxyz;
  HepVector3D fxyz;
  pxyz[0] = (r.x())/cm;
  pxyz[1] = (r.y())/cm;
  pxyz[2] = (r.z())/cm;

  this->fieldGrid( pxyz, fxyz );

  // note the basic unit of Magnetic field is: megavolt*ns/mm2

  b.setX( fxyz[0] * gauss );
  b.setY( fxyz[1] * gauss );
  b.setZ( fxyz[2] * gauss );
  
  return sc;
}

//=============================================================================
// routine to fill the field vector
//=============================================================================
StatusCode MagneticFieldSvc::fieldGrid (const HepPoint3D& r, 
                                   HepVector3D& bf ) const {
  
  StatusCode scs = StatusCode::SUCCESS;
  StatusCode scf = StatusCode::FAILURE;
  
  float xmin_FL, ymin_FL, zmin_FL;
  float xmax_FL, ymax_FL, zmax_FL;
  
  // step size in x, y and z
  float dx_FL, dy_FL, dz_FL;
  
  // number of steps in x, y and z 
  int nx_FL, ny_FL, nz_FL;

  // auxiliary variables defined at the vertices of the cube that
  // contains the (x, y, z) point where the field is interpolated
  float     x,     y,     z,   hx0,   hx1,   hy0,   hy1,   hz0, hz1;
  float   h000,  h001,  h010,  h011,  h100,  h101,  h110,  h111;
  float  cx000, cx001, cx010, cx011, cx100, cx101, cx110, cx111;
  float  cy000, cy001, cy010, cy011, cy100, cy101, cy110, cy111;
  float  cz000, cz001, cz010, cz011, cz100, cz101, cz110, cz111;
  
  int i, j, k;
  int ijk000, ijk001, ijk010, ijk011, ijk100, ijk101, ijk110, ijk111;

  dx_FL = m_Dxyz[0];
  dy_FL = m_Dxyz[1];
  dz_FL = m_Dxyz[2];
  nx_FL = m_Nxyz[0];
  ny_FL = m_Nxyz[1];
  nz_FL = m_Nxyz[2];
  
  xmin_FL = 0.;
  ymin_FL = 0.;
  zmin_FL = 0.;
  xmax_FL = xmin_FL+( nx_FL-1 )*dx_FL;
  ymax_FL = ymin_FL+( ny_FL-1 )*dy_FL;
  zmax_FL = zmin_FL+( nz_FL-1 )*dz_FL;
  
  double xvect[3];
  xvect[0] = r.x();
  xvect[1] = r.y();
  xvect[2] = r.z();
  
  ///  Linear interpolated field
  z = xvect[2] - m_zOffSet;
  if( z < zmin_FL || z >= zmax_FL )  return scf;
  x = fabs( xvect[0] );  
  if( x < xmin_FL || x >= xmax_FL )  return scf;
  y = fabs( xvect[1] );
  if( y <= ymin_FL || y >= ymax_FL )  return scf;
  i = int( x/dx_FL );
  j = int( y/dy_FL );
  k = int( z/dz_FL );
  
  ijk000 = 3*( nx_FL*( ny_FL*k     + j )     + i );
  ijk001 = 3*( nx_FL*( ny_FL*(k+1) + j )     + i );
  ijk010 = 3*( nx_FL*( ny_FL*k     + j + 1 ) + i );
  ijk011 = 3*( nx_FL*( ny_FL*(k+1) + j + 1)  + i );
  ijk100 = 3*( nx_FL*( ny_FL*k     + j)      + i + 1 );
  ijk101 = 3*( nx_FL*( ny_FL*(k+1) + j)      + i + 1 );
  ijk110 = 3*( nx_FL*( ny_FL*k     + j + 1)  + i + 1 );
  ijk111 = 3*( nx_FL*( ny_FL*(k+1) + j + 1 ) + i + 1 );
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
  hx1 = ( x-i*dx_FL )/dx_FL;
  hy1 = ( y-j*dy_FL )/dy_FL;
  hz1 = ( z-k*dz_FL )/dz_FL;
  hx0 = 1.0-hx1;
  hy0 = 1.0-hy1;
  hz0 = 1.0-hz1;
  h000 = hx0*hy0*hz0;
  if( fabs(h000) > 0.0 &&
      cx000> 9.0e05 && cy000 > 9.0e05 && cz000 > 9.0e05) return scf;
  h001 = hx0*hy0*hz1;
  if( fabs(h001) > 0.0 && 
      cx001 > 9.0e05 && cy001 > 9.0e05 && cz001 > 9.0e05) return scf;
  h010 = hx0*hy1*hz0;
  if( fabs(h010) > 0.0 && 
      cx010 > 9.0e05 && cy010 > 9.0e05 && cz010 > 9.0e05) return scf;
  h011 = hx0*hy1*hz1;
  if( fabs(h011) > 0.0 && 
      cx011 > 9.0e05 && cy011 > 9.0e05 && cz011 > 9.0e05) return scf;
  h100 = hx1*hy0*hz0;
  if( fabs(h100) > 0.0 && 
      cx100 > 9.0e05 && cy100 > 9.0e05 && cz100 > 9.0e05) return scf;
  h101 = hx1*hy0*hz1;
  if( fabs(h101) > 0.0 && 
      cx101 > 9.0e05 && cy101 > 9.0e05 && cz101 > 9.0e05) return scf;
  h110 = hx1*hy1*hz0;
  if( fabs(h110) > 0.0 && 
      cx110 > 9.0e05 && cy110 > 9.0e05 && cz110 > 9.0e05) return scf;
  h111 = hx1*hy1*hz1;
  if( fabs(h111) > 0.0 && 
      cx111 > 9.0e05 && cy111 > 9.0e05 && cz111 > 9.0e05) return scf;
  bf(0) = ( cx000*h000 + cx001*h001 + cx010*h010 + cx011*h011 +
            cx100*h100 + cx101*h101 + cx110*h110 + cx111*h111);
  bf(1) = ( cy000*h000 + cy001*h001 + cy010*h010 + cy011*h011 +
            cy100*h100 + cy101*h101 + cy110*h110 + cy111*h111 );
  bf(2) = ( cz000*h000 + cz001*h001 + cz010*h010 + cz011*h011 +
            cz100*h100 + cz101*h101 + cz110*h110 + cz111*h111 );
  if( xvect[0] < 0. && xvect[1] >= 0. ){
    bf(0) = -bf(0);
  }
  else if( xvect[0] < 0. && xvect[1] < 0. ){
    bf(2) = -bf(2);
  }
  else if( xvect[0] >= 0. && xvect[1] < 0. ){    
    bf(1) = -bf(1);
    bf(2) = -bf(2);
  } 
  return scs;      
}
