// $Id: Particle2MCLinks.cpp,v 1.19 2005-02-02 09:49:13 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

// from event model
#include "Event/ProtoParticle.h"
#include "Event/TrgTrack.h"
#include "Event/MCParticle.h"
#include "Event/Particle.h"

// local
#include "Particle2MCLinks.h"

//-----------------------------------------------------------------------------
// Implementation file for class : Particle2MCLinks
//
// 10/05/2002 : Philippe Charpentier
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<Particle2MCLinks>          s_factory ;
const        IAlgFactory& Particle2MCLinksFactory = s_factory ; 

#define ifMsg(sev) msg << sev; if( msg.level() <= (sev) ) msg

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Particle2MCLinks::Particle2MCLinks( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : AsctAlgorithm ( name , pSvcLocator )
  , m_chargedLink(NULL)
  , m_neutralLink(NULL)
{
  m_chargedPPLocation.push_back( ProtoParticleLocation::Charged );
  m_chargedPPLocation.push_back( ProtoParticleLocation::Upstream );
  m_neutralPPLocation.push_back( ProtoParticleLocation::Neutrals );
  m_outputTable = "Phys/Relations/Part2MCfromLinks" ;
  declareProperty( "ChargedPPLocation", m_chargedPPLocation);
  declareProperty( "NeutralPPLocation", m_neutralPPLocation);
}

//=============================================================================
// Destructor
//=============================================================================
Particle2MCLinks::~Particle2MCLinks() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode Particle2MCLinks::initialize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "==> Initialise" << endreq;

  StatusCode sc = GaudiAlgorithm::initialize();
  if( !sc.isSuccess() ) return sc;

  // Create a helper class for each type of Protoparticles
  m_chargedLink = new Object2MCLink( this, Particle2MCMethod::ChargedPP,
                                       m_chargedPPLocation);
  m_neutralLink = new Object2MCLink( this, Particle2MCMethod::NeutralPP,
                                       m_neutralPPLocation);
  return sc;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode Particle2MCLinks::execute() {

  MsgStream  msg( msgSvc(), name() );
  msg << MSG::DEBUG << "==> Execute" << endreq;

  // create an association table and register it in the TES
  Particle2MCLinksAsct::Table* table = 
    outputTable() == "" ? NULL : new Particle2MCLinksAsct::Table();

  
  //////////////////////////////////
  // Loop on Particles containers //
  //////////////////////////////////

  for( std::vector<std::string>::iterator inp = m_inputData.begin(); 
       m_inputData.end()!= inp; inp++) {
    // create a Linker table for this Particle container
    const std::string linkContainer = 
      *inp + Particle2MCMethod::extension[Particle2MCMethod::Links];
    // Just a fake helper class
    Object2MCLink p2MCLink(this);
    Object2MCLink::Linker* linkerTable = 
      p2MCLink.linkerTable( linkContainer );
    if( NULL == table && NULL == linkerTable ) continue;

    // Get Particles
    SmartDataPtr<Particles> parts (eventSvc(), *inp);
    if( 0 == parts ) {
      ifMsg(MSG::VERBOSE) << "Could not find container "
                          << *inp << endreq;
      continue;
    }
    int npp = parts->size();
    int nrel = 0;
    int nass = 0;
    ifMsg(MSG::DEBUG) << "    " << npp 
                      << " Particles retrieved from " << *inp << endreq;
    
    // PK: dirty hack from Olivier Callot <- to be removed
    LinkedTo<MCParticle> myLink( evtSvc(), msgSvc(), "" ) ;
    if ( exist<TrgTracks>(TrgTrackLocation::Long)){      
      myLink = LinkedTo<MCParticle>( evtSvc(), msgSvc(), TrgTrackLocation::Particle );
    } 
    
    // loop on Parts and MCParts to match them
    for( Particles::const_iterator pIt=parts->begin() ;
         parts->end() != pIt; ++pIt ) {
      Particle* PPP = *pIt ;
      verbose() << "Particle " << PPP->momentum() << endreq ;
      ifMsg(MSG::VERBOSE) << "    Particle " << objectName(*pIt);

      MCParticle* mcPart = NULL ;
      Object2MCLink* link = (*pIt)->charge() ? m_chargedLink : m_neutralLink;
      // check if it is from a ProtoParticle
      ProtoParticle* protoPart = dynamic_cast<ProtoParticle*>( (*pIt)->origin() ) ;
      if( NULL != protoPart ) {
        if( msg.level() <= MSG::VERBOSE ) {
          std::string strCharged = (*pIt)->charge() ? "Charged" : "Neutral";
          msg << " from " << strCharged << " ProtoParticle " 
              << protoPart->key();
        }
        // Use the Linker 
        mcPart = link->first( protoPart );
      } else {
      // check if it is from a Trigger track
        const TrgTrack* TrgT = dynamic_cast<const TrgTrack*>( PPP->origin() );
        if ( TrgT ) {
          if( msg.level() <= MSG::VERBOSE ) 
            msg << " from TrgTrack " << TrgT->key();
          mcPart = link->first( TrgT );
          if ( mcPart ){
            always() << "Yahooo! The linker works now for TrgTracks!" 
                     << " -> remove dirty hack from Particle2MCLInks" << endreq ;
          } else {
            // PK: dirty hack from Olivier Callot <- to be removed 
            if (!myLink.notFound()) mcPart = myLink.first( TrgT->key() );
          }
        } else {
          ifMsg(MSG::VERBOSE) << " from nothing";
        } // Trg
      } // Proto
      if( NULL == mcPart ) {
        ifMsg(MSG::VERBOSE) << " is not associated to any MCPart";
      } 
      else {
        nass++;
        ifMsg(MSG::VERBOSE) << " is associated to MCParts";
        do {
          ifMsg(MSG::VERBOSE) << " - " << mcPart->key();
          if( NULL != table )table->relate( *pIt, mcPart, link->weight());
          if( NULL != linkerTable) 
            linkerTable->link( *pIt, mcPart, link->weight());
          nrel++;
          mcPart = link->next();
        } while( NULL != mcPart);
      }
      ifMsg(MSG::VERBOSE) << endreq;
    }
    ifMsg( MSG::DEBUG )
      << "Out of " << npp << " Particles in " << *inp << ", "
      << nass << " are associated, "
      << nrel << " relations found" << endreq;
  }
  // Now register the table on the TES
  if( NULL != table ) {
    put( table, outputTable() );
    ifMsg(MSG::DEBUG)  << "     Registered table " 
                       << outputTable() << endreq;    
  }
  return StatusCode::SUCCESS ;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode Particle2MCLinks::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Finalize" << endreq;

  if( NULL != m_chargedLink ) delete m_chargedLink;
  if( NULL != m_neutralLink ) delete m_neutralLink;
  m_chargedLink = NULL;
  m_neutralLink = NULL;
  return GaudiAlgorithm::finalize();
}

//=============================================================================
