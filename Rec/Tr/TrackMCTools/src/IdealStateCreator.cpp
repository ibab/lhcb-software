// $Id: IdealStateCreator.cpp,v 1.12 2006-11-02 15:43:48 jvantilb Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IMagneticFieldSvc.h"

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
using namespace Gaudi::Units;
using namespace LHCb;

DECLARE_TOOL_FACTORY( IdealStateCreator );

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
  declareProperty( "CorrectSlopes",   m_correctSlopes = false );
  declareProperty( "ErrorX2",  m_eX2  = 0.0*Gaudi::Units::mm2 );
  declareProperty( "ErrorY2",  m_eY2  = 0.0*Gaudi::Units::mm2 );
  declareProperty( "ErrorTx2", m_eTx2 = 0.0                   );
  declareProperty( "ErrorTy2", m_eTy2 = 0.0                   );
  declareProperty( "ErrorP",   m_eP   = 0.0                   );  // dp/p

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

  // Retrieve magnetic field service
  m_magSvc = svc<IMagneticFieldSvc>( "MagneticFieldSvc", true );

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
  // Create the state
  State* pState = new State();
  state = pState;

  // Check if MCParticle exists
  if( mcPart == 0 ) return StatusCode::FAILURE;  

  // Get the closest MCHit
  MCHit* closestHit = 0;
  findClosestHit( mcPart, zRec, closestHit );
  if( !closestHit ) return Error( "No closest MCHit found!!" );
  XYZPoint beginPoint = closestHit -> entry();
  double z = beginPoint.z();

  // Correct tx and ty from the MCHit for the magnetic field
  double tx = closestHit -> dxdz();
  double ty = closestHit -> dydz();
  if ( m_correctSlopes ) correctSlopes( mcPart, closestHit, tx, ty );

  // set the state parameters
  pState -> setState(  beginPoint.x(),  beginPoint.y(), z,
                       tx, ty, qOverP( mcPart, closestHit ) );
  
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
  State* trueState = new State();
  state = trueState;

  // Check if MCParticle exists
  if( mcParticle == 0 ) return StatusCode::FAILURE;

  // retrieve true MC particle info
  const MCVertex* mcVertex   = mcParticle -> originVertex();
  const LorentzVector mc4Mom = mcParticle -> momentum();
  const XYZPoint mcPos       = mcVertex   -> position();

  // determine slopes
  double trueTx, trueTy ;
  double pz = mc4Mom.pz();
  if ( fabs(pz) > TrackParameters::lowTolerance ) {
    trueTx = mc4Mom.px() / pz;
    trueTy = mc4Mom.py() / pz;
  }
  else {
    return Error( "No momentum z component." );
  }

  // construct true State
  trueState -> setState( mcPos.x(), mcPos.y(), mcPos.z(),
                         trueTx, trueTy, qOverP( mcParticle ) );

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
  MCHit* tmpClosestHit  = 0;
  std::vector<std::string>::const_iterator itDets;

  for( itDets = m_dets.begin(); itDets != m_dets.end(); ++itDets ) {
    std::string linkPath = MCParticleLocation::Default + "2MC" + *itDets+"Hits";
    findClosestXxxHit( mcPart, zRec, 
                       linkPath,
                       tmpClosestHit );
    if (tmpClosestHit){
      if (closestHit){ 
        if (fabs( tmpClosestHit -> midPoint().z() - zRec ) <
            fabs( closestHit -> midPoint().z() - zRec ) ) {
          closestHit = tmpClosestHit;
        }
      }
      else {
        closestHit = tmpClosestHit;
      }
    }
  } // for
}

//=============================================================================
// Find the z-closest MCHit of type Xxx associated to an MCParticle
//=============================================================================
void IdealStateCreator::findClosestXxxHit( const MCParticle* mcPart,
                                           const double zRec,
                                           const std::string& linkPath,
                                           MCHit*& closestHit ) const
{
  // Retrieve MCParticle to MCHit linker tables
  LinkedFrom<MCHit,MCParticle> mcp2mchitLink( evtSvc(), msgSvc(), linkPath );

  double closestZ = 1000000.0;

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
                                  const MCHit* mcHit ) const
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
// Correct slopes for magnetic field given an MCHit and a MCParticle
//=============================================================================
void IdealStateCreator::correctSlopes( const MCParticle* mcPart,
                                       const MCHit* mcHit,
                                       double& tx, double& ty ) const
{
  // TODO: I hope this method can be removed as soon as the displacement vector
  // in the MCHit is calculated in Gauss using the momentum direction of the
  // *entry point*. (JvT: 27/10/2006).

  // Get magnetic field vector
  Gaudi::XYZVector B;
  m_magSvc -> fieldVector( mcHit -> midPoint() , B );

  // Calculate new displacement vector and tx,ty slopes
  Gaudi::XYZVector d = mcHit -> displacement();
  Gaudi::XYZVector dNew = d - ( 0.5 * d.R() * qOverP(mcPart, mcHit) * 
                                d.Cross(B) * eplus * c_light);
  tx = dNew.x() / dNew.z();
  ty = dNew.y() / dNew.z();  
}

//=============================================================================
