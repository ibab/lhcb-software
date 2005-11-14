// Include files

// local
#include "TrackAssociator.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// from Event/LinkerEvent
#include "Linker/LinkerWithKey.h"

// from Event/TrackEvent
#include "Event/Track.h"

// from Event/Event
#include "Event/MCParticle.h"

// from Event/VeloEvent
#include "Event/VeloCluster.h"

// from Event/ITEvent
#include "Event/ITCluster.h"

// from Event/OTEvent
#include "Event/OTTime.h"

// from Tr/TrackFitEvent
#include "Event/OTMeasurement.h"
#include "Event/ITMeasurement.h"
#include "Event/VeloRMeasurement.h"
#include "Event/VeloPhiMeasurement.h"

// from Event/Event
#include "Event/MCVertex.h"

// Declaration of the Algorithm Factory
static const  AlgFactory<TrackAssociator>          s_factory ;
const        IAlgFactory& TrackAssociatorFactory = s_factory ;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackAssociator::TrackAssociator( const std::string& name,
                                  ISvcLocator* pSvcLocator ) :
  GaudiAlgorithm( name, pSvcLocator ),
  m_minimalZ(0.),
  m_fractionOK(0.),
  m_veloClusToMCP(0),
  m_itClusToMCP(0),
  m_otTimToMCP(0),
  m_nTotVelo(0.),
  m_nTotTT1(0.),
  m_nTotSeed(0.)
{
  declareProperty( "TracksInContainer" , m_tracksInContainer );
  declareProperty( "LinkerOutTable"    , m_linkerOutTable );
  declareProperty( "MinimalZ"          , m_minimalZ = 5000.*mm );
  declareProperty( "FractionOK"        , m_fractionOK = 0.70 );
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

  // Retrieve Associator tools of subdetectors
  m_veloClusToMCP = tool<VeloClusAsct> ( "VeloCluster2MCParticleAsct" );

  m_itClusToMCP = tool<ITClusAsct> ( "ITCluster2MCParticleAsct" );

  m_otTimToMCP = tool<OTTimAsct> ( "OTTime2MCParticleAsct" );
 
  return StatusCode::SUCCESS;
};

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
  LinkerWithKey< MCParticle, Track > myLinker ( evtSvc(), msgSvc(),
                                                m_linkerOutTable );

  // Get the association VeloCluster => MCParticle
  VeloClusAsct::DirectType* dirTable = m_veloClusToMCP->direct();
  if( 0 == dirTable ) {
    error() << "Failure to retrieve the Velo Cluster Table" << endreq;
    return StatusCode::FAILURE;
  }
  
  // Get the association ITCluster => MCParticle
  ITClusAsct::DirectType* itTable = m_itClusToMCP->direct();
  if( 0 == itTable ) {
    error() << "Failure to retrieve the IT Cluster Table" << endreq;
    return StatusCode::FAILURE;
  }
  
  // Get the association OTCluster => MCParticle
  OTTimAsct::DirectType* otTable = m_otTimToMCP->direct();
  if( 0 == otTable ) {
    error() << "Failure to retrieve the OT Cluster Table" << endreq;
    return StatusCode::FAILURE;
  }
  
  // Declare usefull entities
  const VeloCluster* clu;
  VeloClusAsct::DirectType::iterator itv;
  MCParticle* part;
  ITClusAsct::DirectType::iterator itr;
  OTTimAsct::DirectType::iterator otr;

  // Loop over the Tracks
  Tracks::const_iterator it;
  for( it = tracks->begin(); tracks->end() != it; ++it ) {
    const Track* tr = *it;
    m_nTotVelo = 0.;
    m_nTotTT1 = 0.;
    m_nTotSeed = 0.;
    m_parts.clear();
    m_nVelo.clear();
    m_nTT1.clear();
    m_nSeed.clear();
    
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
        // Loop over the MCparticles associated to the VeloMeasurement
        VeloClusAsct::DirectType::Range range = dirTable->relations ( clu );
        for( itv = range.begin(); range.end() != itv; ++itv ) {
          part = itv->to();
          if( 0 == part ) {
            error() << "Null pointer associated to a Velo cluster" << endreq;
          }
          else if( mcParts != part->parent() ) {
            debug() << " (other BX " <<  part->key() << ")" << endreq;
          }
          else { countMCPart( part, 1., 0., 0. ); }
        }
        // In case it was a Velo Measurement, go to the end of the for-loop
        continue;
      }
      else {
        // True if Measurement has Z < 5 meters, so it is a TT hit
        bool inTT1 = ( m_minimalZ > (*itm)->z() );
        // Note that both IT and TT hits are called ITMeasurement
        ITMeasurement* itc = dynamic_cast<ITMeasurement*> ( *itm );
        if( 0 != itc ) {
          ITCluster* itCl = itc->cluster();
          // Count number of TT hits
          if( inTT1 ) { m_nTotTT1 += 1.; }
          // Count number of IT+OT hits
          else { m_nTotSeed += 1.; }
          // Loop over the MCparticles associated to the ITMeasurement
          ITClusAsct::DirectType::Range itRange = itTable->relations( itCl );
          for( itr = itRange.begin(); itRange.end() != itr; ++itr ) {
            part = itr->to();
            if( 0 == part ) {
              error() << "Null pointer associated to an IT cluster" << endreq;
            }
            else if( mcParts != part->parent() ) {
              debug() << " (other BX " <<  part->key() << ")" << endreq;
            }
            else {
              if( inTT1 ) { countMCPart( part, 0., 1., 0. ); }
              else { countMCPart( part, 0., 0., 1. ); }
            }
          }
          // In case it was a TT/IT Measurement, go to the end of the for-loop
          continue;
        }
        OTMeasurement* otc = dynamic_cast<OTMeasurement*>(*itm);
        if( 0 != otc ) {
          OTTime* otTim = otc->time();
          // Count number of IT+OT hits
          m_nTotSeed += 1.;
          // Loop over the MCparticles associated to the OTMeasurement
          OTTimAsct::DirectType::Range otRange = otTable->relations( otTim );
          for( otr = otRange.begin(); otRange.end() != otr; ++otr ) {
            part = otr->to();
            if( 0 == part ) {
              error() << "Null pointer associated to an OT cluster" << endreq;
            }
            else if( mcParts != part->parent() ) {
              debug() << " (other BX " <<  part->key() << ")" << endreq;
            }
            else { countMCPart( part, 0., 0., 1. ); }
          }
        }
      }
    }

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
        if( m_fractionOK <= ratio ) {
          veloOK = true;
        }
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
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode TrackAssociator::finalize() {

  return GaudiAlgorithm::finalize();
}

//=============================================================================

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
