// $Id: IdealStateCreator.cpp,v 1.8 2006-06-01 16:26:40 erodrigu Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

#include "GaudiKernel/IDataProviderSvc.h"

// from Event/LinkerEvent
#include "Linker/LinkedTo.h"
#include "Linker/LinkedFrom.h"

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

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
IdealStateCreator::IdealStateCreator( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent )
  : GaudiTool( type, name, parent )
{  
  // interfaces
  declareInterface<IIdealStateCreator>(this);

  // declare properties
  declareProperty( "Extrapolator",
                   m_extrapolatorName = "TrackMasterExtrapolator" );
  declareProperty( "ErrorX2",  m_eX2  = 2.e-5*Gaudi::Units::mm2 );
  declareProperty( "ErrorY2",  m_eY2  = 2.e-5*Gaudi::Units::mm2 );
  declareProperty( "ErrorTx2", m_eTx2 = 1.e-7                   );
  declareProperty( "ErrorTy2", m_eTy2 = 1.e-7                   );
  declareProperty( "ErrorP",   m_eP   = 0.005                   );  // dp/p

  m_dets.push_back( "Velo" );
  m_dets.push_back( "TT"   );
  m_dets.push_back( "IT"   );
  m_dets.push_back( "OT"   );
}

//=============================================================================
// Destructor
//=============================================================================
IdealStateCreator::~IdealStateCreator() {};

//=============================================================================
// Initialization
//=============================================================================
StatusCode IdealStateCreator::initialize()
{
  debug() << "==> Initialize" << endreq;

  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;

  // Retrieve extrapolator
  m_extrapolator = tool<ITrackExtrapolator>( m_extrapolatorName );

  return StatusCode::SUCCESS;
}

//=============================================================================
// Creates a state at a z position,
// from a MCParticle using the entry/exit points of the MCHits
//=============================================================================
StatusCode IdealStateCreator::createState( const MCParticle* mcPart,
                                           double zRec,
                                           State*& state ) const
{
  // Check if MCParticle exists
  if( mcPart == 0 ) return StatusCode::FAILURE;
  
  // First create the state
  TrackSymMatrix stateCov = TrackSymMatrix();
  State* pState = new State();
  pState -> setZ( zRec );
  pState -> setCovariance( stateCov );
  state = pState;

  MCHit* closestHit;
  findClosestHit( mcPart, zRec, closestHit );
  if( !closestHit ) {
    warning() << "No closest MCHit found!!" << endreq;
    return StatusCode::FAILURE;
  }

  XYZPoint beginPoint = closestHit -> entry();
  if( beginPoint.z() > closestHit->exit().z() )
    beginPoint = closestHit -> exit();
  double z = beginPoint.z();

  // determine Q/P
  double trueQOverP = qOverP( mcPart, closestHit );

  // set the state parameters
  pState -> setState(  beginPoint.x(),  beginPoint.y(), z,
                       closestHit -> dxdz(), closestHit -> dydz(),
                       trueQOverP );
  
  // set covariance matrix
  TrackSymMatrix cov = TrackSymMatrix();
  cov(0,0) = m_eX2;
  cov(1,1) = m_eY2;
  cov(2,2) = m_eTx2;
  cov(3,3) = m_eTy2;
  cov(4,4) = pow( m_eP * pState->qOverP(), 2. );
  pState -> setCovariance( cov );

  // extrapolate state to exact z position
  StatusCode sc = m_extrapolator -> propagate( *pState, zRec );
  if( sc.isFailure() ) {
    warning() << "Extrapolation of True State from z = "
              << z << " to z = " << zRec << " failed!" << endreq;
  }

  return sc;
}

//=============================================================================
// Creates a state at the origin vertex
// from a MCParticle using the entry/exit points of the MCHits
//=============================================================================
StatusCode IdealStateCreator::createStateVertex( const MCParticle* mcParticle,
                                                 State*& state ) const
{
  /// Create state at track vertex of MCParticle.
  TrackSymMatrix stateCov = TrackSymMatrix();
  State* trueState = new State();
  trueState -> setCovariance( stateCov );  
  state = trueState;

  // Check if MCParticle exists
  if( mcParticle == 0 ) return StatusCode::FAILURE;

  // retrieve true MC particle info
  const MCVertex* mcVertex   = mcParticle -> originVertex();
  const LorentzVector mc4Mom = mcParticle -> momentum();
  const XYZPoint mcPos       = mcVertex   -> position();


  // determine Q/P
  double trueQOverP = qOverP( mcParticle );

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
  trueState -> setState( mcPos.x(), mcPos.y(), mcPos.z(),
                         trueTx, trueTy, trueQOverP );

  // set covariance matrix
  TrackSymMatrix cov = TrackSymMatrix();
  cov(0,0) = m_eX2;
  cov(1,1) = m_eY2;
  cov(2,2) = m_eTx2;
  cov(3,3) = m_eTy2;
  cov(4,4) = pow( m_eP * trueState->qOverP(), 2. );
  trueState -> setCovariance( cov );

  return StatusCode::SUCCESS;
}

//=============================================================================
// Find the z-closest MCHit associated to an MCParticle
// looping over the hits in all the tracking detectors
//=============================================================================
void IdealStateCreator::findClosestHit( const MCParticle* mcPart,
                                        const double zRec,
                                        MCHit*& closestHit ) const
{
  MCHit* tmpClosestHit;
  std::vector<std::string>::const_iterator itDets;

  for( itDets = m_dets.begin(); itDets < m_dets.end(); ++itDets ) {
    findClosestXxxHit( mcPart, zRec,
                       MCParticleLocation::Default + "2MC" + *itDets + "Hits",
                       tmpClosestHit );
    if ( itDets == m_dets.begin() ) closestHit = tmpClosestHit;
    if ( fabs( tmpClosestHit -> midPoint().z() - zRec ) <
         fabs( closestHit -> midPoint().z() - zRec ) ) {
      closestHit = tmpClosestHit;
    }
  }
}

//=============================================================================
// Find the z-closest MCHit of type Xxx associated to an MCParticle
//=============================================================================
void IdealStateCreator::findClosestXxxHit( const MCParticle* mcPart,
                                           const double zRec,
                                           std::string linkPath,
                                           MCHit*& closestHit ) const
{
  // Retrieve MCParticle to MCHit linker tables
  LinkedFrom<MCHit,MCParticle> mcp2mchitLink( evtSvc(), msgSvc(), linkPath );

  double closestZ = 10000;

  MCHit* aMCHit = mcp2mchitLink.first( mcPart );
  while( 0 != aMCHit ) {
    double ZOfMidPoint = aMCHit -> midPoint().z();
    // get the closest hit
    if ( fabs( ZOfMidPoint - zRec ) < closestZ ) {
      closestHit = aMCHit;
      closestZ   = fabs( ZOfMidPoint - zRec );
    }
    aMCHit = mcp2mchitLink.next();
  }
}

//=============================================================================
// Determine Q/P for a MCParticle using the P from the MCHit if available
//=============================================================================
double IdealStateCreator::qOverP( const MCParticle* mcPart,
                                  const LHCb::MCHit* mcHit ) const
{
  double momentum = mcPart -> p();
  if ( mcHit != NULL ) momentum = mcHit -> p();
  double charge = ( mcPart -> particleID().threeCharge() ) / 3.;
  // TODO: hack as it happens that some MCHits have p = 0!
  if ( mcHit != NULL &&  mcHit -> p() == 0. ) momentum = mcPart -> p();
  if( momentum > TrackParameters::lowTolerance ) {
    return charge / momentum;
  }
  else { return 0.; }
}

//=============================================================================
