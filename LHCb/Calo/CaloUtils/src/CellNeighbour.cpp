// ============================================================================
// Include files
#include <algorithm>
#include "CaloKernel/CaloException.h"
#include "CaloDet/DeCalorimeter.h"
#include "CaloUtils/CellNeighbour.h"

// ============================================================================
/** @file CellNeighbour.cpp
 *
 *  Implementation file for class : CellNeighbour
 *  
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 25/11/2001 
 */
// ============================================================================

// ============================================================================
/** the standard constructor
 *  @param Det pointer to calorimeter detector 
 */
// ============================================================================
CellNeighbour::CellNeighbour( const DeCalorimeter* Det )
  : CellMatrix( Det ){}

// ============================================================================
/** Destructor 
 */
// ============================================================================
CellNeighbour::~CellNeighbour(){}

// ============================================================================
/** the major method. It returns "true" if the cell 
 *  is a neighbor for seed cell  
 *  @exeption CaloException if detector information is not available
 *  @param seed  the seed   cell
 *  @param cell  the cell to be tested
 *  @return fraction of area of the second cell which fits inside 
 *          "neighbour" aroud the seed cell, equivalently 0 or 1 
 */
// ============================================================================
double CellNeighbour::operator() 
  ( const LHCb::CaloCellID& seed , 
    const LHCb::CaloCellID& cell ) const
{
  /// trivial case 
  if ( seed == cell                                   ) { return 1 ; } 
  if ( 0 == det() ) { Exception("CellNeighbour:: Detector points to NULL!");}
  ///
  if ( !det()->valid( seed ) || !det()->valid( cell ) ) { return 0 ; }
  /// 
  const CaloNeighbors& neighbours = det()->neighborCells( seed ) ;
  CaloNeighbors::const_iterator it = 
    std::find ( neighbours.begin () , neighbours.end () , cell ) ;
  ///
  return neighbours.end() == it ? 0.0 : 1.0 ;
}

