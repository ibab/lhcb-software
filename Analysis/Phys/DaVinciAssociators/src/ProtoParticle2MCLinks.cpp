// $Id: ProtoParticle2MCLinks.cpp,v 1.3 2002-07-27 19:32:32 gcorti Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

// local
#include "ProtoParticle2MCLinks.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ProtoParticle2MCLinks
//
// 10/07/2002 : Philippe Charpentier
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<ProtoParticle2MCLinks>          s_factory ;
const        IAlgFactory& ProtoParticle2MCLinksFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ProtoParticle2MCLinks::ProtoParticle2MCLinks( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : Algorithm ( name , pSvcLocator )
  , m_outputTable( ProtoParticle2MCAsctLocation )
  , m_trackAsctName( "Track2MCParticleAsct" )
{
  m_inputData.push_back( ProtoParticleLocation::Charged );
  m_inputData.push_back( ProtoParticleLocation::Neutrals );

  declareProperty( "InputData", m_inputData );
  declareProperty( "OutputTable", m_outputTable );
  declareProperty( "TrackAsct", m_trackAsctName );
}

//=============================================================================
// Destructor
//=============================================================================
ProtoParticle2MCLinks::~ProtoParticle2MCLinks() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode ProtoParticle2MCLinks::initialize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Initialise" << endreq;

  // Retrieve the StoredTracks2MC associator
  StatusCode sc = toolSvc()->retrieveTool(
        "AssociatorWeighted<TrStoredTrack,MCParticle,double>", 
        m_trackAsctName, m_track2MCParticleAsct);
  if( !sc.isSuccess() ) return sc;
  
  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode ProtoParticle2MCLinks::execute() {

  MsgStream  log( msgSvc(), name() );
  log << MSG::DEBUG << "==> Execute" << endreq;

  SmartDataPtr<MCParticles> 
    mcParts( eventSvc(), MCParticleLocation::Default );
  if( 0 != mcParts ) {
    log << MSG::DEBUG << "    " << mcParts->size()
        << " MCParts retrieved" << endreq;
  }
  else {
    log << MSG::FATAL << "    *** Could not retrieve MCParts" << endreq;
    return StatusCode::FAILURE;
  }

  // create an association table and register it in the TES
  ProtoParticle2MCAsct::Table* table = new ProtoParticle2MCAsct::Table();
  
  //////////////////////////////////
  // Loop on Particles containers //
  //////////////////////////////////

  for( std::vector<std::string>::iterator inp = m_inputData.begin(); 
       m_inputData.end()!= inp; inp++) {
    // Get Particles
    SmartDataPtr<ProtoParticles> protos (eventSvc(), *inp);
    if( 0 != protos ) {
      log << MSG::DEBUG << "    " << protos->size()
          << " ProtoParticles retrieved from " 
          << *inp << endreq;
    }
    else {
      log << MSG::ERROR << "    *** Could not retrieve ProtoParticles from "
          << *inp << endreq;
      continue;
    }
    
    // loop on Protos and MCParts to match them
    int nrel = 0 ;
    for( ProtoParticles::const_iterator pIt=protos->begin() ;
         protos->end() != pIt; pIt++) {
      log << MSG::DEBUG << "    ProtoParticle " << (*pIt)->key();
      // Follow links from this protoParticle via tracks and clusters
      TrStoredTrack* track = (*pIt)->track() ;
      if( 0 != track ) {
        log << " from track " << track->key();
        MCParticle* mcPart = m_track2MCParticleAsct->associatedFrom( track );
        if( 0 != mcPart ) {
          log << " associated to MCPart " << mcPart->key();
          table->relate( *pIt, mcPart);
          nrel++;
        }
      }
      log << endreq;
    }
  
    log << MSG::DEBUG
        << protos->end() - protos->begin() << " ProtoParts associated with "
        << mcParts->end() - mcParts->begin() << " MCParts: " 
        << nrel << " relations found" << endreq;
  }
  
  // Register the table on the TES
  StatusCode sc = eventSvc()->registerObject( outputTable(), table);
  if( sc.isFailure() ) {
    log << MSG::FATAL << "     *** Could not register table " << outputTable()
        << endreq;
    delete table;
    return sc;
  } else {
    log << MSG::DEBUG << "     Registered table " << outputTable() << endreq;
  }
  return StatusCode::SUCCESS ;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode ProtoParticle2MCLinks::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Finalize" << endreq;

  return StatusCode::SUCCESS;
}

//=============================================================================
