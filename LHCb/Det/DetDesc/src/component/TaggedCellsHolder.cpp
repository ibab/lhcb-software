// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/src/component/TaggedCellsHolder.cpp,v 1.1 2001-02-03 19:49:18 ibelyaev Exp $

// Include files

#include <algorithm>
#include "TaggedCellsHolder.h"
#include "Clustering/TaggedCellFunctor.h"
#include "Calo/CaloException.h"
#include "LHCbEvent/TopLevel/Definitions.h"
#include "CLHEP/Units/SystemOfUnits.h"

TaggedCellsHolder::TaggedCellsHolder ( SeedsHolder& seedsBox,
                                       bool infoStepByStep = false ) {
 
  m_taggedCells.clear();
  m_taggedCellsSeq.clear();
  m_taggedCellsDirect.clear();
  m_numberNotTaggedCells = 0;
  m_infoStepByStep = infoStepByStep;
  m_hitsBox = 0;
  m_geomBox = 0;
  if ( &seedsBox ) {
    HitsHolder& hitsBox = seedsBox.hitsBox();
    DetecHolder& geomBox = seedsBox.geomBox();
    this->createData ( hitsBox );
    this->setSeedsSeedsFind(seedsBox );
    this->evoleTagging( geomBox );
  }
}

TaggedCellsHolder::TaggedCellsHolder ( HitsHolder& hitsBox,
                                       DetecHolder& geomBox,
                                       bool infoStepByStep = false ) {
  m_taggedCells.clear();
  m_taggedCellsSeq.clear();
  m_taggedCellsDirect.clear();
  m_numberNotTaggedCells = 0;
  m_infoStepByStep = infoStepByStep;
  m_hitsBox = 0; 
 if ( &hitsBox && &geomBox ) {
    if ( hitsBox.calorimeter() != geomBox.calorimeter() ) {
      throw CaloException ( "TaggedCellsHolder::TaggedCellsHolder calorimeter for the hits must be calorimeter of the geometry" );
    }
    this->createData ( hitsBox );
    this->setSeedsSeedsNotFind( hitsBox, geomBox );
    this->evoleTagging( geomBox );
  }
}

// Create CaloTaggedCell data 

void TaggedCellsHolder::createData( HitsHolder& hitsBox ) {
  m_hitsBox = &hitsBox; 
 
  // Get sequentiel access for hits

  std::vector<CaloDigit*> digits = hitsBox.hitsSeq();

  // Create tagged cells data

  std::vector<CaloDigit*>::iterator itDigits = digits.begin();
  while ( itDigits != digits.end() ) {
    CelAutoTaggedCell taggedCell( (*itDigits)->cellID(), (*itDigits)->e() );
    m_taggedCells.push_back( taggedCell );
    itDigits++;
  }
  // Create sequentiel access for tagged cells data

  this->createSeqAccess();

  // Create direct access from seq access for tagged cells data

  this->createDirectAccess( m_taggedCellsSeq.begin(),
                            m_taggedCellsSeq.end(),
                            m_taggedCellsDirect );
}

// Create seqentiel access for tagged cells data

void TaggedCellsHolder::createSeqAccess() {
  std::vector<CelAutoTaggedCell>::iterator itTaggeds = m_taggedCells.begin();
  int index = 0;
  while ( itTaggeds != m_taggedCells.end() ) {
    m_taggedCellsSeq.push_back( &m_taggedCells[ index++ ] );
    itTaggeds++;
  }
}

// Create direct access from seq access

void TaggedCellsHolder::createDirectAccess( std::vector<CelAutoTaggedCell*>::iterator itSeqInFirst,
                                            std::vector<CelAutoTaggedCell*>::iterator itSeqInLast,
                                            std::vector<CelAutoTaggedCell*>& bufDirectOut ) { 
  std::vector<CelAutoTaggedCell*>::iterator itSeqIn = itSeqInFirst;
  while ( itSeqIn != itSeqInLast ) {
    CaloCellID::ContentType index = (CaloCellID::ContentType) (*itSeqIn)->cellID();
    while ( bufDirectOut.size() <= index )
      bufDirectOut.push_back( (CelAutoTaggedCell*) 0 );
    bufDirectOut[index] = (*itSeqIn);
    itSeqIn++;
  }
}

// Tag seeds with having seeds

void TaggedCellsHolder::setSeedsSeedsFind( SeedsHolder& seedsBox ) {
  std::vector<CaloCellID> seeds = seedsBox.seeds();  
  std::vector<CaloCellID>::iterator itSeeds = seeds.begin();
  while ( ( itSeeds != seeds.end() ) ) {
    m_taggedCellsDirect[ (CaloCellID::ContentType) (*itSeeds) ]->addSeed( (*itSeeds) ); 
    m_taggedCellsDirect[ (CaloCellID::ContentType) (*itSeeds) ]->setClustered( true ); 
    itSeeds++;
  }
}

// Tag seeds without having seeds

void TaggedCellsHolder::setSeedsSeedsNotFind( HitsHolder& hitsBox,
                                              DetecHolder& geomBox ) {
  std::vector<CaloDigit*> digits = hitsBox.hitsSeq();
  std::vector<CaloDigit*>::iterator itDigits = digits.begin();
  while ( itDigits != digits.end() ) {
    if ( hitsBox.isLocMax( itDigits, geomBox ) ){
      m_taggedCellsDirect[ (*itDigits)->cellID() ]->addSeed( (*itDigits)->cellID() );
      m_taggedCellsDirect[ (CaloCellID::ContentType) (*itDigits)->cellID() ]->setClustered( true ); 
    }
    itDigits++;
  }
}

// Evolution of tagging

void TaggedCellsHolder::evoleTagging ( DetecHolder& geomBox ) {
  std::vector<CelAutoTaggedCell*>::iterator itTagLastClustered;
  itTagLastClustered = std::partition( m_taggedCellsSeq.begin(),
                                       m_taggedCellsSeq.end(),
                                       TaggedCellFunctor::isClustered() );     
  std::vector<CelAutoTaggedCell*>::iterator itTagFirst = itTagLastClustered; 
  while ( itTagLastClustered != m_taggedCellsSeq.end() ) {    
    this->oneEvoleTagging( itTagLastClustered,
                           m_taggedCellsSeq.end(),
                           geomBox );
    itTagLastClustered = std::partition( itTagFirst,
                                         m_taggedCellsSeq.end(),
                                         TaggedCellFunctor::isClusteredOrEdge() );     
    if ( itTagLastClustered == itTagFirst ) {
      m_numberNotTaggedCells = m_taggedCellsSeq.end() - itTagLastClustered;
      itTagLastClustered = m_taggedCellsSeq.end();
    }
    itTagFirst = itTagLastClustered;
    if ( m_infoStepByStep ) { 
      std::cout << "Result of one evolution of tagging" << std::endl;
      this->printCelAutoTaggedCellCells();
    }
  }
}

// One evolution of tagging

void TaggedCellsHolder::oneEvoleTagging ( std::vector<CelAutoTaggedCell*>::iterator itTagFirst,
                                          std::vector<CelAutoTaggedCell*>::iterator itTagLast,
                                          DetecHolder& geomBox   ) {
  m_geomBox = &geomBox;
  std::vector<CelAutoTaggedCell*> bufNotTaggedInDirect;
  this->createDirectAccess( itTagFirst,
                            itTagLast,
                            bufNotTaggedInDirect );
  std::vector<CelAutoTaggedCell*>::iterator itTag = itTagFirst;
  while ( itTag != itTagLast ) {
    this->appliRulesTagger( (*itTag), bufNotTaggedInDirect, geomBox ); 
    if ( m_infoStepByStep ) {
      std::cout << "tag cell " << (CaloCellID::ContentType) (*itTag)->cellID() << "----------" << std::endl;
      this->printCelAutoTaggedCellCells();
    }
    itTag++;
  }
}

// Application of rules of tagging on one cell
//   - No action if no clustered neighbor 
//   - Clustered if only one clustered neighbor
//   - Edge if more then one clustered neighbor
// Edge cell with neighbors cells only edge remain not tagged 

void TaggedCellsHolder::appliRulesTagger( CelAutoTaggedCell*& taggedCell,
                                          std::vector<CelAutoTaggedCell*>& bufNotTaggedDirect,
                                          DetecHolder& geomBox ) {
  std::vector<CaloCellID> taggedNeighbors;
  std::vector<CaloCellID> neighborsCells = geomBox.detector()->neighborCells( taggedCell->cellID() );
  this->findTaggedNeighbors( neighborsCells, bufNotTaggedDirect, taggedNeighbors );
  this->findNeighborsSeeds( taggedNeighbors, taggedCell );
  switch ( taggedCell->numberSeeds() ) {
    case 0: 
      break;
    case 1: 
      taggedCell->setClustered( true );
      break;
    default: 
      taggedCell->setEdge( true );
      break;
  } 
}

// Sort neighbors cells tagged before

void TaggedCellsHolder::findTaggedNeighbors( std::vector<CaloCellID>& neighborsCells,
                                             std::vector<CelAutoTaggedCell*>& bufNotTaggedDirect,
                                             std::vector<CaloCellID>& taggedNeighbors ) {
  taggedNeighbors.clear();
  std::vector<CaloCellID>::iterator itNeighbors = neighborsCells.begin();
  while ( itNeighbors != neighborsCells.end() ) {
    bool taggedBefore = true;
    if ( ( (*itNeighbors).index() < bufNotTaggedDirect.size() ) &&
         ( bufNotTaggedDirect[ (*itNeighbors).index() ] ) ) { 
      taggedBefore = false;
    }
    if ( taggedBefore ) {
      taggedNeighbors.push_back( (*itNeighbors) );
    }
    itNeighbors++;
  }
}

// Sort neighbors seeds

void TaggedCellsHolder::findNeighborsSeeds( std::vector<CaloCellID>& neighborsCells,
                                            CelAutoTaggedCell*& taggedCell ) {
  std::vector<CaloCellID> clusteredNeighbors;
  std::vector<CaloCellID>::iterator itNeighbors = neighborsCells.begin();
  while ( itNeighbors != neighborsCells.end() ) {
    if ( ( (*itNeighbors).index() < m_taggedCellsDirect.size() ) && 
         ( m_taggedCellsDirect[ (*itNeighbors).index() ] ) && 
         ( m_taggedCellsDirect[ (*itNeighbors).index() ]->isClustered() ) &&  
         ( ! taggedCell->isWithSeed( m_taggedCellsDirect[ (*itNeighbors).index() ]->seedForClustered() ) ) ) {
     taggedCell->addSeed( m_taggedCellsDirect[ (*itNeighbors).index() ]->seedForClustered() );
    }
    itNeighbors++;
  }
}

// Print tagged cells

void TaggedCellsHolder::printCelAutoTaggedCellCells() {
  std::cout << "- List of tagged cells        -----------------------------------------" << std::endl;
  std::cout << "| Number of tagged cells = ";
  std::cout << LHCbEventField( LHCbEvent::field12 );
  std::cout << m_taggedCellsSeq.size() << std::endl;
  std::vector<CelAutoTaggedCell*>::iterator itCells = m_taggedCellsSeq.begin();
  while ( itCells != m_taggedCellsSeq.end() ) {
    this->printCelAutoTaggedCell1Cell( itCells );
    itCells++;
  }
  std::cout << "-----------------------------------------------------------------------" << std::endl;
}

// Print one tagged cell

void TaggedCellsHolder::printCelAutoTaggedCell1Cell( CelAutoTaggedCell** cell) {
  std::cout << "| CellIdTaggedCell = ";
  std::cout << LHCbEventField( LHCbEvent::field12 );
  std::cout << (CaloCellID::ContentType) (*cell)->cellID();
  std::cout << " Energy = ";
  std::cout << LHCbEventFloatFormat( LHCbEvent::width, LHCbEvent::precision
);
  std::cout << (*cell)->energy() / MeV << "[MeV]" << std::endl;
  std::cout << "| edge = " << (*cell)->isEdge();
  std::cout << "| clustered = " << (*cell)->isClustered();
  std::cout << "| seed(s) = " << "this" << std::endl;
  std::string tabHeaderLignWithWhite = "|                                    ";
  std::vector<CaloCellID> cells = (*cell)->seeds();
  this->printCaloCellIDCells( cells, tabHeaderLignWithWhite );
}

// Print cells shape CaloCellID

void TaggedCellsHolder::printCaloCellIDCells( std::vector<CaloCellID>& cells,
                                              const std::string& tabHeaderLignWithWhite ) {
  std::cout << tabHeaderLignWithWhite + "- List of cells shape CaloCellID -" << std::endl;
  std::cout << tabHeaderLignWithWhite + "| Number of cells = ";
  std::cout << LHCbEventField( LHCbEvent::field12 );
  std::cout << cells.size() << " " << std::endl;
  std::vector<CaloCellID>::iterator itCells = cells.begin();
  while ( itCells  != cells.end() ) {
    std::cout << tabHeaderLignWithWhite + ": CellIdCell      = ";
    std::cout << LHCbEventField( LHCbEvent::field12 );
    std::cout << (CaloCellID::ContentType) (*itCells) << std::endl;
    itCells++;
  }
  std::cout << tabHeaderLignWithWhite + "----------------------------------" << std::endl;
}

// Print hits shape CaloDigit

void TaggedCellsHolder::printCaloDigitHits( SmartRefVector<CaloDigit>& hits,
                                            const std::string& tabHeaderLignWithWhite ) {
  std::cout << tabHeaderLignWithWhite + "- List of hits shape CaloDigit -----------------------------" << std::endl;
  std::cout << tabHeaderLignWithWhite + "| Number of hits = ";
  std::cout << LHCbEventField( LHCbEvent::field12 );
  std::cout << hits.size() << std::endl;
  SmartRefVector<CaloDigit>::iterator itHits = hits.begin();
  while ( itHits  != hits.end() ) {
    std::cout << tabHeaderLignWithWhite + "| CellIdHit      = ";
    std::cout << LHCbEventField( LHCbEvent::field12 );
    std::cout << (CaloCellID::ContentType) (*itHits)->cellID();
    std::cout << " Energy = ";
    std::cout << LHCbEventFloatFormat( LHCbEvent::width, LHCbEvent::precision
);
    std::cout << (*itHits)->e() / MeV << "[MeV]" << std::endl;
    itHits++;
  }
  std::cout << tabHeaderLignWithWhite + "------------------------------------------------------------" << std::endl;
}














