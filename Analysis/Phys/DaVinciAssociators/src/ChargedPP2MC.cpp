// $Id: ChargedPP2MC.cpp,v 1.1 2002-10-02 07:06:26 phicharp Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

// local
#include "ChargedPP2MC.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ChargedPP2MC
//
// 10/07/2002 : Philippe Charpentier
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<ChargedPP2MC>          s_factory ;
const        IAlgFactory& ChargedPP2MCFactory = s_factory ; 

#define ifLog(sev) log << sev; if( log.level() <= (sev) ) log

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ChargedPP2MC::ChargedPP2MC( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : Algorithm ( name , pSvcLocator )
  , m_outputTable( ChargedPP2MCAsctLocation )
  , m_trackAsctName( "Track2MCParticleAsct" )
{
  m_inputData.push_back( ProtoParticleLocation::Charged );
  m_inputData.push_back( ProtoParticleLocation::Upstream );

  declareProperty( "InputData", m_inputData );
  declareProperty( "OutputTable", m_outputTable );
  declareProperty( "TrackAsct", m_trackAsctName );
}

//=============================================================================
// Destructor
//=============================================================================
ChargedPP2MC::~ChargedPP2MC() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode ChargedPP2MC::initialize() {

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
StatusCode ChargedPP2MC::execute() {

  MsgStream  log( msgSvc(), name() );
  log << MSG::DEBUG << "==> Execute" << endreq;

  // create an association table 
  ProtoParticle2MCAsct::Table* table = new ProtoParticle2MCAsct::Table();
  
  //////////////////////////////////
  // Loop on Particles containers //
  //////////////////////////////////

  for( std::vector<std::string>::const_iterator inp = m_inputData.begin(); 
       m_inputData.end()!= inp; inp++) {
    if( !m_track2MCParticleAsct->tableExists() ) {
      log << MSG::INFO << "Unable to retrieve the Track2MC association table"
          << endreq;
      break;
    }
    // Get ProtoParticles
    SmartDataPtr<ProtoParticles> protos (eventSvc(), *inp);
    if( 0 != protos ) {
      log << MSG::VERBOSE << "    " << protos->size()
          << " ProtoParticles retrieved from " 
          << *inp << endreq;
    }
    else {
      log << MSG::INFO << "    *** Could not retrieve ProtoParticles from "
          << *inp << endreq;
      continue;
    }
    
    // loop on Protos to match them from their tracks
    int nrel = 0 ;
    for( ProtoParticles::const_iterator pIt=protos->begin() ;
         protos->end() != pIt; pIt++) {
      ifLog( MSG::VERBOSE )
        << "    ProtoParticle " << (*pIt)->key();
      // Follow links from this protoParticle via tracks
      const TrStoredTrack* track = (*pIt)->track() ;
      if( 0 != track ) {
        const Tr2MCPartAsct::ToRange mcPartRange
          = m_track2MCParticleAsct->rangeFrom( track);
        ifLog( MSG::VERBOSE )
          << " from track " << track->key();
        if( !mcPartRange.empty() ) {
          Tr2MCPartAsct::ToIterator mcPartIt;
          ifLog( MSG::VERBOSE )
            << " associated to " << mcPartRange.end()-mcPartRange.end()+1
            << " MCParts : ";
          for( mcPartIt = mcPartRange.begin(); 
               mcPartRange.end() != mcPartIt; mcPartIt++) {
            const MCParticle* mcPart = mcPartIt->to();
            double weight = mcPartIt->weight();
            if( 0 != mcPart ) {
              ifLog(MSG::VERBOSE)
                << mcPart->key() << " - ";
              table->relate( *pIt, mcPart, weight);
              nrel++;
            }
          }
        } else {
          log << " not associated to an MCPart";
        }
      } else {
        log << " not originating from a track";
      }
      log << endreq;
    }
    ifLog( MSG::VERBOSE )
        << protos->end() - protos->begin() << " ProtoParts associated: "
        << nrel << " relations found" << endreq;
  }
  
  // Register the table on the TES
  StatusCode sc = eventSvc()->registerObject( outputTable(), table);
  if( sc.isFailure() ) {
    ifLog( MSG::FATAL )
      << "     *** Could not register table " << outputTable() << endreq;
    delete table;
    return sc;
  } else {
    ifLog( MSG::VERBOSE )
      << "     Registered table " << outputTable() << endreq;
  }
  return StatusCode::SUCCESS ;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode ChargedPP2MC::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Finalize" << endreq;

  return StatusCode::SUCCESS;
}

//=============================================================================
