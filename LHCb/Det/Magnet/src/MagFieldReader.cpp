// Include files 
// from Gaudi
#include "GaudiKernel/IMagneticFieldSvc.h"
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

DECLARE_ALGORITHM_FACTORY( MagFieldReader )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MagFieldReader::MagFieldReader( const std::string& name,
                    ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator )
  , m_pIMF(0)
//, m_pIAF(0)
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
  
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
    debug() << "FieldReader intialize() has been called" << endmsg;
  
  m_pIMF = svc<IMagneticFieldSvc>( m_FieldServiceName, true );
  // m_pIAF = svc<IMagneticFieldSvc>( "AnalyticFieldSvc", true );

  info() << "MagFieldReader initialized with service ==> " <<  m_FieldServiceName << endmsg;
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode MagFieldReader::execute() {

  if (m_testbdl) TestBdl();
  
  // Print out info messages with the field value at different locations.

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
    debug() << "m_pIMF = " << m_pIMF << endmsg;

  Tuple nt1 = nTuple( 10, "Field", CLID_ColumnWiseTuple );

  Gaudi::XYZVector B(0.0,0.0,0.0);



  for ( double z = m_zMin; z <= m_zMax; z += m_step ) {
    for( double y = m_yMin; y <= m_yMax; y += m_step ) {
      for( double x = m_xMin; x <= m_xMax; x += m_step ) {
        Gaudi::XYZPoint P( x, y, z );
        

          m_pIMF->fieldVector( P, B );

        
        
        
        // fill ntuple
        nt1->column( "x", P.x()/Gaudi::Units::cm );
        nt1->column( "y", P.y()/Gaudi::Units::cm );
        nt1->column( "z", P.z()/Gaudi::Units::cm );
        nt1->column( "Bx", B.x()/Gaudi::Units::tesla );
        nt1->column( "By", B.y()/Gaudi::Units::tesla );
        nt1->column( "Bz", B.z()/Gaudi::Units::tesla );
 
        nt1->write();
      }
    }

     Gaudi::XYZPoint P0( 0.0, 0.0, z);
     Gaudi::XYZPoint P02( 0.0, 0.0, z);
      m_pIMF->fieldVector( P0, B );
  
      if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
        debug() << "Magnetic Field at ("
                << P0.x() << ", " << P0.y() << ", " << P0.z() << " ) = "
                << (B.x())/Gaudi::Units::tesla << ", "
                << (B.y())/Gaudi::Units::tesla << ", "
                << (B.z())/Gaudi::Units::tesla << " Tesla " 
                << endmsg;
  }



  // Return status code.
  return StatusCode::SUCCESS;
}


StatusCode MagFieldReader::finalize() {
  StatusCode sc = GaudiTupleAlg::finalize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  info() << "Service finalized successfully" << endmsg;
  return StatusCode::SUCCESS;
}


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
  
  releaseTool(bIntegrator).ignore(); 
  
}
