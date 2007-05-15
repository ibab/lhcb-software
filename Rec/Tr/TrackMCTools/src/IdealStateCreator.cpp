// $Id: IdealStateCreator.cpp,v 1.14 2007-05-15 08:34:15 mneedham Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IMagneticFieldSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IIncidentSvc.h"

// from Event
#include "Event/MCParticle.h"
#include "Event/MCHit.h"
#include "Event/State.h"
#include "Event/TrackParameters.h"

// local
#include "IdealStateCreator.h"
#include <boost/assign/list_of.hpp>

#include "gsl/gsl_math.h"

using namespace Gaudi;
using namespace LHCb;

DECLARE_TOOL_FACTORY( IdealStateCreator );

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
                   m_extrapolatorName = "TrackMasterExtrapolator" );
  declareProperty( "ErrorX2",  m_eX2  = 0.0*Gaudi::Units::mm2 );
  declareProperty( "ErrorY2",  m_eY2  = 0.0*Gaudi::Units::mm2 );
  declareProperty( "ErrorTx2", m_eTx2 = 0.0                   );
  declareProperty( "ErrorTy2", m_eTy2 = 0.0                   );
  declareProperty( "ErrorP",   m_eP   = 0.0                   );  // dp/p
  declareProperty( "CorrectSlopes",   m_correctSlopes = false );
  //declareProperty( "Detectors" , 
  //                m_dets = boost::assign::list_of("Velo")("IT")("TT")("OT"));

  declareProperty("Detectors" , 
                  m_dets = boost::assign::list_of("Tracker"));
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
// from a MCHit using the entry/exit point
//=============================================================================
StatusCode IdealStateCreator::createState( const MCHit* aHit,
                                           double zRec,
                                           State& state ) const
{

  if (!m_configured) initEvent();
  
  // First create the state
  state.setZ( zRec );
 
  // Correct tx and ty from the MCHit for the magnetic field
  double tx = aHit->dxdz();
  double ty = aHit->dydz();
  if ( m_correctSlopes ) correctSlopes( aHit, tx, ty );

  // determine Q/P
  const double trueQOverP = qOverP(aHit);

  // set the state parameters
  const XYZPoint& beginPoint = aHit->entry();
  state.setState(beginPoint.x(),  beginPoint.y(), beginPoint.z(),
                   tx, ty, trueQOverP );
  
  // set covariance matrix
  TrackSymMatrix cov = TrackSymMatrix();
  cov(0,0) = m_eX2;
  cov(1,1) = m_eY2;
  cov(2,2) = m_eTx2;
  cov(3,3) = m_eTy2;
  cov(4,4) = gsl_pow_2( m_eP * state.qOverP());
  state.setCovariance( cov );

  // extrapolate state to exact z position
  StatusCode sc = m_extrapolator->propagate(state, zRec );
  if( sc.isFailure() ) {
    warning() << "Extrapolation of True State from z = "
              << beginPoint.z() << " to z = " << zRec << " failed!" << endreq;
  }

  return sc;
}

//=============================================================================
// Creates a state at the origin vertex
// from a MCParticle using the entry/exit points of the MCHits
//=============================================================================
StatusCode IdealStateCreator::createStateVertex( const MCParticle* mcParticle,
                                                 State& state ) const
{
  
  if (!m_configured) initEvent();

  // Check if MCParticle exists
  if( mcParticle == 0 ) return StatusCode::FAILURE;

  // retrieve true MC particle info
  const MCVertex* mcVertex   = mcParticle->originVertex();
  const LorentzVector& mc4Mom = mcParticle->momentum();
  const XYZPoint& mcPos       = mcVertex->position();


  // determine Q/P
  const double trueQOverP = qOverP( mcParticle );

  // determine slopes
  double trueTx; double trueTy;
  if ( fabs(mc4Mom.pz()) > TrackParameters::lowTolerance ) {
    trueTx = mc4Mom.px()/mc4Mom.pz() ;
    trueTy = mc4Mom.py()/mc4Mom.pz();
  }
  else {
    warning() << "No momentum z component." << endreq;
    return StatusCode::FAILURE;
  }

  // construct true State
  state.setState(mcPos.x(), mcPos.y(), mcPos.z(),
                 trueTx, trueTy, trueQOverP );

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
// Find the z-closest MCHit associated to an MCParticle
// looping over the hits in all the tracking detectors
//=============================================================================
MCHit* IdealStateCreator::findClosestHit( const MCParticle* mcPart,
                                        const double zRec) const
{

  MCHit* closestHit = 0; 
  double closestZ = 1000000.0;
  std::vector<HitLinks>::iterator iterDet = m_links.begin();
  for (; iterDet != m_links.end(); ++iterDet){

    // get the links for this detector 
    MCHit* aMCHit = iterDet->first( mcPart );
    while( 0 != aMCHit ) {
      const double ZOfMidPoint = aMCHit -> midPoint().z();
      // get the closest hit
      if ( fabs( ZOfMidPoint - zRec ) < closestZ ) {
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
