// $Id: TestAssociators.cpp,v 1.3 2002-10-10 18:55:39 gcorti Exp $
#define TestAssociators_CPP 

// Include files
// from STL
#include <math.h>

// LHCbKernel
//#include "Relations/Relation2D.h"

// from Gaudi
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IToolSvc.h"

// from Event 
#include "Event/Particle.h"
#include "Event/MCParticle.h"
#include "CLHEP/Units/PhysicalConstants.h"

// local
#include "TestAssociators.h"

#define ifLog(sev) log << sev; if( log.level() <= (sev) ) log

//-----------------------------------------------------------------------------
// Implementation file for class : TestAssociators
// 
// Example of how to access various info from DaVinci
// 
// 26/04/2002 : Philippe Charpentier
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const AlgFactory<TestAssociators>    Factory;
const IAlgFactory& TestAssociatorsFactory = Factory;

//=============================================================================
// Standard creator, initializes variables
//=============================================================================
TestAssociators::TestAssociators(const std::string& name,
                                           ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator)
  , m_pAsctChi2(0)
  , m_pAsctLinks(0)
  , m_pAsctWithChi2(0)
  , m_matchLinks(0)
  , m_matchChi2(0)
  , m_matchFull(0)
  , m_matchLinksNotChi2(0)
  , m_matchLinksHighChi2(0)
  , m_matchChi2NotLinks(0)
  , m_matchDifferent(0)
  , m_nbParts(0)
  , m_mcPart2Track(7,0)
  , m_mcPart2Proto(6,0)
  , m_mcPart2Part(6,0)
  , m_mcPart2PartChi2(6,0)
  , m_mcPart2PartLink(6,0)
  , m_mcPartCount(0)
  , m_skippedEvts(0)
  , m_nbEvts(0)
{ 
  m_inputData.push_back( ParticleLocation::Production ) ;
  declareProperty( "InputData", m_inputData) ;
}

//=============================================================================
// Standard destructor
//=============================================================================
TestAssociators::~TestAssociators() { };

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode TestAssociators::initialize() {

  // Use the message service
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << ">>> Initialize" << endreq;
  
  // Retrieve the StoredTracks2MC associator
  StatusCode sc = toolSvc()->retrieveTool(
        "AssociatorWeighted<TrStoredTrack,MCParticle,double>", 
        "Track2MCAsct", m_pAsctTrack);
  if( !sc.isSuccess() ) {
    log << MSG::FATAL << "    Unable to retrieve the Track2MCPart Associator"
        <<endreq;
    return sc;
  }

  // Retrieve the tools used by this algorithm
  // This is the Particle2MCWithChi2 tool
  sc = toolSvc()->retrieveTool( "Particle2MCWeightedAsct", 
                                           m_pAsctWithChi2, this);
  if( sc.isFailure() || 0 == m_pAsctWithChi2) {
    log << MSG::FATAL << "    Unable to retrieve weighted Chi2 Associator tool" 
        << endreq;
    return sc;
  }  

  // This is the standard Particle2MC tool
  sc = toolSvc()->retrieveTool( "Particle2MCAsct", m_pAsctChi2, this);
  if( sc.isFailure() || 0 == m_pAsctChi2) {
    log << MSG::FATAL << "    Unable to retrieve Chi2 Associator tool" 
        << endreq;
    return sc;
  }

  // This is another type of Particle2MCWeighted tool, 
  //   differentiated by jobOptions
  sc = toolSvc()->retrieveTool( "Particle2MCWeightedAsct", 
                                "LinkAsct", m_pAsctLinks, this);
  if( sc.isFailure() || 0 == m_pAsctLinks) {
    log << MSG::FATAL << "    Unable to retrieve Link Associator tool" 
        << endreq;
    return sc;
  }
  // This is the standard ChargedPP2MC tool
  sc = toolSvc()->retrieveTool( "ProtoParticle2MCAsct", m_pAsctProto, this);
  if( sc.isFailure() || 0 == m_pAsctProto) {
    log << MSG::FATAL << "    Unable to retrieve ChargedPP2MC Associator tool" 
        << endreq;
    return sc;
  }

  // Tool to check reconstructibility of MCParticles
  sc = toolSvc()->retrieveTool( "TrReconstructible", "TrRecons",
                                m_pTrRecons, this);
  if( sc.isFailure() || 0 == m_pTrRecons) {
    log << MSG::FATAL << "    Unable to retrieve MCPart reconstructibility tool"
        << endreq;
    return sc;
  }
  
  // Initialization terminated
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode TestAssociators::execute() {

  MsgStream          log( msgSvc(), name() );
  int matchLinks=0, matchChi2=0, matchFull=0, nbParts=0;
  int matchLinksNotChi2=0, matchLinksHighChi2=0, 
    matchChi2NotLinks=0, matchDifferent=0;
  int skippedEvt = 0;  

  for( std::vector<std::string>::iterator inp = m_inputData.begin();
       m_inputData.end() != inp; inp++) {
    // Retrieve Particles
    SmartDataPtr<Particles> parts (eventSvc(), *inp);
    if ( ! parts ) { 
      ifLog( MSG::DEBUG ) << "    No Particles retrieved from " 
          << *inp << endreq;
      //      continue;
    }
    else {
      // Log number of Candidates retrieved
      ifLog( MSG::VERBOSE ) << "    Number of Particles retrieved from " 
          << *inp << "  = " << parts->size() << endreq;
      if( parts->size() == 0 ) skippedEvt = 1;
    }

    //========================================
    // Example of use of the Associator tool
    //========================================
    
    if( false == m_pAsctChi2->tableExists() ) {
      ifLog( MSG::DEBUG ) 
          <<"         ** Warning ** The relations table from "
          <<"Chi2 doesn't exist" << endreq;
      // One may return at this stage, but one can also go on and receive
      //   empty ranges in the next loops...
      //    return StatusCode::SUCCESS;
    }
    
    if( false == m_pAsctLinks->tableExists() ) {
      ifLog( MSG::DEBUG ) 
          <<"         ** Warning ** The relations table from "
          <<"links  doesn't exist" << endreq;
    }
    
    // Using the direct association
    for(Particles::const_iterator cand = parts->begin(); 
        parts->end() != cand; cand++, nbParts++) {
      Particle* part = *cand;
      
      ifLog( MSG::VERBOSE ) << "Particle " << part->key() 
                            << " , momentum, slopes "
                            << part->p() << " " 
                            << part->slopeX() << " " 
                            << part->slopeY() 
                            << endreq;
      
      // Get the range of MCParts corresponding to a given Part from Chi2
      MCsFromParticle mcPartsChi2 = m_pAsctChi2->rangeFrom( part );
      
      if( m_pAsctLinks->tableExists() ) {
        // There is a table from the Proto links associator...
        MCParticle* mcPartLinks = 0;
        MCParticle* mcPartChi2 = 0;
        double chi2 = 0.;
        MCsFromParticleLinks mcPartLinksRange = 
          m_pAsctLinks->rangeFrom( part );
        MCsFromParticleLinksIterator mcPartLinksIt;
        
        if( !mcPartLinksRange.empty() ) {
          // An association was obtained from links
          ifLog( MSG::VERBOSE ) << "   Associated to "
              << mcPartLinksRange.end()-mcPartLinksRange.begin()+1
              << " MCParts: " << endreq;
          matchLinks++;        
          for( mcPartLinksIt = mcPartLinksRange.begin();
               mcPartLinksRange.end() != mcPartLinksIt; mcPartLinksIt++ ) {
            mcPartLinks = mcPartLinksIt->to();
            double weight = mcPartLinksIt->weight();
          const HepLorentzVector mc4Mom = mcPartLinks->momentum();
          ifLog( MSG::VERBOSE ) 
            << "   MCPart " << mcPartLinks->key() 
            << " (weight "
            << weight << ") from links : momentum, slopes "
            << mc4Mom.v().mag() << " "
            << mc4Mom.px()/mc4Mom.pz() << " "
            << mc4Mom.py()/mc4Mom.pz() << endreq;
          }
        } else {
          ifLog( MSG::VERBOSE ) << "   No MCPart found from links" << endreq;
        }
          
        mcPartChi2 = m_pAsctWithChi2->associatedFrom( part, chi2);
        if( mcPartsChi2.empty() ) {
          // No particles found above threshold with CHi2
          ifLog( MSG::VERBOSE ) 
              << "       MCPart not found from Chi2 below threshold";
          if( mcPartLinks && (mcPartChi2 == mcPartLinks) ) {
            ifLog( MSG::VERBOSE ) 
              << " (Chi2 was " << chi2 << ")";
            matchLinksHighChi2++;
          }
          ifLog( MSG::VERBOSE ) << endreq;
          if( mcPartLinks ) matchLinksNotChi2++;
        } else {
          if( mcPartChi2 == mcPartLinks ) {
            ifLog( MSG::VERBOSE )
                << "       MCPart found from Chi2 as well (Chi2 = " << chi2
                << ")" << endreq;
            matchFull++;
          }
          if( !mcPartLinks ) matchChi2NotLinks++;
        }
        if( mcPartChi2 && (mcPartChi2 != mcPartLinks) ) {
          if( mcPartLinks ) {
            matchDifferent++;
            ifLog( MSG::VERBOSE ) 
                << "       MCPart found from Chi2 is different" << endreq;
          }
          const HepLorentzVector mc4Mom = mcPartChi2->momentum();
          ifLog( MSG::VERBOSE ) 
              << "       MCPart from Chi2  : momentum, slope "
              << mc4Mom.v().mag() << " "
              << mc4Mom.px()/mc4Mom.pz() << " "
              << mc4Mom.py()/mc4Mom.pz() << endreq;
          ifLog( MSG::VERBOSE ) << "       Chi2 was " << chi2 << endreq;
          Particle* partLinks = m_pAsctLinks->associatedTo( mcPartChi2 );
          if( partLinks ) {
            ifLog( MSG::VERBOSE ) << "       It is linked to Particle " 
                << partLinks->key() << endreq;
          }
        }
      }
    
      if( !mcPartsChi2.empty() ) {
        matchChi2++;
      }
    }
  }
  
  int width = (int)log10(nbParts)+1;

  ifLog(MSG::DEBUG) << "========= On " << std::setw(width) <<  nbParts 
        << " Particles =========" 
        <<endreq
        << "   | Matched with Links | " << std::setw(width) << matchLinks 
        << " | Missed with Chi2  | " << std::setw(width) << matchLinksNotChi2 
        << " | Too large Chi2  | " << std::setw(width) << matchLinksHighChi2 
        << endreq
        << "   | Matched with Chi2  | " << std::setw(width) << matchChi2
        << " | Missed with Links | " << std::setw(width) << matchChi2NotLinks 
        << endreq
        << "   | Matched with both  | " << std::setw(width) << matchFull
        << " | Matched different | " << std::setw(width) << matchDifferent 
        << endreq;

  m_matchLinks += matchLinks;
  m_matchChi2 += matchChi2;
  m_matchFull += matchFull;
  m_matchLinksNotChi2 += matchLinksNotChi2;
  m_matchLinksHighChi2 += matchLinksHighChi2;
  m_matchChi2NotLinks += matchChi2NotLinks;
  m_matchDifferent += matchDifferent;
  m_nbParts += nbParts;

  // Now use reverse associators to check how frequently 
  //     an MCParticle is associated

  // Retrieve MCParticles
  SmartDataPtr<MCParticles> 
    mcParts(eventSvc(), MCParticleLocation::Default );

  // Loop on MCParticles now
  std::vector<int> mcPart2Track(7, 0);
  std::vector<int> mcPart2Proto(6, 0);
  std::vector<int> mcPart2Part(6, 0);
  std::vector<int> mcPart2PartChi2(6, 0);
  std::vector<int> mcPart2PartLink(6, 0);

  int mcPartCount = 0;
  int mcPartRecons = 0;
  std::vector<int> trackFound(6, 0);
  m_nbEvts++;
  if( skippedEvt ) {
    m_skippedEvts++;
  } else {
    for( MCParticles::const_iterator mcIt=mcParts->begin();
         mcParts->end() != mcIt; mcIt++) {
      MCParticle* mcPart = *mcIt;
      mcPartCount++;
      // Check whether it was the child of a B or a Ks
      //      if( isChild( "B0" ) ) {
      //        fromB0 = 1;
      //      }
      //      if( isChild( "K0s" ) ) {
      //        fromK0 = 1;
      //      }
      // Check if it was reconstructible
      if( m_pTrRecons->hasVeloAndSeed( mcPart ) ) {
        mcPartRecons++;
        // Look if it was associated to a ProtoPart and to a Track
        ProtoParticlesToMC protoRange = m_pAsctProto->rangeTo( mcPart );
        ProtoParticlesToMCIterator protoIt;
        Tr2MCPartAsct::FromRange trRange = m_pAsctTrack->rangeTo( mcPart );
        Tr2MCPartAsct::FromIterator trIt;
        bool countTr = true;
        bool countProto = true;
        for( trIt = trRange.begin(); trRange.end() != trIt; trIt++) {
          TrStoredTrack* tr = trIt->to();
          if( 0 != tr && tr->unique() ) {
            // Look at the type of track
            int type = trType( tr );
            if( 0 == type ) continue;
            if( countTr ) mcPart2Track[0]++;
            mcPart2Track[type]++;
            countTr = false;
            trackFound[type] = 1;
            // Check the protoPart comes from that track
            for( protoIt = protoRange.begin(); 
                 protoRange.end() != protoIt; protoIt++) {
              ProtoParticle* proto = protoIt->to();
              if( 0 != proto && proto->track() == tr ) {
                if( countProto ) mcPart2Proto[0]++;
                mcPart2Proto[type]++;
                countProto = false;
              }
            }
          }
        }
        // Now look at association to Particles with all associators
        Particle* partLinks = m_pAsctLinks->associatedTo( mcPart ) ;
        ProtoParticle* proto;
        int partTrType = 0;
        if( 0 != partLinks ) {
          mcPart2PartLink[0]++;
          proto = dynamic_cast<ProtoParticle*>(partLinks->origin());
          if( proto ) {
            partTrType = trType( proto->track() );
            if( partTrType ) mcPart2PartLink[partTrType]++;
          }
        }
        Particle* partChi2 = m_pAsctChi2->associatedTo( mcPart ) ;
        if( 0 != partChi2 ) {
          mcPart2PartChi2[0]++;
          proto = dynamic_cast<ProtoParticle*>(partChi2->origin());
          if( proto ) {
            partTrType = trType( proto->track() );
            if( partTrType ) mcPart2PartChi2[partTrType]++;
          }
        }
        if( (0 != partLinks) || (0 != partChi2) ) {
          mcPart2Part[0]++;
          if( partTrType ) mcPart2Part[partTrType]++;
        }
        if( !trackFound[1] && !trackFound[2] 
            && trackFound[3] && trackFound[4] ) {
          mcPart2Track[6]++;
        }
      }
    }
  }

  width = (int)log10(mcPartCount)+1;
  ifLog( MSG::DEBUG ) 
    << "========= On " << std::setw(width) << mcPartCount
    << " MCParticles ========="
    << endreq
    << "   |                       |  Total  | Forward |  Match  |  Velo   |"
    << "  Seed   | Upstream|  Missed |" << endreq;
  width = 7;
  ifLog( MSG::DEBUG ) 
    << "   | Reconstructible long  | " << std::setw(width) << mcPartRecons
    <<endreq;

  prTable( log, MSG::DEBUG, 
           "   | Linked to a track     | ", mcPart2Track, width);
  prTable( log, MSG::DEBUG, 
           "   | Linked to a ProtoPart | ", mcPart2Proto, width);
  prTable( log, MSG::DEBUG, 
           "   | Linked to a Part/Link | ", mcPart2PartLink, width);
  prTable( log, MSG::DEBUG, 
           "   | Linked to a Part/Chi2 | ", mcPart2PartChi2, width);
  prTable( log, MSG::DEBUG, 
           "   | Linked to a Particle  | ", mcPart2Part, width);
  
  // Collect full statistics
  m_mcPartRecons += mcPartRecons;
  m_mcPartCount += mcPartCount;
  unsigned int i =0;
  for( i=0; i < m_mcPart2Track.size(); i++ ) {
    m_mcPart2Track[i] += mcPart2Track[i];
  }
  for( i=0; i < m_mcPart2Proto.size(); i++ ) {  
    m_mcPart2Proto[i] += mcPart2Proto[i];
    m_mcPart2Part[i] += mcPart2Part[i];
    m_mcPart2PartChi2[i] += mcPart2PartChi2[i];
    m_mcPart2PartLink[i] += mcPart2PartLink[i];
  }
  
  
  // End of execution for each event
  return StatusCode::SUCCESS;
}
void TestAssociators::prTable( MsgStream& log, MSG::Level level,  
                               const std::string title, 
                               const std::vector<int>& table, const int width)
{
  if( log.level() <= level ) {
    log << level << title;
    for( unsigned int i=0; i < table.size(); i++ ) {
      log << std::setw(width) << table[i] << " | ";
    }
    log << endreq;
  }
}

int TestAssociators::trType( TrStoredTrack* tr )
{
  int type = 0;
  if( tr ) {
    if( tr->forward() ) type = 1;
    if( tr->match() ) type = 2;
    if( tr->velo() ) type = 3;
    if( tr->seed() ) type = 4;
    if( tr->upstream() ) type = 5;
  }
  return type;
}
  
//=============================================================================
//  Finalize
//=============================================================================
StatusCode TestAssociators::finalize() {

  MsgStream log(msgSvc(), name());
  ifLog( MSG::DEBUG ) << ">>> Finalize" << endreq;
  int width = (int)log10(m_nbParts)+1;
 
  ifLog( MSG::INFO ) 
      << "======== Statistics for Particles to MCParticles association"
      << "========" << endreq
      << "======== On " << std::setw(width) <<  m_nbParts 
      << " Particles ( " << m_nbEvts << " events) ========" 
      <<endreq
      << "   | Matched with Links | " << std::setw(width) << m_matchLinks 
      << " | Missed with Chi2  | " << std::setw(width) << m_matchLinksNotChi2 
      << " | Too large Chi2  | " << std::setw(width) << m_matchLinksHighChi2 
      << endreq
      << "   | Matched with Chi2  | " << std::setw(width) << m_matchChi2
      << " | Missed with Links | " << std::setw(width) << m_matchChi2NotLinks 
      << endreq
      << "   | Matched with both  | " << std::setw(width) << m_matchFull
      << " | Matched different | " << std::setw(width) << m_matchDifferent 
      << endreq;

  width = (int)log10(m_mcPartCount)+1;
  ifLog( MSG::INFO )
      << "======== Statistics on MCParticle associations ========"
      << endreq
      << "======== On " << std::setw(width) << m_mcPartCount
      << " MCParticles ( " << m_nbEvts-m_skippedEvts << " events, "
      << m_skippedEvts << " skipped) ========"
      << endreq
      << "   |                       |  Total  | Forward |  Match  |  Velo   |"
      << "  Seed   | Upstream|  Missed |" << endreq;
  width = 7;
  ifLog( MSG::INFO ) 
    << "   | Reconstructible long  | " << std::setw(width) << m_mcPartRecons
    <<endreq;

  prTable( log, MSG::INFO, 
           "   | Linked to a track     | ", m_mcPart2Track, width);
  prTable( log, MSG::INFO, 
           "   | Linked to a ProtoPart | ", m_mcPart2Proto, width);
  prTable( log, MSG::INFO, 
           "   | Linked to a Part/Link | ", m_mcPart2PartLink, width);
  prTable( log, MSG::INFO, 
           "   | Linked to a Part/Chi2 | ", m_mcPart2PartChi2, width);
  prTable( log, MSG::INFO, 
           "   | Linked to a Particle  | ", m_mcPart2Part, width);

  // End of finalization step
  return StatusCode::SUCCESS;
}

