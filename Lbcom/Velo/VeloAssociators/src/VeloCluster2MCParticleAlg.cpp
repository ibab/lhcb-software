// Include files 
#include "Event/MCParticle.h"
#include "Linker/LinkerTool.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "VeloAssociators/VeloCluster2MCParticleAsct.h"
#include "VeloCluster2MCParticleAlg.h"

//-----------------------------------------------------------------------------
// Implementation file for class : VeloCluster2MCParticleAlg
//
// 22/05/2002 : Chris Parkes
// 10/07/2003 : David Hutchcroft Modified to use VeloCluster2MCHit table
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<VeloCluster2MCParticleAlg>          s_factory ;
const        IAlgFactory& VeloCluster2MCParticleAlgFactory = s_factory ; 

VeloCluster2MCParticleAlg::VeloCluster2MCParticleAlg( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : GaudiAlgorithm (name,pSvcLocator) 
{
  // constructor
  declareProperty( "OutputData", 
                   m_outputData = VeloCluster2MCParticleAsctLocation );
  m_clusters = 0;
}

VeloCluster2MCParticleAlg::~VeloCluster2MCParticleAlg() {
  // destructor
}; 

StatusCode VeloCluster2MCParticleAlg::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  m_pV2MCHit = tool<VeloCluster2MCHitAsct::IAsct>("VeloCluster2MCHitAsct");
 
  return StatusCode::SUCCESS;
};


StatusCode VeloCluster2MCParticleAlg::execute() {

  // get VeloClusters
  m_clusters = get<VeloClusters>( VeloClusterLocation::Default );

  StatusCode sc = associateFromLinker();
  if( sc.isFailure() ) {
    sc = associateFromRelations();
  }
  
  return sc;
  
}

StatusCode VeloCluster2MCParticleAlg::associateFromLinker() {

  typedef LinkerTool<VeloCluster,MCVeloHit> VCl2MCHit;
  typedef VCl2MCHit::DirectType Table;
  typedef Table::Range Range;
  typedef Table::iterator iterator;

  if( 0 == m_clusters ) return Error( "VeloClusters not available" );

  // Retrieve the VeloClusters2MCHits table
  VCl2MCHit associator( evtSvc(), "VeloClusters2MCHits" );
  const Table* table = associator.direct();
  if (0==table){
    return Error( "Could not retrieve VeloClusters2MCHits Linker table" );
  }

  // create the output (Relations!) table 
  VeloCluster2MCParticleAsct::Table* aTable = 
    new VeloCluster2MCParticleAsct::Table(); 

  // Loop and link VeloClusters to MC truth
  VeloClusters::const_iterator iterClus;
  for(iterClus = m_clusters->begin(); iterClus != m_clusters->end(); iterClus++){

    Range range1 = table->relations(*iterClus);
    iterator relation;
    for (relation=range1.begin(); relation !=range1.end(); relation++){
      // loop over relations
      const MCVeloHit* hit = relation->to ();
      const MCParticle* aParticle = hit->mcParticle();
      double charge = relation->weight();
      debug() << "Relation output - particle charge " << charge << endmsg;
      if(0==aParticle){
        warning() << "MCHit did not have an MCParticle parent" << endmsg;
      }else{
        aTable->relate(*iterClus,aParticle,charge);
      }
    }
  } // loop iterClus
  
  // register table in store
  put( aTable, outputData() );
 
  return StatusCode::SUCCESS;
}

StatusCode VeloCluster2MCParticleAlg::associateFromRelations() {

  if( 0 == m_clusters ) return Error( "VeloClusters not available" );

  typedef AssociatorWeighted<VeloCluster,MCVeloHit,double> VCl2MCHit;
  typedef VCl2MCHit::DirectType Table;
  typedef Table::Range Range;
  typedef Table::iterator iterator;

  // Retrieve the VeloClusters2MCHits table
  const Table* table = m_pV2MCHit->direct();
  if (0==table){
    return Error( "Could not retrieve VeloCluster2MCHit table" );
  }

  // create an association table 
  VeloCluster2MCParticleAsct::Table* aTable = 
    new VeloCluster2MCParticleAsct::Table(); 

  // loop and link VeloClusters to MC truth
  VeloClusters::const_iterator iterClus;
  for(iterClus = m_clusters->begin(); iterClus != m_clusters->end(); iterClus++){

    Range range1 = table->relations(*iterClus);
    iterator relation;
    for (relation=range1.begin(); relation !=range1.end(); relation++){
      // loop over relations
      MCVeloHit * hit = relation->to ();
      MCParticle * aParticle = hit->mcParticle();
      double charge = relation->weight();
      debug() << "Relation output - particle charge " << charge << endmsg;
      if(0==aParticle){
        warning() << "MCHit did not have an MCParticle parent" << endmsg;
      }else{
        aTable->relate(*iterClus,aParticle,charge);
      }
    }
  } // loop iterClus
  
  // register table in store
  put( aTable, outputData() );
 
  return StatusCode::SUCCESS;
}
