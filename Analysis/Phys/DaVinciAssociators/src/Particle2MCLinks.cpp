// $Id: Particle2MCLinks.cpp,v 1.26 2008-09-25 12:00:54 odescham Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

// from event model
#include "Event/ProtoParticle.h"
#include "Event/MCParticle.h"
#include "Event/Particle.h"

// local
#include "Particle2MCLinks.h"

using namespace LHCb;


//-----------------------------------------------------------------------------
// Implementation file for class : Particle2MCLinks
//
// 10/05/2002 : Philippe Charpentier
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory

DECLARE_ALGORITHM_FACTORY(Particle2MCLinks )

#define _verbose if( msgLevel(MSG::VERBOSE) ) verbose()
#define _debug if( msgLevel(MSG::DEBUG) ) debug()
#define _info if( msgLevel(MSG::INFO) ) info()

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Particle2MCLinks::Particle2MCLinks( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : AsctAlgorithm ( name , pSvcLocator )
  , m_chargedLink(NULL)
  , m_neutralLink(NULL)
{

  
  declareProperty( "ChargedPPLocation", m_chargedPPLocation);
  declareProperty( "NeutralPPLocation", m_neutralPPLocation);

  m_chargedPPLocation.push_back( ProtoParticleLocation::Charged );
  m_chargedPPLocation.push_back( ProtoParticleLocation::Upstream );
  

}

//=============================================================================
// Destructor
//=============================================================================
Particle2MCLinks::~Particle2MCLinks() {} 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode Particle2MCLinks::initialize() {

  _debug << "==> Initialise" << endmsg;

  StatusCode sc = GaudiAlgorithm::initialize();
  if( !sc.isSuccess() ) return sc;


  // property setting for Neutral is based on context() 
  // default setup in initialize() to please the linker design
  if(m_neutralPPLocation.empty()){
      std::string out( context() );
      std::transform( context().begin() , context().end() , out.begin () , ::toupper ) ;
    if( out == "HLT" ){
      m_neutralPPLocation.push_back( ProtoParticleLocation::HltNeutrals );
    }else{
      m_neutralPPLocation.push_back( ProtoParticleLocation::Neutrals );
    }
  }
  

  // Create a helper class for each type of Protoparticles
  m_chargedLink = new Object2MCLinker< LHCb::ProtoParticle >( this, 
                                                            Particle2MCMethod::ChargedPP,
                                                            m_chargedPPLocation);
  m_neutralLink = new Object2MCLinker< LHCb::ProtoParticle >( this, 
                                                              Particle2MCMethod::NeutralPP,
                                                              m_neutralPPLocation
                                                              );
  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode Particle2MCLinks::execute() {

  _debug << "==> Execute" << endmsg;

  if( "" != outputTable() ) {
    return Error( "Usage of Relations in Particle Associators is obsolete, use Linkers instead");
  }
  
  //////////////////////////////////
  // Loop on Particles containers //
  //////////////////////////////////

  for( std::vector<std::string>::iterator inp = m_inputData.begin(); 
       m_inputData.end()!= inp; inp++) {
    // create a Linker table for this Particle container
    const std::string linkContainer = 
      *inp + Particle2MCMethod::extension[Particle2MCMethod::Links];

    // Just a fake helper class
    Object2MCLinker< LHCb::Particle > p2MCLink(this);
    Object2MCLinker< LHCb::Particle >::Linker* linkerTable = p2MCLink.linkerTable( linkContainer );
    if( NULL == linkerTable ) continue;

    // Get Particles
    SmartDataPtr<Particles> parts (eventSvc(), *inp);
    if( !parts ) {
      _verbose << "Could not find container " << *inp << endmsg;
      continue;
    }
    int npp = parts->size();
    int nrel = 0;
    int nass = 0;
    _debug << "    " << npp 
           << " Particles retrieved from " << *inp << endmsg;
    
    // loop on Parts and MCParts to match them
    for( Particles::const_iterator pIt=parts->begin() ;
         parts->end() != pIt; ++pIt ) {
      Particle* PPP = *pIt ;
      _verbose << "Particle " << PPP->momentum() << endmsg
               << "    Particle " << objectName(*pIt);

      const MCParticle* mcPart = nullptr ;
      Object2MCLinker< LHCb::ProtoParticle >* link = (*pIt)->charge() ? m_chargedLink : m_neutralLink;
      // check if it is from a ProtoParticle
      const ProtoParticle* protoPart = (*pIt)->proto() ;
      if( protoPart ) {
        if( msgLevel( MSG::VERBOSE )) {
          std::string strCharged = (*pIt)->charge() ? "Charged" : "Neutral";
          verbose() << " from " << strCharged << " ProtoParticle " 
                    << protoPart->key();
        }
        // Use the Linker 
        mcPart = link->first( protoPart );
      } else 
      {
        verbose() << " is not from a ProtoParticle";
      }
      
      if( mcPart ) {
        nass++;
        _verbose << " is associated to MCParts:" << endmsg ;
        do {
          _verbose << "     " << mcPart->key() << " " << mcPart->particleID().pid() << " " << mcPart->momentum() << endmsg ;
          linkerTable->link( *pIt, mcPart, link->weight());
          nrel++;
          mcPart = link->next();
        } while( NULL != mcPart);
      } 
      else {
        _verbose << " is not associated to any MCPart" << endmsg ;
      } 
    }
    _debug << "Out of " << npp << " Particles in " << *inp << ", "
           << nass << " are associated, "
           << nrel << " relations found" << endmsg;
  }
  return StatusCode::SUCCESS ;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode Particle2MCLinks::finalize() {

  _debug << "==> Finalize" << endmsg;

  delete m_chargedLink; m_chargedLink = nullptr;
  delete m_neutralLink; m_neutralLink = nullptr;
  return GaudiAlgorithm::finalize();
}

//=============================================================================
