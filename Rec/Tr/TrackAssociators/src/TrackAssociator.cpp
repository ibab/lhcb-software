// $Id: TrackAssociator.cpp,v 1.10 2006-06-09 15:50:39 erodrigu Exp $
// Include files

// local
#include "TrackAssociator.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// from Event/LinkerEvent
#include "Linker/LinkerWithKey.h"
#include "Linker/LinkedTo.h"

// from Event/TrackEvent
#include "Event/Track.h"

// from Event/Event
#include "Event/MCParticle.h"

// from Event/VeloEvent
#include "Event/VeloCluster.h"

// from Event/STEvent
#include "Event/STCluster.h"

// from Event/OTEvent
#include "Event/OTTime.h"

// from Tr/TrackFitEvent
#include "Event/OTMeasurement.h"
#include "Event/STMeasurement.h"
#include "Event/VeloRMeasurement.h"
#include "Event/VeloPhiMeasurement.h"

// from Event/Event
#include "Event/MCVertex.h"

using namespace LHCb;

// Declaration of the Algorithm Factory
static const  AlgFactory<TrackAssociator>          s_factory ;
const        IAlgFactory& TrackAssociatorFactory = s_factory ;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackAssociator::TrackAssociator( const std::string& name,
                                  ISvcLocator* pSvcLocator ) :
  GaudiAlgorithm( name, pSvcLocator ),
  m_fractionOK(0.),
  m_nTotVelo(0.),
  m_nTotTT1(0.),
  m_nTotSeed(0.)
{
  declareProperty( "TracksInContainer",
                   m_tracksInContainer = TrackLocation::Default );
  declareProperty( "LinkerOutTable", m_linkerOutTable = "" );
  declareProperty( "FractionOK"    , m_fractionOK = 0.70 );
}

//=============================================================================
// Destructor
//=============================================================================
TrackAssociator::~TrackAssociator() {};

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode TrackAssociator::initialize() {

  // Mandatory initialization of GaudiAlgorithm
  StatusCode sc = GaudiAlgorithm::initialize();
  if( sc.isFailure() ) { return sc; }
  
  // Set the path for the linker table Track - MCParticle
  if ( m_linkerOutTable == "" ) m_linkerOutTable = m_tracksInContainer;

  m_debugLevel = msgLevel( MSG::DEBUG );

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode TrackAssociator::execute() {

  // Retrieve the Tracks
  Tracks* tracks = get<Tracks> ( m_tracksInContainer );

  // Retrieve the MCParticles
  MCParticles* mcParts = get<MCParticles> ( MCParticleLocation::Default );

  // Create the Linker table from Track to MCParticle
  // Linker table is stored in "Link/" + m_linkerOutTable
  // Sorted by decreasing weight, so first retrieved has highest weight
  LinkerWithKey<MCParticle,Track>
    myLinker( evtSvc(), msgSvc(), m_linkerOutTable );

  // Get the linker table VeloCluster => MCParticle
  LinkedTo<MCParticle,VeloCluster>
    veloLink( evtSvc(), msgSvc(), LHCb::VeloClusterLocation::Default );
  if( veloLink.notFound() ) {
    error() << "Unable to retrieve VeloCluster to MCParticle linker table."
            << endreq;
    return StatusCode::FAILURE;
  }
  
  // Get the linker table TTCluster => MCParticle
  LinkedTo<MCParticle,STCluster>
    ttLink( evtSvc(), msgSvc(), LHCb::STClusterLocation::TTClusters );
  if( ttLink.notFound() ) {
    error() << "Unable to retrieve TTCluster to MCParticle linker table."
            << endreq;
    return StatusCode::FAILURE;
  }

  // Get the linker table ITCluster => MCParticle
  LinkedTo<MCParticle,STCluster> itLink(evtSvc(),msgSvc(),LHCb::STClusterLocation::ITClusters);
  if( itLink.notFound() ) {
    error() << "Unable to retrieve ITCluster to MCParticle linker table."
            << endreq;
    return StatusCode::FAILURE;
  }

  // Get the linker table OTCluster => MCParticle
  LinkedTo<MCParticle,OTTime> otLink(evtSvc(),msgSvc(),LHCb::OTTimeLocation::Default);
  if( otLink.notFound() ) {
    error() << "Unable to retrieve OTCluster to MCParticle linker table."
            << endreq;
    return StatusCode::FAILURE;
  }

  // Loop over the Tracks
  Tracks::const_iterator it;
  for( it = tracks->begin(); tracks->end() != it; ++it ) {
    const Track* tr = *it;
    m_nTotVelo = 0.;
    m_nTotTT1  = 0.;
    m_nTotSeed = 0.;
    m_parts.clear();
    m_nVelo.clear();
    m_nTT1.clear();
    m_nSeed.clear();

    const VeloCluster* clu;
    
    // Loop over the Measurements of the Track
    std::vector<Measurement*>::const_iterator itm;
    for( itm = tr->measurements().begin();
         tr->measurements().end() != itm; ++itm ) {

      clu = 0;
      VeloPhiMeasurement* meas = dynamic_cast<VeloPhiMeasurement*> ( *itm );
      if( 0 != meas ) { clu = meas->cluster(); }
      else {
        VeloRMeasurement* meas = dynamic_cast<VeloRMeasurement*> ( *itm );
        if( 0 != meas ) { clu = meas->cluster(); }
      }
      if( 0 != clu ) {
        // Count number of Velo hits
        m_nTotVelo += 1.;
        // Loop over the MCparticles linked to the VeloMeasurement
        MCParticle* mcParticle = veloLink.first( clu );
        if( m_debugLevel && 0 == mcParticle ) {
          debug() << "No MCParticle linked with VeloCluster " 
                  << clu -> key() << endreq;
        }
        while( 0 != mcParticle ) {
          if( mcParts != mcParticle->parent() ) {
            debug() << " (other BX " <<  mcParticle->key() << ")" << endreq;
          }
          else { countMCPart( mcParticle, 1., 0., 0. ); }
          mcParticle = veloLink.next();
        }
        // In case it was a Velo Measurement, go to the end of the for-loop
        continue;
      }

      else {
        bool inTT1 = (*itm) -> lhcbID().isTT();
        // Note that both IT and TT hits are STMeasurements!
        STMeasurement* itc = dynamic_cast<STMeasurement*> ( *itm );
        if( 0 != itc ) {
          const STCluster* itCl = itc->cluster();
          // Count number of TT hits
          if( inTT1 ) {
            m_nTotTT1 += 1.;
            // Loop over the MCparticles associated to the TT STMeasurement
            MCParticle* mcParticle = ttLink.first( itCl );
            if( m_debugLevel && 0 == mcParticle ) {
              debug() << "No MCParticle linked with TT STCluster "
                      << itCl -> key() << endreq;
            }
            while( 0 != mcParticle ) {
              if( mcParts != mcParticle->parent() ) {
                debug() << " (other BX " <<  mcParticle->key() << ")" << endreq;
              }
              else { countMCPart( mcParticle, 0., 1., 0. ); }
              mcParticle = ttLink.next();
            }
          }
          // Count number of IT+OT hits
          else {
            m_nTotSeed += 1.;
            // Loop over the MCparticles associated to the IT STMeasurement
            MCParticle* mcParticle = itLink.first( itCl );
            if( m_debugLevel && 0 == mcParticle ) {
              debug() << "No MCParticle linked with IT STCluster "
                      << itCl -> key() << endreq;
            }
            while( 0 != mcParticle ) {
              if( mcParts != mcParticle->parent() ) {
                debug() << " (other BX " <<  mcParticle->key() << ")" << endreq;
              }
              else { countMCPart( mcParticle, 0., 0., 1. ); }
              mcParticle = itLink.next();
            }
          }
          // In case it was a TT/IT Measurement, go to the end of the for-loop
          continue;
        }
        

        OTMeasurement* otc = dynamic_cast<OTMeasurement*>(*itm);
        if( 0 != otc ) {
          const OTTime* otTim = otc->time();
          // Count number of IT+OT hits
          m_nTotSeed += 1.;
          // Loop over the MCparticles associated to the STMeasurement
          MCParticle* mcParticle = otLink.first( otTim );
          if( m_debugLevel && 0 == mcParticle ) {
            debug() << "No MCParticle linked with OTTime "
                    << otTim -> key() << endreq;
          }
          while( 0 != mcParticle ) {
            if( mcParts != mcParticle->parent() ) {
              debug() << " (other BX " <<  mcParticle->key() << ")" << endreq;
            }
            else { countMCPart( mcParticle, 0., 0., 1. ); }
            mcParticle = otLink.next();
          }
        }
      } // end else

    } // end for

    //== For ST match, count also parents hits if in VELO !
    // If the Track has total # Velo hits > 2 AND total # IT+OT hits > 2
    if( ( 2 < m_nTotVelo ) && ( 2 < m_nTotSeed ) ) {
      unsigned int j1, j2;
      // Loop over the vector of MCParticles which have a Measurement associated
      for( j1 = 0; m_parts.size() > j1; ++j1 ) {
        // If the # of IT+OT hits associated to the MCParticle is
        // smaller than 0.70*total # of IT+OT hits, then go to the end
        // of the loop over the MCParticles with a Measurement associated
        // This prevents one from looking at the GodMother MCParticle
        if( m_nSeed[j1] < m_fractionOK * m_nTotSeed ) { continue; }
        const MCVertex* vOrigin = m_parts[j1]->originVertex();
        while( 0 != vOrigin ) {
          const MCParticle* mother = vOrigin->mother();
          // If the originVertex has no mother, then exit the while loop
          if( 0 == mother ) break;
          // Loop over vector of MCParticles which have a Measurement associated
          for( j2 = 0; m_parts.size() > j2; ++j2 ) {
            // If the # of Velo hits associated to the MCParticle is
            // smaller than 0.70*total # of Velo hits, then go to the end
            // of the loop over the MCParticles with a Measurement associated
            // This prevents one from looking at the GodMother MCParticle
            if( m_nVelo[j2] < m_fractionOK * m_nTotVelo ) { continue;}
            // If one of the MCParticles of m_parts is the mother of originVertex
            if( mother == m_parts[j2] ) {
              
              debug() << "  *** Particle " << m_parts[j1]->key() 
                      << "[" << m_parts[j1]->particleID().pid()
                      << "] (" << m_nVelo[j1] << "," << m_nTT1[j1] 
                      << "," << m_nSeed[j1] << ")" 
                      << " is daughter of " << m_parts[j2]->key()
                      << "[" << m_parts[j2]->particleID().pid()
                      << "] (" << m_nVelo[j2] << "," << m_nTT1[j2] 
                      << "," << m_nSeed[j2] << ")" 
                      << ". Merge hits to tag both." << endreq;
              
              //== Daughter hits are added to mother.
              m_nVelo[j2] += m_nVelo[j1];
              m_nTT1 [j2] += m_nTT1 [j1];
              m_nSeed[j2] += m_nSeed[j1];
              
              //== Mother hits overwrite Daughter hits
              m_nVelo[j1] = m_nVelo[j2];
              m_nTT1 [j1] = m_nTT1 [j2];
              m_nSeed[j1] = m_nSeed[j2];
            }
          }
          // Go one step into the past of family line, so find grandmother
          vOrigin = mother->originVertex();
        }
      }
    }

    // For each MCParticle with a Measurement associated,
    //  Relate the Track to the MCParticles matching the below criteria,
    //  using as weight the ratio of ( total # Measurements associated
    //                                 to this MCParticle )
    //                           and ( total # Measurements )
    //   if total # Velo hits > 2,
    //   or if 0.70 <= ( # Velo hits associated to the MCParticle /
    //                   total # Velo hits )
    //  AND
    //   if total # IT+OT hits > 2,
    //   or if 0.70 <= ( # IT+OT hits associated to the MCParticle /
    //                   total # IT+OT hits )
    //  AND
    //   if # associated TT hits > ( total # TT hits - 2 ) ??
    //   or if total # Velo hits > 2 AND total # IT+OT hits > 2
    double ratio;
    for( unsigned int jj = 0; m_parts.size() > jj; ++jj ) {
      bool veloOK = true;
      if( 2 < m_nTotVelo ) {
        veloOK = false;
        ratio = m_nVelo[jj] / m_nTotVelo;
        if( m_fractionOK <= ratio ) { veloOK = true; }
      }
      bool seedOK = true;
      if( 2 < m_nTotSeed ) {
        seedOK = false;
        ratio =  m_nSeed[jj] / m_nTotSeed;
        if( m_fractionOK <= ratio ) { seedOK = true; }
      }
      bool TT1OK = m_nTT1[jj] > m_nTotTT1 - 2;
      if( 2 < m_nTotVelo && 2 < m_nTotSeed ) { TT1OK = true; }

      verbose() << " MC " << m_parts[jj]->key() 
                << " Velo " <<  m_nVelo[jj] << "/" << m_nTotVelo
                << " TT1 " <<  m_nTT1[jj] << "/" << m_nTotTT1
                << " Seed " <<  m_nSeed[jj] << "/" << m_nTotSeed
                << endreq;

      //=== Decision. Use TT1. Fill Linker
      if( veloOK && seedOK && TT1OK ) {
        ratio = ( m_nVelo[jj] + m_nSeed[jj] + m_nTT1[jj] ) / tr->nMeasurements();
        myLinker.link( tr, m_parts[jj], ratio );
      }
    }
  } // End loop over Tracks

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode TrackAssociator::finalize() {

  return GaudiAlgorithm::finalize();
}

//=============================================================================
// Adjust the counters for this MCParticle, create one if needed
//=============================================================================
void TrackAssociator::countMCPart( const MCParticle* part, 
                                   double incVelo, 
                                   double incTT1,
                                   double incSeed ) {
  bool found = false;
  // Loop over the MCParticles vector to see whether part is already in m_parts
  for( unsigned int jj = 0; m_parts.size() > jj; ++jj ) {
    if( m_parts[jj] == part ) {
      m_nVelo[jj] += incVelo;
      m_nTT1[jj] += incTT1;
      m_nSeed[jj] += incSeed;
      // Prevent next if loop to run when this one already did the work
      found = true;
      // Stop looping when successful 
      break;
    }
  }
  // If part an element of m_parts if it was not already and update counters
  if( !found ) {
    m_parts.push_back( part );
    m_nVelo.push_back( incVelo );
    m_nTT1.push_back( incTT1 );
    m_nSeed.push_back( incSeed );
  }
}
