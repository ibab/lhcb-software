// $Id: ParticleTisTos.cpp,v 1.2 2010-07-23 20:51:48 tskwarni Exp $
// Include files
#include <algorithm>
#include <vector>
#include <sstream>

// from Gaudi
#include "GaudiKernel/StatusCode.h"

#include "GaudiKernel/Plane3DTypes.h"

#include "Kernel/CaloCellCode.h"

// local
#include "TisTos/ParticleTisTos.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : ParticleTisTos
//
// 2010-07-08 : Tomasz Skwarnicki
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ParticleTisTos::ParticleTisTos( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : TisTos ( type, name , parent )
  , m_track2calo(0)
  , m_hcalDeCal(0)
  , m_ecalDeCal(0)
  , m_reportDepth(0)
{
  declareInterface<IParticleTisTos>(this);

  declareProperty("ProjectTracksToCalo", m_projectTracksToCalo=true );
  declareProperty("CaloClustForCharged", m_caloClustForCharged=true );
  declareProperty("CaloClustForNeutral", m_caloClustForNeutral=true );

  declareProperty("CompositeTPSviaPartialTOSonly", m_compositeTPSviaPartialTOSonly=false );
  declareProperty("FullAnalysisReport", m_fullAnalysisReport=true );
}

//=============================================================================
// Destructor
//=============================================================================
ParticleTisTos::~ParticleTisTos() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode ParticleTisTos::initialize()
{
  const StatusCode sc = TisTos::initialize();
  if ( sc.isFailure() ) return sc;

  debug() << "==> Initialize" << endmsg;

  m_track2calo = tool<ITrack2Calo>( "Track2Calo", "Track2CaloForTisTos" );

  return sc;
}

std::vector<LHCb::LHCbID> ParticleTisTos::projectTrack(const Track& track )
{
  std::vector<LHCb::LHCbID>  cells3x3;
  if( !m_projectTracksToCalo )return cells3x3;
  // ------------------ add expected Hcal hits (3x3 group around cell crossed at shower max) ------------
  // do it only if needed
  if(  m_TOSFrac[kHcal] > 0.0 ){
    if ( msgLevel(MSG::VERBOSE) ) verbose() << " projectTrack PROJECT TO HCAL " << endmsg;
    if( m_track2calo != 0 ){
      if( m_hcalDeCal==0 ){ m_hcalDeCal = getDet<DeCalorimeter>( DeCalorimeterLocation::Hcal ); }
      if( m_hcalDeCal != 0 ){

        // do not project through the magnet
        if( track.closestState( m_hcalDeCal->plane( CaloPlane::ShowerMax ) ).z() > 5000.0 ){
          if( m_track2calo->match( &track, DeCalorimeterLocation::Hcal) ){
            if( m_track2calo->isValid() ){ // is matched cellid valid
              const LHCb::CaloCellID centerCell  = m_track2calo->caloCellID();
              cells3x3.push_back(centerCell);
              BOOST_FOREACH( LHCb::CaloCellID cell, m_hcalDeCal->neighborCells( centerCell ) )
              {
                cells3x3.push_back(cell);
              };
            }
          }
        }

      }
    }
  }
  // ------------------ add expected Ecal hits (3x3 group around cell crossed at shower max) ------------
  // do it only if needed
  if( m_TOSFrac[kEcal] > 0.0 ){
    if ( msgLevel(MSG::VERBOSE) ) verbose() << " addToSignal with Track PROJECT TO ECAL " << endmsg;
    if( m_track2calo != 0 ){
      if( m_ecalDeCal==0 ){ m_ecalDeCal = getDet<DeCalorimeter>( DeCalorimeterLocation::Ecal ); }
      if( m_ecalDeCal != 0 ){

        // do not project through the magnet
        if( track.closestState( m_ecalDeCal->plane( CaloPlane::ShowerMax ) ).z() > 5000.0 ){
          if( m_track2calo->match( &track, DeCalorimeterLocation::Ecal) ){ // play with 3rd parameter?
            if( m_track2calo->isValid() ){ // is matched cellid valid
              const LHCb::CaloCellID centerCell  = m_track2calo->caloCellID();
              cells3x3.push_back(centerCell);
              BOOST_FOREACH( LHCb::CaloCellID cell, m_ecalDeCal->neighborCells( centerCell ) )
              {
                cells3x3.push_back(cell);
              };
            }
          }
        }

      }
    }
  }
  return sortedHits(cells3x3);
}


// Coolect hits from protoparticle (used both for signal and trigger parts)
//   extend=true: project tracks to Calo to get Calo hits and extend CaloClust hits to 3x3
//          (use for Signal, don't use for Trigger object)
std::vector<LHCb::LHCbID> ParticleTisTos::protoParticleHits(const ProtoParticle & pp, bool extend )
{
  if ( msgLevel(MSG::VERBOSE) ) verbose() << " protoParticleHits enter " << endmsg;

  std::vector< LHCbID > hits;
  const Track* onit=pp.track();
  if( 0!=onit ){
    hits.insert(hits.end(),onit->lhcbIDs().begin(),onit->lhcbIDs().end());
    if ( msgLevel(MSG::VERBOSE) ) verbose() << " protoParticleHits copied track hits " << endmsg;
    if( extend && m_projectTracksToCalo ){
      const std::vector<LHCbID> caloHits = projectTrack( *onit );
      hits.insert(hits.end(),caloHits.begin(),caloHits.end());
    }
  }

  // Ecal via CaloHypo
  if( ( m_TOSFrac[kEcal] > 0.0 && (m_caloClustForCharged||m_caloClustForNeutral)  ) ){
    if ( msgLevel(MSG::VERBOSE) ) verbose() << " protoParticleHits ECAL VIA CaloHypo " << endmsg;


    // new Apr 6, 2012:  collect calo cell IDs from ProtoParticle::ExtraInfo if available
    LHCb::CaloCellID caloCell( LHCb::CaloCellID::ContentType(0) );
    bool caloOK(false);

    if( 0!=onit ){
      caloOK = true; // don't care about calo info for charged if not found (we also project tracks!)
      if( m_caloClustForCharged )
        caloCell = LHCb::CaloCellID( LHCb::CaloCellID::ContentType( pp.info( LHCb::ProtoParticle::CaloChargedID, 0 ) ) );
      //deb info() << " charged " << caloCell.all() << endmsg;
    } else {
      if( m_caloClustForNeutral ){
        caloCell = LHCb::CaloCellID( LHCb::CaloCellID::ContentType( pp.info( LHCb::ProtoParticle::CaloNeutralID, 0 ) ) );
        //deb info() << " neutral " << caloCell.all() << endmsg;
      } else {
        caloOK = true; // don't want calo info for neutrals
      }
    }
    // deb caloCell = LHCb::CaloCellID( LHCb::CaloCellID::ContentType(0) );
    if( caloCell.all() != 0 ){
      caloOK = true;
      hits.push_back(caloCell);
      if( extend ){
        if( m_ecalDeCal==0 ){
          m_ecalDeCal = getDet<DeCalorimeter>( DeCalorimeterLocation::Ecal );
        }
        BOOST_FOREACH( LHCb::CaloCellID cell,m_ecalDeCal->neighborCells(caloCell))
        {
          hits.push_back(cell);
        }
      }
      // ExtraInfo failed: dig into CaloHypo and Clusters only in case of neutrals when requsted
    } else if( 0==onit && m_caloClustForNeutral ) {

      //deb info() << " trying calo hypos" << endmsg;

      const SmartRefVector< LHCb::CaloHypo > &caloVec = pp.calo();
      if( caloVec.size() > 0 ){
        const LHCb::CaloHypo*   hypo  = *(caloVec.begin());
        if( hypo ){
          //deb info() << " inside hypo" << endmsg;
          if( hypo->clusters().size() > 0 ){
            //deb info() << " inside hypo passed clusters " << endmsg;
            LHCb::CaloCellID centerCell,centerCell1,dummyCell;
            //       next if always false: left in for historical reasons
            if( 0!=onit && m_caloClustForCharged ){
              if( LHCb::CaloHypo::EmCharged == hypo->hypothesis() ){
                //deb info() << " charged hypo " << endmsg;
                if ( msgLevel(MSG::VERBOSE) ) verbose() << " protoParticleHits EmCharged " << endmsg;
                if( (hypo->clusters().begin())->target() ){
                  centerCell  = (*(hypo->clusters().begin()))->seed();
                  //deb info() << " charged hypo ok " << endmsg;
                }
              }
            } else if( m_caloClustForNeutral ){
              if( LHCb::CaloHypo::Photon == hypo->hypothesis() ){
                if ( msgLevel(MSG::VERBOSE) ) verbose() << " protoParticleHits Photon " << endmsg;
                if( (hypo->clusters().begin())->target() ){
                  centerCell  = (*(hypo->clusters().begin()))->seed();
                  //deb info() << " gamma hypo ok " << endmsg;
                }
              } else if (  LHCb::CaloHypo::Pi0Merged == hypo->hypothesis() ){
                if ( msgLevel(MSG::VERBOSE) ) verbose() << " protoParticleHits Pi0Merged " << endmsg;
                // Split Photons
                const SmartRefVector<LHCb::CaloHypo>& hypos = hypo->hypos();
                if( hypos.size()>1 ){
                  const LHCb::CaloHypo* g1 = *(hypos.begin() );
                  if( g1 ){
                    if( (g1->clusters().begin())->target() ){
                      centerCell  = (*(g1->clusters().begin()))->seed();
                      //deb info() << " piomareged hypo ok " << endmsg;
                    }
                  }
                  const LHCb::CaloHypo* g2 = *(hypos.begin()+1 );
                  if( g2 ){
                    if( (g2->clusters().begin())->target() )
                      centerCell1 = (*(g2->clusters().begin()))->seed();
                  }
                }
              } else {
                caloOK = true; // not a photon nor pi0 ; don't expect calo
              }

            } else {
              caloOK = true; // do not want calo even for neutrals
            }

            if( !(centerCell == dummyCell) ){
              //deb info() << " CALOOK " << endmsg;
              caloOK = true;
              hits.push_back(centerCell);
              if( extend ){
                if( m_ecalDeCal==0 ){ m_ecalDeCal = getDet<DeCalorimeter>( DeCalorimeterLocation::Ecal ); }
                BOOST_FOREACH( LHCb::CaloCellID cell,m_ecalDeCal->neighborCells(centerCell))
                {
                  hits.push_back(cell);
                }
              }
              if( !(centerCell1 == dummyCell) ){
                hits.push_back(centerCell1);
                if( extend ){
                  if( m_ecalDeCal==0 ){ m_ecalDeCal = getDet<DeCalorimeter>( DeCalorimeterLocation::Ecal ); }
                  BOOST_FOREACH( LHCb::CaloCellID cell,m_ecalDeCal->neighborCells(centerCell))
                  {
                    hits.push_back(cell);
                  }
                }
              }
            }
          }
        }
      }
    } else {
      caloOK = true; // OK not to find calo info for charged tracks
    }

    if( caloOK == false )
      Warning( "No Calo info found: TisTos of photons and pi0s will be defunct", StatusCode::SUCCESS, 1 ).setChecked();

  }



  // add muon hits only if needed
  if(  ( m_TOSFrac[kMuon] > 0.0 ) && (0!=onit)  ){
    if ( msgLevel(MSG::VERBOSE) ) verbose() << " protoParticleHits fs trying for muons " << endmsg;
    const LHCb::MuonPID* muid = pp.muonPID();
    if( muid!=0 ){
      if ( msgLevel(MSG::VERBOSE) ) verbose() << " protoParticleHits fs muid " << endmsg;
      if( muid->IsMuon() ){
        if ( msgLevel(MSG::VERBOSE) ) verbose() << " protoParticleHits fs mu " << endmsg;
        const LHCb::Track*  mu=muid->muonTrack();
        if( mu!=0 ){
          hits.insert(hits.end(),mu->lhcbIDs().begin(),mu->lhcbIDs().end());
          if ( msgLevel(MSG::VERBOSE) ) verbose() << " protoParticleHits fs muon hits inserted " << endmsg;
        } else {
          Warning( "No Muon Tracks found: TisTos of muon triggers may be defunct", StatusCode::SUCCESS, 1 ).setChecked();
        }
      }
    }
  }

  return sortedHits(hits);
}


//=============================================================================
// -------------- Signal (offline inputs) -------------------------------------
//=============================================================================

//    Track input ---------------------------------------------------------------
bool ParticleTisTos::addToSignal( const LHCb::Track & track )
{
  if ( msgLevel(MSG::VERBOSE) ) verbose() << " addToSignal with Track ENTER " << endmsg;

  //   add hits saved directly on the track ------------------------------------------------------------
  bool sigModified = addSortedHitsToSignal( track.lhcbIDs() );
  if( addSortedHitsToSignal( projectTrack( track ) ) )sigModified = true;

  if ( msgLevel(MSG::VERBOSE) ) verbose() << " addToSignal with Track END " << endmsg;

  return sigModified;
}

//    Proto-particle input -----------------------------------------------------------------------
bool ParticleTisTos::addToSignal( const LHCb::ProtoParticle & protoParticle )
{
  if ( msgLevel(MSG::VERBOSE) ) verbose() << " addToSignal with ProtoParticle ENTER " << endmsg;

  return addSortedHitsToSignal( protoParticleHits( protoParticle, true ) );

}

//    Particle input; for composite particles loop over daughters will be executed ------------------------------
bool ParticleTisTos::addToSignal( const LHCb::Particle & particle )
{
  bool sigModified(false);

  if ( msgLevel(MSG::VERBOSE) ) verbose() << " addToSignal with Particle ENTER " << endmsg;
  std::vector<const LHCb::Particle*> daughters = particle.daughtersVector();
  if( daughters.size() >0 ){
    for(std::vector<const LHCb::Particle*>::const_iterator p = daughters.begin(); p!=daughters.end(); ++p){
      if ( msgLevel(MSG::VERBOSE) ) verbose() << " addToSignal with Particle DAUGHTER " << endmsg;
      if(*p){
        const LHCb::Particle & part = *(*p);
        if( addToSignal(part) )sigModified=true;
      }
    }
  } else {
    const ProtoParticle* pp = particle.proto();
    if( 0!=pp ){
      if ( msgLevel(MSG::VERBOSE) ) verbose() << " addToSignal with Particle PROTOPARTICLE " << endmsg;
      //deb info() << particle << endmsg;
      if( addToSignal(*pp) )sigModified=true;
    } else {
      Warning("Particle passed as signal has no daughters and ProtoParticle is not accessible; TisTossing is not possible",
              StatusCode::SUCCESS, 3 ).setChecked();
    }
  }
  if ( msgLevel(MSG::VERBOSE) ) verbose() << " addToSignal with Particle EXIT " << endmsg;

  return sigModified;

}

//=============================================================================
// outputs: classified trigger track
//=============================================================================
unsigned int ParticleTisTos::tisTos(const Track & track)
{
  return tisTosSortedHits( track.lhcbIDs() );
}
bool  ParticleTisTos::tos(const Track & track)
{
  return tosSortedHits(  track.lhcbIDs() );
}
bool  ParticleTisTos::tis(const Track & track)
{
  return tisSortedHits(  track.lhcbIDs() );
}
bool  ParticleTisTos::tus(const Track & track)
{
  return tusSortedHits(  track.lhcbIDs() );
}

std::string ParticleTisTos::analysisReport(const LHCb::Track & track )
{
  return (offset()+" Track "+analysisReportSortedHits( track.lhcbIDs() ));
}


//=============================================================================
// outputs: classified RecVertex
//=============================================================================
unsigned int ParticleTisTos::tisTos(const RecVertex & vtx)
{
  unsigned int resultTISTOS=kTOSandTIS;
  unsigned int resultTPS=0;
  bool empty(true);
  const SmartRefVector<LHCb::Track> & ontracks=vtx.tracks();
  for (SmartRefVector<LHCb::Track>::const_iterator onit = ontracks.begin();onit != ontracks.end(); ++onit) {
    if( onit->target() ){
      const Track& ontrack = *(*onit);
      unsigned int result = tisTos( ontrack );
      if( !result )continue;
      empty=false;
      //  every Track in vertex must be TOS (TIS) for vertex to be TOS (TIS)
      resultTISTOS &= result;
      // set TPS unless done already
      if( !resultTPS ){
        if( m_compositeTPSviaPartialTOSonly ){
          if( result & kTOS ){
            resultTPS = kTPS;
          }
        } else {
          resultTPS = result & kTPS;
        }
      }
      //  see if can quite already
      if( ! (resultTISTOS & kTOSandTIS) ){
        if( !m_compositeTPSviaPartialTOSonly )resultTPS=kTPS;
        // all conditions determined?
        if( resultTPS )break;
      }
    }
  }
  if( empty )return 0;
  return (resultTISTOS|resultTPS);
}

std::string ParticleTisTos::analysisReport(const RecVertex & vtx)
{
  std::ostringstream report;
  unsigned int resultTISTOS=kTOSandTIS;
  unsigned int resultTPS=0;
  bool empty(true);
  const SmartRefVector<LHCb::Track> & ontracks=vtx.tracks();
  report << offset() << " RecVertex #-of-Tracks " << ontracks.size() << std::endl;
  for (SmartRefVector<LHCb::Track>::const_iterator onit = ontracks.begin();onit != ontracks.end(); ++onit) {
    if( onit->target() ){
      const Track& ontrack = *(*onit);
      unsigned int result = tisTos( ontrack );
      ++m_reportDepth;
      report << analysisReport(ontrack) << std::endl;
      --m_reportDepth;
      if( !result )continue;
      empty=false;
      //  every Track in vertex must be TOS (TIS) for vertex to be TOS (TIS)
      resultTISTOS &= result;
      // set TPS unless done already
      if( !resultTPS ){
        if( m_compositeTPSviaPartialTOSonly ){
          if( result & kTOS ){
            resultTPS = kTPS;
          }
        } else {
          resultTPS = result & kTPS;
        }
      }
      //  see if can quite already
      if( ! (resultTISTOS & kTOSandTIS) ){
        if( !m_compositeTPSviaPartialTOSonly )resultTPS=kTPS;
        // all conditions determined?
        if( resultTPS )if( !m_fullAnalysisReport ) break;
      }
    }
  }
  unsigned int result;
  if( empty ){
    result =0;
  } else {
    result =(resultTISTOS|resultTPS);
  }
  TisTosTob res( result );
  report << offset() << " RecVertex TIS= " << res.tis() << " TOS= " << res.tos() << " TPS= " << res.tps() << std::endl;
  return report.str();
}

bool  ParticleTisTos::tos(const RecVertex & vtx)
{
  bool empty(true);
  const SmartRefVector<LHCb::Track> & ontracks=vtx.tracks();
  for (SmartRefVector<LHCb::Track>::const_iterator onit = ontracks.begin();onit != ontracks.end(); ++onit) {
    if( onit->target() ){
      const Track& ontrack = *(*onit);
      //  every Track in vertex must be TOS (TIS) for vertex to be TOS (TIS)
      if( !tos( ontrack ) )return false;
      empty = false;
    }
  }
  if( empty )return false;
  return true;
}

bool  ParticleTisTos::tis(const RecVertex & vtx)
{
  bool empty(true);
  const SmartRefVector<LHCb::Track> & ontracks=vtx.tracks();
  for (SmartRefVector<LHCb::Track>::const_iterator onit = ontracks.begin();onit != ontracks.end(); ++onit) {
    if( onit->target() ){
      const Track& ontrack = *(*onit);
      //  every Track in vertex must be TOS (TIS) for vertex to be TOS (TIS)
      if( !tis( ontrack ) )return false;
      empty = false;
    }
  }
  if( empty )return false;
  return true;
}

bool  ParticleTisTos::tus(const RecVertex & vtx)
{
  const SmartRefVector<LHCb::Track> & ontracks=vtx.tracks();
  for (SmartRefVector<LHCb::Track>::const_iterator onit = ontracks.begin();onit != ontracks.end(); ++onit) {
    if( onit->target() ){
      const Track& ontrack = *(*onit);
      //  it is enough for one track to use signal for vertex to use it
      if( m_compositeTPSviaPartialTOSonly ){
        if( tos( ontrack ) )return true;
      } else {
        if( tus( ontrack ) )return true;
      }
    }
  }
  return false;
}

//=============================================================================
// outputs: classified Particle
//=============================================================================
unsigned int ParticleTisTos::tisTos(const LHCb::Particle & particle)
{

  std::vector<const LHCb::Particle*> daughters = particle.daughtersVector();
  if ( msgLevel(MSG::VERBOSE) ) verbose() << " tisTos Partcile daughters size " << daughters.size() << endmsg;
  if( daughters.size() >0 ){
    unsigned int resultTISTOS=kTOSandTIS;
    unsigned int resultTPS=0;
    bool empty(true);
    for( std::vector<const LHCb::Particle*>::const_iterator p = daughters.begin(); p!=daughters.end(); ++p){
      if(!(*p))continue;
      const LHCb::Particle & daug = *(*p);
      unsigned int result = tisTos( daug );
      if( m_compositeTPSviaPartialTOSonly ){
        // no way to tell that it wasn't analyzed
        empty=false;
        //  every daughter of the Parent must be TOS (TIS) for Parent to be TOS (TIS)
        resultTISTOS &= result;
        // set TPS unless done already
        if( !resultTPS ){
          if( result & kTOS ){
            // daughter TOS sets parent TPS
            resultTPS = kTPS;
          } else if( result & kTPS ){
            if( daug.daughtersVector().size()>1 ){
              // non-trivially-composite daughter TPS sets parent TPS
              resultTPS= kTPS;
            } else if ( daughters.size()==1 ){
              // non-composite or trivially composite daughter propagates its TPS to parent with only daughter
              resultTPS= kTPS;
            }
          }
        }
        // see if can quit already
        if( ! (resultTISTOS & kTOSandTIS) && resultTPS ){
          break;
        }
      } else {
        //   no info? skip
        if( !result )continue;
        empty=false;
        //  every daughter of the Partcile must be TOS (TIS) for Partcile to be TOS (TIS)
        resultTISTOS &= result;
        //  any TPS daughter is fine for TPS
        resultTPS |=  (result & kTPS);
        // see if can quite already
        if( ! (resultTISTOS & kTOSandTIS) ){
          // neither TIS nor TOS already: by definition TPS
          resultTPS=kTPS;
          break;
        }
      }
    }
    if( empty )return 0;
    return (resultTISTOS|resultTPS);
  } else {
    // non-composite daughter
    const ProtoParticle* pp = particle.proto();
    if( 0!=pp ){
      return tisTosSortedHits( protoParticleHits(*pp) );
    }
  }
  return 0;
}

std::string ParticleTisTos::analysisReport(const LHCb::Particle & particle)
{
  std::ostringstream report;
  std::vector<const LHCb::Particle*> daughters = particle.daughtersVector();
  report << offset() << " Particle #-of-daughters " << daughters.size() << std::endl;
  if( daughters.size() >0 ){
    unsigned int resultTISTOS=kTOSandTIS;
    unsigned int resultTPS=0;
    bool empty(true);
    for( std::vector<const LHCb::Particle*>::const_iterator p = daughters.begin(); p!=daughters.end(); ++p){
      if(!(*p))continue;
      const LHCb::Particle & daug = *(*p);
      unsigned int result = tisTos( daug );
      //      report << offset() << " Daughter ";
      ++m_reportDepth;
      report << analysisReport(daug );
      --m_reportDepth;
      if( m_compositeTPSviaPartialTOSonly ){
        // no way to tell that it wasn't analyzed
        empty=false;
        //  every daughter of the Parent must be TOS (TIS) for Parent to be TOS (TIS)
        resultTISTOS &= result;
        // set TPS unless done already
        if( !resultTPS ){
          if( result & kTOS ){
            // daughter TOS sets parent TPS
            resultTPS = kTPS;
          } else if( result & kTPS ){
            if( daug.daughtersVector().size()>1 ){
              // non-trivially-composite daughter TPS sets parent TPS
              resultTPS= kTPS;
            } else if ( daughters.size()==1 ){
              // non-composite or trivially composite daughter propagates its TPS to parent with only daughter
              resultTPS= kTPS;
            }
          }
        }
        // see if can quit already
        if( ! (resultTISTOS & kTOSandTIS) && resultTPS ){
          if( !m_fullAnalysisReport )break;
        }
      } else {
        //   no info? skip
        if( !result )continue;
        empty=false;
        //  every daughter of the Partcile must be TOS (TIS) for Partcile to be TOS (TIS)
        resultTISTOS &= result;
        //  any TPS daughter is fine for TPS
        resultTPS |=  (result & kTPS);
        // see if can quite already
        if( ! (resultTISTOS & kTOSandTIS) ){
          // neither TIS nor TOS already: by definition TPS
          resultTPS=kTPS;
          if( !m_fullAnalysisReport )break;
        }
      }
    }
    unsigned int result;
    if( empty ){
      result =0;
    } else {
      result =(resultTISTOS|resultTPS);
    }
    TisTosTob res( result );
    report << offset()
           << " Particle TIS= " << res.tis() << " TOS= " << res.tos() << " TPS= " << res.tps() << std::endl;
    return report.str();
  } else {
    // non-composite daughter
    const ProtoParticle* pp = particle.proto();
    if( 0!=pp ){
      report << offset() <<" ProtoParticle "
             << analysisReportSortedHits( protoParticleHits(*pp) ) << std::endl;
      return report.str();
    }
  }
  return report.str();
}


bool ParticleTisTos::tos(const LHCb::Particle & particle)
{

  std::vector<const LHCb::Particle*> daughters = particle.daughtersVector();
  if ( msgLevel(MSG::VERBOSE) ) verbose() << " tos Partcile daughters size " << daughters.size() << endmsg;
  if( daughters.size() >0 ){
    bool empty(true);
    for( std::vector<const LHCb::Particle*>::const_iterator p = daughters.begin(); p!=daughters.end(); ++p){
      if(!(*p))continue;
      const LHCb::Particle & daug = *(*p);
      if( !tos( daug ) )return false;
      empty=false;
    }
    if( empty )return false;
    return true;
  } else {
    const ProtoParticle* pp = particle.proto();
    if( 0!=pp ){
      return tosSortedHits( protoParticleHits(*pp) );
    }
  }
  return false;

}


bool ParticleTisTos::tis(const LHCb::Particle & particle)
{

  std::vector<const LHCb::Particle*> daughters = particle.daughtersVector();
  if ( msgLevel(MSG::VERBOSE) ) verbose() << " tos Partcile daughters size " << daughters.size() << endmsg;
  if( daughters.size() >0 ){
    bool empty(true);
    for( std::vector<const LHCb::Particle*>::const_iterator p = daughters.begin(); p!=daughters.end(); ++p){
      if(!(*p))continue;
      const LHCb::Particle & daug = *(*p);
      if( !tis( daug ) )return false;
      empty=false;
    }
    if( empty )return false;
    return true;
  } else {
    const ProtoParticle* pp = particle.proto();
    if( 0!=pp ){
      return tisSortedHits( protoParticleHits(*pp) );
    }
  }
  return false;

}

bool ParticleTisTos::tus(const LHCb::Particle & particle)
{

  std::vector<const LHCb::Particle*> daughters = particle.daughtersVector();
  if ( msgLevel(MSG::VERBOSE) ) verbose() << " tos Partcile daughters size " << daughters.size() << endmsg;
  if( daughters.size() >0 ){
    for( std::vector<const LHCb::Particle*>::const_iterator p = daughters.begin(); p!=daughters.end(); ++p){
      if(!(*p))continue;
      const LHCb::Particle & daug = *(*p);
      if( tus( daug ) )return true;
    }
    return false;
  } else {
    const ProtoParticle* pp = particle.proto();
    if( 0!=pp ){
      if( m_compositeTPSviaPartialTOSonly ){
        return tosSortedHits( protoParticleHits(*pp) );
      } else {
        return tusSortedHits( protoParticleHits(*pp) );
      }
    }
  }
  return false;

}

//=============================================================================
// outputs: classified HltObjectSummary
//=============================================================================
unsigned int ParticleTisTos::tisTos(const LHCb::HltObjectSummary & hos, unsigned int & valid)
{
  valid = 1;
  unsigned int dvalid;
  const SmartRefVector< LHCb::HltObjectSummary > & sub = hos.substructure();
  if ( msgLevel(MSG::VERBOSE) ) verbose() << " tisTos HltObjectSummary substr size " << sub.size() << endmsg;

  if( hos.summarizedObjectCLID() == 1 ){
    // this is Selection summary: take OR between candidates
    unsigned int result=0;
    valid = 0;  
    for( SmartRefVector< LHCb::HltObjectSummary >::const_iterator ihos=sub.begin();ihos!=sub.end();++ihos){
      if(!(ihos->target()))continue;
      const LHCb::HltObjectSummary &  daug = *(ihos->target());
      result |= tisTos( daug, dvalid );
      if( dvalid )valid = 1;
      if( (result & kTOS) && (result & kTIS) && (result & kTPS ) )break;
    }
    return result;
  } else if( sub.size() >0 ){
    // object with substructure
    unsigned int resultTISTOS=kTOSandTIS;
    unsigned int resultTPS=0;
    bool empty(true);
    for( SmartRefVector< LHCb::HltObjectSummary >::const_iterator ihos=sub.begin();ihos!=sub.end();++ihos){
      if(!(ihos->target()))continue;
      const LHCb::HltObjectSummary &  daug = *(ihos->target());
      unsigned int result = tisTos( daug, dvalid );
      // skip if no info
      if( !dvalid )continue;
      empty=false;
      if( m_compositeTPSviaPartialTOSonly ){
        //***no longer*** true no way to tell that it wasn't analyzed
        //empty=false;
        //  every daughter of the Parent must be TOS (TIS) for Parent to be TOS (TIS)
        resultTISTOS &= result;
        // set TPS unless done already
        if( !resultTPS ){
          if( result & kTOS ){
            // daughter TOS sets parent TPS
            resultTPS = kTPS;
          } else if( result & kTPS ){
            if( daug.substructure().size()>1 ){
              // non-trivially-composite daughter TPS sets parent TPS
              resultTPS= kTPS;
            } else if ( sub.size()==1 ){
              // non-composite or trivially composite daughter propagates its TPS to parent with only daughter
              resultTPS= kTPS;
            }
          }
        }
        // see if can quit already
        if( ! (resultTISTOS & kTOSandTIS) && resultTPS ){
          break;
        }
      } else {
        //   no info? skip
	if( !result )continue;
        //***done already*** 
        //empty=false;
        //  every daughter of the Parent must be TOS (TIS) for Parent to be TOS (TIS)
        resultTISTOS &= result;
        //  any TPS daughter is fine for TPS
        resultTPS |=  (result & kTPS);
        // see if can quit already
        if( ! (resultTISTOS & kTOSandTIS) ){
          // neither TIS nor TOS already: by definition TPS
          resultTPS=kTPS;
          break;
        }
      }
    }
    if( empty ){
      valid = 0;
      return 0;
    }
    return (resultTISTOS|resultTPS);
  } else {
    // object with hits
    return tisTosSortedHits( sortedHits(hos.lhcbIDs()),valid );
  }
  return 0;
}

std::string ParticleTisTos::analysisReport(const LHCb::HltObjectSummary & hos)
{
  std::ostringstream report;
  unsigned int dvalid;
  report << offset() << " HltObjectSummary CLID " << hos.summarizedObjectCLID();
  const SmartRefVector< LHCb::HltObjectSummary > & sub = hos.substructure();
  if( hos.summarizedObjectCLID() == 1 ){
    // this is Selection summary: take OR between candidates
    report << " Selection #-of-candidates " << sub.size() << std::endl;
    unsigned int result=0;
    for( SmartRefVector< LHCb::HltObjectSummary >::const_iterator ihos=sub.begin();ihos!=sub.end();++ihos){
      if(!(ihos->target()))continue;
      const LHCb::HltObjectSummary &  daug = *(ihos->target());
      result |= tisTos( daug,dvalid );
      // report << offset() << " Selection Candidates ";
      ++m_reportDepth;
      report << analysisReport( daug ) ; // << std::endl;
      --m_reportDepth;
      if( (result & kTOS) && (result & kTIS) && (result & kTPS ) )break;
    }
    TisTosTob res( result );
    report << offset()
           << " HltObjectSummary Selection TIS= " << res.tis() << " TOS= " << res.tos() << " TPS= " << res.tps() << std::endl;
    return report.str();
  } else if( sub.size() >0 ){
    report << " Substructure size " << sub.size() << std::endl;
    // object with substructure
    unsigned int resultTISTOS=kTOSandTIS;
    unsigned int resultTPS=0;
    bool empty(true);
    for( SmartRefVector< LHCb::HltObjectSummary >::const_iterator ihos=sub.begin();ihos!=sub.end();++ihos){
      if(!(ihos->target()))continue;
      const LHCb::HltObjectSummary &  daug = *(ihos->target());
      unsigned int result = tisTos( daug,dvalid );
      // report << offset() << " Subobject ";
      ++m_reportDepth;
      report << analysisReport( daug ) ;
      --m_reportDepth;
      // skip if no info
      if( !dvalid )continue;
      empty=false;
      if( m_compositeTPSviaPartialTOSonly ){
        // *** no longer true *** no way to tell that it wasn't analyzed
        // empty=false;
        //  every daughter of the Parent must be TOS (TIS) for Parent to be TOS (TIS)
        resultTISTOS &= result;
        // set TPS unless done already
        if( !resultTPS ){
          if( result & kTOS ){
            // daughter TOS sets parent TPS
            resultTPS = kTPS;
          } else if( result & kTPS ){
            if( daug.substructure().size()>1 ){
              // non-trivially-composite daughter TPS sets parent TPS
              resultTPS= kTPS;
            } else if ( sub.size()==1 ){
              // non-composite or trivially composite daughter propagates its TPS to parent with only daughter
              resultTPS= kTPS;
            }
          }
        }
        // see if can quit already
        if( ! (resultTISTOS & kTOSandTIS) && resultTPS ){
          if( !m_fullAnalysisReport )break;
        }
      } else {
        //   no info? skip
        if( !result )continue;
        //  empty=false;
        //  every daughter of the Parent must be TOS (TIS) for Parent to be TOS (TIS)
        resultTISTOS &= result;
        //  any TPS daughter is fine for TPS
        resultTPS |=  (result & kTPS);
        // see if can quit already
        if( ! (resultTISTOS & kTOSandTIS) ){
          // neither TIS nor TOS already: by definition TPS
          resultTPS=kTPS;
          if( !m_fullAnalysisReport )break;
        }
      }
    }
    unsigned int result;
    if( empty ){
      result =0;
    } else {
      result =(resultTISTOS|resultTPS);
    }
    TisTosTob res( result );
    report << offset()
           << " HltObjectSummary Composite TIS= " << res.tis() << " TOS= " << res.tos() << " TPS= " << res.tps() << std::endl;
    return report.str();
  } else {
    // object with hits
    report << " " + analysisReportSortedHits( sortedHits(hos.lhcbIDs()) ) << std::endl;
    return report.str();
  }
  return report.str();
}


bool ParticleTisTos::tos(const LHCb::HltObjectSummary & hos, unsigned int & valid)
{
  valid = 1; 
  unsigned int dvalid;
  const SmartRefVector< LHCb::HltObjectSummary > & sub = hos.substructure();
  if ( msgLevel(MSG::VERBOSE) ) verbose() << " tos HltObjectSummary substr size " << sub.size() << endmsg;

  if( hos.summarizedObjectCLID() == 1 ){

    // this is Selection summary: take OR between candidates
    valid = 0;
    for( SmartRefVector< LHCb::HltObjectSummary >::const_iterator ihos=sub.begin();ihos!=sub.end();++ihos){
      if(!(ihos->target()))continue;
      const LHCb::HltObjectSummary &  daug = *(ihos->target());
      if( tos( daug,dvalid ) ){
	if( dvalid )valid=1;
	return true;
      }
      if( dvalid )valid=1;
    }
    return false;

  } else if( sub.size() ) {

    // this is object with substructure: take AND between parts
    bool empty(true);
    for( SmartRefVector< LHCb::HltObjectSummary >::const_iterator ihos=sub.begin();ihos!=sub.end();++ihos){
      if(!(ihos->target()))continue;
      const LHCb::HltObjectSummary &  daug = *(ihos->target());
      //      if( !tos( daug ) )return false;
      if( !tos( daug, dvalid ) ){
	if( dvalid ){
	  return false;
	} else {
	  continue;
	}
      }
      empty=false;
    }
    if( empty ){
      valid = 0;
      return false;
    }
    return true;

  } else {
    // object with hits
    return tosSortedHits( sortedHits(hos.lhcbIDs()), valid );
  }
}

bool ParticleTisTos::tis(const LHCb::HltObjectSummary & hos, unsigned int & valid)
{
  valid = 1;
  unsigned int dvalid;
  const SmartRefVector< LHCb::HltObjectSummary > & sub = hos.substructure();
  if ( msgLevel(MSG::VERBOSE) ) verbose() << " tis HltObjectSummary substr size " << sub.size() << endmsg;

  if( hos.summarizedObjectCLID() == 1 ){

    // this is Selection summary: take OR between candidates
    valid =0;
    for( SmartRefVector< LHCb::HltObjectSummary >::const_iterator ihos=sub.begin();ihos!=sub.end();++ihos){
      if(!(ihos->target()))continue;
      const LHCb::HltObjectSummary &  daug = *(ihos->target());
      if( tis( daug,dvalid ) ){
	if( dvalid )valid=1;
	return true;
      }
      if( dvalid )valid=1;
    }
    return false;

  } else if( sub.size() ) {

    // this is object with substructure: take AND between parts
    bool empty(true);
    for( SmartRefVector< LHCb::HltObjectSummary >::const_iterator ihos=sub.begin();ihos!=sub.end();++ihos){
      if(!(ihos->target()))continue;
      const LHCb::HltObjectSummary &  daug = *(ihos->target());
      //      if( !tis( daug ) )return false;
      if( !tis( daug, dvalid ) ){
	if( dvalid ){
	  return false;
	} else {
	  continue;
	}
      }
      empty=false;
    }
    if( empty ){
      valid = 0;
      return false;
    }
    return true;

  } else {
    // object with hits
    return tisSortedHits( sortedHits(hos.lhcbIDs()),valid );
  }
}

bool ParticleTisTos::tus(const LHCb::HltObjectSummary & hos, unsigned int & valid)
{
  valid = 1;
  unsigned int dvalid;
  const SmartRefVector< LHCb::HltObjectSummary > & sub = hos.substructure();
  if ( msgLevel(MSG::VERBOSE) ) verbose() << " tus HltObjectSummary substr size " << sub.size() << endmsg;

  if( hos.summarizedObjectCLID() == 1 ){

    // this is Selection summary: take OR between candidates
    valid =0;
    for( SmartRefVector< LHCb::HltObjectSummary >::const_iterator ihos=sub.begin();ihos!=sub.end();++ihos){
      if(!(ihos->target()))continue;
      const LHCb::HltObjectSummary &  daug = *(ihos->target());
      if( tus( daug,dvalid ) ){
	if( dvalid )valid=1;
	return true;
      }
      if( dvalid )valid=1;
    }
    return false;

  } else if( sub.size() ) {

    // this is object with substructure
    bool empty(true);
    for( SmartRefVector< LHCb::HltObjectSummary >::const_iterator ihos=sub.begin();ihos!=sub.end();++ihos){
      if(!(ihos->target()))continue;
      const LHCb::HltObjectSummary &  daug = *(ihos->target());
      if( m_compositeTPSviaPartialTOSonly ){
        if( daug.substructure().size() > 1 ){
          if( tus( daug, dvalid ) )return true;
	  if( dvalid )empty=false;
	} else {
          if( tos( daug,dvalid ) )return true;
	  if( dvalid )empty=false;
        }
      } else {
        if( tus( daug,dvalid ) )return true;
	if( dvalid )empty=false;
      }
    }
    if( empty )valid = 0;
    return false;

  } else {
    // object with hits
    if( m_compositeTPSviaPartialTOSonly ){
      return tosSortedHits( sortedHits(hos.lhcbIDs()),valid );
    } else {
      return tusSortedHits( sortedHits(hos.lhcbIDs()),valid );
    }
  }

}

//=============================================================================
// outputs: classified Vertex
//=============================================================================
unsigned int ParticleTisTos::tisTos(const Vertex & onvertex)
{
  unsigned int resultTISTOS=kTOSandTIS;
  unsigned int resultTPS=0;
  bool empty(true);
  const SmartRefVector<LHCb::Particle> & ontracks=onvertex.outgoingParticles();
  for (SmartRefVector<LHCb::Particle>::const_iterator onit = ontracks.begin();onit != ontracks.end(); ++onit) {
    if( onit->target() ){
      const Particle& ontrack = *(*onit);
      unsigned int result = tisTos( ontrack );
      if( !result )continue;
      empty=false;
      //  every Track in vertex must be TOS (TIS) for vertex to be TOS (TIS)
      resultTISTOS &= result;
      // set TPS unless done already
      if( !resultTPS ){
        if( m_compositeTPSviaPartialTOSonly ){
          if( result & kTOS ){
            resultTPS = kTPS;
          }
        } else {
          resultTPS = result & kTPS;
        }
      }
      //  see if can quite already
      if( ! (resultTISTOS & kTOSandTIS) ){
        if( !m_compositeTPSviaPartialTOSonly )resultTPS=kTPS;
        // all conditions determined?
        if( resultTPS )break;
      }
    }
  }
  if( empty )return 0;
  return (resultTISTOS|resultTPS);
}

std::string ParticleTisTos::analysisReport(const Vertex & onvertex)
{
  std::ostringstream report;
  unsigned int resultTISTOS=kTOSandTIS;
  unsigned int resultTPS=0;
  bool empty(true);
  const SmartRefVector<LHCb::Particle> & ontracks=onvertex.outgoingParticles();
  report << offset() << " Vertex #-of-Particles " << ontracks.size() << std::endl;
  for (SmartRefVector<LHCb::Particle>::const_iterator onit = ontracks.begin();onit != ontracks.end(); ++onit) {
    if( onit->target() ){
      const Particle& ontrack = *(*onit);
      unsigned int result = tisTos( ontrack );
      ++m_reportDepth;
      report << analysisReport(ontrack) << std::endl;
      --m_reportDepth;
      if( !result )continue;
      empty=false;
      //  every Track in vertex must be TOS (TIS) for vertex to be TOS (TIS)
      resultTISTOS &= result;
      // set TPS unless done already
      if( !resultTPS ){
        if( m_compositeTPSviaPartialTOSonly ){
          if( result & kTOS ){
            resultTPS = kTPS;
          }
        } else {
          resultTPS = result & kTPS;
        }
      }
      //  see if can quite already
      if( ! (resultTISTOS & kTOSandTIS) ){
        if( !m_compositeTPSviaPartialTOSonly )resultTPS=kTPS;
        // all conditions determined?
        if( resultTPS )if( !m_fullAnalysisReport ) break;
      }
    }
  }
  unsigned int result;
  if( empty ){
    result =0;
  } else {
    result =(resultTISTOS|resultTPS);
  }
  TisTosTob res( result );
  report << offset() << " Vertex TIS= " << res.tis() << " TOS= " << res.tos() << " TPS= " << res.tps() << std::endl;
  return report.str();
}

bool  ParticleTisTos::tos(const Vertex & onvertex)
{
  bool empty(true);
  const SmartRefVector<LHCb::Particle> & ontracks=onvertex.outgoingParticles();
  for (SmartRefVector<LHCb::Particle>::const_iterator onit = ontracks.begin();onit != ontracks.end(); ++onit) {
    if( onit->target() ){
      const Particle& ontrack = *(*onit);
      //  every Track in vertex must be TOS (TIS) for vertex to be TOS (TIS)
      if( !tos( ontrack ) )return false;
      empty = false;
    }
  }
  if( empty )return false;
  return true;
}

bool  ParticleTisTos::tis(const Vertex & onvertex)
{
  bool empty(true);
  const SmartRefVector<LHCb::Particle> & ontracks=onvertex.outgoingParticles();
  for (SmartRefVector<LHCb::Particle>::const_iterator onit = ontracks.begin();onit != ontracks.end(); ++onit) {
    if( onit->target() ){
      const Particle& ontrack = *(*onit);
      //  every Track in vertex must be TOS (TIS) for vertex to be TOS (TIS)
      if( !tis( ontrack ) )return false;
      empty = false;
    }
  }
  if( empty )return false;
  return true;
}

bool  ParticleTisTos::tus(const Vertex & onvertex)
{
  const SmartRefVector<LHCb::Particle> & ontracks=onvertex.outgoingParticles();
  for (SmartRefVector<LHCb::Particle>::const_iterator onit = ontracks.begin();onit != ontracks.end(); ++onit) {
    if( onit->target() ){
      const Particle& ontrack = *(*onit);
      //  it is enough for one track to use signal for vertex to use it
      if( m_compositeTPSviaPartialTOSonly ){
        if( tos( ontrack ) )return true;
      } else {
        if( tus( ontrack ) )return true;
      }
    }
  }
  return false;
}
