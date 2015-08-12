// $Id: CaloElectron.cpp,v 1.2 2009-05-19 13:48:22 cattanem Exp $
// Include files 

// LHCb
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "Event/Particle.h"

// local
#include "CaloElectron.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CaloElectron
//
// 2006-11-30 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( CaloElectron )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CaloElectron::CaloElectron( const std::string& type,
                            const std::string& name,
                            const IInterface* parent )
  : Part2Calo ( type, name , parent )
    , m_electron       ( NULL )
    , m_bremstrahlung  ( NULL )
    , m_calopos        ( NULL ){
  declareInterface<ICaloElectron>(this);
  declareProperty ( "zOffset"          , m_zOffset   = 0. ) ; // Should be 0.0 if ShowerMax plane is correctly defined in condDB
}
//=============================================================================
// Destructor
//=============================================================================
CaloElectron::~CaloElectron() {} 

//=============================================================================
StatusCode CaloElectron::initialize(){
  StatusCode sc = Part2Calo::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);
  return StatusCode::SUCCESS;  
}


//=============================================================================
bool CaloElectron::set(const LHCb::Particle* particle, std::string det,CaloPlane::Plane plane , double delta){
  m_status = setting(particle) && caloSetting();
  if( m_status )return Part2Calo::match(particle , det, plane, delta );
  return m_status;
}
bool CaloElectron::set(const LHCb::ProtoParticle* proto, std::string det,CaloPlane::Plane plane , double delta){
  m_status = setting(proto) && caloSetting();
  if( m_status )return Part2Calo::match(proto , det, plane, delta );
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
LHCb::State CaloElectron::closestState(std::string toWhat ){
  LHCb::State state; // empty state
  if( !m_status ) return state;
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
  LHCb::ParticleID pid( 11 );// electron hypothesis
  if( NULL != m_particle) pid =  m_particle->particleID();
  return Part2Calo::closestState(x,y, pid);
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
bool CaloElectron::caloSetting(){
  // Calo setting
  m_electron = NULL;
  m_bremstrahlung = NULL;
  m_calopos = NULL;

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

