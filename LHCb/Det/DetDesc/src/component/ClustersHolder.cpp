// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/src/component/ClustersHolder.cpp,v 1.1 2001-02-03 19:49:17 ibelyaev Exp $

// Include files

#include <algorithm>
#include <numeric>
#include "ClustersHolder.h"
#include "DetecHolder.h"
#include "Clustering/TaggedCellFunctor.h"
#include "CaloEvent/CaloDigit.h"
#include "CaloEvent/CaloDataFunctor.h"
#include "LHCbEvent/TopLevel/Definitions.h"
#include "CLHEP/Units/SystemOfUnits.h"

ClustersHolder::ClustersHolder( PreClustersHolder& preClustersBox,
                                IDataProviderSvc* dataService,
                                const std::string& outputPath ) {
  m_clustersSeq = this->createOutput( dataService,
  				      outputPath );
  if ( &preClustersBox ) {
    m_taggedCellsBox = &preClustersBox.taggedCellsBox();
    HitsHolder& hitsBox = preClustersBox.taggedCellsBox().hitsBox();

    // Create cluster data and store in output

    std::vector<CelAutoPreCluster*> preClusters = preClustersBox.preClustersSeq();
    std::vector<CelAutoPreCluster*>::iterator itPreCluster = preClusters.begin();
    while ( itPreCluster != preClusters.end() ) {
      CelAutoCluster* cluster = new CelAutoCluster( hitsBox.hitsDirect()[ (*itPreCluster)->seed() ] );
      std::vector<CaloCellID> ownedCells = (*itPreCluster)->ownedCells();
      this->setOwnedHits( ownedCells, hitsBox, *cluster);
      std::vector<CaloCellID> sharedCells = (*itPreCluster)->sharedCells();
      this->setSharedHits( sharedCells, hitsBox, *cluster);
      this->setEnergyBarycenter( *cluster );
      m_clustersSeq->push_back( cluster );
      itPreCluster++;
    }
  }

}

ClustersHolder::ClustersHolder( TaggedCellsHolder& taggedCellsBox,
                                IDataProviderSvc* dataService,
                                const std::string& outputPath ) {
  m_clustersSeq = this->createOutput( dataService,
                                      outputPath ); 
  if ( &taggedCellsBox ) {
    m_taggedCellsBox = &taggedCellsBox;
    HitsHolder& hitsBox = taggedCellsBox.hitsBox();
   
    // Get sequentiel access of tagged cells data

    std::vector<CelAutoTaggedCell*> taggedCells = taggedCellsBox.taggedCellsSeq();

    // Partionne taggedCells (seeds -> another clustered -> shared)

    std::vector<CelAutoTaggedCell*>::iterator itTagLastSeed;
    itTagLastSeed  = std::partition( taggedCells.begin(),
                                     taggedCells.end(),
                                     TaggedCellFunctor::isSeed() );
    std::vector<CelAutoTaggedCell*>::iterator itTagLastClustered;
    itTagLastClustered = std::partition( itTagLastSeed,
                                         taggedCells.end(),
                                         TaggedCellFunctor::isClustered() );  

    // Create cluster data and stor in output
 
    std::vector<CelAutoTaggedCell*>::iterator itTagSeed = taggedCells.begin();
    std::vector<CelAutoTaggedCell*>::iterator itTagClustered1;
    std::vector<CelAutoTaggedCell*>::iterator itTagClustered2;
    itTagClustered1 = itTagLastSeed;
    while ( itTagSeed != itTagLastSeed ) {
      CelAutoCluster* cluster = new CelAutoCluster ( hitsBox.hitsDirect()[ (*itTagSeed)->cellID() ] );
      itTagClustered2 = std::partition ( itTagClustered1,
                                         itTagLastClustered,
                                         TaggedCellFunctor::isWithSeed( (*itTagSeed)->cellID() ) );
      this->setOwnedHits( itTagClustered1, itTagClustered2, hitsBox, *cluster );
      std::vector<CelAutoTaggedCell*>::iterator itTagLastEdge;
      itTagLastEdge = std::partition ( itTagLastClustered,
                                       taggedCells.end(),
                                       TaggedCellFunctor::isWithSeed( (*itTagSeed)->cellID() ) );
      this->setSharedHits( itTagLastClustered, itTagLastEdge, hitsBox, *cluster ); 
      this->setEnergyBarycenter( *cluster );
      m_clustersSeq->push_back( cluster );
      itTagClustered1 = itTagClustered2;
      itTagSeed++;
    }
  }
}

// Create output of clusters 

CelAutoClusterVector* ClustersHolder::createOutput( IDataProviderSvc* dataService,
                                                    const std::string& outputPath ) {

  // check for data servic

  if ( 0 == dataService ) {
    throw CaloException ( "ClustersHolder::ClustersHolder::createOutput:: IDataProviderSvc* points to NULL" );
  }

  // Create the container 
  
  CelAutoClusterVector* output = new (std::nothrow) CelAutoClusterVector();
  if ( 0 == output ) {
    throw CaloException ( "ClustersHolder::ClustersHolder::createOutput:: Unable to create the output clusters container");
  }
 
  // Registering of  clusters container object into the event data store
  
  StatusCode sc = dataService->registerObject( outputPath, output );
  if ( sc.isFailure() ) {
    delete output;
    output = 0;
    throw CaloException ( "ClustersHolder::ClustersHolder::createOutput:: Unable tp register clusters container" ); 
  } 
  return output;
}  


// Set owned hits 

void ClustersHolder::setOwnedHits( std::vector<CaloCellID>& cells,
                                   HitsHolder& hitsBox, 
                                   CelAutoCluster& cluster ) {
  std::vector<CaloCellID>::iterator itCells = cells.begin();
  itCells++;
  while ( itCells != cells.end() ) {
    cluster.addOwnedMember( hitsBox.hitsDirect()[ ( *itCells ) ] );
    itCells++;
  }
  cluster.setNumberOwnedHits();
}  

// Set owned hits
 
void ClustersHolder::setOwnedHits( std::vector<CelAutoTaggedCell*>::iterator itTagFirstInt,
                                   std::vector<CelAutoTaggedCell*>::iterator itTagLastInt,
                                   HitsHolder& hitsBox,    
                                   CelAutoCluster& cluster ) {
  while ( itTagFirstInt != itTagLastInt ) {
    cluster.addOwnedMember( hitsBox.hitsDirect()[ (*itTagFirstInt)->cellID() ] );
    itTagFirstInt++;
  }
  cluster.setNumberOwnedHits();
}

// Set shared hits 

void ClustersHolder::setSharedHits( std::vector<CaloCellID>& cells,
                                    HitsHolder& hitsBox, 
                                    CelAutoCluster& cluster ) {
  std::vector<CaloCellID>::iterator itCells = cells.begin();
  while ( itCells != cells.end() ) {
    cluster.addSharedMember( hitsBox.hitsDirect()[ ( *itCells ) ] );
    itCells++;
  }
  cluster.setNumberSharedHits();
}  

// Set shared hits

void ClustersHolder::setSharedHits( std::vector<CelAutoTaggedCell*>::iterator itTagFirstInt,
                                        std::vector<CelAutoTaggedCell*>::iterator itTagLastInt,
                                        HitsHolder& hitsBox,
                                        CelAutoCluster& cluster ) {
  while ( itTagFirstInt != itTagLastInt ) {
    cluster.addSharedMember( hitsBox.hitsDirect()[ (*itTagFirstInt)->cellID() ] );
    itTagFirstInt++;
  }
  cluster.setNumberSharedHits();
}

// Set energy and barycenter

void ClustersHolder::setEnergyBarycenter( CelAutoCluster& cluster ) {
  double sum_Energy = 0.0;
  double sum_XEnergy = 0.0; 
  SmartRefVector<CaloDigit>& ownedHits = cluster.ownedHits();
  SmartRefVector<CaloDigit>::iterator itHits = ownedHits.begin();
  while ( itHits != ownedHits.end() ) {
    DetecHolder& geomBox = m_taggedCellsBox->geomBox();
    sum_XEnergy += ( geomBox.detector()->cellX ( (*itHits)->cellID() ) ) * (*itHits)->e();
    sum_Energy += (*itHits)->e();
    itHits++;
  }
  double barycenter = sum_XEnergy / sum_Energy ;
  cluster.setEnergy( sum_Energy );
  cluster.setBarycenter( barycenter );
} 

// Print clusters

void ClustersHolder::printCelAutoClusters() {
  std::cout << "- List of clusters        ---------------------------------------------------" << std::endl;
  std::cout << "| Number of clusters = ";
  std::cout << LHCbEventField( LHCbEvent::field12 );
  std::cout << m_clustersSeq->size() << std::endl;
  std::vector<CelAutoCluster*>::iterator itClusters = m_clustersSeq->begin();
  while ( itClusters != m_clustersSeq->end() ) {
    this->printCelAuto1Cluster( itClusters );
    itClusters++;
  }
  std::cout << "-----------------------------------------------------------------------------" << std::endl;
}

// Print one cluster

void ClustersHolder::printCelAuto1Cluster( CelAutoCluster** Cluster ) {
  std::cout << "| CellIdCluster = ";
  std::cout << LHCbEventField( LHCbEvent::field12 );
  std::cout << (CaloCellID::ContentType) (*Cluster)->center()->cellID() << std::endl;
  std::cout << "| ownedHit(s)  = " << "this" << std::endl;
  std::string tabHeaderLignWithWhite = "|                ";
  SmartRefVector<CaloDigit> ownedHits = (*Cluster)->ownedHits();
  m_taggedCellsBox->printCaloDigitHits( ownedHits, tabHeaderLignWithWhite );
  std::cout << "| sharedHit(s) = " << "this" << std::endl;
  tabHeaderLignWithWhite = "|                ";
  SmartRefVector<CaloDigit> sharedHits = (*Cluster)->sharedHits();
  m_taggedCellsBox->printCaloDigitHits( sharedHits, tabHeaderLignWithWhite );
  std::cout << "| Energy     = ";
  std::cout << LHCbEventFloatFormat( LHCbEvent::width, LHCbEvent::precision );
  std::cout << (*Cluster)->energy() / MeV << "[MeV]" << std::endl;
  std::cout << "| Barycenter = ";
  std::cout << LHCbEventFloatFormat( LHCbEvent::width, LHCbEvent::precision );
  std::cout << (*Cluster)->barycenter() / mm << "[mm]" << std::endl;
}















