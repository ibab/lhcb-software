// $Id: CellNeighbour.h,v 1.5 2006-03-22 18:25:05 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.4  2005/11/07 11:57:13  odescham
// v5r0 - Adapt to the new Track Event Model
//
// Revision 1.3  2005/05/09 13:48:47  cattanem
// fix some doxygen warnings
//
// Revision 1.2  2001/11/25 15:26:20  ibelyaev
//  update for newer CaloKernel package
//
// Revision 1.1  2001/11/22 16:02:33  ibelyaev
//  new utilities
// 
// ============================================================================
#ifndef CALOUTILS_CELLNEIGHBOUR_H 
#define CALOUTILS_CELLNEIGHBOUR_H 1
// Include files
// CaloUtils
#include "CaloUtils/CellMatrix.h"

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
  CellNeighbour( const DeCalorimeter* Det = 0 );
    
  virtual ~CellNeighbour( ); ///< Destructor
  
  /** the major method. It returns "true" if the cell 
   *  is a neighbor for seed cell  
   *  @exception CaloException if detector information is not available
   *  @param seed  the seed   cell
   *  @param cell  the cell to be tested
   *  @return fraction of area of the second cell which fits inside 
   *          "neighbour" aroud the seed cell, equivalently 0 or 1 
   */
  double operator() 
    ( const LHCb::CaloCellID& seed , 
      const LHCb::CaloCellID& cell ) const ;
};

// ============================================================================
#endif // CALOUTILS_CELLNEIGHBOUR_H
// ============================================================================
