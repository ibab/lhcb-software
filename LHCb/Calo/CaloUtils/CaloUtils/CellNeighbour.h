// $Id: CellNeighbour.h,v 1.1 2001-11-22 16:02:33 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef CALOUTILS_CELLNEIGHBOUR_H 
#define CALOUTILS_CELLNEIGHBOUR_H 1
// Include files
// CaloGen 
#include "CaloGen/CaloException.h"
// CaloUtils
#include "CaloUtils/CellMatrix.h"
// CaloDet 
#include "CaloDet/DeCalorimeter.h"

/** @class CellNeighbour CellNeighbour.h CaloUtils/CellNeighbour.h
 *  
 *  The simple utility class to determine if 2 cells are neignbours 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   22/11/2001
 */

class CellNeighbour 
  : public CellMatrix
{
public:

  /** the standard constructor
   *  @param Det pointer to calorimeter detector 
   */
  CellNeighbour( const DeCalorimeter* Det = 0 )
    : CellMatrix( Det ){};
  
  virtual ~CellNeighbour( ); ///< Destructor
  
  /** the major method. It returns "true" if the cell 
   *  is a neighbor for seed cell  
   *  @exeption CaloException if detector information is not available
   *  @param seed  the seed   cell
   *  @param cell  the cell to be tested
   *  @return fraction of area of the second cell which fits inside 
   *          "neighbour" aroud the seed cell, equivalently 0 or 1 
   */
  inline double operator() 
    ( const CaloCellID& seed , 
      const CaloCellID& cell ) const
  {
    /// trivial case 
    if( seed == cell                                   ) { return 1 ; } 
    if( 0 == det() ) { Exception("CellNeighbour:: Detector points to NULL!");}
    ///
    if( !det()->valid( seed ) || !det()->valid( cell ) ) { return 0 ; }
    /// 
    const CaloNeighbors& neighbours = det()->neighborCells( seed ) ;
    CaloNeighbors::const_iterator it = 
      std::find( neighbours.begin () , neighbours.end () , cell ) ;
    ///
    return neighbours.end() == it ? 0.0 : 1.0 ;
  };
  
};

// ============================================================================
// The End 
// ============================================================================
#endif // CALOUTILS_CELLNEIGHBOUR_H
// ============================================================================
