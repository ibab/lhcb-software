// $Id: DebugTrackingLosses.cpp,v 1.1 2009-04-01 09:03:56 ocallot Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "Event/Track.h"
#include "Event/MCParticle.h"
#include "Event/MCTrackInfo.h"
#include "Event/VeloCluster.h"
#include "Linker/LinkedFrom.h"
#include "Linker/LinkedTo.h"

// local
#include "DebugTrackingLosses.h"

//-----------------------------------------------------------------------------
// Implementation file for class : DebugTrackingLosses
//
// 2009-03-26 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( DebugTrackingLosses );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DebugTrackingLosses::DebugTrackingLosses( const std::string& name,
                                          ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "VeloRZ",      m_veloRZ      = false );
  declareProperty( "VeloSpace",   m_veloSpace   = false );
  declareProperty( "Forward",     m_forward     = false );
  declareProperty( "Ghost",       m_ghost       = false );
  declareProperty( "Clone",       m_clone       = false );
  declareProperty( "FromStrange", m_fromStrange = false );
}
//=============================================================================
// Destructor
//=============================================================================
DebugTrackingLosses::~DebugTrackingLosses() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode DebugTrackingLosses::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_ppSvc = svc<IParticlePropertySvc>( "ParticlePropertySvc", true );

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode DebugTrackingLosses::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  const LHCb::MCParticles* partCont = get<LHCb::MCParticles>(LHCb::MCParticleLocation::Default); 

  LinkedFrom<LHCb::Track,LHCb::MCParticle> veloRZLinker( evtSvc(), msgSvc(), LHCb::TrackLocation::RZVelo );
  LinkedFrom<LHCb::Track,LHCb::MCParticle> veloSpaceLinker( evtSvc(), msgSvc(), LHCb::TrackLocation::Velo );
  LinkedFrom<LHCb::Track,LHCb::MCParticle> forwardLinker( evtSvc(), msgSvc(), LHCb::TrackLocation::Forward );

  MCTrackInfo trackInfo( evtSvc(), msgSvc() );

  LHCb::MCParticles::const_iterator itP = partCont->begin();
  for (; itP != partCont->end(); ++itP){
    LHCb::MCParticle* part = *itP;
    if ( 0 == trackInfo.fullInfo( part ) ) continue;
    if ( ! trackInfo.hasVeloAndT( part ) ) continue;
    if ( abs( part->particleID().pid() ) == 11 ) continue; // reject electron    
    if ( m_fromStrange ) {
      bool isStrange = false;
      const LHCb::MCParticle* mother = part;
      while( 0 != mother->originVertex() ) {
        mother = mother->originVertex()->mother();
        if ( 0 == mother ) break;
        if ( mother->particleID().pid() ==   310 ) isStrange = true;
        if ( mother->particleID().pid() ==  3122 ) isStrange = true;
        if ( mother->particleID().pid() == -3122 ) isStrange = true;
      }
      if ( !isStrange ) continue;
    } else {
      if ( 5000. > fabs( part->p() ) ) continue;
    }
    
    
    bool hasVeloRZ    = veloRZLinker.first( part ) != NULL;
    bool hasVeloSpace = veloSpaceLinker.first( part ) != NULL;

    if ( m_veloRZ && !hasVeloRZ ) {
      info() << "Missed VeloRZ for MCParticle " << part->key() << endreq;
    }
    if ( m_veloSpace && hasVeloRZ && !hasVeloSpace ) {
      info() << "Missed VeloSpace for MCParticle " << part->key() << " ";
      printMCParticle( part );
    } 
    if ( m_forward ) {
      if ( forwardLinker.first(part) == NULL ) {
        info() << "Missed Forward for MCParticle " << part->key() << " ";
        printMCParticle( part );
      } else if ( m_clone && forwardLinker.next( ) != NULL ) {
        info() << "Forward clone for MCParticle " << part->key() << " ";
        printMCParticle( part );
      } 
    }
  }

  if ( m_ghost ) {
    if ( m_forward ) {
      LinkedTo<LHCb::MCParticle> trackLinker( evtSvc(), msgSvc(), LHCb::TrackLocation::Forward );
      LinkedTo<LHCb::MCParticle> vLink( evtSvc(), msgSvc(), LHCb::VeloClusterLocation::Default );

      LHCb::Tracks* tracks = get<LHCb::Tracks>( LHCb::TrackLocation::Forward );
      for ( LHCb::Tracks::const_iterator itT = tracks->begin(); tracks->end() != itT; ++itT ) {
        if ( trackLinker.first( *itT ) == NULL ) {
          info() << "Forward ghost track, nb " << (*itT)->key() << endmsg;
          for ( std::vector<LHCb::LHCbID>::const_iterator itId = (*itT)->lhcbIDs().begin(); 
                (*itT)->lhcbIDs().end() != itId; ++itId ) {
            if ( (*itId).isVelo() ) {
              LHCb::VeloChannelID idV = (*itId).veloID();          
              info() << format( "   Sensor %2d Strip %4d ", idV.sensor(), idV.strip() );
              LHCb::MCParticle* part = vLink.first( idV );
              while ( 0 != part ) {
                info() << " " << part->key();
                part = vLink.next();
              }
              info() << endmsg;
            }
          }
        }
      }
    }
  }
  
  return StatusCode::SUCCESS;
}

//=========================================================================
//  
//=========================================================================
void DebugTrackingLosses::printMCParticle ( const LHCb::MCParticle* part ) {
  const LHCb::MCParticle* mother = part;
  const LHCb::MCVertex*   vert   = part->originVertex();
  ParticleProperty* pp;
  double p = double( int(  part->p() ) /1000. );
  info() << "MC: [" << p << " GeV]";
  while ( 0 != mother ) {
    pp = m_ppSvc->findByStdHepID( mother->particleID().pid() );
    if ( 0 == pp ) {
      info() << mother->key() << "[" << mother->particleID().pid() <<"]";
    } else {
      info() << mother->key() << "[" <<  pp->particle() <<"]";
    }
    vert = mother->originVertex();
    if ( 0 == vert ) {
      mother = 0;
    } else {
      info() << format( " <-(z=%7.2f)", vert->position().z() );
      mother = vert->mother();
    }
  }
  info() << endreq;
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode DebugTrackingLosses::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
