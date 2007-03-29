// $Id: CaloElectron.cpp,v 1.5 2007-03-29 16:34:12 odescham Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// LHCb
#include "GaudiKernel/Plane3DTypes.h"
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "LHCbMath/Line.h"
#include "LHCbMath/GeomFun.h"
// local
#include "CaloUtils/CaloElectron.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CaloElectron
//
// 2006-11-30 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( CaloElectron );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CaloElectron::CaloElectron( const std::string& type,
                            const std::string& name,
                            const IInterface* parent )
  : GaudiTool ( type, name , parent )
    , m_status         ( false)
    , m_particle       ( NULL )
    , m_proto          ( NULL )
    , m_track          ( NULL )
    , m_electron       ( NULL )
    , m_bremstrahlung  ( NULL )
    , m_calopos        ( NULL )
    , m_calo (){
  declareInterface<ICaloElectron>(this);
  declareProperty ( "ExtrapolatorType" , m_extrapolatorType = "TrackHerabExtrapolator" ) ;
  declareProperty ( "Tolerance"        , m_tolerance = 0.01 ) ;
  declareProperty ( "zOffset"          , m_zOffset   = 0. ) ; // Should be 0.0 if ShowerMax plane is correctly defined in condDB
  declareProperty ( "Detector"         , m_det   = DeCalorimeterLocation::Ecal );
}
//=============================================================================
// Destructor
//=============================================================================
CaloElectron::~CaloElectron() {} 

//=============================================================================
StatusCode CaloElectron::initialize(){
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);
  m_calo = getDet<DeCalorimeter>( m_det ) ;
  m_extrapolator = tool<ITrackExtrapolator>( m_extrapolatorType,"Extrapolator",this );
  return StatusCode::SUCCESS;  
}


//=============================================================================
bool CaloElectron::set(const LHCb::Particle* particle){
  m_status = setting(particle);
  debug() << " CaloElectron setting " << m_status << endreq;
  return m_status;
}
bool CaloElectron::set(const LHCb::ProtoParticle* proto){
  m_status = setting(proto);
  debug() << " CaloElectron setting " << m_status << endreq;
  return m_status;
}
//=============================================================================
LHCb::CaloHypo* CaloElectron::electron(){
  return m_electron;
}
//=============================================================================
LHCb::CaloHypo* CaloElectron::bremstrahlung(){
  return m_bremstrahlung;
}
//=============================================================================
LHCb::CaloMomentum CaloElectron::bremCaloMomentum(){
  if(!m_status || NULL == m_bremstrahlung )return LHCb::CaloMomentum();
  Gaudi::XYZPoint point;
  Gaudi::SymMatrix3x3 matrix;
  m_track->position(point,matrix);  
  LHCb::CaloMomentum bremPhoton( m_bremstrahlung ,point, matrix);
  return bremPhoton;
}

//=============================================================================
double CaloElectron::ecalE(){
  if( !m_status )return 0.;
  return (double) m_electron->e() ;
}
//=============================================================================
double CaloElectron::eOverP(){
  if( !m_status )return 0.;
  return (double) m_electron->e()/m_track->p();
}
//=============================================================================
LHCb::State CaloElectron::caloState(CaloPlane::Plane plane , double deltaShower ){
  LHCb::State state; // empty state
  if( !m_status ) return state;
  
  // get caloPlane
  ROOT::Math::Plane3D refPlane = m_calo->plane( plane );
  // propagate state to refPlane
  LHCb::State calostate( m_track->closestState( refPlane ) );
  LHCb::ParticleID pid( 11 );// electron hypothesis
  if(NULL != m_particle)pid = m_particle->particleID();
  StatusCode sc = m_extrapolator->propagate ( calostate, refPlane , m_tolerance , pid);
  if(sc.isFailure())return state;  

  if( 0. == deltaShower)return calostate; 
  
  Gaudi::XYZVector dir (calostate.tx(), calostate.ty(), 1.);
  Gaudi::XYZPoint  point = calostate.position() + deltaShower * dir/dir.R();
  // extrapolate to the new point
  sc = m_extrapolator->propagate ( calostate, point.z() , pid);
  if(sc.isFailure())return state;
  return calostate;

}
//=============================================================================
LHCb::State CaloElectron::closestState(std::string toWhat ){
  LHCb::State state; // empty state
  if( !m_status ) return state;
  
  // get state on Front of Ecal
  LHCb::State calostate = caloState(CaloPlane::Front);
  if(calostate.z() == 0 ) return state;

  // get hypo position
  double x = 0. ;
  double y = 0. ;
  if( "hypo" == toWhat ){
    x = m_calopos->parameters()(LHCb::CaloPosition::X);
    y = m_calopos->parameters()(LHCb::CaloPosition::Y);
  }else if("cluster" == toWhat ){
    x = m_calopos->center()(LHCb::CaloPosition::X);
    y = m_calopos->center()(LHCb::CaloPosition::Y);
  }else{
    return state;
  }

  // get frontPlane
  ROOT::Math::Plane3D frontPlane = m_calo->plane( CaloPlane::Front );

  // Define calo line (from transversal barycenter) and track line in Ecal
  typedef Gaudi::Line<Gaudi::XYZPoint,Gaudi::XYZVector> Line;
  Gaudi::XYZVector normal = frontPlane.Normal();
  double zEcal = ( -normal.X()*x -normal.Y()*y - frontPlane.HesseDistance() )/normal.Z(); // tilt
  Gaudi::XYZPoint point( x , y , zEcal );
  Line cLine( point ,frontPlane.Normal() );
  Line tLine( calostate.position() , calostate.slopes() );

  // Find points of closest distance between calo Line and track Line
  Gaudi::XYZPoint cP,tP;
  Gaudi::Math::closestPoints<Line,Gaudi::XYZPoint>(cLine,tLine,cP,tP);

  // propagate the state the new Z of closest distance 

  LHCb::ParticleID pid( 11 );// electron hypothesis
  if( NULL != m_particle) pid =  m_particle->particleID();
  
  StatusCode sc = m_extrapolator->propagate ( calostate, tP.Z() , pid);

  if(sc.isFailure())return state;
  return calostate;
}
//=============================================================================
double CaloElectron::caloTrajectoryZ(CaloPlane::Plane refPlane , std::string toWhat ){
  LHCb::State theState = closestState( toWhat );
  LHCb::State refState = caloState( refPlane );
  return m_zOffset + theState.z() - refState.z();
}
//=============================================================================
double CaloElectron::caloTrajectoryL(CaloPlane::Plane refPlane , std::string toWhat ){
  LHCb::State theState = closestState( toWhat );
  LHCb::State refState = caloState( refPlane );
  Gaudi::XYZVector depth = theState.position()-refState.position();
  ROOT::Math::Plane3D plane = m_calo->plane( refPlane );
  double dist = plane.Distance( theState.position() );//signed distance to refPlane
  return depth.R() * dist/fabs(dist);
}
//=============================================================================
bool CaloElectron::setting(const LHCb::Particle* particle){
  m_particle = particle;
  if( NULL == m_particle)return false;
  const LHCb::ProtoParticle* proto = m_particle -> proto();
  return setting(proto);
}
//=============================================================================
bool CaloElectron::setting(const LHCb::ProtoParticle* proto){

  m_track = NULL;
  m_electron = NULL;
  m_bremstrahlung = NULL;
  m_calopos = NULL;

  m_proto   = proto;
  if( NULL == m_proto)return false;
  m_track   = m_proto->track();
  if( NULL == m_track) return false;
  SmartRefVector<LHCb::CaloHypo> hypos = m_proto->calo();
  if(0 == hypos.size())return false;

  for(SmartRefVector<LHCb::CaloHypo>::const_iterator ihypo =  hypos.begin(); ihypo != hypos.end() ; ++ihypo){
    const LHCb::CaloHypo* hypo =  *ihypo;
    if(NULL == hypo)continue;
    if( LHCb::CaloHypo::EmCharged == hypo->hypothesis() ) m_electron      = (LHCb::CaloHypo*)  hypo;
    if( LHCb::CaloHypo::Photon ==   hypo->hypothesis() )  m_bremstrahlung = (LHCb::CaloHypo*) hypo;
  }
  if( NULL == m_electron )return false; // Electron hypo is mandatory - brem. not
  m_calopos = m_electron->position();
  if( NULL == m_calopos) return false;
  return true;  
}

