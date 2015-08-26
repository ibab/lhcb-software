// Include files
// local
#include "BestPIDParticleMaker.h"

using namespace Gaudi::Units ;
using namespace LHCb ;
using namespace std ;

//-----------------------------------------------------------------------------
/** @class BestPIDParticleMaker BestPIDParticleMaker.h
 *  
 *  @Version 1r0
 *  @author Neal Gueissaz
 *  @date   22 june 2009
 */
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( BestPIDParticleMaker )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
BestPIDParticleMaker::BestPIDParticleMaker( const std::string& name,
                ISvcLocator* pSvcLocator)
  : CombinedParticleMaker ( name , pSvcLocator ){
  
  // Particle types to create
  m_ParticleList.push_back("muon");
  m_ParticleList.push_back("electron");
  m_ParticleList.push_back("kaon");
  m_ParticleList.push_back("proton");
  m_ParticleList.push_back("pion");
  declareProperty ( "Particles" , m_ParticleList, 
                    "Possible Particles to create : muon, electron, kaon, proton, pion" ) ;
}

//=============================================================================
// Destructor
//=============================================================================
BestPIDParticleMaker::~BestPIDParticleMaker() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode BestPIDParticleMaker::initialize() {

  // intialize base class
  const StatusCode sc = CombinedParticleMaker::initialize();
  if ( sc.isFailure() ) 
    return Error( "Failed to initialize base class : make sure you have set the dummy Particle job option to 'pion'" );
  
  if (msgLevel(MSG::DEBUG)) debug() << "==> Initialize the BestPIDParticleMaker algorithm" << endmsg;

  if ( m_ParticleList.empty() )
    return Error( "A list of particles types must be specified" );
  if (msgLevel(MSG::DEBUG)) debug() << "Will produce : " << m_ParticleList << endmsg;

  // loop over selection and load ProtoParticle selectors
  m_protoMap.clear();
  for ( std::vector<std::string>::const_iterator ip = m_ParticleList.begin();
        ip != m_ParticleList.end(); ++ip ){
    
    // Get particle type name
    const string name = *ip;//convertName( *ip );
    // get pp name and tooltype
    std::string ppName, toolType;
    if( "muon" == name ){
      ppName   = "mu+";
      toolType = m_muProtoFilter;
    }
    else if( "electron" == name ){
      ppName   = "e+";
      toolType = m_elProtoFilter;
    }
    else if( "kaon" == name ){
      ppName   = "K+";
      toolType = m_kaProtoFilter;
    }
    else if( "proton" == name ){
      ppName   = "p+";
      toolType = m_prProtoFilter;
    }
    else if( "pion" == name ){
      ppName   = "pi+";
      toolType = m_piProtoFilter;
    }
    else{
      return Error( "Unknown particle selection '" + *ip + "'" );
    }
    
    // Get particle properties
    const LHCb::ParticleProperty * partProp = ppSvc()->find( ppName );
    
    // load tool into map
    if (msgLevel(MSG::DEBUG)) debug() << "Particle type " << name 
                                      << " using ProtoParticle Filter '"
                                      << toolType << "'" << endmsg;
    const IProtoParticleFilter * t = tool<IProtoParticleFilter>
      ( toolType, name, this );
    m_protoMap.push_back( ProtoPair(partProp,t) );
    
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode BestPIDParticleMaker::makeParticles( Particle::Vector & parts ){
  
  // Load the ProtoParticles
  const LHCb::ProtoParticle::ConstVector& pps = protos() ;
  if (msgLevel(MSG::DEBUG)) 
    debug() << "Making Particles from " << pps.size() 
            << " ProtoParticles at "<< m_input << endmsg;
  
  // loop over ProtoParticles
  for ( LHCb::ProtoParticle::ConstVector::const_iterator ipp = pps.begin();
        pps.end() != ipp; ++ipp ){
    
    const ProtoParticle * pp = *ipp;
    
    // get pointer to track (should always exist for charged tracks)
    const Track * track = (*ipp)->track();
    if ( !track ) 
      return Error( "Charged ProtoParticle has null track reference !" );

    if (track->states().empty()){
      Warning("Track has empty states. This is likely to be bug https://savannah.cern.ch/bugs/index.php?70979");
      continue ;
    }
   
    TrackTally & tally = trackTally( track->type() );
    ++tally.totProtos;
    
    // Select tracks
    if( msgLevel(MSG::VERBOSE))
      verbose() <<"Trying Track "<< track->key() << endmsg;
    if( !trSel()->accept(*track) ) continue;
    if( msgLevel(MSG::VERBOSE)) {
      verbose() << " -> Track selected " << track->key()  
                << " " << track->firstState().momentum() << endmsg;
    }
    ++tally.selProtos;
    
    // Do PID checks ?
    if ( m_testPIDinfo ) checkPIDInfo(*ipp);
    
    //Create a vector of PID and DLL pairs
    DLLPIDVector dllpid;
    dllpid.push_back( DLLPIDPair( "mu+", 
                                  pp->info( ProtoParticle::CombDLLmu,0) ) );
    dllpid.push_back( DLLPIDPair( "e+", 
                                  pp->info( ProtoParticle::CombDLLe,0) ) );
    dllpid.push_back( DLLPIDPair( "K+", 
                                  pp->info( ProtoParticle::CombDLLk,0) ) );
    dllpid.push_back( DLLPIDPair( "p+", 
                                  pp->info( ProtoParticle::CombDLLp,0) ) );
    
    dllpid.push_back( DLLPIDPair( "pi+", 
                                  pp->info( ProtoParticle::CombDLLpi,0) ) );
    
    
    //Sort the DLL
    sort( dllpid.begin(), dllpid.end(), DLLPIDPair::cmp );
    
    if(msgLevel(MSG::VERBOSE)){
      verbose()<<"ProtoParticle DLL ";
      for( DLLPIDVector::iterator j = dllpid.begin();j != dllpid.end();j++){
        verbose() << (*j).GetPid()<<" : "<< (*j).GetDll() <<" ";
      }
      verbose() << endmsg;
    }
    
    DLLPIDVector::iterator i = dllpid.begin();
    string pid;
    ProtoMap::const_iterator imap;
    while( i != dllpid.end() ){
      
      pid = (*i).GetPid();
      
      //Continue if filters are not satisfied
      bool sel = false;
      for ( imap = m_protoMap.begin(); imap != m_protoMap.end(); ++imap ){
        if( (*imap).first->particle() == pid ){
          sel = (*imap).second->isSatisfied( *ipp );
          break;
        }
      }
      
      if(msgLevel(MSG::VERBOSE)) 
        verbose() << " -> Particle type " << pid 
                  << " selected=" << sel << endmsg;
      if( !sel ){ i++; continue; }
      
      //get out of here
      break;
    }
    if( i==dllpid.end() ) continue;
    
    // make a new Particle
    Particle * part = new Particle();
    
    // fill Parameters
    StatusCode sc = fillParticle( *ipp, (*imap).first, part );
    if ( sc.isFailure() ){
      Warning( "Failed to fill Particle -> rejected" );
      delete part;
    } else {
      // add to container
      parts.push_back(part);
      // increment tally
      tally.addToType( pid );
      //fill the map
      //m_map.insert( track, part );
    }
    
  }//end loop over protoparticles
  
  if (msgLevel(MSG::DEBUG)) debug()<< parts.size() <<" Particles have been made !"<< endmsg;
  
  return StatusCode::SUCCESS;
}

//===========================================================================
// Finalize
//===========================================================================
StatusCode BestPIDParticleMaker::finalize() {
  
  if (msgLevel(MSG::DEBUG)) debug() << "==> Finalize the BestPIDParticleMaker algorithm" << endmsg;
  // finalize base class
  return CombinedParticleMaker::finalize();
}
