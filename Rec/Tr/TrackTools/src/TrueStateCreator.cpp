// Include files
// -------------
// from Gaudi
#include "GaudiKernel/ToolFactory.h"

#include "GaudiKernel/IDataProviderSvc.h"

// from Event
#include "Event/State.h"
#include "Event/TrackParameters.h"

// local
#include "TrackTools/TrueStateCreator.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TrueStateCreator
//
// 2005-04-06 : Eduardo Rodrigues (adaptations to new track event model)
//
//  3-7-2002: Rutger van der Eijk, Jeroen van Tilburg
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<TrueStateCreator>          s_factory;
const        IToolFactory& TrueStateCreatorFactory = s_factory;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrueStateCreator::TrueStateCreator( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
  : GaudiTool( type, name, parent )
  , m_velo(0)
  , m_p2VeloHitAsct(0)
  , m_p2ITHitAsct(0)
  , m_p2OTHitAsct(0)
  , m_extrapolator(0)
  , m_p2VeloHitAsctName("")
  , m_p2ITHitAsctName("")
  , m_p2OTHitAsctName("")
  , m_eX2(2.e-5*mm2)
  , m_eY2(2.e-5*mm2)
  , m_eTx2(1.e-7)
  , m_eTy2(1.e-7)
  , m_eP(0.005)  
{  
  // interfaces
  declareInterface<IStateCreator>(this);

  // declare properties
  declareProperty( "MCP2VeloMCHitAscName",
                   m_p2VeloHitAsctName = "MCP2VeloMCHitAsc" );
  declareProperty( "MCP2ITMCHitAscName",
                   m_p2ITHitAsctName = "MCP2ITMCHitAsc" );
  declareProperty( "MCP2OTMCHitAscName",
                   m_p2OTHitAsctName = "MCP2OTMCHitAsc" );
  declareProperty( "extrapolatorName", 
                   m_extrapolatorName = "TrFirstCleverExtrapolator" );
  declareProperty( "eX2",  m_eX2 );
  declareProperty( "eY2",  m_eY2 );
  declareProperty( "eTx2", m_eTx2 );
  declareProperty( "eTy2", m_eTy2 );
  declareProperty( "eP",   m_eP );  // dp/p
}

//=============================================================================
// Destructor
//=============================================================================
TrueStateCreator::~TrueStateCreator() {};

//=============================================================================
// Initialization
//=============================================================================
StatusCode TrueStateCreator::initialize()
{
  /// TrueStateCreator initialise
  StatusCode sc;

  std::string ascType     = "Associator<MCParticle,MCHit>";
  std::string ascVeloType = "Associator<MCParticle,MCVeloHit>";

  // Retrieve MCParticle 2 Velo MCHit associator
  sc = toolSvc() -> retrieveTool( ascVeloType, 
                                  m_p2VeloHitAsctName, m_p2VeloHitAsct );
  if ( sc.isFailure() ) { 
    error() << "Unable to retrieve the Velo MCHit Associator " 
            << m_p2VeloHitAsctName << endreq;
    return sc;
  }

  // Retrieve MCParticle 2 OT MCHit associator
  sc = toolSvc() -> retrieveTool( ascType, m_p2OTHitAsctName, m_p2OTHitAsct );
  if ( sc.isFailure() ) { 
    error() << "Unable to retrieve the OT MCHit Associator " 
            << m_p2OTHitAsctName << endreq;
    return sc; 
  }

  // Retrieve MCParticle 2 IT MCHit associator
  sc = toolSvc()->retrieveTool(ascType, m_p2ITHitAsctName, m_p2ITHitAsct);
  if ( sc.isFailure() ) {
    error() << "Unable to retrieve the IT MCHit Associator " 
            << m_p2ITHitAsctName << endreq;
    return sc;
  }

  // Retrieve extrapolator
  m_extrapolator = tool<ITrackExtrapolator>( m_extrapolatorName );

  return StatusCode::SUCCESS;
}

//=============================================================================
// Creates a state at a z position,
// from a MCParticle using the entry/exit points of the MCHits
//=============================================================================
StatusCode TrueStateCreator::createState( const MCParticle* mcPart,
                                          double zRec,
                                          State*& state ) const {
  // First create the state
  HepSymMatrix stateCov = HepSymMatrix(5, 1);
  State* pState = new State();
  pState -> setZ( zRec );
  pState -> setCovariance( stateCov );
  state = pState;
   
  // Check if MCParticle exists
  if ( mcPart == 0 ) return StatusCode::FAILURE;

  // check for associators
  if ( !m_p2VeloHitAsct || !m_p2OTHitAsct || !m_p2ITHitAsct )
    return Error( "MCParticle 2 MCHit Associator missing." );

  MCHit* closestHit = 0;
  MCHit* secondClosestHit = 0;
  double closestZ = 10000;
  double secondClosestZ = 10000;

  // loop over associated Velo MCHits and find closest hits
  MCVeloHitAsct::ToRange mcVeloHitsRange = m_p2VeloHitAsct->rangeFrom(mcPart);
  MCVeloHitAsct::ToIterator vt;
  for ( vt = mcVeloHitsRange.begin(); vt != mcVeloHitsRange.end(); ++vt) { 
    // retrieve MCHit
    MCVeloHit* mcVeloHit = vt->to();
    if ( !mcVeloHit )
      return Error( "Failed retrieving Velo MCHit" );

    // calculate center point
    // workaround for CLHEP 1.9!
    HepPoint3D midPointTmp = ( mcVeloHit->entry() + mcVeloHit->exit() ) / 2.0;
    Hep3Vector midPoint = ( midPointTmp.x(), midPointTmp.y(), midPointTmp.z() );
    //Hep3Vector midPoint = (mcVeloHit->entry() + mcVeloHit->exit())/2.0;

    // get the closest and second closest hits
    if ( fabs(midPoint.z() - zRec) < closestZ ) {
      secondClosestHit = closestHit;
      secondClosestZ   = closestZ;
      closestHit       = mcVeloHit;
      closestZ         = fabs(midPoint.z()- zRec);
    }
    else if ( fabs(midPoint.z()- zRec) < secondClosestZ ) {
      secondClosestHit = mcVeloHit;
      secondClosestZ   = fabs(midPoint.z()- zRec);
    }
  }
  
  // loop over associated OT MCHits and find closest hits
  MCHitAsct::ToRange mcOTHitsRange = m_p2OTHitAsct->rangeFrom(mcPart);
  MCHitAsct::ToIterator ot;
  for ( ot = mcOTHitsRange.begin(); ot != mcOTHitsRange.end(); ++ot) { 
    // retrieve MCHit
    MCHit* mcHit = ot->to();
    if ( !mcHit )
      return Error( "Failed retrieving OT MCHit" );

    // calculate center point
    // workaround for CLHEP 1.9!
    HepPoint3D midPointTmp = ( mcHit->entry() + mcHit->exit() ) / 2.0;
    Hep3Vector midPoint = ( midPointTmp.x(), midPointTmp.y(), midPointTmp.z() );
    //Hep3Vector midPoint = (mcHit->entry() + mcHit->exit())/2.0;

    // get the closest and second closest hits
    if ( fabs(midPoint.z() - zRec) < closestZ ) {
      secondClosestHit = closestHit;
      secondClosestZ   = closestZ;
      closestHit       = mcHit;
      closestZ         = fabs(midPoint.z()- zRec);
    }
    else if ( fabs(midPoint.z()- zRec) < secondClosestZ ) {
      secondClosestHit = mcHit;
      secondClosestZ   = fabs(midPoint.z()- zRec);
    }
  }

  // loop over associated IT MCHits and find closest hits
  MCHitAsct::ToRange mcITHitsRange = m_p2ITHitAsct->rangeFrom(mcPart);
  MCHitAsct::ToIterator it;
  for ( it = mcITHitsRange.begin(); it != mcITHitsRange.end(); ++it) { 
    // retrieve MCHit
    MCHit* mcHit = it->to();
    if ( !mcHit )
      return Error( "Failed retrieving IT MCHit" );

    // calculate center point
    // workaround for CLHEP 1.9!
    HepPoint3D midPointTmp = ( mcHit->entry() + mcHit->exit() ) / 2.0;
    Hep3Vector midPoint = ( midPointTmp.x(), midPointTmp.y(), midPointTmp.z() );
    //Hep3Vector midPoint = (mcHit->entry() + mcHit->exit())/2.0;

    // get the closest and second closest hits
    if ( fabs(midPoint.z()- zRec) < closestZ ) {
      secondClosestHit = closestHit;
      secondClosestZ   = closestZ;
      closestHit       = mcHit;
      closestZ         = fabs(midPoint.z()- zRec);
    }
    else if ( fabs(midPoint.z()- zRec) < secondClosestZ ) {
      secondClosestHit = mcHit;
      secondClosestZ = fabs(midPoint.z()- zRec);
    }

  }

  if ( !closestHit || !secondClosestHit ) {
    warning() << "No two closest MCHits found!!" << endreq;
    return StatusCode::FAILURE;
  }
  
  // Find beginPoint (smallest z-value)
  Hep3Vector beginPoint = closestHit->entry();
  if ( beginPoint.z() > closestHit->exit().z() )
    beginPoint = closestHit->exit();
  if ( beginPoint.z() > secondClosestHit->entry().z() )
    beginPoint = secondClosestHit->entry();
  if ( beginPoint.z() > secondClosestHit->exit().z() )
    beginPoint = secondClosestHit->exit();

  // Find endPoint (highest z-value)
  Hep3Vector endPoint = secondClosestHit->exit();
  if ( endPoint.z() < secondClosestHit->entry().z() )
    endPoint = secondClosestHit->entry();
  if ( endPoint.z() < closestHit->entry().z() )
    endPoint = closestHit->entry();
  if ( endPoint.z() < closestHit->exit().z() )
    endPoint = closestHit->exit();

  double dz = (beginPoint.z() - endPoint.z());
  if ( dz == 0. ) {
    warning() << "Delta z between two hits equals zero." << endreq;
    return StatusCode::FAILURE;
  }
  double slopeX = (beginPoint.x() - endPoint.x()) / dz;
  double slopeY = (beginPoint.y() - endPoint.y()) / dz;
  double x = (beginPoint.x()); 
  double y = (beginPoint.y());
  double z = (beginPoint.z());

  // set the state parameters
  pState->setState( x, y, z, slopeX, slopeY, this->qOverP(mcPart) );
  
  // set covariance matrix
  HepSymMatrix cov = HepSymMatrix(5,0);
  cov.fast(1,1) = m_eX2;
  cov.fast(2,2) = m_eY2;
  cov.fast(3,3) = m_eTx2;
  cov.fast(4,4) = m_eTy2;
  cov.fast(5,5) = pow(m_eP * pState->qOverP(), 2.);
  pState -> setCovariance( cov );

  // extrapolate state to exact z position
  ParticleID partID = 211; // pion
  StatusCode sc = m_extrapolator -> propagate( *pState, zRec, partID );
  if ( sc.isFailure() ) {
    debug() << "Extrapolation of True State from z = "
            << z << " to z = " << zRec << " failed!" << endreq;
  }

  return sc;
}

//=============================================================================
// Creates a state at the origin vertex
// from a MCParticle using the entry/exit points of the MCHits
//=============================================================================
StatusCode TrueStateCreator::createStateVertex( const MCParticle* mcParticle,
                                                State*& state ) const
{
  /// Create state at track vertex of MCParticle.
  HepSymMatrix stateCov = HepSymMatrix(5, 1);
  State* trueState = new State();
  trueState -> setCovariance( stateCov );  
  state = trueState;

  // Check if MCParticle exists
  if ( mcParticle == 0 ) return StatusCode::FAILURE;

  // retrieve true MC particle info
  const MCVertex* mcVertex      = mcParticle -> originVertex();
  const HepPoint3D mcPos        = mcVertex -> position();
  const HepLorentzVector mc4Mom = mcParticle -> momentum();

  // determine QdivP
  double trueQdivP = this -> qOverP( mcParticle );

  // determine slopes
  double trueTx = 99999999;
  double trueTy = 99999999;
  double px = mc4Mom.px();
  double py = mc4Mom.py();
  double pz = mc4Mom.pz();
  if ( fabs(pz) > TrackParameters::lowTolerance ) {
    trueTx = px/pz;
    trueTy = py/pz;
  }
  else {
    warning() << "No momentum z component." << endreq;
    return StatusCode::FAILURE;
  }

  // construct true State
  trueState->setState( mcPos.x(),
                       mcPos.y(),
                       mcPos.z(),
                       trueTx,
                       trueTy,
                       trueQdivP );

  // set covariance matrix
  HepSymMatrix cov = HepSymMatrix(5,0);
  cov.fast(1,1) = m_eX2;
  cov.fast(2,2) = m_eY2;
  cov.fast(3,3) = m_eTx2;
  cov.fast(4,4) = m_eTy2;
  cov.fast(5,5) = pow(m_eP * trueState->qOverP(), 2.);
  trueState -> setCovariance( cov );

  return StatusCode::SUCCESS;
}

//=============================================================================
// Determine Q/P for a MCParticle
//=============================================================================
double TrueStateCreator::qOverP( const MCParticle* mcPart ) const
{
  /// Determine Q/P for a MCParticle
  double momentum = mcPart -> momentum().vect().mag();
  double charge = (mcPart -> particleID().threeCharge())/3.;
  if ( momentum > TrackParameters::lowTolerance) {
    return charge/momentum;
  }
  else {
    return 0.0;
  }
}

//=============================================================================
