// $Id: IdealStateCreator.cpp,v 1.3 2006-03-03 14:18:25 ebos Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

#include "GaudiKernel/IDataProviderSvc.h"

// from Event/LinkerEvent
#include "Linker/LinkedTo.h"

// from Event
#include "Event/MCParticle.h"
#include "Event/MCHit.h"
#include "Event/State.h"
#include "Event/TrackParameters.h"

// local
#include "IdealStateCreator.h"

using namespace Gaudi;
using namespace LHCb;

// Declaration of the Tool Factory
static const  ToolFactory<IdealStateCreator>          s_factory;
const        IToolFactory& IdealStateCreatorFactory = s_factory;

/// Standard constructor, initializes variables
IdealStateCreator::IdealStateCreator( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent )
  : GaudiTool( type, name, parent )
  , m_eX2(2.e-5*mm2)
  , m_eY2(2.e-5*mm2)
  , m_eTx2(1.e-7)
  , m_eTy2(1.e-7)
  , m_eP(0.005)  
{  
  // interfaces
  declareInterface<IIdealStateCreator>(this);

  // declare properties
  declareProperty( "extrapolatorName", 
                   m_extrapolatorName = "TrackHerabExtrapolator" );
  declareProperty( "eX2",  m_eX2 );
  declareProperty( "eY2",  m_eY2 );
  declareProperty( "eTx2", m_eTx2 );
  declareProperty( "eTy2", m_eTy2 );
  declareProperty( "eP",   m_eP );  // dp/p
}

/// Destructor
IdealStateCreator::~IdealStateCreator() {};

/// Initialization
StatusCode IdealStateCreator::initialize()
{
  debug() << "==> Initialize" << endreq;

  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;

  // Retrieve extrapolator
  m_extrapolator = tool<ITrackExtrapolator>( m_extrapolatorName );

  return StatusCode::SUCCESS;
}

/// Creates a state at a z position,
/// from a MCParticle using the entry/exit points of the MCHits
StatusCode IdealStateCreator::createState( const MCParticle* mcPart,
                                           double zRec,
                                           LHCb::State*& state ) const {

  // Retrieve MCParticle to MCHit linker table
  LinkedTo<MCHit,MCParticle> mcp2mchLink( evtSvc(), msgSvc(),
                                          MCParticleLocation::Default+"2MCHits" );
  if( mcp2mchLink.notFound() ) {
    error() << "Unable to retrieve MCParticle to MCHit Linker table." << endreq;
  }

  // First create the state
  TrackMatrix stateCov = TrackMatrix();
  for( int i=0; i<5; ++i ) { stateCov(i,i) = 0.; }
  LHCb::State* pState = new LHCb::State();
  pState -> setZ( zRec );
  pState -> setCovariance( stateCov );
  state = pState;

  // Check if MCParticle exists
  if( mcPart == 0 ) return StatusCode::FAILURE;

  MCHit* closestHit = 0;
  MCHit* secondClosestHit = 0;
  double closestZ = 10000;
  double secondClosestZ = 10000;

  // Use first link with highest weight.
  MCHit* aMCHit = mcp2mchLink.first( mcPart );
  if( 0 == aMCHit ) return Error( "No MCHit linked to this MCParticle" );
  while( 0 != aMCHit ) {
    // calculate center point
    XYZPoint midPoint = aMCHit->midPoint();

    // get the closest and second closest hits
    if ( fabs( midPoint.z() - zRec ) < closestZ ) {
      secondClosestHit = closestHit;
      secondClosestZ   = closestZ;
      closestHit       = aMCHit;
      closestZ         = fabs( midPoint.z()- zRec );
    }
    else if ( fabs( midPoint.z()- zRec ) < secondClosestZ ) {
      secondClosestHit = aMCHit;
      secondClosestZ   = fabs( midPoint.z()- zRec );
    }
    aMCHit = mcp2mchLink.next();
  }
  
  if( !closestHit || !secondClosestHit ) {
    warning() << "No two closest MCHits found!!" << endreq;
    return StatusCode::FAILURE;
  }
  
  // Find beginPoint (smallest z-value)
  XYZPoint beginPoint = closestHit->entry();
  if( beginPoint.z() > closestHit->exit().z() )
    beginPoint = closestHit->exit();
  if( beginPoint.z() > secondClosestHit->entry().z() )
    beginPoint = secondClosestHit->entry();
  if( beginPoint.z() > secondClosestHit->exit().z() )
    beginPoint = secondClosestHit->exit();

  // Find endPoint (highest z-value)
  XYZPoint endPoint = secondClosestHit->exit();
  if( endPoint.z() < secondClosestHit->entry().z() )
    endPoint = secondClosestHit->entry();
  if( endPoint.z() < closestHit->entry().z() )
    endPoint = closestHit->entry();
  if( endPoint.z() < closestHit->exit().z() )
    endPoint = closestHit->exit();

  double dz = ( beginPoint.z() - endPoint.z() );
  if( dz == 0. ) {
    warning() << "Delta z between two hits equals zero." << endreq;
    return StatusCode::FAILURE;
  }
  double slopeX = ( beginPoint.x() - endPoint.x() ) / dz;
  double slopeY = ( beginPoint.y() - endPoint.y() ) / dz;
  double x = ( beginPoint.x() ); 
  double y = ( beginPoint.y() );
  double z = ( beginPoint.z() );

  // set the state parameters
  pState->setState( x, y, z, slopeX, slopeY, this->qOverP( mcPart ) );

  // set covariance matrix
  TrackMatrix cov = TrackMatrix();
  cov(0,0) = m_eX2;
  cov(1,1) = m_eY2;
  cov(2,2) = m_eTx2;
  cov(3,3) = m_eTy2;
  cov(4,4) = pow(m_eP * pState->qOverP(), 2.);
  pState -> setCovariance( cov );

  // extrapolate state to exact z position
  ParticleID partID = 211; // pion
  StatusCode sc = m_extrapolator -> propagate( *pState, zRec, partID );
  if( sc.isFailure() ) {
    debug() << "Extrapolation of True State from z = "
            << z << " to z = " << zRec << " failed!" << endreq;
  }

  return sc;
}

/// Creates a state at the origin vertex
/// from a MCParticle using the entry/exit points of the MCHits
StatusCode IdealStateCreator::createStateVertex( const MCParticle* mcParticle,
                                                 LHCb::State*& state ) const
{
  /// Create state at track vertex of MCParticle.
  TrackMatrix stateCov = TrackMatrix();
  for( int i=0; i<5; ++i ) { stateCov(i,i) = 0.; }
  LHCb::State* trueState = new State();
  trueState -> setCovariance( stateCov );  
  state = trueState;

  // Check if MCParticle exists
  if( mcParticle == 0 ) return StatusCode::FAILURE;

  // retrieve true MC particle info
  const MCVertex* mcVertex = mcParticle -> originVertex();
  const XYZPoint mcPos = mcVertex -> position();
  const LorentzVector mc4Mom = mcParticle -> momentum();

  // determine QdivP
  double trueQdivP = this -> qOverP( mcParticle );

  // determine slopes
  double trueTx = 99999999;
  double trueTy = 99999999;
  double px = mc4Mom.px();
  double py = mc4Mom.py();
  double pz = mc4Mom.pz();
  if ( fabs(pz) > TrackParameters::lowTolerance ) {
    trueTx = px / pz;
    trueTy = py / pz;
  }
  else {
    warning() << "No momentum z component." << endreq;
    return StatusCode::FAILURE;
  }

  // construct true State
  trueState->setState( mcPos.x(), mcPos.y(), mcPos.z(),
                       trueTx, trueTy, trueQdivP );

  // set covariance matrix
  TrackMatrix cov = TrackMatrix();
  cov(0,0) = m_eX2;
  cov(1,1) = m_eY2;
  cov(2,2) = m_eTx2;
  cov(3,3) = m_eTy2;
  cov(4,4) = pow(m_eP * trueState->qOverP(), 2.);
  trueState -> setCovariance( cov );

  return StatusCode::SUCCESS;
}

/// Determine Q/P for a MCParticle
double IdealStateCreator::qOverP( const MCParticle* mcPart ) const
{
  /// Determine Q/P for a MCParticle
  double momentum = mcPart -> momentum().M();
  double charge = (mcPart -> particleID().threeCharge()) / 3.;
  if( momentum > TrackParameters::lowTolerance ) {
    return charge / momentum;
  }
  else { return 0.0; }
}
