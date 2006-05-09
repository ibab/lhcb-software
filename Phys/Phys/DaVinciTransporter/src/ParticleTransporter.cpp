// $Id: ParticleTransporter.cpp,v 1.4 2006-05-09 16:29:26 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/IParticlePropertySvc.h"

#include "Kernel/TrackTypes.h" /// @todo temporary
#include "TrackInterfaces/ITrackExtrapolator.h"        // TrackExtrapolator
#include "Kernel/state2Particle.h"
// local
#include "ParticleTransporter.h"
//-----------------------------------------------------------------------------
// Implementation file for class : ParticleTransporter
//
// 2006-01-20 : Patrick KOPPENBURG
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( ParticleTransporter );
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ParticleTransporter::ParticleTransporter( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent )
  : GaudiTool ( type, name , parent )
  , m_tracks()
  , m_neutrals()
  , m_chargedComp()
  , m_neutralComp()
  , m_ppSvc(0)
{
  declareInterface<IParticleTransporter>(this);

  declareProperty("TrackExtrapolator", m_tracksName = "TrackMasterExtrapolator");
  declareProperty("NeutralExtrapolator", m_neutralsName = "TrackLinearExtrapolator");
  declareProperty("ChargedCompositeExtrapolator", m_chargedCompName = 
                  "TrackParabolicExtrapolator");
  declareProperty("NeutralCompositeExtrapolator", m_neutralCompName = 
                  "<NeutralExtrapolator>");
}
//=============================================================================
// Destructor
//=============================================================================
ParticleTransporter::~ParticleTransporter() {} 

//=============================================================================
// Initialize
//=============================================================================
StatusCode ParticleTransporter::initialize(){
  StatusCode sc = GaudiTool::initialize();
  if (!sc) return sc;
  
  if ( m_tracksName != "" ){
    debug() << "Using the " << m_tracksName 
            << " tool to extrapolate particles from tracks" << endmsg;
    m_tracks = tool<ITrackExtrapolator>(m_tracksName,this);
  } else warning() << "No TrackExtrapolator given for tracks" << endmsg ;

  if ( m_neutralsName != "" ){
    debug() << "Using the " << m_neutralsName 
            << " tool to extrapolate neutral particles" << endmsg;
    m_neutrals = tool<ITrackExtrapolator>(m_neutralsName,this);
  } else warning() << "No TrackExtrapolator given for neutrals" << endmsg ;

  if ( m_chargedCompName != "" ){
    debug() << "Using the " << m_chargedCompName 
            << " tool to extrapolate charged composite particles" << endmsg;
    m_chargedComp = tool<ITrackExtrapolator>(m_chargedCompName,this);
  } else warning() << "No TrackExtrapolator given for charged composites" << endmsg ;
  
  if ( m_neutralCompName == "<NeutralExtrapolator>" ){
    debug() << "Using the " << m_neutralsName  
            << " tool to extrapolate charged composite particles" << endmsg;
    m_neutralComp = tool<ITrackExtrapolator>(m_neutralsName,this);    
  } else if ( m_neutralCompName != "" ){
    debug() << "Using the " << m_neutralCompName 
            << " tool to extrapolate charged composite particles" << endmsg;
    m_neutralComp = tool<ITrackExtrapolator>(m_neutralCompName,this);
  } else warning() << "No TrackExtrapolator given for charged composites" << endmsg ;

  m_ppSvc = svc<IParticlePropertySvc>("ParticlePropertySvc", true);

  return sc;
}
//=============================================================================
// transport method
//=============================================================================
StatusCode ParticleTransporter::transport(const LHCb::Particle* P, 
                                          const double znew,
                                          LHCb::Particle& transParticle){
  StatusCode sc = StatusCode::SUCCESS;
  
  LHCb::State s ; // state to extrapolate
  sc = state(P,znew,s);
  if (!sc) return sc;
  ITrackExtrapolator* extra = extrapolator(P);
  if (NULL!=extra) extra->propagate(s,znew,P->particleID());
  else Warning("No extrapolator defined");

  transParticle = LHCb::Particle(*P);
  
  return state2Particle(s,transParticle);
}
//=============================================================================
// get a state from a Particle
//=============================================================================
StatusCode ParticleTransporter::state(const LHCb::Particle* P, const double znew, LHCb::State& s) const { 
  // charged basic: no need to make 
  if (P->charge()!=0 && P->isBasicParticle()){
    if (NULL==P->proto()){
      ParticleProperty *pp = m_ppSvc->findByPythiaID(P->particleID().pid());
      if (0!=pp) Warning(pp->particle()+" has no proto nor endVertex. Assuming it's from MC.", 
                         StatusCode::SUCCESS) ;
      else err() << "Particle with unknown PID " << P->particleID().pid() << " has no endVertex. " 
                 <<  "Assuming it's from MC" << endmsg ;
      return makeState(P,s);
    } else if (NULL==P->proto()->track()){
      err() << "Basic Particle of ID " << P->particleID().pid() << " has no track" << endmsg;
      return StatusCode::FAILURE; 
    } else {
      s = P->proto()->track()->closestState(znew);
    }
  } else { // make a state
    return makeState(P,s);
  }  
  return StatusCode::SUCCESS;
}  
//=============================================================================
// make a state from a Particle
//=============================================================================
StatusCode ParticleTransporter::makeState(const LHCb::Particle* P, LHCb::State& s) const { 
  s.setState(P->momentum().X(),P->momentum().Y(),P->momentum().Z(),
             P->slopes().X(),P->slopes().Y(),P->charge()/P->p());
  
  /// @todo Implement error matrix.

  return StatusCode::SUCCESS;
}  
//=============================================================================
// type of extrapolator
//=============================================================================
ITrackExtrapolator* ParticleTransporter::extrapolator(const LHCb::Particle* P) const {  
  if ( P->isBasicParticle() ){
    if (P->charge()==0) return m_neutrals;
    else return m_tracks;
  } else {
    if (P->charge()==0) return m_neutralComp;
    else return m_chargedComp;
  }
}
