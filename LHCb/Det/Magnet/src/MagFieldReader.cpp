// $Id: MagFieldReader.cpp,v 1.3 2006-01-20 16:24:11 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IMagneticFieldSvc.h"

// from LHCbDefinitions
#include "Kernel/Vector3DTypes.h"
#include "Kernel/Point3DTypes.h"
#include "Kernel/PhysicalConstants.h"

// local
#include "MagFieldReader.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MagFieldReader
//
// 08/05/2002 : Edgar De Oliveira
// 16/03/2004 : Gloria Corti, modified to fill ntuple
//-----------------------------------------------------------------------------
// Declaration of the Algorithm Factory
static const  AlgFactory<MagFieldReader>          s_factory ;
const        IAlgFactory& MagFieldReaderFactory = s_factory ;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MagFieldReader::MagFieldReader( const std::string& name,
                    ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator )
  , m_pIMF(0) 
{
  declareProperty("Zmin", m_zMin =  -500.0*mm);
  declareProperty("Zmax", m_zMax = 14000.0*mm);  
  declareProperty("Step", m_step =   100.0*mm);  
  declareProperty("Xmin", m_xMin =     0.0*mm);  
  declareProperty("Xmax", m_xMax =  4000.0*mm);
  declareProperty("Xmin", m_yMin =     0.0*mm);  
  declareProperty("Ymax", m_yMax =  4000.0*mm);
}

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode MagFieldReader::initialize() {

  StatusCode sc = GaudiTupleAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
  debug() << "FieldReader intialize() has been called" << endmsg;
  
  m_pIMF = svc<IMagneticFieldSvc>( "MagneticFieldSvc", true );

  info() << "MagFieldReader initialized" << endmsg;
  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode MagFieldReader::execute() {
  
  // Print out info messages with the field value at different locations.

  debug() << "m_pIMF = " << m_pIMF << endreq;

  Tuple nt1 = nTuple( 10, "Field", CLID_ColumnWiseTuple );

  Gaudi::XYZVector B(0.0,0.0,0.0);
  for ( double z = m_zMin; z <= m_zMax; z += m_step ) {
    for( double y = m_yMin; y <= m_yMax; y += m_step ) {
      for( double x = m_xMin; x <= m_xMax; x += m_step ) {
        Gaudi::XYZPoint P( x, y, z );
        // get field at point P
        m_pIMF->fieldVector( P, B );
        // fill ntuple
        nt1->column( "x", P.x()/cm );
        nt1->column( "y", P.y()/cm );
        nt1->column( "z", P.z()/cm );
        nt1->column( "Bx", B.x()/gauss );
        nt1->column( "By", B.y()/gauss );
        nt1->column( "Bz", B.z()/gauss );
        nt1->write();
      }
    }
    Gaudi::XYZPoint P0( 0.0, 0.0, z );
    m_pIMF->fieldVector( P0, B );
    debug() << "Magnetic Field at ("
        << P0.x() << ", " << P0.y() << ", " << P0.z() << " ) = "
        << (B.x())/tesla << ", "
        << (B.y())/tesla << ", "
        << (B.z())/tesla << " Tesla " 
        << endmsg;
  }

  // Return status code.
  return StatusCode::SUCCESS;
}
