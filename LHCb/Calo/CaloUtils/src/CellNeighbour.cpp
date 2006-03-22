// $Id: CellNeighbour.cpp,v 1.4 2006-03-22 18:25:06 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2005/11/07 11:57:13  odescham
// v5r0 - Adapt to the new Track Event Model
//
// Revision 1.2  2004/10/22 09:29:08  ibelyaev
//  bug fix in the evaluation of energy sharing fractions
//
// Revision 1.1  2001/11/25 15:26:21  ibelyaev
//  update for newer CaloKernel package
// 
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
  : CellMatrix( Det ){};

// ============================================================================
/** Destructor 
 */
// ============================================================================
CellNeighbour::~CellNeighbour(){}; 

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
};

