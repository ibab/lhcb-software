// $Id: Part2Calo.cpp,v 1.2 2009-05-19 13:48:22 cattanem Exp $
// Include files 

// from LHCb
#include "Event/Particle.h"

// local
#include "Part2Calo.h"

//-----------------------------------------------------------------------------
// Implementation file for class : Part2Calo
//
// 2006-11-30 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( Part2Calo )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Part2Calo::Part2Calo( const std::string& type,
                        const std::string& name,
                        const IInterface* parent )
  : Track2Calo ( type, name , parent )
    , m_particle       ( NULL )
    , m_proto          ( NULL ){
  declareInterface<IPart2Calo>(this);
}
//=============================================================================
// Destructor
//=============================================================================
Part2Calo::~Part2Calo() {} 

//=============================================================================
StatusCode Part2Calo::initialize(){
  StatusCode sc = Track2Calo::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);
  return StatusCode::SUCCESS;  
}
//=============================================================================
bool Part2Calo::match(const LHCb::Particle* part, std::string det,CaloPlane::Plane plane , double delta){
  m_status = setting( part );
  if( m_status )return Track2Calo::match(m_track, det, plane, delta, m_particle->particleID());
  return m_status;
}
//=============================================================================
bool Part2Calo::match(const LHCb::ProtoParticle* proto, std::string det,CaloPlane::Plane plane , double delta){
  m_status = setting( proto );
  if( m_status )return Track2Calo::match(m_track, det, plane, delta);
  return m_status;
}
//=============================================================================
bool Part2Calo::setting(const LHCb::Particle* particle){
  m_particle = particle;
  if( NULL == m_particle)return false;
  const LHCb::ProtoParticle* proto = m_particle -> proto();
  return setting(proto);
}
//=============================================================================
bool Part2Calo::setting(const LHCb::ProtoParticle* proto){
  m_proto   = proto;
  if( NULL == m_proto)return false;
  return Track2Calo::setting( m_proto->track() );
}
//=============================================================================

