// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/src/component/HitsHolder.cpp,v 1.1 2001-02-03 19:49:17 ibelyaev Exp $

// Include files
#include <functional>
#include <algorithm>
#include "HitsHolder.h"
#include "DetecHolder.h"
#include "CaloEvent/CaloCellID.h"
#include "Calo/CaloData.h"
#include "Gaudi/Kernel/StatusCode.h"
#include "Gaudi/DataSvc/SmartDataPtr.h"
#include "LHCbEvent/TopLevel/Definitions.h"
#include "CLHEP/Units/SystemOfUnits.h"

// Gets once per event the data needed for clusterization

HitsHolder::HitsHolder ( IDataProviderSvc* dataService,
                         const std::string& inputPath,
                         const std::string& calorimeter,
                         IMessageSvc* messageService = 0 ) {  

  /// check for data service
  
  if ( 0 == dataService ) { 
    throw CaloException ( "HitsHolder::HitsHolder:: IDataProviderSvc* points to NULL" );
  }

  /// check calorimeter

  if ( inputPath.find( calorimeter ) == string::npos ) {
    throw CaloException ( "HitsHolder::HitsHolder:: inputPath hits is not for this calorimeter" );
  }

  /// retrieve data
           
  CaloData::Digits( dataService,
                    inputPath,
                    std::back_inserter( m_hits ),
                    messageService ) ;
  m_calorimeter = calorimeter;
  this->hitsDirect();
}

// Get the digits for direct access

const std::vector<CaloDigit*>& HitsHolder::hitsDirect() { 
  m_hitsDirect.clear();
  std::vector<CaloDigit*>::iterator itDigits = m_hits.begin();
  while ( itDigits != m_hits.end() ) {
    CaloCellID::ContentType index = (CaloCellID::ContentType) (*itDigits)->cellID(); 
    while ( m_hitsDirect.size() <= index )
       m_hitsDirect.push_back( (CaloDigit*) 0 );
    m_hitsDirect[index] = (*itDigits);
    itDigits++;
  }
  return m_hitsDirect; 
}

// IsLocMax 

bool HitsHolder::isLocMax( CaloDigit** hit,
                           DetecHolder& geomBox ) {
  if ( m_calorimeter != geomBox.calorimeter() ) {
    throw CaloException ( "HitsHolder::isLocMax calorimeter for the hits is not the calorimeter of the geometry" );
  }
  std::vector<CaloCellID> neighborsCellID = geomBox.detector()->neighborCells( (*hit)->cellID() );   
  std::vector<CaloCellID>::iterator itNeighborsCellID = neighborsCellID.begin();
  while ( itNeighborsCellID != neighborsCellID.end() ) { 
  if ( (*itNeighborsCellID).index() < m_hitsDirect.size() ) 
    if (m_hitsDirect[(*itNeighborsCellID).index()] ) 
      if ( m_hitsDirect[(*itNeighborsCellID).index()]->e() > (*hit)->e() ) break;
        itNeighborsCellID++;
  }

  //If true, this hit is one seed
 
  return( itNeighborsCellID == neighborsCellID.end() ? true : false );
}

// Print hits shape CaloDigit 

void HitsHolder::printCaloDigitHits() {
  std::cout << "- List of hits shape CaloDigit -----------------------------" << std::endl;
  std::cout << "| Number of hits = ";
  std::cout << LHCbEventField( LHCbEvent::field12 );
  std::cout << m_hits.size() << std::endl;
  std::vector<CaloDigit*>::iterator itHits = m_hits.begin();
  while ( itHits != m_hits.end() ) {
    this->printCaloDigit1Hit( itHits );
    itHits++;
  }
  std::cout << "------------------------------------------------------------" << std::endl;
}

// Print one hit shape CaloDigit

void HitsHolder::printCaloDigit1Hit( CaloDigit** hit ) {
  std::cout << "| CellIdHit = ";
  std::cout << LHCbEventField( LHCbEvent::field12 );
  std::cout << (CaloCellID::ContentType) (*hit)->cellID();
  std::cout << " Energy = ";
  std::cout << LHCbEventFloatFormat( LHCbEvent::width, LHCbEvent::precision
);
  std::cout << (*hit)->e() / MeV << "[MeV]" << std::endl;
}

// Print the neighbors of all hits shape CaloCellID

void HitsHolder::printCaloCellIDNeighborsHits( DetecHolder& geomBox ) {
  std::cout << "- List of neighbors of all hits shape CaloCellID ------" << std::endl;
  std::cout << " | Number of hits = ";
  std::cout << LHCbEventField( LHCbEvent::field12 );
  std::cout << m_hits.size() << std::endl;
  if ( m_calorimeter == geomBox.calorimeter() ) {
    std::vector<CaloDigit*>::iterator itHits = m_hits.begin();
    while ( itHits != m_hits.end() ) {
      std::cout << "| CellIdHit       = " ;
      std::cout << LHCbEventField( LHCbEvent::field12 );
      std::cout << (CaloCellID::ContentType) (*itHits)->cellID() << std::endl;
      geomBox.printCaloCellIDNeighbors1Cell( (*itHits)->cellID(), false );
      std::cout << "-------------------------------------------------------" << std::endl;
      itHits++;
    }
  }
  else {
    std::cout << "| Error = Calorimeter for the hits is not the" << std::endl;
    std::cout << "|         calorimeter of the geometry" << std::endl;
    std::cout << "-------------------------------------------------------" << std::endl;
  }
}

// Print the neighbors of all hits shape CaloDigit

void HitsHolder::printCaloDigitNeighborsHits( DetecHolder& geomBox ) {
  std::cout << "- List of neighbors of all hits shape CaloDigit  -----------" << std::endl;
  std::cout << "| Number of hits = ";
  std::cout << LHCbEventField( LHCbEvent::field12 );;
  std::cout << m_hits.size() << std::endl;
  if ( m_calorimeter == geomBox.calorimeter() ) {
    std::vector<CaloDigit*>::iterator itHits = m_hits.begin();
    while ( itHits != m_hits.end() ) {
      this->printCaloDigit1Hit( itHits );
      this->printCaloDigitNeighbors1Hit( itHits, geomBox, false );
      std::cout << "------------------------------------------------------------" << std::endl;
      itHits++;
    }
  }
  else {
    std::cout << "| Error = Calorimeter for the hits is not the" << std::endl;
    std::cout << "|         calorimeter of the geometry" << std::endl;
    std::cout << "------------------------------------------------------------" << std::endl;
  }
}

// Print the neighbors of one hit shape CaloDigit

void HitsHolder::printCaloDigitNeighbors1Hit( CaloDigit** hit,
                                              DetecHolder& geomBox,
                                              bool printWithHit ) {
  if ( printWithHit ) {  
    std::cout << "- List of neighbors shape CaloDigit of one hit -------------" << std::endl;
    this->printCaloDigit1Hit( hit );
  } 
  if ( m_calorimeter == geomBox.calorimeter() ) {
    std::vector<CaloCellID> neighbors = geomBox.detector()->neighborCells( (*hit)->cellID() ); 
    std::cout << "| Number of neighbors = ";
    std::cout << LHCbEventField( LHCbEvent::field12 );
    std::cout <<  neighbors.size() << std::endl;
    std::vector<CaloCellID>::iterator itNeighbors = neighbors.begin();
    while ( itNeighbors != neighbors.end() ) {
      std::cout << "| CellIdNeighbor = ";
      std::cout << LHCbEventField( LHCbEvent::field12 );
      std::cout << (CaloCellID::ContentType) (*itNeighbors);
      if ( (*itNeighbors).index() < m_hitsDirect.size() ) {
        if ( m_hitsDirect[(*itNeighbors).index()] ) {
          std::cout << " Energy = ";
          std::cout << LHCbEventFloatFormat( LHCbEvent::width, LHCbEvent::precision);
          std::cout << m_hitsDirect[(*itNeighbors).index()]->e() / MeV << "[MeV]";
        }
        else 
          std::cout << " No hit with";
      }
      else
        std::cout << " No hit with";
      std::cout << std::endl;
      itNeighbors++;
    }
  }
  else {
      std::cout << "| Error = Calorimeter for the hits is not the" << std::endl;
      std::cout << ":         calorimeter of the geometry" << std::endl;
    }
  if ( printWithHit )
    std::cout << "------------------------------------------------------------" << std::endl;
}

                           























