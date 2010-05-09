// $Id: FlatZSmearVertex.cpp,v 1.14 2010-05-09 17:05:24 gcorti Exp $
// Include files 

// local
#include "FlatZSmearVertex.h"

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/IRndmGenSvc.h" 
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/Vector4DTypes.h"

// from Event
#include "Event/HepMCEvent.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FlatZSmearVertex
//
// 2005-08-17 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( FlatZSmearVertex );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FlatZSmearVertex::FlatZSmearVertex( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
  : GaudiTool ( type, name , parent ) {
    declareInterface< IVertexSmearingTool >( this ) ;
    declareProperty( "SigmaX" , m_sigmaX = 0.100 * Gaudi::Units::mm ) ;
    declareProperty( "SigmaY" , m_sigmaY = 0.100 * Gaudi::Units::mm ) ;
    declareProperty( "MeanXat0" , m_meanX = 0. * Gaudi::Units::mm ) ;
    declareProperty( "MeanYat0" , m_meanY = 0. * Gaudi::Units::mm ) ;
    declareProperty( "ZMin"   , m_zmin   = -1500. * Gaudi::Units::mm ) ;
    declareProperty( "ZMax"   , m_zmax   =  1500. * Gaudi::Units::mm ) ;

    declareProperty( "HorizontalCrossingAngle", m_hXAngle = 0. * Gaudi::Units::milliradian ) ;
    declareProperty( "VerticalCrossingAngle", m_vXAngle = 0. * Gaudi::Units::milliradian ) ;

    declareProperty( "Xcut" , m_xcut = 4. ) ; // times SigmaX 
    declareProperty( "Ycut" , m_ycut = 4. ) ; // times SigmaY

    declareProperty( "BeamDirection", m_zDir = 1 );

}

//=============================================================================
// Destructor 
//=============================================================================
FlatZSmearVertex::~FlatZSmearVertex( ) { ; }

//=============================================================================
// Initialize 
//=============================================================================
StatusCode FlatZSmearVertex::initialize( ) {
  StatusCode sc = GaudiTool::initialize( ) ;
  if ( sc.isFailure() ) return sc ;
  
  IRndmGenSvc * randSvc = svc< IRndmGenSvc >( "RndmGenSvc" , true ) ;
  sc = m_gaussDist.initialize( randSvc , Rndm::Gauss( 0. , 1. ) ) ;
  if ( ! sc.isSuccess() ) 
    return Error( "Could not initialize gaussian random number generator" ) ;
  if ( m_zmin > m_zmax ) return Error( "zMin > zMax !" ) ;
  sc = m_flatDist.initialize( randSvc , Rndm::Flat( m_zmin , m_zmax ) ) ;
  if ( ! sc.isSuccess() ) 
    return Error( "Could not initialize flat random number generator" ) ;

  std::string infoMsg = " applying TOF of interaction with ";
  if ( m_zDir == -1 ) {
    infoMsg = infoMsg + "negative beam direction";
  } else if ( m_zDir == 1 ) {
    infoMsg = infoMsg + "positive beam direction";
  } else if ( m_zDir == 0 ) {
    infoMsg = " with TOF of interaction equal to zero ";
  } else {
    return Error("BeamDirection can only be set to -1 or 1, or 0 to switch off TOF");
  }

  info() << "Smearing of interaction point with transverse Gaussian "
         << " distribution " << endmsg;
  info() << infoMsg << endmsg;
  info() << "and flat longitudinal z distribution" << endmsg;
  info() << " with Xmean, Ymean at z=0 = " << m_meanX / Gaudi::Units::mm
         << " mm,  " << m_meanY / Gaudi::Units::mm << " and " << endmsg;
  if( msgLevel(MSG::DEBUG) ) {
 
    debug() << " with sigma(X) = " << m_sigmaX / Gaudi::Units::mm 
            << " mm troncated at " << m_xcut << " sigma(X)" << endmsg ;
    debug() << " with sigma(Y) = " << m_sigmaY / Gaudi::Units::mm 
            << " mm troncated at " << m_ycut << " sigma(Y)" << endmsg ;
    debug() << " with z between " << m_zmin / Gaudi::Units::mm 
            << " mm and " << m_zmax / Gaudi::Units::mm << " mm." << endmsg ;
  } else {
    info() << " with sigma(X,Y) = " << m_sigmaX / Gaudi::Units::mm << " mm, "
           << m_sigmaY / Gaudi::Units::mm << " mm, and "
           << m_zmin / Gaudi::Units::mm << " mm <= z <= " 
           << m_zmax / Gaudi::Units::mm << " mm and" << endmsg;
    info() << "horizontal and vertical crossing angles = " 
           << m_hXAngle/Gaudi::Units::milliradian << "mrad , "
           << m_vXAngle/Gaudi::Units::milliradian << "mrad." 
           << endmsg;

  }

  release( randSvc ) ;
 
  return sc ;
}
 
//=============================================================================
// Smearing function
//=============================================================================
StatusCode FlatZSmearVertex::smearVertex( LHCb::HepMCEvent * theEvent ) {
  double dx , dy , dz , dt;
  
  dz = m_flatDist( ) ;
  dt = m_zDir * dz/Gaudi::Units::c_light;

  do { dx = m_gaussDist( ) ; } while ( fabs( dx ) > m_xcut ) ;
  dx = dx * m_sigmaX ;
  do { dy = m_gaussDist( ) ; } while ( fabs( dy ) > m_ycut ) ;
  dy = dy * m_sigmaY ;

  // take into account mean at z=0 and crossing angle
  dx = dx/cos(m_hXAngle) + m_meanX + dz*sin(m_hXAngle)*m_zDir;
  dy = dy/cos(m_vXAngle) + m_meanY + dz*sin(m_vXAngle)*m_zDir;

  Gaudi::LorentzVector dpos( dx , dy , dz , dt ) ;
  
  HepMC::GenEvent::vertex_iterator vit ;
  HepMC::GenEvent * pEvt = theEvent -> pGenEvt() ;
  for ( vit = pEvt -> vertices_begin() ; vit != pEvt -> vertices_end() ; 
        ++vit ) {
    Gaudi::LorentzVector pos ( (*vit) -> position() ) ;
    pos += dpos ;
    (*vit) -> set_position( HepMC::FourVector( pos.x() , pos.y() , 
                                               pos.z() , pos.t() ) ) ;
  }

  return StatusCode::SUCCESS ;      
}

