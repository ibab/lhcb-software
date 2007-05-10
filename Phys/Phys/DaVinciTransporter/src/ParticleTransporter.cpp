// $Id: ParticleTransporter.cpp,v 1.17 2007-05-10 12:28:08 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/IParticlePropertySvc.h"

#include "Event/TrackTypes.h" /// @todo temporary
#include "TrackInterfaces/ITrackExtrapolator.h"        // TrackExtrapolator
#include "Kernel/IParticle2State.h"
#include "LHCbMath/MatrixManip.h"
// local
#include "ParticleTransporter.h"
#include "TransporterFunctions.h"
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
  , m_p2s()
  , m_eID(0)
{
  declareInterface<IParticleTransporter>(this);

  declareProperty("TrackExtrapolator", m_tracksName = "TrackFastParabolicExtrapolator");
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
            << " tool to extrapolate charged composite particles with" <<
      "instance name NeutralCompExtrapolator" << endmsg;
    m_neutralComp = tool<ITrackExtrapolator>(m_neutralsName+"/NeutralCompExtrapolator",this);    
  } else if ( m_neutralCompName != "" ){
    debug() << "Using the " << m_neutralCompName 
            << " tool to extrapolate charged composite particles" << endmsg;
    m_neutralComp = tool<ITrackExtrapolator>(m_neutralCompName,this);
  } else warning() << "No TrackExtrapolator given for charged composites" << endmsg ;

  m_ppSvc = svc<IParticlePropertySvc>("ParticlePropertySvc", true);

  m_p2s = tool<IParticle2State>("Particle2State"); // not private

  // a complicated way of getting 11
  ParticleProperty* pe = m_ppSvc->find("e-");
  m_eID = abs(pe->jetsetID());

  return sc;
}
//=============================================================================
// transport method
//=============================================================================
StatusCode ParticleTransporter::transport(const LHCb::Particle* P, 
                                          const double znew,
                                          LHCb::Particle& transParticle){
  StatusCode sc = StatusCode::SUCCESS;
  debug() << "Transport PID " << P->particleID().pid() 
          << " p " << P->momentum() << " from " 
          << P->referencePoint()  << " to " << znew << endmsg ;

  transParticle = LHCb::Particle(*P);

  if ( ! (P->isBasicParticle()) ) {
    verbose() << "Using DaVinciTransporter::transportComposite" << endmsg;
    sc = DaVinciTransporter::transportComposite(P, znew, transParticle);
    if (!sc) return sc;
  } else if (P->charge()==0. ) {
    verbose() << "Using DaVinciTransporter::transportNeutralBasic" << endmsg;
    sc = DaVinciTransporter::transportNeutralBasic(P, znew, transParticle);
    if (!sc) return sc;
  } else {
    if (msgLevel(MSG::VERBOSE)){
      sc = m_p2s->test(*P);
      if (!sc) return sc;
    }

    LHCb::State s ; // state to extrapolate
    sc = state(P,znew,s);
    if (!sc) return sc;
    ITrackExtrapolator* extra = extrapolator(P);
    if (NULL!=extra) sc = extra->propagate(s,znew,P->particleID());
    else Warning("No extrapolator defined").ignore();
    if (!sc) return sc;

    verbose() << "Extrapolated state is" << endmsg;
    verbose() << s << endmsg ;

    sc = m_p2s->state2Particle(s,transParticle);
    if (!sc) return sc;
  }
  verbose() << "Obtained Particle " << transParticle.particleID().pid() << endmsg;
  verbose() << transParticle << endmsg ;

  debug() << "Transported " << P->particleID().pid() << " " << transParticle.momentum() << " to " 
          << transParticle.referencePoint() << endmsg ;

  return sc;
}
//=============================================================================
// get a state from a Particle
//=============================================================================
StatusCode ParticleTransporter::state(const LHCb::Particle* P, const double znew, LHCb::State& s) const { 
  // charged basic: no need to make 
  verbose() << "Starting from Particle \n " << *P << endmsg ;

  StatusCode sc = StatusCode::SUCCESS ;
  
  if (P->charge()!=0 && P->isBasicParticle()){
    // Particles from MCParticles
    if (NULL==P->proto()){
      ParticleProperty *pp = m_ppSvc->findByPythiaID(P->particleID().pid());
      if (0!=pp) Warning(pp->particle()+" has no proto nor endVertex. Assuming it's from MC.", 
                         StatusCode::SUCCESS) ;
      else err() << "Particle with unknown PID " << P->particleID().pid() << " has no endVertex. " 
                 <<  "Assuming it's from MC" << endmsg ;
      sc = m_p2s->particle2State(*P,s);
    } else if (m_eID==P->particleID().abspid()){
      // Electrons. We don't want to lose Bremsstrahlung correction
      verbose() << "Special treatment for electrons" << endmsg ;
      sc = m_p2s->particle2State(*P,s);
    } else if (NULL==P->proto()->track()){
      // Charged protopraticle without track -> error
      err() << "Basic Particle of ID " << P->particleID().pid() << " has no track" << endmsg;
      return StatusCode::FAILURE; 
    } else {
      // That's fine
      s = P->proto()->track()->closestState(znew);
    }
  } else { // make a state
    return m_p2s->particle2State(*P,s);
  }
  verbose() << "Got state: \n" << s << endmsg ;

  return sc ;
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
