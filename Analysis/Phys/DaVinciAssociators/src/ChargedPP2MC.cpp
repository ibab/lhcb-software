// $Id: ChargedPP2MC.cpp,v 1.3 2004-06-11 15:26:15 phicharp Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

// from Event
#include "Event/TrStoredTrack.h"
#include "Event/MCParticle.h"

// local
#include "ChargedPP2MC.h"
#include "DaVinciAssociators/Particle2MCLink.h"


//-----------------------------------------------------------------------------
// Implementation file for class : ChargedPP2MC
//
// 10/07/2002 : Philippe Charpentier
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<ChargedPP2MC>          s_factory ;
const        IAlgFactory& ChargedPP2MCFactory = s_factory ; 

#define ifMsg(sev) msg << sev; if( msg.level() <= (sev) ) msg

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ChargedPP2MC::ChargedPP2MC( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : AsctAlgorithm ( name , pSvcLocator )
  , m_trackAsctName( "Track2MCAsct" )
{
  m_inputData.push_back( ProtoParticleLocation::Charged );
  m_inputData.push_back( ProtoParticleLocation::Upstream );

  m_outputTable = ChargedPP2MCAsctLocation;
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

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "==> Initialise" << endreq;

  StatusCode sc = GaudiAlgorithm::initialize();
  if( !sc.isSuccess() ) return sc;

  // Retrieve the StoredTracks2MC associator (for old data only)
  m_track2MCAsct = 
    tool<Tr2MCPartAsct>("AssociatorWeighted<TrStoredTrack,MCParticle,double>",
                        m_trackAsctName);

  // Get also a Link helper class for Tr2MCP
  m_track2MCLink = new Object2MCLink( this, 
                                      "", "", 
                                      TrStoredTrackLocation::Default);
  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode ChargedPP2MC::execute() {

  MsgStream  msg( msgSvc(), name() );
  msg << MSG::DEBUG << "==> Execute" << endreq;

  // create a Relations table if needed
  ProtoParticle2MCAsct::Table* table = 
    "" == outputTable() ? NULL : new ProtoParticle2MCAsct::Table();

  // Try and find a Linker table. If absent we shall use the Relations table
  //  LinkedTo<MCParticle, TrStoredTrack> 
  //    tr2MCPLink( eventSvc(), msgSvc(), TrStoredTrackLocation::Default);
  
  if( !m_track2MCLink->notFound() ) {
    ifMsg( MSG::DEBUG) << "Using the Tr2MCParts Linker" << endreq;
  }
  else if( m_track2MCAsct->tableExists() ) {
    ifMsg( MSG::DEBUG) << "Using the Tr2MCParts Relations table" << endreq;
  } else {
    // No relations found, give up
    msg << MSG::INFO << "Unable to retrieve any Track2MC association table"
        << endreq;
    if( NULL != table ) put( table, outputTable() );
    return StatusCode::SUCCESS;
  }

  ///////////////////////////////////////
  // Loop on ProtoParticles containers //
  ///////////////////////////////////////

  for( std::vector<std::string>::const_iterator inp = m_inputData.begin(); 
       m_inputData.end()!= inp; inp++) {
    // Get ProtoParticles
    SmartDataPtr<ProtoParticles> protos ( evtSvc(), *inp);
    if( 0 == protos ) continue;
    // Create a linker table
    const std::string linkContainer = 
      *inp + Particle2MCMethod::extension[Particle2MCMethod::ChargedPP];
    // Just a fake helper class
    Object2MCLink p2MCLink(this);
    Object2MCLink::Linker*
      linkerTable = p2MCLink.linkerTable( linkContainer );
    if( NULL != linkerTable ) {
      ifMsg(MSG::VERBOSE) << "    Created Linker table for container "
                          << linkContainer << endreq;
    } else {
      ifMsg(MSG::VERBOSE) << "    Linker table for container "
                          << linkContainer << " already exists"
                          << endreq;
    }
    if( NULL == table && NULL == linkerTable ) continue;

    int npp = protos->size();
    ifMsg( MSG::VERBOSE ) << "    " << npp
        << " ProtoParticles retrieved from " 
        << *inp << endreq;
    
    // loop on Protos to match them from their tracks
    int nrel = 0 ;
    int nass = 0;
    for( ProtoParticles::const_iterator pIt=protos->begin() ;
         protos->end() != pIt; pIt++) {
      ifMsg( MSG::VERBOSE )
        << "    ProtoParticle " << (*pIt)->key();
      // Follow links from this protoParticle via tracks
      const TrStoredTrack* track = (*pIt)->track() ;
      if( 0 != track ) {
        ifMsg( MSG::VERBOSE )
          << " from track " << track->key();
        if( m_track2MCLink->notFound() ) {
          // Use the RelationsTable
          const Tr2MCPartAsct::ToRange mcPartRange
            = m_track2MCAsct->rangeFrom( track);
          if( !mcPartRange.empty() ) {
            nass++;
            Tr2MCPartAsct::ToIterator mcPartIt;
            ifMsg( MSG::VERBOSE )
              << " associated to " << mcPartRange.end()-mcPartRange.begin()+1
              << " MCParts";
            for( mcPartIt = mcPartRange.begin(); 
                 mcPartRange.end() != mcPartIt; mcPartIt++) {
              const MCParticle* mcPart = mcPartIt->to();
              double weight = mcPartIt->weight();
              if( 0 != mcPart ) {
                ifMsg(MSG::VERBOSE)
                  << " - " << mcPart->key();
                if( NULL != table )table->relate( *pIt, mcPart, weight);
                if( NULL != linkerTable ) 
                  linkerTable->link( *pIt, mcPart, weight);
                nrel++;
              }
            }
          } else {
            msg << " not associated to an MCPart";
          }
        } else {
            // Use the Linker relations
          double weight = 0.;
          const MCParticle* mcPart = m_track2MCLink->first(track, weight);
          if( NULL == mcPart ) {
            ifMsg( MSG::VERBOSE ) << " not associated to an MCPart";
          } else {
            ifMsg( MSG::VERBOSE ) << " associated to MCParts";
            nass++;
            do {
              ifMsg( MSG::VERBOSE ) << " - " << mcPart->key();
              if( NULL != table ) 
                table->relate( *pIt, mcPart, weight);
              if( NULL != linkerTable ) 
                linkerTable->link( *pIt, mcPart, weight);
              nrel++;
              mcPart = m_track2MCLink->next();
            } while( NULL != mcPart );
          }
        }
      } else {
        ifMsg(MSG::VERBOSE) << " not originating from a track";
      }
      ifMsg(MSG::VERBOSE) << endreq;
    }
    ifMsg( MSG::DEBUG )
      << "Out of " << npp << " Charged ProtoParts in " << *inp << ", "
      << nass << " are associated, "
      << nrel << " relations found" << endreq;
  }
  
  // Register the relations table on the TES
  if( NULL != table ) {
    put( table, outputTable() );
    ifMsg( MSG::DEBUG )
      << "     Registered table " << outputTable() << endreq;
  }
  return StatusCode::SUCCESS ;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode ChargedPP2MC::finalize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "==> Finalize" << endreq;
  if( NULL != m_track2MCLink ) delete m_track2MCLink;
  return GaudiAlgorithm::finalize();
}

//=============================================================================
