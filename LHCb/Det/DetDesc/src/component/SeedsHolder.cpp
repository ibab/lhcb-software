// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/src/component/SeedsHolder.cpp,v 1.1 2001-02-03 19:49:18 ibelyaev Exp $

// Include files
#include <iostream>
#include "SeedsHolder.h"
#include "Calo/CaloException.h"
#include "LHCbEvent/TopLevel/Definitions.h"
#include "CLHEP/Units/SystemOfUnits.h"

SeedsHolder::SeedsHolder( HitsHolder& hitsBox,
                          DetecHolder& geomBox ) {
  m_seeds.clear();
  if ( &hitsBox && &geomBox ) {
    if ( hitsBox.calorimeter() != geomBox.calorimeter() ) { 
      throw CaloException ( "SeedsHolder::SeedsHolder calorimeter for the hits must be calorimeter of the geometry" );
    }
    std::vector<CaloDigit*> digits = hitsBox.hitsSeq();
    std::vector<CaloDigit*>::iterator itDigits = digits.begin();
    while ( itDigits != digits.end() ) { 
      if ( hitsBox.isLocMax( itDigits, geomBox ) ) { 
       	m_seeds.push_back( (*itDigits)->cellID() );
      }
      itDigits++;
    }
    m_hitsBox = &hitsBox;
    m_geomBox = &geomBox;
  }
  else {
    m_hitsBox = 0;
    m_geomBox = 0;
  }
}

// Print seeds shape CaloCellID

void SeedsHolder::printCaloCellIDSeeds() {
  std::cout << "- List of seeds shape CaloDigit ------------------------" << std::endl;
  std::cout << "| Number of seeds = ";
  std::cout << LHCbEventField( LHCbEvent::field12 );
  std::cout << m_seeds.size() << std::endl;
  std::vector<CaloCellID>::iterator itSeeds = m_seeds.begin();    
  while ( itSeeds  != m_seeds.end() ) {
    std::cout << "| CellIdSeed = ";
    std::cout << LHCbEventField( LHCbEvent::field12 );
    std::cout << (CaloCellID::ContentType) (*itSeeds)<< std::endl;
    itSeeds++;
  }
  std::cout << "--------------------------------------------------------" << std::endl;
}

// Print seeds shape CaloDigit

void SeedsHolder::printCaloDigitSeeds() {
  std::cout << "- List of seeds shape CaloDigit ------------------------" << std::endl;
  std::cout << "| Number of seeds = ";
  std::cout << LHCbEventField( LHCbEvent::field12 );
  std::cout << m_seeds.size() << std::endl;
  std::vector<CaloCellID>::iterator itSeeds = m_seeds.begin();    
  while ( itSeeds  != m_seeds.end() ) {
    std::cout << "| CellIdSeed = ";
    std::cout << LHCbEventField( LHCbEvent::field12 );
    std::cout << (CaloCellID::ContentType) (*itSeeds);
    std::cout << " Energy = ";
    std::cout << LHCbEventFloatFormat( LHCbEvent::width, LHCbEvent::precision );
    std::cout << m_hitsBox->hitsDirect()[ ( *itSeeds ) ]->e() / MeV;
    std::cout << "[MeV]" << std::endl;
    itSeeds++;
  }
  std::cout << "--------------------------------------------------------" << std::endl;
}


