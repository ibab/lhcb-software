// $Id: MagFieldReader.cpp,v 1.5 2007-02-26 14:50:47 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IMagneticFieldSvc.h"
#include "GaudiKernel/SystemOfUnits.h"

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
  declareProperty("Xmin", m_yMin =     0.0*Gaudi::Units::mm);  
  declareProperty("Ymax", m_yMax =  4000.0*Gaudi::Units::mm);
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
        nt1->column( "x", P.x()/Gaudi::Units::cm );
        nt1->column( "y", P.y()/Gaudi::Units::cm );
        nt1->column( "z", P.z()/Gaudi::Units::cm );
        nt1->column( "Bx", B.x()/Gaudi::Units::gauss );
        nt1->column( "By", B.y()/Gaudi::Units::gauss );
        nt1->column( "Bz", B.z()/Gaudi::Units::gauss );
        nt1->write();
      }
    }
    Gaudi::XYZPoint P0( 0.0, 0.0, z );
    m_pIMF->fieldVector( P0, B );
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
