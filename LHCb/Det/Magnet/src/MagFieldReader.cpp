// $Id: MagFieldReader.cpp,v 1.14 2008-07-14 15:26:14 ahicheur Exp $
// Include files 
#include "Riostream.h"
#include <cctype>
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IMagneticFieldSvc.h"
#include "GaudiKernel/IChronoStatSvc.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/RndmGenerators.h"

// from LHCbKernel
#include "Kernel/IBIntegrator.h"

// Math Definitions
#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/Point3DTypes.h"

// local
#include "MagFieldReader.h"


//-----------------------------------------------------------------------------
// Implementation file for class : MagFieldReader
//
// 08/05/2002 : Edgar De Oliveira
// 16/03/2004 : Gloria Corti, modified to fill ntuple
// 05/2007: Adlene Hicheur, modified to enable tests of different mappings
// 07/2007: Adlene Hicheur, added B field integral testing Ntuple
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( MagFieldReader );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MagFieldReader::MagFieldReader( const std::string& name,
                    ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator )
  , m_pIMF(0) 
{
  declareProperty("Zmin", m_zMin =  -500.0*Gaudi::Units::mm);
  declareProperty("Zmax", m_zMax = 14000.0*Gaudi::Units::mm);  
  declareProperty("Step", m_step =   100.0*Gaudi::Units::mm);  
  declareProperty("Xmin", m_xMin =     0.0*Gaudi::Units::mm);  
  declareProperty("Xmax", m_xMax =  4000.0*Gaudi::Units::mm);
  declareProperty("Ymin", m_yMin =     0.0*Gaudi::Units::mm);  
  declareProperty("Ymax", m_yMax =  4000.0*Gaudi::Units::mm);
  declareProperty("FieldSvcName",m_FieldServiceName="MagneticFieldSvc");
  declareProperty("TestFieldInt",m_testbdl=false);
  declareProperty("NInt",m_nInt=1000);
  
}

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode MagFieldReader::initialize() {

  StatusCode sc = GaudiTupleAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
  debug() << "FieldReader intialize() has been called" << endmsg;
  
  m_pIMF = svc<IMagneticFieldSvc>( m_FieldServiceName, true );
  //  m_pIAF = svc<IMagneticFieldSvc>( "AnalyticFieldSvc", true );

  info() << "MagFieldReader initialized with service ==> " <<  m_FieldServiceName << endmsg;
  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode MagFieldReader::execute() {

  if (m_testbdl) TestBdl();
  
  // Print out info messages with the field value at different locations.

  debug() << "m_pIMF = " << m_pIMF << endreq;

  Tuple nt1 = nTuple( 10, "Field", CLID_ColumnWiseTuple );

  Gaudi::XYZVector B(0.0,0.0,0.0);
  Gaudi::XYZVector Ba(0.0,0.0,0.0);
  //  ofstream gridmap;
  //  gridmap.open("/afs/cern.ch/user/a/ahicheur/w0/bfieldvalid/gridmap10_new.txt");
  // gridmap.open("gridmap_nomag8-9y30.txt");    
  /*  gridmap<<"DETECTOR"<<endl;
  gridmap<<"NAME: FLDT"<<endl;
  gridmap<<"VERSION: test"<<endl;
  gridmap<<"#"<<endl;
  gridmap<<"#"<<endl;
  gridmap<<"#"<<endl;
  gridmap<<"# Units are in cgs"<<endl;
  gridmap<<"#"<<endl;
  gridmap<<"AUTHOR: Adlene Hicheur"<<endl;
  //  gridmap<<"PARAMETERS: 736285"<<endl;
  gridmap<<"PARAMETERS: 5727760"<<endl;
  gridmap<<"GEOMETRY"<<endl;
  gridmap<<"#   DX   DY   DZ  NX  NY  NZ   Z_OFFSET"<<endl;
  //  gridmap<<"   10.0 10.0 10.0 41  41  146  -50.0"<<endl;
  gridmap<<"   5.0 5.0 5.0 81  81  291  -50.0"<<endl;
  gridmap<<"#"<<endl;
  gridmap<<"#"<<endl;*/

  for ( double z = m_zMin; z <= m_zMax; z += m_step ) {
    for( double y = m_yMin; y <= m_yMax; y += m_step ) {
      for( double x = m_xMin; x <= m_xMax; x += m_step ) {
        Gaudi::XYZPoint P( x, y, z );
        
        //     bool vetoy30 = z>=470.0*Gaudi::Units::cm && z<510.0*Gaudi::Units::cm && fabs(y)>(fabs(z)*tan(0.25)- 300.0*Gaudi::Units::mm) && fabs(y)<fabs(z)*tan(0.25) && fabs(x) < fabs(z)*tan(0.3);

bool vetoy30 = fabs(y)<fabs(z)*tan(0.25) && fabs(x) < fabs(z)*tan(0.3);
        //   vetoy30=true;
        
        
        // get field at point P
        //    if (fabs(x)<fabs(z)*tan(0.3) && fabs(y)<(fabs(z)*tan(0.25)) && z<10000.0*Gaudi::Units::mm && !vetoy30) {     
/*        if(vetoy30 && z > m_zMin && z<1000*Gaudi::Units::cm) {
          
          m_pIAF->fieldVector( P, B );
           } else {
             m_pIMF->fieldVector( P, B );
             }*/
        m_pIMF->fieldVector( P, B );
          //     double dbx,dby,dbz;
          //     dbx=B.x()/Gaudi::Units::tesla-Ba.x()/Gaudi::Units::tesla;
          //     dby=B.y()/Gaudi::Units::tesla-Ba.y()/Gaudi::Units::tesla;
          //     dbz=B.z()/Gaudi::Units::tesla-Ba.z()/Gaudi::Units::tesla;
          //   }
        //     gridmap << setprecision(9) << B.x()/Gaudi::Units::tesla*1e4 << "  " << B.y()/Gaudi::Units::tesla*1e4 << "  " <<  B.z()/Gaudi::Units::tesla*1e4;
        //if (fabs(B.x())<1e-19 && fabs(B.y())<1e-19 && fabs(B.z())<1e-19) gridmap << " - "<<P.x()<<"  "<<P.y()<<"  "<<P.z();
        //    if ((B.x()/Gaudi::Units::tesla*1e4-1702.42041)<1e-7) gridmap << " - "<<P.x()<<"  "<<P.y()<<"  "<<P.z();
        //       if(isnan(B.x()) || isnan(B.y()) || isnan(B.x())) gridmap << " - "<<P.x()<<"  "<<P.y()<<"  "<<P.z();
//     gridmap<<endl;
        if(0) {
        //   gridmap << setprecision(9) << B.x()/Gaudi::Units::tesla*1e4 << "  " << B.y()/Gaudi::Units::tesla*1e4 << "  " <<  B.z()/Gaudi::Units::tesla*1e4;
          //    if(fabs(B.x()/Gaudi::Units::tesla*1e4)<1e-12 && fabs(B.y()/Gaudi::Units::tesla*1e4)<1e-12 && fabs(B.z()/Gaudi::Units::tesla*1e4)<1e-12) gridmap<<"  ANOM: "<< x << " " <<y<< " "<<z;
        //   gridmap<<endl;
        
        // fill ntuple
        nt1->column( "x", P.x()/Gaudi::Units::cm );
        nt1->column( "y", P.y()/Gaudi::Units::cm );
        nt1->column( "z", P.z()/Gaudi::Units::cm );
        nt1->column( "Bx", B.x()/Gaudi::Units::tesla );
        nt1->column( "By", B.y()/Gaudi::Units::tesla );
        nt1->column( "Bz", B.z()/Gaudi::Units::tesla );
        //  nt1->column( "dBx",dbx);
        //  nt1->column( "dBy",dby);
        //  nt1->column( "dBz",dbz);
 
        nt1->write();
        }
        
      }
    }

     Gaudi::XYZPoint P0( 0.0, 0.0, z);
   
      m_pIMF->fieldVector( P0, B );
  

      debug() << "Magnetic Field at ("
              << P0.x() << ", " << P0.y() << ", " << P0.z() << " ) = "
              << (B.x())/Gaudi::Units::tesla << ", "
              << (B.y())/Gaudi::Units::tesla << ", "
              << (B.z())/Gaudi::Units::tesla << " Tesla " 
              << endmsg;
      /*                 m_pIAF->fieldVector( P0, B );
      debug() << "Analytic Magnetic Field at ("
              << P0.x() << ", " << P0.y() << ", " << P0.z() << " ) = "
              << (B.x())/Gaudi::Units::tesla << ", "
              << (B.y())/Gaudi::Units::tesla << ", "
              << (B.z())/Gaudi::Units::tesla << " Tesla " 
              << endmsg;*/

  }

  //field value at the four probes:
  std::cout<<" ----- FIELD VALUE AT THE FOUR PROBES POSITIONS ----- "<<std::endl;
  std::cout<<"Probe 0: "<<std::endl;
    Gaudi::XYZPoint Pr0( 99.86*Gaudi::Units::cm, -132.47*Gaudi::Units::cm, 511.04*Gaudi::Units::cm);
      m_pIMF->fieldVector( Pr0, B );
      std::cout<<"Position: "<<Pr0.x()<<" * "<<Pr0.y()<<" * "<<Pr0.z()<<std::endl;
      std::cout<<"Field: "<<B.x()/Gaudi::Units::tesla<<" * "<<B.y()/Gaudi::Units::tesla<<" * "<<B.z()/Gaudi::Units::tesla<<std::endl;
std::cout<<"Probe 1: "<<std::endl;
    Gaudi::XYZPoint Pr1( 155.41*Gaudi::Units::cm, 82.20*Gaudi::Units::cm, 511.36*Gaudi::Units::cm); 
m_pIMF->fieldVector( Pr1, B );
      std::cout<<"Position: "<<Pr1.x()<<" * "<<Pr1.y()<<" * "<<Pr1.z()<<std::endl;
      std::cout<<"Field: "<<B.x()/Gaudi::Units::tesla<<" * "<<B.y()/Gaudi::Units::tesla<<" * "<<B.z()/Gaudi::Units::tesla<<std::endl;
std::cout<<"Probe 2: "<<std::endl;
    Gaudi::XYZPoint Pr2( 100.36*Gaudi::Units::cm, 131.96*Gaudi::Units::cm, 511.13*Gaudi::Units::cm); 
m_pIMF->fieldVector( Pr2, B );
      std::cout<<"Position: "<<Pr2.x()<<" * "<<Pr2.y()<<" * "<<Pr2.z()<<std::endl;
      std::cout<<"Field: "<<B.x()/Gaudi::Units::tesla<<" * "<<B.y()/Gaudi::Units::tesla<<" * "<<B.z()/Gaudi::Units::tesla<<std::endl;
std::cout<<"Probe 3: "<<std::endl;
    Gaudi::XYZPoint Pr3( -154.74*Gaudi::Units::cm, 83.02*Gaudi::Units::cm, 511.34*Gaudi::Units::cm); 
m_pIMF->fieldVector( Pr3, B );
       std::cout<<"Position: "<<Pr3.x()<<" * "<<Pr3.y()<<" * "<<Pr3.z()<<std::endl;
      std::cout<<"Field: "<<B.x()/Gaudi::Units::tesla<<" * "<<B.y()/Gaudi::Units::tesla<<" * "<<B.z()/Gaudi::Units::tesla<<std::endl;



  // gridmap.close();

  // Return status code.
  return StatusCode::SUCCESS;
}


StatusCode MagFieldReader::finalize() {

  StatusCode sc = GaudiTupleAlg::finalize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
if ( sc.isSuccess() )
    info() << "Service finalized successfully" << endmsg;
  return StatusCode::SUCCESS;
};


void MagFieldReader::TestBdl() 
{
  
Tuple nt2 = nTuple( 20, "Field Integral", CLID_ColumnWiseTuple );

  IBIntegrator* bIntegrator = tool<IBIntegrator>("BIntegrator");

  Gaudi::XYZPoint start(0,0,0);
  Gaudi::XYZPoint stop(0,0,9000/Gaudi::Units::mm); // start and end points
  double sigtx(0.3);
  double sigty(0.25); // slopes at start
  double zC; // z centre of field returned by tool
  Gaudi::XYZVector bdl;

  // random number generation
  Rndm::Numbers gausstx(randSvc(),Rndm::Gauss(0.,sigtx/2));
  Rndm::Numbers gaussty(randSvc(),Rndm::Gauss(0.,sigty/2));
  
  for (int i=0;i<m_nInt;i++) {
    double tx = gausstx();
    double ty = gaussty();
    if (fabs(tx) < sigtx && fabs(ty) < sigty) {
      
      bIntegrator->calculateBdlAndCenter(start, stop, tx, ty, zC, bdl);
  
      nt2->column( "tx", tx);
      nt2->column( "ty", ty);
      nt2->column( "Bdlx", bdl.x());
      nt2->column( "Bdly", bdl.y());
      nt2->column( "Bdlz", bdl.z());
      nt2->column( "zCenter", zC);
      nt2->write();
    }
  }
  
  bIntegrator->release(); 
                          

  
}
