// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/src/component/PreClustersHolder.cpp,v 1.1 2001-02-03 19:49:18 ibelyaev Exp $

// Include files

#include <algorithm>
#include "PreClustersHolder.h"
#include "Clustering/TaggedCellFunctor.h"
#include "LHCbEvent/TopLevel/Definitions.h"

PreClustersHolder::PreClustersHolder( TaggedCellsHolder& taggedCellsBox ) {
  m_preClusters.clear();
  m_preClustersSeq.clear();
  m_taggedCellsBox = 0;  
  if ( &taggedCellsBox ) {
    m_taggedCellsBox = &taggedCellsBox ;

    // Get taggedCells acces sequentiel

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

    // Create precluster data

    std::vector<CelAutoTaggedCell*>::iterator itTagSeed = taggedCells.begin();
    std::vector<CelAutoTaggedCell*>::iterator itTagClustered1;
    std::vector<CelAutoTaggedCell*>::iterator itTagClustered2;
    itTagClustered1 = itTagLastSeed;
    while ( itTagSeed != itTagLastSeed ) {
      CelAutoPreCluster preCluster( (*itTagSeed)->seedForClustered() );
      itTagClustered2 = std:: partition ( itTagClustered1,
                                          itTagLastClustered,
                                          TaggedCellFunctor::isWithSeed( (*itTagSeed)->cellID() ) );
      this->setOwnedCells( itTagClustered1, itTagClustered2, preCluster ); 
      std::vector<CelAutoTaggedCell*>::iterator itTagLastEdge;
      itTagLastEdge = std::partition ( itTagLastClustered,
                                       taggedCells.end(),
                                       TaggedCellFunctor::isWithSeed( (*itTagSeed)->cellID() ) );
      this->setSharedCells( itTagLastClustered, itTagLastEdge, preCluster );
      m_preClusters.push_back( preCluster );
      itTagClustered1 = itTagClustered2;
      itTagSeed++;
    }

    // Create sequentiel access for precluster data
   
    this->createSeqAccess();
  }
}

// Create seqentiel access for precluster data

void PreClustersHolder::createSeqAccess() {
  std::vector<CelAutoPreCluster>::iterator itPreClusters = m_preClusters.begin();
  int index = 0;
  while ( itPreClusters != m_preClusters.end() ) {
    m_preClustersSeq.push_back( &m_preClusters[ index++ ] );
    itPreClusters++;
  }
}

// Set owned cells

void PreClustersHolder::setOwnedCells( std::vector<CelAutoTaggedCell*>::iterator itTagFirstInt,
                                       std::vector<CelAutoTaggedCell*>::iterator itTagLastInt,
                                       CelAutoPreCluster& preCluster ) {
  while ( itTagFirstInt != itTagLastInt ) {
    preCluster.addOwnedMember( (*itTagFirstInt)->cellID() );
    itTagFirstInt++;
  }
  preCluster.setNumberOwnedCells();
}

// Set sharedcells

void PreClustersHolder::setSharedCells( std::vector<CelAutoTaggedCell*>::iterator itTagFirstInt,
                                        std::vector<CelAutoTaggedCell*>::iterator itTagLastInt,
                                        CelAutoPreCluster& preCluster ) {
  while ( itTagFirstInt != itTagLastInt ) {
    preCluster.addSharedMember( (*itTagFirstInt)->cellID() );
    itTagFirstInt++;
  }
  preCluster.setNumberOwnedCells();
}

// Print preclusters

void PreClustersHolder::printCelAutoPreClusters() {
  std::cout << "- List of preclusters        -----------------------" << std::endl;
  std::cout << "| Number of preclusters = ";
  std::cout << LHCbEventField( LHCbEvent::field12 );  
  std::cout << m_preClustersSeq.size() << std::endl;
  std::vector<CelAutoPreCluster*>::iterator itPreClusters = m_preClustersSeq.begin();
  while ( itPreClusters != m_preClustersSeq.end() ) {
    this->printCelAuto1PreCluster( itPreClusters );
    itPreClusters++;
  }
  std::cout << "----------------------------------------------------" << std::endl;
}

// Print one precluster

void PreClustersHolder::printCelAuto1PreCluster( CelAutoPreCluster** preCluster ) {
  std::cout << "| CellIdPreCluster = ";
  std::cout << LHCbEventField( LHCbEvent::field12 );
  std::cout << (CaloCellID::ContentType) (*preCluster)->seed() << std::endl;
  std::cout << "| ownedCell(s)  = " << "this" << std::endl;
  std::string tabHeaderLignWithWhite = "|                 ";
  std::vector<CaloCellID> ownedCells = (*preCluster)->ownedCells();
  m_taggedCellsBox->printCaloCellIDCells( ownedCells, tabHeaderLignWithWhite );
  std::cout << "| sharedCell(s) = " << "this" <<  std::endl;
  tabHeaderLignWithWhite = "|                 ";
  std::vector<CaloCellID> sharedCells = (*preCluster)->sharedCells();
  m_taggedCellsBox->printCaloCellIDCells( sharedCells, tabHeaderLignWithWhite );
}


















