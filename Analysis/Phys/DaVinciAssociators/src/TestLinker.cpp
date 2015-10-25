// $Id: TestLinker.cpp,v 1.7 2008-02-12 15:10:30 jpalac Exp $
#define TestLinker_CPP

// Include files
// from STL
#include <math.h>

// LHCbKernel
//#include "Relations/Relation2D.h"

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IIncidentSvc.h"

// from Event
#include "Event/Particle.h"
#include "Event/MCParticle.h"
#include "Event/ProtoParticle.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "Event/MCTrackInfo.h"
#include "Event/Track.h"

// local
#include "TestLinker.h"

using namespace LHCb;

#define _verbose if( msgLevel(MSG::VERBOSE) ) verbose()
#define _debug if( msgLevel(MSG::DEBUG) ) debug()
#define _info if( msgLevel(MSG::INFO) ) info()

//-----------------------------------------------------------------------------
// Implementation file for class : TestLinker
//
// Example of how to access various info from DaVinci
//
// 26/04/2002 : Philippe Charpentier
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory

DECLARE_ALGORITHM_FACTORY( TestLinker )

//=============================================================================
// Standard creator, initializes variables
//=============================================================================
TestLinker::TestLinker(const std::string& name,
                       ISvcLocator* pSvcLocator) :
AsctAlgorithm(name, pSvcLocator)
  , m_linkChi2(0)
  , m_linkLinks(0)
  , m_linkComp(0)
  , m_linkWithChi2(0)
  , m_matchLinks(0)
  , m_matchChi2(0)
  , m_matchFull(0)
  , m_matchLinksNotChi2(0)
  , m_matchLinksHighChi2(0)
  , m_matchChi2NotLinks(0)
  , m_matchDifferent(0)
  , m_matchLinksDiffComp(0)
  , m_matchMissedComp(0)
  , m_matchComp(0)
  , m_nbParts(0)
  , m_mcPartRecons(0)
  , m_mcPart2Track(7,0)
  , m_mcPart2Proto(6,0)
  , m_mcPart2Part(6,0)
  , m_mcPart2PartChi2(6,0)
  , m_mcPart2PartLink(6,0)
  , m_mcPartCount(0)
  , m_skippedEvts(0)
  , m_nbEvts(0)
  , m_setInputData(true)
{
  declareProperty( "SetInputData", m_setInputData);
}

//=============================================================================
// Standard destructor
//=============================================================================
TestLinker::~TestLinker() { }

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode TestLinker::initialize() {

  // Use the message service
  _info << ">>> Initialize" << endmsg;

  std::vector<std::string> inp =
    m_setInputData ? m_inputData : std::vector<std::string>();

  // Retrieve the link objects used by this algorithm
  m_linkWithChi2 = new Particle2MCLinker( this, Particle2MCMethod::WithChi2,
                                          inp);

  m_linkChi2 = new Particle2MCLinker( this, Particle2MCMethod::Chi2,
                                      inp);

  m_linkComp = new Particle2MCLinker( this, Particle2MCMethod::Composite,
                                      inp);

  m_linkLinks = new Particle2MCLinker( this, Particle2MCMethod::Links,
                                       inp);

  m_linkChargedPP =
    new ProtoParticle2MCLinker( this,
                                Particle2MCMethod::ChargedPP,
                                std::vector<std::string>(1,ProtoParticleLocation::Charged));

  m_linkNeutralPP =
    new ProtoParticle2MCLinker( this,
                                Particle2MCMethod::NeutralPP,
                                std::vector<std::string>(1,ProtoParticleLocation::Neutrals));


  // Initialization terminated
  return GaudiAlgorithm::initialize();
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode TestLinker::execute() {

  int matchLinks=0, matchChi2=0, matchFull=0, nbParts=0;
  int matchLinksNotChi2=0, matchLinksHighChi2=0
    , matchChi2NotLinks=0, matchDifferent=0, matchLinksDiffComp=0
    , matchMissedComp=0, matchComp=0;
  int skippedEvt = 0;

  for( std::vector<std::string>::iterator inp = m_inputData.begin();
       m_inputData.end() != inp; inp++) {
    // Retrieve Particles
    SmartDataPtr<Particles> parts (eventSvc(), *inp);
    if ( ! parts ) {
      _debug << "    No Particles retrieved from "
             << *inp << endmsg;
      continue;
    }
    else {
      // msg number of Candidates retrieved
      _verbose << "    Number of Particles retrieved from "
               << *inp << "  = " << parts->size() << endmsg;
      if( parts->size() == 0 ) skippedEvt = 1;
    }

    //========================================
    // Example of use of the Associator tool
    //========================================

    // Using the direct association
    for(Particles::const_iterator cand = parts->begin();
        parts->end() != cand; cand++, nbParts++) {
      Particle* part = *cand;

      const MCParticle* mcPartLinks = 0;
      const MCParticle* mcPartChi2 = 0;
      const MCParticle* mcPartComp = 0;
      double chi2 = 0.;
      int nbChi2 = m_linkChi2->associatedMCP(part);

      mcPartLinks = m_linkLinks->first(part);
      std::string strCharged = part->charge()? "Charged" : "Neutral";
      _verbose << "+++ " << strCharged << " particle " << part->key()
               << " " << part->particleID().pid()
               << " , momentum, slopes "
               << part->p() << " "
               << part->slopes().X() << " "
               << part->slopes().Y()
               << endmsg;

      if( NULL != mcPartLinks ) {
        // An association was obtained from links
        _verbose << "    Associated to "
                 << m_linkLinks->associatedMCP(part)
                 << " MCParts: " << endmsg;
        if( part->charge() ) matchLinks++;
        int nass = 0;
        do {
          nass++;
          double weight = m_linkLinks->weight();
          const Gaudi::LorentzVector mc4Mom = mcPartLinks->momentum();
          _verbose << "    MCPart " << mcPartLinks->key() << " "
                   << mcPartLinks->particleID().pid()
                   << " (weight "
                   << weight << ") from links : momentum, slopes "
                   << mc4Mom.Vect().R() << " "
                   << mc4Mom.Px()/mc4Mom.Pz() << " "
                   << mc4Mom.Py()/mc4Mom.Pz() << endmsg;
          mcPartLinks = m_linkLinks->next();
        } while( NULL != mcPartLinks );

        // Reset the link to the first one to test the "decreasing" feature
        mcPartLinks = m_linkLinks->first( part );
        mcPartComp = m_linkComp->first( part );
        if( part->charge() ){
          if( 0 != mcPartComp ) {
            matchComp++;
            if( mcPartLinks != mcPartComp ) {
              _verbose << "    MCPart from Composite != Links"
                       << endmsg;
              if( 0 != mcPartLinks ) matchLinksDiffComp++;
            }
          } else {
            matchMissedComp++;
          }
        }
      } else {
        _verbose << "    No MCPart found from links" << endmsg;
      }
      if( 0 == part->charge() ) continue;
      mcPartChi2 = m_linkWithChi2->first( part, chi2);
      if( 0 == nbChi2 ) {
        // No particles found above threshold with CHi2
        _verbose << "      MCPart not found from Chi2 below threshold";
        if( mcPartLinks && (mcPartChi2 == mcPartLinks) ) {
          _verbose << " (Chi2 was " << chi2 << ")";
          matchLinksHighChi2++;
        }
        _verbose << endmsg;
        if( mcPartLinks ) matchLinksNotChi2++;
      } else {
        if( mcPartChi2 == mcPartLinks ) {
          _verbose << "      MCPart found from Chi2 as well (Chi2 = " << chi2
                   << ")" << endmsg;
          matchFull++;
        }
        if( !mcPartLinks ) matchChi2NotLinks++;
      }
      if( mcPartChi2 && (mcPartChi2 != mcPartLinks) ) {
        if( mcPartLinks ) {
          matchDifferent++;
          _verbose << "      MCPart found from Chi2 is different" << endmsg;
        }
        const Gaudi::LorentzVector mc4Mom = mcPartChi2->momentum();
        _verbose << "      MCPart from Chi2 "
                 << part->particleID().pid()
                 << " : momentum, slope "
                 << mc4Mom.Vect().R() << " "
                 << mc4Mom.Px()/mc4Mom.Pz() << " "
                 << mc4Mom.Py()/mc4Mom.Pz() << endmsg;
        _verbose << "       Chi2 was " << chi2 << endmsg;
        Particle* partLinks = m_linkLinks->firstP( mcPartChi2 );
        if( partLinks ) {
          _verbose << "       It is linked to Particle "
                   << partLinks->key() << endmsg;
        }
      }
      if( nbChi2 ) {
        matchChi2++;
      }
    }
  }

  if( 0 == nbParts ) return StatusCode::SUCCESS;

  int width = (int)log10((double)nbParts+1)+1;

  _debug << "========= On " << std::setw(width) <<  nbParts
         << " Particles ========="
         << endmsg
         << "   | Matched with Links | " << std::setw(width) << matchLinks
         << " | Missed with Chi2  | " << std::setw(width) << matchLinksNotChi2
         << " | Too large Chi2    | " << std::setw(width) << matchLinksHighChi2
         << endmsg
         << "   | Matched with Comp. | " << std::setw(width) << matchComp
         << " | Diff. from Links  | " << std::setw(width) << matchLinksDiffComp
         << " | Missed with Comp. | " << std::setw(width) << matchMissedComp
         << endmsg
         << "   | Matched with Chi2  | " << std::setw(width) << matchChi2
         << " | Missed with Links | " << std::setw(width) << matchChi2NotLinks
         << endmsg
         << "   | Matched with both  | " << std::setw(width) << matchFull
         << " | Matched different | " << std::setw(width) << matchDifferent
         << endmsg;

  m_matchLinks += matchLinks;
  m_matchChi2 += matchChi2;
  m_matchFull += matchFull;
  m_matchLinksNotChi2 += matchLinksNotChi2;
  m_matchLinksHighChi2 += matchLinksHighChi2;
  m_matchChi2NotLinks += matchChi2NotLinks;
  m_matchLinksDiffComp += matchLinksDiffComp;
  m_matchMissedComp += matchMissedComp;
  m_matchComp += matchComp;
  m_matchDifferent += matchDifferent;
  m_nbParts += nbParts;


#ifdef TR_EFFICIENCY
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
    LinkedFrom<Track> trLink( eventSvc(),
                              NULL,
                              TrackLocation::Default);

    // Get the track info if present
    MCTrackInfo trackInfo( eventSvc(), msgSvc() );
    for( MCParticles::const_iterator mcIt=mcParts->begin();
         mcParts->end() != mcIt; mcIt++) {
      const MCParticle* mcPart = *mcIt;
      mcPartCount++;
      // Check whether it was the child of a B or a Ks
      //      if( isChild( "B0" ) ) {
      //        fromB0 = 1;
      //      }
      //      if( isChild( "K0s" ) ) {
      //        fromK0 = 1;
      //      }
      // Check if it was reconstructible
      if( trackInfo.hasVeloAndT( mcPart ) ) {
        verbose() << "    MCParticle " << mcPart->key() << endmsg
                  << "      Is reconstructable as Long track" << endmsg;
        mcPartRecons++;
        bool countTr = true;
        bool countProto = true;
        for( Track* tr = trLink.first(mcPart);
             NULL != tr;
             tr = trLink.next()) {
          verbose() << "      Is associated to track " << tr->key();
          if( !tr->checkFlag( Track::Clone ) ) {
            verbose() << " that is unique" << endmsg;
            // Look at the type of track
            int type = trType( tr );
            if( 0 == type ) continue;
            if( countTr ) mcPart2Track[0]++;
            mcPart2Track[type]++;
            countTr = false;
            trackFound[type] = 1;
            // Check the protoPart comes from that track
            ProtoParticle2MCLinker* protoLink =
              mcPart->particleID().threeCharge() ?
              m_linkChargedPP : m_linkNeutralPP;

            for( ProtoParticle* proto = protoLink->firstP(mcPart);
                 NULL != proto;
                 proto = protoLink->nextP() ) {
              verbose() << "      and gave ProtoParticle "
                        << proto->key() << endmsg;
              if( 0 != proto && proto->track() == tr ) {
                if( countProto ) mcPart2Proto[0]++;
                mcPart2Proto[type]++;
                countProto = false;
              }
            }
          } else {
            verbose() << " that is NOT unique" << endmsg;
          }
        }
        // Now look at association to Particles with all associators
        Particle* partLinks = m_linkLinks->firstP( mcPart ) ;
        const ProtoParticle* proto = NULL;
        int partTrType = 0;
        if( 0 != partLinks ) {
          mcPart2PartLink[0]++;
          proto = partLinks->proto();
          if( proto ) {
            partTrType = trType( proto->track() );
            if( partTrType ) mcPart2PartLink[partTrType]++;
          }
        }
        Particle* partChi2 = m_linkChi2->firstP( mcPart ) ;
        if( 0 != partChi2 ) {
          mcPart2PartChi2[0]++;
          proto = partChi2->proto();
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

  width = (int)log10((double)mcPartCount+1)+1;
  _debug
    << "========= On " << std::setw(width) << mcPartCount
    << " MCParticles ========="
    << endmsg
    << "   |                       |  Total  | Forward |  Match  |  Velo   |"
    << "  Seed   | Upstream|  Missed |" << endmsg;
  width = 7;
  _debug
    << "   | Reconstructible long  | " << std::setw(width) << mcPartRecons
    <<endmsg;

  prTable( MSG::DEBUG,
           "   | Linked to a track     | ", mcPart2Track, width);
  prTable( MSG::DEBUG,
           "   | Linked to a ProtoPart | ", mcPart2Proto, width);
  prTable( MSG::DEBUG,
           "   | Linked to a Part/Link | ", mcPart2PartLink, width);
  prTable( MSG::DEBUG,
           "   | Linked to a Part/Chi2 | ", mcPart2PartChi2, width);
  prTable( MSG::DEBUG,
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

#endif

  // End of execution for each event
  return StatusCode::SUCCESS;
}
void TestLinker::prTable( const MSG::Level level,
                          const std::string title,
                          const std::vector<int>& table, const int width)
{
  if( msgLevel( level ) ) {
    msgStream(level) << title;
    for( unsigned int i=0; i < table.size(); i++ ) {
      msgStream(level) << std::setw(width) << table[i] << " | ";
    }
    msgStream(level) << endmsg;
  }
}

int TestLinker::trType( const Track* tr ){
  //
  // This mixes track types and how they have been found.
  // -> modify according to what this function is needed for
  //
  int type = 0;
  if( tr ) {
    if( tr->checkHistory( Track::PatForward) ) type = 1;
    if( tr->checkHistory( Track::TrackMatching) ) type = 2;
    if( tr->checkHistory( Track::PatVelo) ) type = 3;
    if( tr->checkHistory( Track::TrackSeeding) ) type = 4;
    if( tr->checkHistory( Track::PatKShort ) ) type = 5;
  }
  return type;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode TestLinker::finalize() {

  _debug << ">>> Finalize" << endmsg;
  int width = (int)log10((double)m_nbParts+1)+1;

  _info << "======== Statistics for Particles to MCParticles association"
        << "========" << endmsg
        << "======== On " << std::setw(width) <<  m_nbParts
        << " Particles ( " << m_nbEvts << " events) ========"
        <<endmsg
        << "   | Matched with Links | " << std::setw(width) << m_matchLinks
        << " | Missed with Chi2  | " << std::setw(width) << m_matchLinksNotChi2
        << " | Too large Chi2    | " << std::setw(width) << m_matchLinksHighChi2
        << endmsg
        << "   | Matched with Comp. | " << std::setw(width) << m_matchComp
        << " | Diff. from Links  | " << std::setw(width) << m_matchLinksDiffComp
        << " | Missed with Comp. | " << std::setw(width) << m_matchMissedComp
        << endmsg
        << "   | Matched with Chi2  | " << std::setw(width) << m_matchChi2
        << " | Missed with Links | " << std::setw(width) << m_matchChi2NotLinks
        << endmsg
        << "   | Matched with both  | " << std::setw(width) << m_matchFull
        << " | Matched different | " << std::setw(width) << m_matchDifferent
        << endmsg;

#ifdef TR_EFFICIENCY
  width = (int)log10((double)m_mcPartCount+1)+1;
  _info << "======== Statistics on MCParticle associations ========"
        << endmsg
        << "======== On " << std::setw(width) << m_mcPartCount
        << " MCParticles ( " << m_nbEvts-m_skippedEvts << " events, "
        << m_skippedEvts << " skipped) ========"
        << endmsg
        << "   |                       |  Total  | Forward |  Match  |  Velo   |"
        << "  Seed   | Upstream|  Missed |" << endmsg;
  width = 7;
  _info  << "   | Reconstructible long  | " << std::setw(width) << m_mcPartRecons
         <<endmsg;

  prTable( MSG::INFO,
           "   | Linked to a track     | ", m_mcPart2Track, width);
  prTable( MSG::INFO,
           "   | Linked to a ProtoPart | ", m_mcPart2Proto, width);
  prTable( MSG::INFO,
           "   | Linked to a Part/Link | ", m_mcPart2PartLink, width);
  prTable( MSG::INFO,
           "   | Linked to a Part/Chi2 | ", m_mcPart2PartChi2, width);
  prTable( MSG::INFO,
           "   | Linked to a Particle  | ", m_mcPart2Part, width);
#endif

  // End of finalization step
  return GaudiAlgorithm::finalize();
}

