/** @file AnalyticFieldSvc.cpp
 *  Implementation of AnalyticFieldSvc class
 *
 *  @authors Geraldine Conti, Adlene Hicheur
 *  @date   03/07
 */
// Include files
#include "Riostream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SystemOfUnits.h"

#include "AnalyticFieldSvc.h"


#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/Point3DTypes.h"

#include "MagVec.h"
#include "MagMat.h"


#include <cstdlib>
#include <fstream>
#include <cstring> // for strtok with gcc 4.3

// number of regions for the parametrization
#define NREG 15
//#define NREG 16


DECLARE_SERVICE_FACTORY( AnalyticFieldSvc )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
AnalyticFieldSvc::AnalyticFieldSvc( const std::string& name, 
				    ISvcLocator* svc ) : Service( name, svc )
{

  m_nREG =0;
  m_nREGmin =0;
  m_nREGmax =0;
  
  // Bxmap.reserve(NREG);
  // Bymap.reserve(NREG);
  // Bzmap.reserve(NREG); 
  //  zmin.reserve(NREG);
  //  zmax.reserve(NREG);

  m_constFieldVector.push_back( 0. );
  m_constFieldVector.push_back( 0. );
  m_constFieldVector.push_back( 0. );
  

  declareProperty( "BxMapFile",m_filename[0] = "BxMapFileNotSet");
  declareProperty( "ByMapFile",m_filename[1] = "ByMapFileNotSet");
  declareProperty( "BzMapFile",m_filename[2] = "BzMapFileNotSet");
  declareProperty( "UseConstantField",    m_useConstField = false );
  declareProperty( "ConstantFieldVector", m_constFieldVector );
  declareProperty( "ScaleFactor",         m_scaleFactor = 1. );
  declareProperty( "ZOffSet", m_zOffSet = -50.);
  
}
//=============================================================================
// Standard destructor
//=============================================================================
AnalyticFieldSvc::~AnalyticFieldSvc()
{
}

//=============================================================================
// Initialize
//=============================================================================
StatusCode AnalyticFieldSvc::initialize()
{
  MsgStream log(msgSvc(), name());
  StatusCode status = Service::initialize();
  if( status.isFailure() ) return status;
  
  if( m_useConstField ) {
    log << MSG::WARNING << "using constant magnetic field with field vector "
        << m_constFieldVector << " (Tesla)" << endmsg;
    return StatusCode::SUCCESS;
  }
 
  if( 1. != m_scaleFactor ) {
    log << MSG::WARNING << "Field map will be scaled by a factor = "
        << m_scaleFactor << endmsg;
  }
  
  status = GetParam();
  if ( status.isSuccess() ) {
    if( UNLIKELY(log.level() <= MSG::DEBUG) )
      log << MSG::DEBUG << "B maps read successfully" << endmsg;
 
    return status;
  }
  else {
    if( UNLIKELY(log.level() <= MSG::DEBUG) )
      log << MSG::DEBUG << "B maps failed" << endmsg;
    return StatusCode::FAILURE;
  }

  
  

}

//=============================================================================
// Finalize
//=============================================================================
StatusCode AnalyticFieldSvc::finalize()
{
  MsgStream log( msgSvc(), name() );
  StatusCode status = Service::finalize();

  ClearMaps();
  
  
  if ( status.isSuccess() )
    log << MSG::INFO << "Service finalized successfully" << endmsg;
  return status;
}

//=============================================================================
// QueryInterface
//=============================================================================
StatusCode AnalyticFieldSvc::queryInterface( const InterfaceID& riid, 
                                             void** ppvInterface      ) 
{
  StatusCode sc = StatusCode::FAILURE;
  if ( ppvInterface ) {
    *ppvInterface = 0;
    
    if ( riid == IMagneticFieldSvc::interfaceID() ) {
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
// GetParam: reads coefficients from regions parameterization
// ---------------------------------------------------------------------------


StatusCode AnalyticFieldSvc::GetParam() {
  StatusCode sc = StatusCode::FAILURE;
  
  MsgStream log( msgSvc(), name() );

  for(int ifile=0;ifile<3;ifile++) {
    
  char line[ 255 ];
 
  // int NREG=0;
  
  std::ifstream infile( m_filename[ifile].c_str() );



  
  if (!infile) {
      log << MSG::ERROR << "Unable to open BMap : " 
           << m_filename[ifile] << endmsg;
  
  }

 
  if (infile) {
    sc = StatusCode::SUCCESS;
      log << MSG::INFO << "BMap opened successfully : " << m_filename[ifile]
    	<< endmsg;

       // Skip the header till NREG
    do{
	    infile.getline( line, 255 );
	  } while( line[0] != 'N' || line[1] !='R'); 


    // Reserve maps
    int nmap = 0;
    
    char* token = strtok( line, " " );
    do{
 
      if ( token ) {
        if (nmap==1) m_nREG = atoi(token); 
        token = strtok( NULL, " ");
      }
      else continue;
      nmap++;
    } while (token != NULL);

    std::cout<<" READ NUMBER OF REGIONS m_nREG: "<<m_nREG<<std::endl;
    if (ifile==0) Bxmap.reserve(m_nREG);
    if (ifile==1) Bymap.reserve(m_nREG);
    if (ifile==2) Bzmap.reserve(m_nREG);
    if (ifile==0) {zmin.reserve(m_nREG);
    zmax.reserve(m_nREG);
    }
    

       // Skip the header till NY30
    do{
	    infile.getline( line, 255 );
	  } while( line[0] != 'N' || line[1] !='Y'); 

    token = strtok( line, " " );
    nmap=0;
  
    
    do{
 
      if ( token ) {
        if (nmap==1) m_nREGmin = atoi(token); 
        if (nmap==2) m_nREGmax = atoi(token); 
        token = strtok( NULL, " ");
      }
      else continue;
      nmap++;
    } while (token != NULL);

    std::cout<<" FIRST REGION WITH Y30: "<<m_nREGmin<<std::endl;
    std::cout<<" LAST REGION WITH Y30: "<<m_nREGmax<<std::endl;

       // Skip the header till ZMINS
    do{
	    infile.getline( line, 255 );
	  } while( line[0] != 'M' && line[1] !='I');

      // Get the ZMINS in a vector

 
    std::string* sZmin = new std::string[m_nREG];
  
    token = strtok( line, " " );
    int izmin = 0;
 
  
    do{
 
      if ( token ) {
        if (izmin>0) sZmin[izmin-1] = token; 
        token = strtok( NULL, " ");
      }
      else continue;
      izmin++;
    } while (token != NULL);

    for(int w=0;w<m_nREG;w++) {
     
      zmin[w] = atof( sZmin[w].c_str() ) ; 
    }


       // Skip the header till ZMAXS
    do{
	    infile.getline( line, 255 );
	  } while( line[0] != 'M' && line[1] !='A');

      // Get the ZMAXS in a vector

    std::string* sZmax = new std::string[NREG];
  
    token = strtok( line, " " );
    int izmax = 0;
    do{
      if ( token ) {

        if (izmax>0) sZmax[izmax-1] = token; 
        token = strtok( NULL, " ");
      }
      else continue;
      izmax++;
    } while (token != NULL);



    for(int x=0;x<m_nREG;x++) {
     
      zmax[x] = atof( sZmax[x].c_str() ) ; 
    }
       

 for(int k=0;k<m_nREG;k++) {

       // Skip the header till next REGION
    do{
	    infile.getline( line, 255 );
	  } while( line[0] != 'R' );


    // Get the number of parameterization terms
    std::string sTerms[4];
    char* token = strtok( line, " " );
    int ip = 0;
    do{
      if ( token ) { sTerms[ip] = token; token = strtok( NULL, " " );} 
      else continue;
      ip++;
    } while ( token != NULL );

    int nterms = atoi( sTerms[1].c_str() );
    int nterms2 = 0;
    if (m_nREGmax>0 && k>=(m_nREGmin-1) && k<=(m_nREGmax-1)) nterms2 = atoi( sTerms[3].c_str() );

    log << MSG::INFO <<" REGION NUMBER: "<<k<<endmsg;
    
    log << MSG::INFO << "NTERMS " << nterms << endmsg;
    if (m_nREGmax>0 && k>=(m_nREGmin-1) && k<=(m_nREGmax-1))
      log << MSG::INFO << "NTERMS FOR Y>YMAX-30 REGION: " << nterms2 << endmsg;
    MagMat* temp = new MagMat(nterms+2,4);

   
    for(int l=0;l<nterms+3;l++) {
     
      infile.getline( line, 255 );
      if ( line[0] == '#' ) continue;
	    std::string sPowx, sPowy, sPowz, sCoef; 
	    char* token = strtok( line, " " );
	    if ( token ) { sPowx = token; token = strtok( NULL, " " );} else continue;
	    if ( token ) { sPowy = token; token = strtok( NULL, " " );} else continue;
	    if ( token ) { sPowz = token; token = strtok( NULL, " " );} else continue;
	    if ( token ) { sCoef = token; token = strtok( NULL, " " );} else continue;
	    if ( token != NULL ) continue;
      
      double Powx = atof( sPowx.c_str() );
      double Powy = atof( sPowy.c_str() );
      double Powz = atof( sPowz.c_str() );
      double Coef = atof( sCoef.c_str() );

     
      
      // Add the powers and coefficients parameters of each region to 
      // sequentialy in a matrice 

      if (l>0) {       
      (*temp)[l-1][0]= Powx ;
      (*temp)[l-1][1]= Powy ;
      (*temp)[l-1][2]= Powz ;
      (*temp)[l-1][3]= Coef ;
      }
      
 
    }

    if (ifile ==0) Bxmap.push_back(temp);
    if (ifile ==1) Bymap.push_back(temp);
    if (ifile ==2) Bzmap.push_back(temp);

    //In case one has a second parametrization for ymax - 30cm < y < ymax
    if (m_nREGmax>0 && k>=(m_nREGmin-1) && k<=(m_nREGmax-1)) {
      
      MagMat* temp2 = new MagMat(nterms2+2,4);

      infile.getline( line, 255 );
  
      
      for(int l=0;l<nterms2+2;l++) {
        
        
        infile.getline( line, 255 );
        if ( line[0] == '#' ) continue;
        std::string sPowx, sPowy, sPowz, sCoef; 
        char* token = strtok( line, " " );
        if ( token ) { sPowx = token; token = strtok( NULL, " " );} else continue;
        if ( token ) { sPowy = token; token = strtok( NULL, " " );} else continue;
        if ( token ) { sPowz = token; token = strtok( NULL, " " );} else continue;
        if ( token ) { sCoef = token; token = strtok( NULL, " " );} else continue;
        if ( token != NULL ) continue;
      
        double Powx = atof( sPowx.c_str() );
        double Powy = atof( sPowy.c_str() );
        double Powz = atof( sPowz.c_str() );
        double Coef = atof( sCoef.c_str() );
      



        // Add the powers and coefficients parameters of each region to 
        // sequentialy in a matrice 

   
        (*temp2)[l][0]= Powx ;
        (*temp2)[l][1]= Powy ;
        (*temp2)[l][2]= Powz ;
        (*temp2)[l][3]= Coef ;
   
      
 
      }

    if (ifile ==0) Bxmap.push_back(temp2);
    if (ifile ==1) Bymap.push_back(temp2);
    if (ifile ==2) Bzmap.push_back(temp2);
    
    }
    
      


}

 delete sZmin;
 delete sZmax;
 infile.close(); 
  }
  
  }
 
  return sc;
  }


// Old version of GetParam - keep it temporarily
/*StatusCode AnalyticFieldSvc::GetParam() {
  StatusCode sc = StatusCode::FAILURE;
  
  MsgStream log( msgSvc(), name() );

  for(int ifile=0;ifile<3;ifile++) {
    
  char line[ 255 ];
 

  std::ifstream infile( m_filename[ifile].c_str() );



  
  if (!infile) {
      log << MSG::ERROR << "Unable to open BMap : " 
           << m_filename[ifile] << endmsg;
  
  }

 
  if (infile) {
    sc = StatusCode::SUCCESS;
      log << MSG::INFO << "BMap opened successfully : " << m_filename[ifile]
    	<< endmsg;
 

       // Skip the header till ZMINS
    do{
	    infile.getline( line, 255 );
	  } while( line[0] != 'M' && line[1] !='I');

      // Get the ZMINS in a vector

 
    std::string sZmin[NREG];
    char* token = strtok( line, " " );
    int izmin = 0;
 
  
    do{
 
      if ( token ) {
        if (izmin>0) sZmin[izmin-1] = token; 
        token = strtok( NULL, " ");
      }
      else continue;
      izmin++;
    } while (token != NULL);

    for(int w=0;w<NREG;w++) {
     
      zmin[w] = atof( sZmin[w].c_str() ) ; 
    }


       // Skip the header till ZMAXS
    do{
	    infile.getline( line, 255 );
	  } while( line[0] != 'M' && line[1] !='A');

      // Get the ZMAXS in a vector

    std::string sZmax[NREG];
    token = strtok( line, " " );
    int izmax = 0;
    do{
      if ( token ) {

        if (izmax>0) sZmax[izmax-1] = token; 
        token = strtok( NULL, " ");
      }
      else continue;
      izmax++;
    } while (token != NULL);



    for(int x=0;x<NREG;x++) {
     
      zmax[x] = atof( sZmax[x].c_str() ) ; 
    }
       

 for(int k=0;k<NREG;k++) {

       // Skip the header till next REGION
    do{
	    infile.getline( line, 255 );
	  } while( line[0] != 'R' );


    // Get the number of parameterization terms
    std::string sTerms[2];
    char* token = strtok( line, " " );
    int ip = 0;
    do{
      if ( token ) { sTerms[ip] = token; token = strtok( NULL, " " );} 
      else continue;
      ip++;
    } while ( token != NULL );

    int nterms = atoi( sTerms[1].c_str() );

    log << MSG::INFO << "NTERMS " << nterms << endmsg;

    MagMat* temp = new MagMat(nterms+2,4);
  

    for(int l=0;l<nterms+3;l++) {
     
       infile.getline( line, 255 );
	    if ( line[0] == '#' ) continue;
	    std::string sPowx, sPowy, sPowz, sCoef; 
	    char* token = strtok( line, " " );
	    if ( token ) { sPowx = token; token = strtok( NULL, " " );} else continue;
	    if ( token ) { sPowy = token; token = strtok( NULL, " " );} else continue;
	    if ( token ) { sPowz = token; token = strtok( NULL, " " );} else continue;
	    if ( token ) { sCoef = token; token = strtok( NULL, " " );} else continue;
	    if ( token != NULL ) continue;
      
      double Powx = atof( sPowx.c_str() );
      double Powy = atof( sPowy.c_str() );
      double Powz = atof( sPowz.c_str() );
      double Coef = atof( sCoef.c_str() );
      
      // Add the powers and coefficients parameters of each region to 
      // sequentialy in a matrice 

      if (l>0) {       
      (*temp)[l-1][0]= Powx ;
      (*temp)[l-1][1]= Powy ;
      (*temp)[l-1][2]= Powz ;
      (*temp)[l-1][3]= Coef ;
      }
      
 
    }

    if (ifile ==0) Bxmap.push_back(temp);
    if (ifile ==1) Bymap.push_back(temp);
    if (ifile ==2) Bzmap.push_back(temp);

      


} 
 infile.close(); 
  }
  
  }
  
  
  return sc;
  }*/

//=============================================================================
// FieldVector: find the magnetic field value at a given point in space
//=============================================================================
StatusCode AnalyticFieldSvc::fieldVector(const Gaudi::XYZPoint&  r,
					 Gaudi::XYZVector& b ) const {
  
    Bcalculation(r,b);

  return StatusCode::SUCCESS;
}

//=============================================================================
// Bcalculation: choose the adequate parameterization depending on space point
//=============================================================================
void AnalyticFieldSvc::Bcalculation (const Gaudi::XYZPoint&  point, 
				  Gaudi::XYZVector& bf ) const {

  //initialize B field value
  bf.SetXYZ(0,0,0);
  // Use constant field?
  if( m_useConstField ) {
    bf.SetXYZ( m_constFieldVector[0]*Gaudi::Units::tesla,
               m_constFieldVector[1]*Gaudi::Units::tesla,
               m_constFieldVector[2]*Gaudi::Units::tesla );
    return;
  }
  
  MagVec coord(3);

  coord[0]=fabs(point.x())/Gaudi::Units::m;   
  coord[1]=fabs(point.y())/Gaudi::Units::m;
  coord[2]=point.z()/Gaudi::Units::m;

  int iReg = 0;
  //For Regions (z slices) 6 to 12, division between y < ymax - 30 cm and y>ymax - 30 cm  
  
  for (int iz=0;iz <m_nREG;iz++) {

    iReg=iz;
    
    bool condZ = zmin[iz] <= point.z()/Gaudi::Units::cm && 
                             point.z()/Gaudi::Units::cm < zmax[iz] && 
                             coord[0]<(fabs(coord[2])*tan(0.3)) &&
                             coord[1]<(fabs(coord[2])*tan(0.25));
 
    if (m_nREGmax>0 && iz>=(m_nREGmin-1) && iz<=(m_nREGmax-1)) {
      iReg = 2*iz-(m_nREGmin-1);
      if (fabs(coord[1]) > (*(Bxmap[iReg]))[1][1]) iReg++;
    }
  
    if(m_nREGmax>0 && iz>=m_nREGmax) iReg = iz+(m_nREGmax-m_nREGmin+1);
    

    if (condZ) {      


      bf.SetXYZ(EvaluateField(coord,*(Bxmap[iReg])),EvaluateField(coord,*(Bymap[iReg])),EvaluateField(coord,*(Bzmap[iReg])));

    }

  }

  bf*=Gaudi::Units::tesla;
  
  if( point.x() < 0. && point.y() >= 0. ){
    bf.SetX( -bf.x() );
  }
  else if(  point.x() < 0. &&  point.y()  < 0. ){
    bf.SetZ( -bf.z() );
  }
  else if( point.x() >= 0. && point.y() < 0. ){
    bf.SetX( -bf.x() );
    bf.SetZ( -bf.z() );
  } 

  return ;
}


//Old version of Bcalculation - keep it temporarily
/*void AnalyticFieldSvc::Bcalculation (const Gaudi::XYZPoint&  point, 
				  Gaudi::XYZVector& bf ) const {

  //initialize B field value
  bf.SetXYZ(0,0,0);
  // Use constant field?
  if( m_useConstField ) {
    bf.SetXYZ( m_constFieldVector[0]*Gaudi::Units::tesla,
               m_constFieldVector[1]*Gaudi::Units::tesla,
               m_constFieldVector[2]*Gaudi::Units::tesla );
    return;
  }
  
  MagVec coord(3);
  coord[0]=fabs(point.x())/Gaudi::Units::m;   
  coord[1]=fabs(point.y())/Gaudi::Units::m;
  coord[2]=point.z()/Gaudi::Units::m;

  
  for (int iz=0;iz < NREG;iz++) {

bool condZ = zmin[iz]<=point.z()/Gaudi::Units::cm && 
             point.z()/Gaudi::Units::cm<zmax[iz] && 
             coord[0]<(fabs(coord[2])*tan(0.3)) &&
             coord[1]<(fabs(coord[2])*tan(0.25));

    if (condZ) {      


      bf.SetXYZ(EvaluateField(coord,*(Bxmap[iz])),EvaluateField(coord,*(Bymap[iz])),EvaluateField(coord,*(Bzmap[iz])));

    }

  }

  bf*=Gaudi::Units::tesla;
  
  if( point.x() < 0. && point.y() >= 0. ){
    bf.SetX( -bf.x() );
  }
  else if(  point.x() < 0. &&  point.y()  < 0. ){
    bf.SetZ( -bf.z() );
  }
  else if( point.x() >= 0. && point.y() < 0. ){
    bf.SetX( -bf.x() );
    bf.SetZ( -bf.z() );
  } 

  return ;
  }*/


//=============================================================================
// evaluateField: compute field analytically from parameterization
//=============================================================================   


double AnalyticFieldSvc::EvaluateField(MagVec&  pos, MagMat& bmap) const{

  double returnValue = bmap[0][3];
  int    i = 0, j = 0, k = 0;
  for (i = 2; i < bmap.nrow(); i++) {
    // Evaluate the ith term in the expansion
     double term = bmap[i][3];
    for (j = 0; j < 3; j++) {
      // Evaluate the polynomial in the jth variable.
      int power = int(bmap[i][j]); 
      double p1 = 1, p2 = 0, p3 = 0, r = 0;
      double v =  1 + 2. / (bmap[1][j] - bmap[0][j]) * (pos[j] - bmap[1][j]);
      // what is the power to use!
      switch(power) {
      case 1: r = 1; break; 
      case 2: r = v; break; 
      default: 
        p2 = v; 
        for (k = 3; k <= power; k++) { 
          p3 = p2 * v;
          p3 = 2 * v * p2 - p1; 
          p1 = p2; p2 = p3; 
        }
        r = p3;
      }
      // multiply this term by the poly in the jth var
      term *= r; 
    }
    // Add this term to the final result
    returnValue += term;
  }

  return returnValue;

}



 
//Maps clearing

 void AnalyticFieldSvc::ClearMaps() 
   {
     

 for (std::vector<MagMat*>::const_iterator iMatBx = Bxmap.begin();
       iMatBx != Bxmap.end();++iMatBx) {
    delete (*iMatBx);
  }

 for (std::vector<MagMat*>::const_iterator iMatBy = Bymap.begin();
       iMatBy != Bymap.end();++iMatBy) {
    delete (*iMatBy);
  }

 for (std::vector<MagMat*>::const_iterator iMatBz = Bzmap.begin();
       iMatBz != Bzmap.end();++iMatBz) {
    delete (*iMatBz);
 }

   }

