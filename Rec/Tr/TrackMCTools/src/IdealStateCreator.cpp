// Include files

// from Gaudi
#include "GaudiKernel/IMagneticFieldSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/PhysicalConstants.h"

// from Event
#include "Event/MCParticle.h"
#include "Event/MCHit.h"
#include "Event/State.h"
#include "Event/StateVector.h"
#include "Event/TrackParameters.h"
#include "Event/Track.h"

// local
#include "IdealStateCreator.h"
#include <boost/assign/list_of.hpp>

#include "gsl/gsl_math.h"

using namespace Gaudi;
using namespace LHCb;

DECLARE_TOOL_FACTORY( IdealStateCreator )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
IdealStateCreator::IdealStateCreator( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent )
  : GaudiTool( type, name, parent ),
    m_configured(false)
{  
  // interfaces
  declareInterface<IIdealStateCreator>(this);

  // declare properties
  declareProperty( "Extrapolator",
                   m_extrapolatorName = "TrackParabolicExtrapolator" );
  declareProperty( "ErrorX2",  m_eX2  = 0.0*Gaudi::Units::mm2 );
  declareProperty( "ErrorY2",  m_eY2  = 0.0*Gaudi::Units::mm2 );
  declareProperty( "ErrorTx2", m_eTx2 = 0.0                   );
  declareProperty( "ErrorTy2", m_eTy2 = 0.0                   );
  declareProperty( "ErrorP",   m_eP   = 0.0                   );  // dp/p
  declareProperty( "CorrectSlopes",   m_correctSlopes = false );

  //std::vector<std::string> tmp = boost::assign::list_of("Velo")("IT")("TT")("OT");
  std::vector<std::string> tmp = boost::assign::list_of("Tracker");
  declareProperty("Detectors", m_dets = tmp);
}

//=============================================================================
// Destructor
//=============================================================================
IdealStateCreator::~IdealStateCreator() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode IdealStateCreator::initialize()
{
  debug() << "==> Initialize" << endmsg;

  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;

  // Retrieve extrapolator
  m_extrapolator = tool<ITrackExtrapolator>( m_extrapolatorName );

  // Retrieve magnetic field service
  m_magSvc = svc<IMagneticFieldSvc>( "MagneticFieldSvc", true );

  // make the links 
  for (unsigned int i = 0; i< m_dets.size(); ++i){
    HitLinks link(0,0,"");
    m_links.push_back(link);
  } // i

  incSvc()->addListener(this, IncidentType::BeginEvent);

  return StatusCode::SUCCESS;
}

void IdealStateCreator::handle ( const Incident& incident )
{
  if ( IncidentType::BeginEvent == incident.type() ) { m_configured = false; }
}

void IdealStateCreator::initEvent() const{

  m_configured = true;
  for( unsigned int idet = 0 ; idet< m_dets.size(); ++idet ) {
    std::string linkPath = MCParticleLocation::Default + "2MC" + m_dets[idet] + "Hits";
    m_links[idet] = HitLinks( evtSvc(), msgSvc(),linkPath);    
  } //idet
}

//=============================================================================
// Creates a state at a z position,
// from a MCParticle using the entry/exit points of the MCHits
//=============================================================================
StatusCode IdealStateCreator::createState( const MCParticle* mcPart,
                                           double zRec,
                                           State& state ) const
{

  if (!m_configured) initEvent();

  // Check if MCParticle exists
  if( mcPart == 0 ) return StatusCode::FAILURE;

  // Get the closest MCHit
  MCHit* closestHit = findClosestHit( mcPart, zRec);
  if( !closestHit ) return Error( "No closest MCHit found!!" );
 
  return createState(closestHit, zRec, state);
}




//=============================================================================
// Creates a state at a z position,
// from a MCParticle using the entry/exit points of the MCHits
//=============================================================================
StatusCode IdealStateCreator::createStateVector(const MCParticle* mcPart,
                                                double zRec,
                                                StateVector& state ) const
{

  if (!m_configured) initEvent();

  // Check if MCParticle exists
  if( mcPart == 0 ) return StatusCode::FAILURE;

  // Get the closest MCHit
  MCHit* closestHit = findClosestHit( mcPart, zRec);
  if( !closestHit ) return Error( "No closest MCHit found!!" );
 
  return createStateVector(closestHit, zRec, state);
}

//=============================================================================
// Creates a state at a z position,
// from a MCHit using the entry/exit point
//=============================================================================
StatusCode IdealStateCreator::createState( const MCHit* aHit,
                                           double zRec,
                                           State& state ) const
{

  if (!m_configured) initEvent();

  StateVector pVec;
  createStateVector(aHit,pVec);
                                                                                
  state.setState(pVec.parameters());
  state.setZ(pVec.z());
  
  // set covariance matrix
  TrackSymMatrix cov = TrackSymMatrix();
  cov(0,0) = m_eX2;
  cov(1,1) = m_eY2;
  cov(2,2) = m_eTx2;
  cov(3,3) = m_eTy2;
  cov(4,4) = gsl_pow_2( m_eP * state.qOverP());
  state.setCovariance( cov );

  // transport to the z we want
  StatusCode sc = m_extrapolator->propagate(state,zRec );
  if( sc.isFailure() ) {
    warning() << "Extrapolation of True State from z = "
              << state.z() << " to z = " << zRec << " failed!" << endmsg;
  }

  return sc;
}


//=============================================================================
// Creates a state at a z position,
// from a MCHit using the entry/exit point
//=============================================================================
StatusCode IdealStateCreator::createStateVector(const MCHit* aHit,
                                                double zRec,
                                                StateVector& pVec ) const
{

  if (!m_configured) initEvent();
 
  // createState vector
  createStateVector(aHit,pVec);
                                                                                
  // extrapolate state to exact z position
  //  TrackVector& trackVec = pVec.parameters();
  //StatusCode sc = m_extrapolator->propagate(trackVec,pVec.z() ,zRec );
  StatusCode sc = m_extrapolator->propagate(pVec ,zRec );
  if( sc.isFailure() ) {
    warning() << "Extrapolation of True State from z = "
              << pVec.z() << " to z = " << zRec << " failed!" << endmsg;
  }
  //  pVec.setZ(zRec);

  return StatusCode::SUCCESS;
}

//=============================================================================
// Creates a state at a z position,
// from a MCHit using the entry/exit point
//=============================================================================
void IdealStateCreator::createStateVector(const MCHit* aHit,
                                          StateVector& pVec ) const
{
  // Correct tx and ty from the MCHit for the magnetic field
  double tx = aHit->dxdz();
  double ty = aHit->dydz();
  if ( m_correctSlopes ) correctSlopes( aHit, tx, ty );
  XYZVector direction(tx,ty,1.0);
 
  // determine Q/P
  const double trueQOverP = qOverP(aHit);
   
  // construct true State
  pVec = StateVector(aHit->entry(),direction,trueQOverP);
}
 

//=============================================================================
// Creates a state at the origin vertex
// from a MCParticle using the entry/exit points of the MCHits
//=============================================================================
StatusCode IdealStateCreator::createStateVertex( const MCParticle* mcParticle,
                                                 State& state ) const
{

  // Check if MCParticle exists
  if( mcParticle == 0 ) return StatusCode::FAILURE;

  StateVector pVec; 
  StatusCode sc = createStateVectorVertex(mcParticle,pVec);
  if (sc.isFailure()){
    return Warning("Failed to create state vector",StatusCode::SUCCESS);
  }

  state.setZ(pVec.z());
  state.setState(pVec.parameters());

  // set covariance matrix
  TrackSymMatrix cov = TrackSymMatrix();
  cov(0,0) = m_eX2;
  cov(1,1) = m_eY2;
  cov(2,2) = m_eTx2;
  cov(3,3) = m_eTy2;
  cov(4,4) = gsl_pow_2( m_eP * state.qOverP());
  state.setCovariance( cov );

  return StatusCode::SUCCESS;
}



//=============================================================================
// Creates a state at the origin vertex
// from a MCParticle using the entry/exit points of the MCHits
//=============================================================================
StatusCode IdealStateCreator::createStateVectorVertex( const MCParticle* mcParticle,
                                                       StateVector& pVec ) const
{

  // Check if MCParticle exists
  if( mcParticle == 0 ) return StatusCode::FAILURE;

  // retrieve true MC particle info
  const MCVertex* mcVertex   = mcParticle->originVertex();
  const XYZPoint& mcPos       = mcVertex->position();
  const LorentzVector& mc4Mom = mcParticle->momentum();
  
  // determine Q/P
  const double trueQOverP = qOverP( mcParticle );

  // construct true State
  pVec = StateVector(mcPos,Gaudi::XYZVector(mc4Mom),trueQOverP);

  return StatusCode::SUCCESS;
}



//=============================================================================
// Find the z-closest MCHit associated to an MCParticle
// looping over the hits in all the tracking detectors
//=============================================================================
MCHit* IdealStateCreator::findClosestHit( const MCParticle* mcPart,
                                        const double zRec) const
{
  if (!m_configured) initEvent();
  MCHit* closestHit = 0; 
  double closestZ = 1000000.0;
  std::vector<HitLinks>::iterator iterDet = m_links.begin();
  for (; iterDet != m_links.end(); ++iterDet){
    // get the links for this detector 
    MCHit* aMCHit = iterDet->first( mcPart );
    while( 0 != aMCHit ) {
      const double ZOfMidPoint = aMCHit -> midPoint().z();
      // get the closest hit
      if ( closestHit==0 || fabs( ZOfMidPoint - zRec ) < closestZ ) {
        closestHit = aMCHit;
        closestZ = fabs( ZOfMidPoint - zRec );
      }
      aMCHit = iterDet->next();
    } // while
  } // iterDet
  
  return closestHit;
}

//=============================================================================

double  IdealStateCreator::qOverP( const MCHit* mcHit) const {
  const double charge = ( mcHit->mcParticle()->particleID().threeCharge() ) / 3.;
  const double p = mcHit->p();
  return ( p < TrackParameters::lowTolerance ? qOverP(mcHit->mcParticle()) : charge/p );
}

double  IdealStateCreator::qOverP( const MCParticle* mcPart) const {
  const double charge = ( mcPart -> particleID().threeCharge() ) / 3.;
  const double p = mcPart->p();
  return ( p < TrackParameters::lowTolerance  ? 0.0 : charge/p );
}

//=============================================================================
// Correct slopes for magnetic field given an MCHit and a MCParticle
//=============================================================================
void IdealStateCreator::correctSlopes(const MCHit* mcHit,
                                      double& tx, double& ty ) const
{
  // TODO: I hope this method can be removed as soon as the displacement vector
  // in the MCHit is calculated in Gauss using the momentum direction of the
  // *entry point*. (JvT: 27/10/2006).
  using namespace Gaudi::Units;

  // Get magnetic field vector
  Gaudi::XYZVector B;
  m_magSvc -> fieldVector( mcHit -> midPoint() , B );

  // Calculate new displacement vector and tx,ty slopes
  Gaudi::XYZVector d = mcHit -> displacement();
  Gaudi::XYZVector dNew = d - ( 0.5 * d.R() * qOverP(mcHit) * 
                                d.Cross(B) * eplus * c_light);
  tx = dNew.x() / dNew.z();
  ty = dNew.y() / dNew.z();  
}

//=============================================================================
// Adds states for all related MCHits to the track
//=============================================================================
StatusCode IdealStateCreator::addMCHitStates(const LHCb::MCParticle& mcPart, LHCb::Track& track) const
{
  // adds states at all MC hit positions
  if (!m_configured) initEvent();
  LHCb::Track::StateContainer newstates ;
  for (std::vector<HitLinks>::iterator iterDet = m_links.begin(); 
       iterDet != m_links.end(); ++iterDet){
    // get the links for this detector 
    MCHit* aMCHit = iterDet->first( &mcPart );
    while( 0 != aMCHit ) {
      LHCb::StateVector statevec ;
      createStateVector(aMCHit,statevec ) ;
      newstates.push_back( new LHCb::State(statevec) ) ;
      aMCHit = iterDet->next();
    } 
  }
  track.addToStates( newstates ) ;
  return StatusCode::SUCCESS ;
}
