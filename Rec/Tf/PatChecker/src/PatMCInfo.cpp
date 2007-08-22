// $Id: PatMCInfo.cpp,v 1.1.1.1 2007-08-22 15:38:27 smenzeme Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

#include "Linker/LinkedFrom.h"

// collect the various linked object types
#include "Event/VeloCluster.h"
#include "Event/STCluster.h"
#include "Event/OTTime.h"

// local
#include "PatMCInfo.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PatMCInfo
//
// 2005-03-24 : Olivier Callot
//-----------------------------------------------------------------------------

using namespace Tf;

DECLARE_TOOL_FACTORY( PatMCInfo );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PatMCInfo::PatMCInfo( const std::string& type,
                      const std::string& name,
                      const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<PatMCInfo>(this);
}
//=============================================================================
// Destructor
//=============================================================================
PatMCInfo::~PatMCInfo() {}; 


//=========================================================================
//  Initialisation, get services, tools and detector elements
//=========================================================================
StatusCode PatMCInfo::initialize ( ) {
  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiTool

  debug() << "==> Initialize" << endmsg;

  m_ppSvc = svc<IParticlePropertySvc>( "ParticlePropertySvc", true );
  m_velo  = getDet<DeVelo>( DeVeloLocation::Default );
  std::string storeName = "";
  PatDataStore* store = 
    tool<PatDataStore> ( "PatDataStore", "" );
  m_veloContainer = store->veloCoords();
  m_tContainer    = store->tCoords();
  m_ttContainer   = store->ttCoords();
  release( store );

  return StatusCode::SUCCESS;
}
//=========================================================================
//  Print the history of a MCParticle
//=========================================================================
void PatMCInfo::printMCParticle ( const LHCb::MCParticle* part ) {

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
      info() << mother->key() << "[" << pp->particle() <<"]";
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

//=========================================================================
//  Print the Velo/IT/OT clusters of this MCParticle
//=========================================================================
void PatMCInfo::printTrueMeasurements ( const LHCb::MCParticle* part ) {

  std::vector<int>::const_iterator itI;
  
  LinkedFrom<LHCb::VeloCluster,LHCb::MCParticle> 
    linkVelo( evtSvc(), msgSvc(), LHCb::VeloClusterLocation::Default );
  std::vector<int> iClus = linkVelo.intRange( part );
  for ( itI = iClus.begin() ; iClus.end() != itI ; itI++ ) {
    LHCb::VeloChannelID vCl( (*itI) );
    unsigned int sensor = vCl.sensor();
    unsigned int strip  = vCl.strip();
    double z = m_velo->sensor( vCl )->z();
    std::string type = "Phi";
    if ( vCl.isRType() ) type = "R  ";
    double coord = 0.;
    bool used = false;
    int charge = 0;

    for ( std::vector<PatVeloCoord*>::const_iterator itV = 
	    m_veloContainer->begin(); m_veloContainer->end() != itV; ++itV ) {
      if ( vCl == (*itV)->veloID() ) {
        coord = (*itV)->coord();
        used  = (*itV)->used();
        charge = int((*itV)->signal());
      }      
    }
    info() << "  Velo " << type 
           << format( " sensor %3d strip %4d  z%8.2f  coord %8.4f used %d charge%4d",
                      sensor, strip, z, coord, used, charge ) << endreq;
  }

  //== TT information
  LinkedFrom<LHCb::STCluster,LHCb::MCParticle> 
    linkTT( evtSvc(), msgSvc(),LHCb::STClusterLocation::TTClusters );
  iClus = linkTT.intRange( part );
  for ( itI = iClus.begin() ; iClus.end() != itI ; itI++ ) {
    LHCb::STChannelID sCl( (*itI) );
    LHCb::LHCbID lId( sCl );
    double z     = 0.;
    
    double coord = 0.;
    int used = -1;

    for ( std::vector<PatTTCoord*>::const_iterator itT = m_ttContainer->begin();
          m_ttContainer->end() != itT; ++itT ) {
      if ( lId == (*itT)->lhcbID() ) {
        z    = (*itT)->z();
        coord = (*itT)->measure();
        used  = (*itT)->isUsed();
      }      
    }
    if ( 0 != coord ) {
      info() << "    TT "
             << format( "sta %1d lay %1d    strip %4d  z%8.2f  coord %8.2f used %3d",
                        sCl.station(), sCl.layer(), sCl.strip(), z, coord, used ) << endreq;
    }
  }

  //== ST information
  LinkedFrom<LHCb::STCluster,LHCb::MCParticle> 
    linkIT( evtSvc(), msgSvc(),LHCb::STClusterLocation::ITClusters );
  iClus = linkIT.intRange( part );
  for ( itI = iClus.begin() ; iClus.end() != itI ; itI++ ) {
    LHCb::STChannelID sCl( (*itI) );
    LHCb::LHCbID lId( sCl );
    double z     = 0.;
    
    double coord = 0.;
    bool used = false;

    for ( std::vector<PatTCoord*>::const_iterator itT = m_tContainer->begin();
          m_tContainer->end() != itT; ++itT ) {
      if ( lId == (*itT)->lhcbID() ) {
        z    = (*itT)->z();
        coord = (*itT)->measure();
        used  = (*itT)->isUsed();
      }      
    }
    if ( 0 != coord ) {
      info() << "    ST "
             << format( "sta %1d lay %1d    strip %4d  z%8.2f  coord %8.2f used %d",
                        sCl.station(), sCl.layer(), sCl.strip(), z, coord, used ) << endreq;
    }
  }
  //== OT information

  LinkedFrom<LHCb::OTTime,LHCb::MCParticle> 
    linkOt( evtSvc(), msgSvc(),LHCb::OTTimeLocation::Default );
  iClus = linkOt.intRange( part );
  for ( itI = iClus.begin() ; iClus.end() != itI ; itI++ ) {
    LHCb::OTChannelID oId( (*itI) );
    LHCb::LHCbID lId( oId );
    
    double coord = 0.;
    bool used = false;
    double z = 0.;

    for ( std::vector<PatTCoord*>::const_iterator itT = m_tContainer->begin();
          m_tContainer->end() != itT; ++itT ) {
      if ( lId == (*itT)->lhcbID() ) {
        z    = (*itT)->z();
        coord = (*itT)->measure();
        used  = (*itT)->isUsed();
      }      
    }
    info() << "    OT "
           << format( "sta %1d lay %1d mo %2d str %3d  z%8.2f  coord %8.2f used %d",
                      oId.station(), oId.layer(), oId.module(), oId.straw(),
                      z, coord, used ) << endreq;
  }
}

//=========================================================================
//  
//=========================================================================
bool PatMCInfo::isBChild ( const LHCb::MCParticle* part ) {

  const LHCb::MCVertex*  vert = part->originVertex();
  while ( 0 != vert ) {
    const LHCb::MCParticle* mother = vert->mother();
    if ( 0 == mother )  return false;   // reached PV.
    if ( mother->particleID().hasBottom() ) {
      return true;
    }
    vert = mother->originVertex();
  }
  return false;
}
//=============================================================================

