// $Id: TrackAssociator.cpp,v 1.1.1.1 2005-09-22 08:27:06 erodrigu Exp $
// Include files 
// -------------
// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// from LHCb Kernel
#include "Relations/RelationWeighted2D.h"

// from Event
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"

// from TrackEvent
#include "Event/Track.h"

// from TrackFitEvent
#include "Event/OTMeasurement.h"
#include "Event/ITMeasurement.h"
#include "Event/VeloRMeasurement.h"
#include "Event/VeloPhiMeasurement.h"

// from XxxEvent
#include "Event/VeloCluster.h"
#include "Event/ITCluster.h"
#include "Event/OTTime.h"

// local
#include "TrackAssociator.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TrackAssociator
//
// 2005-09-12 : Edwin Bos
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<TrackAssociator>          s_factory ;
const        IAlgFactory& TrackAssociatorFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackAssociator::TrackAssociator( const std::string& name,
                                  ISvcLocator* pSvcLocator )
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputContainer" , m_inputContainer = "" );
  declareProperty( "OutputTable" ,    m_outputTable    = "" );
  declareProperty( "MinimalZ"    ,    m_minimalZ       = 5000.*mm );
  declareProperty( "FractionOK"  ,    m_fractionOK     = 0.70 );
}

//=============================================================================
// Destructor
//=============================================================================
TrackAssociator::~TrackAssociator() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode TrackAssociator::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialise" << endreq;

  sc = toolSvc()->retrieveTool( "VeloCluster2MCParticleAsct",m_veloClusToMCP );
  if ( !sc ) { return sc; }

  sc = toolSvc()->retrieveTool( "ITCluster2MCParticleAsct", m_itClusToMCP );
  if ( !sc ) { return sc; }

  sc = toolSvc()->retrieveTool( "OTTime2MCParticleAsct", m_otTimToMCP );
  if ( !sc ) { return sc; }

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode TrackAssociator::execute() {

  debug() << "==> Execute" << endreq;

  Tracks* tracks = get<Tracks>( m_inputContainer );

  MCParticles* mcParts = get<MCParticles>( MCParticleLocation::Default );

  // Create relation table and register it in the transient event store
  typedef RelationWeighted2D< Track, MCParticle, float >  Table;
  Table* table = new Table();
  StatusCode sc = put( table, m_outputTable );
  if ( sc.isFailure() ) {
    error() << "Unable to register the output container at "
            << m_outputTable << ". Status is " << sc << endreq;
    return StatusCode::FAILURE;
  }

  // Get the association VeloCluster => MCParticle
  VeloClusAsct::DirectType* dirTable = m_veloClusToMCP->direct();
  if ( 0 == dirTable ) {
    error() << "Failure to retrieve the Velo Cluster Table" << endreq;
    return StatusCode::FAILURE;
  }
  
  // Get the association ITCluster => MCParticle
  ITClusAsct::DirectType* itTable = m_itClusToMCP->direct();
  if ( 0 == itTable )
    return Error( "Failure to retrieve the IT Cluster Table" );

  // Get the association OT CLuster => MCParticle
  OTTimAsct::DirectType* otTable = m_otTimToMCP->direct();
  if ( 0 == otTable ) 
    return Error( "Failure to retrieve the OT Cluster Table" );

  Tracks::const_iterator it;
  double ratio;
  
  VeloClusAsct::DirectType::iterator itv;
  ITClusAsct::DirectType::iterator itr;
  OTTimAsct::DirectType::iterator otr;
  
  MCParticle* part;
  const VeloCluster* clu;

  for ( it = tracks->begin() ; tracks->end() != it; it++ ) {
    const Track* tr = *it;
    m_nTotSeed = 0.;
    m_nTotTT1  = 0.;
    m_nTotVelo = 0.;
    m_parts.clear();
    m_nVelo.clear();
    m_nTT1.clear();
    m_nSeed.clear();

    debug() << "++++ MC truth for hits on track " << tr->key() 
            << " nHit = "
            << (tr->measurements()).end() - (tr->measurements()).begin()
            << endreq;

    std::vector<Measurement*>::const_iterator itm;
    for ( itm = (tr->measurements()).begin();
          (tr->measurements()).end() != itm  ; itm++  ) {
      clu = 0;
      VeloPhiMeasurement* meas = dynamic_cast<VeloPhiMeasurement*>(*itm);
      if ( 0 != meas ) {
        clu = meas->cluster();
        verbose() << " ..VP hit z " << meas->z();
      } else {
        VeloRMeasurement* meas = dynamic_cast<VeloRMeasurement*>(*itm);
        if ( 0 != meas ) {
          clu = meas->cluster();
          verbose() << " ..VR hit z " << meas->z();
        }
      }

      if ( 0 != clu ) {
        // Count each VELO cluster match
        m_nTotVelo += 1.;
        VeloClusAsct::DirectType::Range range = dirTable->relations( clu );
        for ( itv = range.begin(); range.end() != itv; itv++ ) {
          part = itv->to();
          if ( 0 == part ) {
            error() << "Null track pointer associated to a Velo cluster";
          } else if( mcParts != part->parent() ) {
            error() << " (other BX " <<  part->key() << ")";
          } else {
            error() << " " << part->key();
            countMCPart( part, 1., 0., 0. );
          }
        }
        error() << endreq;   // Ends also the possible ERROR message.
        continue;
      } else {
        bool inTT1 = ( m_minimalZ > (*itm)->z() );  // ignore TT1
        
        // Count each IT cluster match 
        ITMeasurement* itc = dynamic_cast<ITMeasurement*>(*itm);
        if ( 0 != itc ) {
          ITCluster* itCl = itc->cluster();
          if ( inTT1 ) {
            m_nTotTT1 += 1.;
          } else {
            m_nTotSeed += 1.;
          }
          verbose() << " ..IT hit z " << itc->z() ;
          
          ITClusAsct::DirectType::Range itRange = itTable->relations( itCl );
          for ( itr = itRange.begin(); itRange.end() != itr; itr++ ) {
            part = itr->to();
            if ( 0 == part ) {
              error() << "Null track pointer associated to an IT cluster";
            } else if( mcParts != part->parent() ) {
              error() << " (other BX " <<  part->key() << ")";
            } else {
              error() << " " << part->key();
              if ( inTT1 ) { countMCPart( part, 0., 1., 0. ); }
              else { countMCPart( part, 0., 0., 1. ); }
            }
          }
          error() << endreq;   // Ends also the possible ERROR message.
          continue;
        }
        
        // Count each OT cluster match
        OTMeasurement* otc = dynamic_cast<OTMeasurement*>(*itm);
        if ( 0 != otc ) {
          OTTime* otTim = otc->time();
          if ( inTT1 ) { m_nTotTT1 += 1.; }
          else { m_nTotSeed += 1.; }
          verbose() << " ..OT hit z " << otc->z();
          
          OTTimAsct::DirectType::Range otRange = otTable->relations( otTim );
          for ( otr = otRange.begin(); otRange.end() != otr; otr++ ) {
            part = otr->to();
            if ( 0 == part ) {
              error() << "Null track pointer associated to an OT cluster";
            } else if( mcParts != part->parent() ) {
              error() << " (other BX " <<  part->key() << ")";
            } else {
              error() << " " << part->key();
              if ( inTT1 ) { countMCPart( part, 0., 1., 0. ); }
              else { countMCPart( part, 0., 0., 1. ); }
            }
          }
          error() << endreq;   // Ends also the possible ERROR message.
          continue;
        }
        debug() << " ..?? hit z " << (*itm)->z() << endreq;
      }
    }
    
    //== For ST match, count also parents hits if in VELO !
    if ( ( 2 < m_nTotVelo ) && ( 2 < m_nTotSeed ) ) {
      unsigned int j1, j2;
      for ( j1 = 0; m_parts.size() > j1; j1++ ) {
        if ( m_nSeed[j1] <  m_fractionOK* m_nTotSeed ) continue;

        const MCVertex* vOrigin = m_parts[j1]->originVertex();
        while ( 0 != vOrigin ) {
          const MCParticle* mother = vOrigin->mother();
          if ( 0 == mother ) break;
          for ( j2 = 0 ; m_parts.size() > j2 ; j2++ ) {
            if ( m_nVelo[j2] < m_fractionOK* m_nTotVelo ) continue;
            if ( mother == m_parts[j2] ) {

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
              m_nVelo[j2] = m_nVelo[j1] + m_nVelo[j2];
              m_nTT1 [j2] = m_nTT1 [j1] + m_nTT1 [j2];
              m_nSeed[j2] = m_nSeed[j1] + m_nSeed[j2];
              
              //== Daughter is also fully tagged
              m_nVelo[j1] = m_nVelo[j2];
              m_nTT1 [j1] = m_nTT1 [j2];
              m_nSeed[j1] = m_nSeed[j2];
            }
          }
          vOrigin = mother->originVertex();
        }
      }
    }
    
    //== We have collected the information. Now decide...
    for ( unsigned int jj = 0 ; m_parts.size() > jj ; jj++  ) {

      //== Velo part : OK if less than 3 hits, else more than m_fractionOK.
      bool veloOK = true;
      if ( 2 < m_nTotVelo ) {
        veloOK = false;
        ratio =  m_nVelo[jj] / m_nTotVelo;
        if ( m_fractionOK <= ratio ) {
          veloOK = true;
        }
      }

      //== Seed part : OK if less than 3 hits, else more than m_fractionOK.
      bool seedOK = true;
      if ( 2 < m_nTotSeed ) {
        seedOK = false;
        ratio =  m_nSeed[jj] / m_nTotSeed;
        if ( m_fractionOK <= ratio ) { seedOK = true; }
      }

      //== TT1 : OK if 0 or 1 wrong hits. OK if both Velo and Seed on track
      bool TT1OK = m_nTT1[jj] > m_nTotTT1 - 2;
      if (  2 < m_nTotVelo && 2 < m_nTotSeed ) TT1OK = true;

      verbose() << " MC " << m_parts[jj]->key() 
                << " Velo " <<  m_nVelo[jj] << "/" << m_nTotVelo
                << " TT1 "  <<  m_nTT1[jj]  << "/" << m_nTotTT1
                << " Seed " <<  m_nSeed[jj] << "/" << m_nTotSeed
                << endreq;

      //=== Decision. Use TT1

      if ( veloOK && seedOK && TT1OK) {
        debug() << "  => match MC " << m_parts[jj]->key() << endreq;
        ratio = ( m_nVelo[jj] + m_nSeed[jj] + m_nTT1[jj] ) / 
                ( m_nTotVelo  + m_nTotSeed  + m_nTotTT1  );
        table->relate( tr, m_parts[jj], ratio );
      }
    }
  }
  debug() << " Track matching finished." << endreq;

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode TrackAssociator::finalize() {

  debug() << "==> Finalize" << endreq;
  
  // Release all tools
  if( m_veloClusToMCP ) toolSvc()->releaseTool( m_veloClusToMCP );
  if( m_itClusToMCP )   toolSvc()->releaseTool( m_itClusToMCP );
  if( m_otTimToMCP )    toolSvc()->releaseTool( m_otTimToMCP );
  
  return GaudiAlgorithm:: finalize();  // must be called after all other actions
}

//=============================================================================
