// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/src/component/DetecHolder.cpp,v 1.1 2001-02-03 19:49:17 ibelyaev Exp $

// Include files
#include <functional>
#include <algorithm>
#include "DetecHolder.h"
#include "Calo/CaloException.h"
#include "Gaudi/DataSvc/SmartDataPtr.h"
#include "LHCbEvent/TopLevel/Definitions.h"

// Constructor

DetecHolder::DetecHolder( IDataProviderSvc* dataService,
                          const std::string& inputPath,
                          const std::string& calorimeter ) {

  /// check for data service

  if ( 0 == dataService ) {
    throw CaloException ( "DetecHolder::DetecHolder:: IDataProviderSvc* points to NULL" ); 
  }

  /// check calorimeter

  if ( inputPath.find( calorimeter ) == string::npos ) {
    throw CaloException ( "DetecHolder::DetecHolder:: InputPath geom is not for this calorimeter" );
  }

  /// Retrieve calorimeter detector

  SmartDataPtr<DeCalorimeter> smartCaloDetector ( dataService,
                                                  inputPath );
  if ( ! smartCaloDetector ) {
    throw CaloException( "DetecHolder::DetecHolder:: Unable to retrieve "+inputPath ); }
  m_detector = (DeCalorimeter*) smartCaloDetector;
  m_detector->buildCells();
  m_calorimeter = calorimeter;  
}
   
// Print the neighbors of one cell shape CaloCellID

void DetecHolder::printCaloCellIDNeighbors1Cell( const CaloCellID& ID,
                                                  bool printWithCell ) {
  if ( printWithCell ) {
    std::cout << "- List of neighbors shape CaloCellID of one cell ------" << std::endl;
    std::cout << "| CellIdCell      = " ;
    std::cout << LHCbEventField( LHCbEvent::field12 );
    std::cout << (CaloCellID::ContentType) ID << std::endl;
  }
  std::vector<CaloCellID> neighborsCells = m_detector->neighborCells( ID );
  std::cout << "| Number of neighbors = ";
  std::cout << LHCbEventField( LHCbEvent::field12 );
  std::cout << neighborsCells.size() << std::endl;
  std::vector<CaloCellID>::iterator itNeighbors = neighborsCells.begin();
  while ( itNeighbors != neighborsCells.end() ) {
    std::cout << "| CellIdNeighBor = ";
    std::cout << LHCbEventField( LHCbEvent::field12 );
    std::cout << (CaloCellID::ContentType) (*itNeighbors) << std::endl;
    itNeighbors++;
  }
  if ( printWithCell ) {
    std::cout << "-------------------------------------------------------" << std::endl;
  }
}






