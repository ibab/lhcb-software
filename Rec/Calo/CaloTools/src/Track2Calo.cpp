// Include files 

// LHCb
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "LHCbMath/Line.h"
#include "LHCbMath/GeomFun.h"
#include "Event/Track.h"
#include "Event/CaloCluster.h"
#include "Event/CaloHypo.h"
#include "TrackInterfaces/ITrackExtrapolator.h"
// local
#include "Track2Calo.h"

//-----------------------------------------------------------------------------
// Implementation file for class : Track2Calo
//
// Simple tool to propagate track to Calo reference planes
//
// 2007-06-25 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( Track2Calo )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Track2Calo::Track2Calo( const std::string& type,
                            const std::string& name,
                            const IInterface* parent )
  : GaudiTool ( type, name , parent )
  , m_status         ( false)
  , m_track          ( NULL )
  , m_det            ()
  , m_valid          ( false)
{
  declareInterface<ITrack2Calo>(this);
  declareProperty ( "ExtrapolatorType" , m_extrapolatorType = "TrackRungeKuttaExtrapolator" ) ;
  declareProperty ( "Tolerance"        , m_tolerance = 0.01 ) ;
}
//=============================================================================
// Destructor
//=============================================================================
Track2Calo::~Track2Calo() {} 

//=============================================================================
StatusCode Track2Calo::initialize(){
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);
  m_extrapolator = tool<ITrackExtrapolator>( m_extrapolatorType,"Extrapolator",this );
  return StatusCode::SUCCESS;  
}
//=============================================================================
bool Track2Calo::match(const LHCb::Track* track, std::string det,CaloPlane::Plane plane , double delta, LHCb::ParticleID pid ){
  m_status = setting(track);
  m_det    = det;
  m_calo   = getDet<DeCalorimeter>( det );
  m_state  = caloState(plane, delta, pid);
  m_cell   = m_calo->Cell( m_state.position() );
  m_valid  = m_calo->valid( m_cell );
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
    debug() << " Track2Calo setting [" << *track <<","<< det<<"] status : " <<m_status << endmsg;
  return m_status;
}
//=============================================================================
LHCb::State Track2Calo::caloState(CaloPlane::Plane plane , double delta, LHCb::ParticleID pid ){

  LHCb::State state; // empty state
  if( !m_status ) return state;
  
  // get caloPlane
  ROOT::Math::Plane3D refPlane = m_calo->plane( plane );
  // propagate state to refPlane
  LHCb::State calostate( m_track->closestState( refPlane ) );
  StatusCode sc = m_extrapolator->propagate ( calostate, refPlane , m_tolerance , pid  );
  if(sc.isFailure())return state;  

  if( 0. == delta)return calostate; 
  
  Gaudi::XYZVector dir (calostate.tx(), calostate.ty(), 1.);
  Gaudi::XYZPoint  point = calostate.position() + delta * dir/dir.R();
  // extrapolate to the new point
  sc = m_extrapolator->propagate ( calostate, point.z(), pid );
  if(sc.isFailure())return state;
  return calostate;
}
//=============================================================================
bool Track2Calo::setting(const LHCb::Track* track){
  m_track   = track;
  return ( NULL == m_track) ? false : true;
}




LHCb::State Track2Calo::closestState(LHCb::CaloCluster* cluster,LHCb::ParticleID pid  ){
  return closestState( cluster->position(),pid);
}

LHCb::State Track2Calo::closestState(LHCb::CaloHypo* hypo,LHCb::ParticleID pid ){
  LHCb::State state ;//emtpy state
  LHCb::CaloPosition* calopos  = hypo->position();
  if(calopos == NULL)return state;
  return closestState( *calopos ,pid);
}
LHCb::State Track2Calo::closestState(LHCb::CaloPosition calopos,LHCb::ParticleID pid ){
  double x = calopos.parameters()(LHCb::CaloPosition::X);
  double y = calopos.parameters()(LHCb::CaloPosition::Y);
  return closestState(x,y, pid);
}
LHCb::State Track2Calo::closestState(LHCb::CaloCellID cellID,LHCb::ParticleID pid ){
  Gaudi::XYZPoint point = m_calo->cellCenter( cellID );
  return closestState(point.X(),point.Y(), pid);
}

//=============================================================================
LHCb::State Track2Calo::closestState(double x, double y,LHCb::ParticleID pid){
  LHCb::State state; // empty state
  if( !m_status ) return state;
  
  // get state on Front of Ecal
  LHCb::State calostate = caloState( CaloPlane::Front);
  if(calostate.z() == 0 ) return state;

  // get frontPlane
  ROOT::Math::Plane3D frontPlane = m_calo->plane( CaloPlane::Front );

  // Define calo line (from transversal barycenter) and track line in Ecal
  typedef Gaudi::Math::Line<Gaudi::XYZPoint,Gaudi::XYZVector> Line;
  Gaudi::XYZVector normal = frontPlane.Normal();
  double zEcal = ( -normal.X()*x -normal.Y()*y - frontPlane.HesseDistance() )/normal.Z(); // tilt
  Gaudi::XYZPoint point( x , y , zEcal );
  Line cLine( point ,frontPlane.Normal() );
  Line tLine( calostate.position() , calostate.slopes() );

  // Find points of closest distance between calo Line and track Line
  Gaudi::XYZPoint cP,tP;
  Gaudi::Math::closestPoints<Line,Line,Gaudi::XYZPoint>(cLine,tLine,cP,tP);

  // propagate the state the new Z of closest distance
  StatusCode sc = m_extrapolator->propagate ( calostate, tP.Z() , pid);

  if(sc.isFailure())return state;
  return calostate;
}
