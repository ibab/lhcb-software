// $Id: CheatedReco.cpp,v 1.3 2003-06-20 11:59:49 odie Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IToolSvc.h"

#include "Event/EventHeader.h"
#include "Event/MCParticle.h"
#include "Event/Vertex.h"
#include "Event/Particle.h"

#include "DaVinciMCTools/IMCDecayFinder.h"
#include "DaVinciTools/IDecayFinder.h"

// local
#include "CheatedReco.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CheatedReco
//
// 2003-02-05 : Olivier Dormond
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<CheatedReco>          s_factory ;
const        IAlgFactory& CheatedRecoFactory = s_factory ; 

std::string strip(std::string text)
{
  std::string::size_type start = 0, end = text.length();
  while( (start != end) && (text[start]==' ' || text[start]=='\t') )
    start++;
  while( (start != end) && (text[end-1]==' ' || text[end-1]=='\t') )
    end--;
  return text.substr(start,end-start);
}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CheatedReco::CheatedReco( const std::string& name,
              ISvcLocator* pSvcLocator)
  : DVAlgorithm ( name , pSvcLocator ), m_count(0), m_mcDecayFinder(0),
    m_decays(), m_keys(), m_pids(), m_sources(), m_products(), m_nameMCAsct(),
    m_pAsctLinks(0)
{
  m_decays.clear();
	declareProperty( "Key", m_key = "" );
	declareProperty( "Decays", m_decays );
  declareProperty( "MCAssociator",  m_nameMCAsct = "Particle2MCLinksAsct" );
}

//=============================================================================
// Destructor
//=============================================================================
CheatedReco::~CheatedReco() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode CheatedReco::initialize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "==> Initialise" << endreq;

  StatusCode sc = loadTools();
  if(sc.isFailure()) {
    msg << MSG::ERROR << "Unable to load tools" << endreq;
    return StatusCode::FAILURE;
  }

  sc = toolSvc()->retrieveTool( "MCDecayFinder", m_mcDecayFinder, this );
  if( sc.isFailure() ) {
    msg << MSG::FATAL << "Unable to retrieve the MCDecayFinder" << endreq;
    return sc;
  }

  sc = toolSvc()->retrieveTool( "Particle2MCLinksAsct", m_nameMCAsct, m_pAsctLinks, this);
  if( sc.isFailure() || 0 == m_pAsctLinks) {
    msg << MSG::FATAL << "Unable to retrieve Link Associator tool" << endreq;
    return sc;
  }
  
  std::string error_msg =
    "Bad decay definition. The following syntax is expected:\n"\
    "<key name> # <particle name> {\% <key name>} [@ <decay description>]\n"\
    "'{}' means 'zero or more instance' and '[]' means '0 or 1 instance'.\n"\
    "At least one of '\%' or '@' must be present.";
  std::vector<std::string>::const_iterator di;
  for( di=m_decays.begin(); di!=m_decays.end(); di++ ) {
    std::string::size_type kpos = di->find('#');
    if( kpos == std::string::npos ) {
      msg << MSG::FATAL << error_msg << endreq;
      return StatusCode::FAILURE;
    }
    std::string key = strip(di->substr(0,kpos-1));

    std::string partname, sourcekeys, decaydesc;
    std::string::size_type atpos = di->find('@');
    if( atpos != std::string::npos ) {
      // '@' is present
      std::string::size_type perpos = di->find('%');
      if( perpos == std::string::npos ) {
        // No '%'
        perpos = atpos;
        sourcekeys = "";
      }
      else
        sourcekeys = strip(di->substr(perpos+1, atpos-1-(perpos+1)));
      partname = strip(di->substr(kpos+1, perpos-1-(kpos+1)));
      decaydesc = strip(di->substr(atpos+1, std::string::npos));
    } else {
      // No '@'
      std::string::size_type perpos = di->find('%');
      if( perpos == std::string::npos ) {
        msg << MSG::FATAL << error_msg << endreq;
        return StatusCode::FAILURE;
      }
      partname = strip(di->substr(kpos+1, perpos-1-(kpos+1)));
      sourcekeys = strip(di->substr(perpos+1, std::string::npos));
      decaydesc = "";
    }
    if( !decaydesc.empty() ) {
      StatusCode sc = m_mcDecayFinder->setDecay(decaydesc);
      if( !sc.isSuccess() ) {
        msg << MSG::FATAL << "Invalid decay description '" << decaydesc
            << "'" << endreq;
        return StatusCode::FAILURE;
      }
    }
    msg << MSG::DEBUG << "Source keys: " << sourcekeys << endreq;
    std::vector<std::string> sources;
    sources.clear();
    while( !sourcekeys.empty() ) {
      std::string::size_type pos = sourcekeys.find('%');
      std::string source = strip(sourcekeys.substr(0,pos));
      sources.push_back(source);
      if( pos == std::string::npos )
        break;
      sourcekeys = strip(sourcekeys.substr(pos+1,std::string::npos));
    }
    msg << MSG::DEBUG << "Number of source keys collected: "
        << sources.size() << endreq;
    std::vector<std::string>::const_iterator si;
    for( si = sources.begin(); si != sources.end(); si++ ) {
      std::vector<std::string>::const_iterator ki;
      for( ki=m_keys.begin(); ki!=m_keys.end(); ki++ )
        if( *ki == *si )
          break;
      if( ki == m_keys.end() ) {
        msg << MSG::ERROR << "Attempting to create key '" << key
            << "' from unregistered key '" << *si << "'" << endreq;
        return StatusCode::FAILURE;
      }
    }
    ParticleProperty *prop = ppSvc()->find(partname);
    if( !prop ) {
      msg << MSG::ERROR << "Unknown particle: " << partname << endreq;
      return StatusCode::FAILURE;
    }
    m_keys.push_back(key);
    m_pids[key] = prop->jetsetID();
    m_sources[key].first = sources;
    m_sources[key].second = decaydesc;
  }

  std::vector<std::string>::const_iterator ki;
  for( ki=m_keys.begin(); ki!=m_keys.end(); ki++ )
    if( *ki == m_key )
      break;
  if( ki == m_keys.end() ) {
    msg << MSG::ERROR << "The requested final key '" << m_key
        << "' was not defined!" << endreq;
    return StatusCode::FAILURE;
  }
	
  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode CheatedReco::execute() {

  MsgStream  msg( msgSvc(), name() );
  msg << MSG::DEBUG << "==> Execute" << endreq;

  SmartDataPtr<EventHeader> evt_head(eventSvc(), EventHeaderLocation::Default );
  if( !evt_head )
  {
    msg << MSG::ERROR << "Could not retrieve event header. Skipping it!"
        << endreq;
    setFilterPassed(false);
    return StatusCode::SUCCESS;
  }

  SmartDataPtr<MCParticles> mcparts(eventSvc(), MCParticleLocation::Default );
  if( !mcparts ) {
    msg << MSG::FATAL << "Enable to find MC particles at '"
        << MCParticleLocation::Default << "'" << endreq;
    return StatusCode::FAILURE;
  }

  bool found_decay = true;
  m_products.clear();
  std::vector<std::string>::const_iterator ki;
  for( ki=m_keys.begin(); ki!=m_keys.end(); ki++ )
  {
    msg << MSG::DEBUG << "Processing key: " << *ki << endreq;
    const MCParticle *mcpart = NULL;
    ParticleVector products;
    products.clear();
    if( !m_sources[*ki].second.empty() ) {
      m_mcDecayFinder->setDecay(m_sources[*ki].second);
      if( !m_mcDecayFinder->findDecay( mcparts, mcpart ) ) {
        msg << MSG::DEBUG << "DecayFinder failed" << endreq;
        found_decay = false;
        break;
      }
      std::vector<MCParticle *> mclifted;
      m_mcDecayFinder->decayMembers( mcpart, mclifted );
      msg << MSG::DEBUG << "Number of mc-particles lifted: " << mclifted.size()
          << endreq;
      std::vector<MCParticle *>::const_iterator mcpi;
      bool reconstructed = true;
      for(mcpi=mclifted.begin(); (mcpi!=mclifted.end()) && reconstructed; mcpi++){
        ParticlesToMCLinks partsLinks = m_pAsctLinks->rangeTo( *mcpi );
        if( partsLinks.size() == 0 ) {
          reconstructed = false;
          continue;
        }
        Particle *ap = partsLinks.front().to();
        if( (*mcpi)->particleID().pid() != ap->particleID().pid() ) {
          msg << MSG::DEBUG << "Mis-ID particle in signal!  "
              << ap->particleID().pid() << endreq;
          double mcmass = (*mcpi)->momentum().m();
          HepLorentzVector pap = ap->momentum();
          HepLorentzVector newpap( pap.x(), pap.y(), pap.z(),
                                   sqrt(pap.vect().mag2()+mcmass*mcmass) );
          ap->setMomentum(newpap); //OVERWRITE mass and PID
          ap->setParticleID((*mcpi)->particleID());
        }
        products.push_back(ap);
      }
      msg << MSG::DEBUG << "Number of products reconstructed: "
          << products.size() << endreq;
      if( !reconstructed ) {
        msg << MSG::DEBUG << "Reconstruction failed" << endreq;
        found_decay = false;
        break;
      }
    }
    msg << MSG::DEBUG << "Number of particles after mc collect: "
        << products.size() << endreq;
    
    std::vector<std::string>::const_iterator ski;
    for(ski=m_sources[*ki].first.begin();ski!=m_sources[*ki].first.end();ski++){
      products.push_back(m_products[*ski]);
      msg << MSG::DEBUG << "endVertex address for key '" << *ski << "': "
          << m_products[*ski]->endVertex() << endreq;
    }
    msg << MSG::DEBUG << "Number of particles after key collect: "
        << products.size() << endreq;
    
    Vertex *vtx = new Vertex();
    StatusCode sc = vertexFitter()->fitVertex(products, *vtx);
    msg << MSG::DEBUG << "Number of particles after vtx fit: "
        << products.size() << endreq;
    if( sc.isFailure() ) {
      msg << MSG::DEBUG << "Vertex fit failed" << endreq;
      found_decay = false;
      delete vtx;
      break;
    }
    m_products[*ki] = new Particle();
    sc = particleStuffer()->fillParticle(*vtx,*m_products[*ki],m_pids[*ki]);
    if( sc.isFailure() ) {
      msg << MSG::DEBUG << "Particle stuffing failed" << endreq;
      found_decay = false;
      break;
    }
  }

  if( !found_decay ) {
    msg << MSG::DEBUG << "Couldn't reconstruct the decay." << endreq;
    setFilterPassed(false);
  } else {
    m_count++;
    desktop()->createParticle( m_products[m_key] );
    desktop()->saveDesktop();
  }
  
  std::map<std::string, Particle *>::iterator pi;
  for( pi = m_products.begin(); pi != m_products.end(); pi++ )
    if( pi->second ) {
      if( pi->second->endVertex() )
        delete pi->second->endVertex();
      delete pi->second;
    }
      
  msg << MSG::INFO << "CHEATEDRECO "
      << evt_head->runNum() << ' ' << evt_head->evtNum() << ' '
      << found_decay << endreq;

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode CheatedReco::finalize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "==> Finalize" << endreq;

  msg << MSG::INFO << "Number of reconstructed events:" << m_count << endreq;

  return StatusCode::SUCCESS;
}

//=============================================================================
