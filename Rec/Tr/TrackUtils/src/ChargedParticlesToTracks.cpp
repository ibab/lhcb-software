// Include files 

// local
#include "ChargedParticlesToTracks.h"
#include "TrackInterfaces/ITrackFitter.h"

#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"

#include <boost/assign/list_of.hpp>
#include <boost/algorithm/string.hpp>

#include <iostream>

using namespace std;
using namespace boost::assign;

//-----------------------------------------------------------------------------
// Implementation file for class : ChargedParticlesToTracks
//
// 2012-10-08 : Frederic Dupertuis
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( ChargedParticlesToTracks )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ChargedParticlesToTracks::ChargedParticlesToTracks( const std::string& name,
                                          ISvcLocator* pSvcLocator)
  : GaudiHistoAlg ( name , pSvcLocator )
{
  std::vector<std::string> tmp = list_of( "/Event/CharmCompleteEvent/Phys/D2hhCompleteEventPromptD2KPiLine/Particles" );
  declareProperty( "ParticlesLocations"    , m_partloc = tmp ) ;
  declareProperty( "TracksOutputLocation"  , 
                   m_trackOutputLocation = "/Event/Rec/Track/MyBest" ) ;
  declareProperty( "MassWindow"            , m_masswindow = -1. );
  declareProperty( "MassOffset"            , m_massoffset = 0. );
  declareProperty( "RefitTracks"           , m_refit = false );
}
//=============================================================================
// Destructor
//=============================================================================
ChargedParticlesToTracks::~ChargedParticlesToTracks() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode ChargedParticlesToTracks::initialize() {
  StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_trackPreFit = tool<ITrackFitter>("TrackMasterFitter","preFit", this);
  m_trackFit = tool<ITrackFitter>("TrackMasterFitter", "fit", this);

  if(fullDetail()){
    vector<string> tokens;
    for( std::vector<std::string>::const_iterator iterC = m_partloc.begin();
         iterC != m_partloc.end(); ++iterC ){
      boost::split(tokens, *iterC, boost::is_any_of("/"));
      m_linesname[*iterC]=*(--(--tokens.end()));
    }
  }
    
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode ChargedParticlesToTracks::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  m_trackCont = new LHCb::Track::Container();
  put(m_trackCont, m_trackOutputLocation);

  LHCb::IParticlePropertySvc* propertysvc = 
    svc<LHCb::IParticlePropertySvc>("LHCb::ParticlePropertySvc",true) ;
  
  LHCb::Particle::ConstVector particles;
  //const LHCb::Particle::Container* particlesCont;
  LHCb::Particle::Range particlesRange;
  const LHCb::ParticleProperty* prop;
  
  for( std::vector<std::string>::const_iterator iterC = m_partloc.begin();
       iterC != m_partloc.end(); ++iterC ){
    /*if(exist<LHCb::Particle::Container>(*iterC)){
      particlesCont = get<LHCb::Particle::Container>(*iterC);
      for( LHCb::Particle::Container::const_iterator iterP = particlesCont->begin();
           iterP != particlesCont->end(); ++iterP ){
        prop = propertysvc->find((*iterP)->particleID());
        if(m_masswindow<0.||abs((*iterP)->measuredMass()-(prop->mass()-m_massoffset))<m_masswindow){
          particles.push_back(*iterP);
          if(fullDetail()){
            plot((*iterP)->measuredMass(), m_linesname[*iterC], prop->mass()+m_massoffset-1.5*m_masswindow, 
                 prop->mass()+m_massoffset+1.5*m_masswindow, (int)(3*m_masswindow));
          }
        }
      }
      }else */
    if(exist<LHCb::Particle::Range>(*iterC)){
      particlesRange = get<LHCb::Particle::Range>(*iterC);
      for( LHCb::Particle::Range::const_iterator iterP = particlesRange.begin();
           iterP != particlesRange.end(); ++iterP ){
        prop = propertysvc->find((*iterP)->particleID());
        if(m_masswindow<0.||abs((*iterP)->measuredMass()-(prop->mass()+m_massoffset))<m_masswindow){
          particles.push_back(*iterP);
          if(fullDetail()){
            plot((*iterP)->measuredMass(), m_linesname[*iterC], prop->mass()+m_massoffset-1.5*m_masswindow, 
                 prop->mass()+m_massoffset+1.5*m_masswindow, (int)(3*m_masswindow));
          }
        }
      }
    }
  }
  
  if(particles.empty()){
    setFilterPassed(false);
    return StatusCode::SUCCESS;
  }

  DumpTracks(particles);
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode ChargedParticlesToTracks::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiHistoAlg::finalize();  // must be called after all other actions
}

//=============================================================================

void ChargedParticlesToTracks::DumpTracks(LHCb::Particle::ConstVector particles)
{
  LHCb::Particle::ConstVector::const_iterator iterP;
  const LHCb::ProtoParticle* proto;
  const LHCb::Track* track;
  LHCb::Track* refittedtrack;
  for( iterP = particles.begin(); iterP != particles.end(); ++iterP ){
    if((*iterP)->isBasicParticle()){
      proto = (*iterP)->proto();
      if(proto){
        if(proto->charge() != 0){
          track = (*iterP)->proto()->track();
          if(track){
            refittedtrack = const_cast<LHCb::Track*>((*iterP)->proto()->track());
            if(m_refit){
              m_trackPreFit->fit(*refittedtrack);
              m_trackFit->fit(*refittedtrack);
            }
            if(find(m_trackCont->begin(), m_trackCont->end(), refittedtrack) == m_trackCont->end()){
              m_trackCont->add(refittedtrack);
            }
          }
        }
      }
    }else{
      DumpTracks((*iterP)->daughtersVector());
    }
  }
}
