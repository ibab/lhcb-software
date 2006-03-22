// $Id: CellMatrix3x3.h,v 1.7 2006-03-22 18:25:05 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.6  2005/11/07 11:57:13  odescham
// v5r0 - Adapt to the new Track Event Model
//
// Revision 1.5  2005/05/09 13:48:46  cattanem
// fix some doxygen warnings
//
// Revision 1.4  2004/05/27 13:40:03  cattanem
// v4r7
//
// Revision 1.3  2001/11/22 16:02:33  ibelyaev
//  new utilities
//
// Revision 1.2  2001/11/08 20:04:22  ibelyaev
//  update and bug fix
//
// Revision 1.1  2001/11/07 17:24:48  ibelyaev
//  new utilities for determination of relative positions of cells
//
// ============================================================================
#ifndef CALOUTILS_CELLMATRIX3X3_H 
#define CALOUTILS_CELLMATRIX3X3_H 1
// Include files
// CaloUtils
#include "CaloUtils/CellMatrix.h"
// 

/** @class CellMatrix3x3 CellMatrix3x3.h CaloUtils/CellMatrix3x3.h
 *  
 *  The simple utility class to determine if 2 cells belongs 
 *  to the same 3x3 matrix  
 *
 *  @author Vanya Belyaev Ivan.Belyaev@cern.ch
 *  @date   07/11/2001
 */

class CellMatrix3x3 
  : public CellMatrix
{
public:
  
  /** the standard constructor
   *  @param Det pointer to calorimeter detector 
   */
  CellMatrix3x3( const DeCalorimeter* Det = 0 )
    : CellMatrix( Det ){};
  
  virtual ~CellMatrix3x3( ); ///< Destructor
  
  /** the major method. It returns "true" if the cell 
   *  belongs to 3x3 sub-matrix around the seed 
   *  @param seed  the seed   cell
   *  @param cell  the cell to be tested
   *  @return fraction of area of the second cell which fits inside 
   *          3x3 matrix aroud the seed cell
   */
  inline double operator()
    ( const LHCb::CaloCellID& seed , 
      const LHCb::CaloCellID& cell ) const 
  {
    /// the same cells
    if( seed             == cell             ) { return 1 ; } ///< RETURN
    /// the same calorimeter?
    if( seed.calo ()     != cell.calo ()     ) { return 0 ; } ///< RETURN 
    /// the same area ?
    if( seed.area ()     != cell.area ()     )  
      { return treatDifferentAreas( seed , cell ); }          ///< RETURN 
    /// x ?
    if( cell.col  ()     >  seed.col  () + 1 ) { return 0 ; } ///< RETURN 
    if( cell.col  () + 1 <  seed.col  ()     ) { return 0 ; } ///< RETURN
    /// y ?  
    if( cell.row  ()     >  seed.row  () + 1 ) { return 0 ; } ///< RETURN 
    if( cell.row  () + 1 <  seed.row  ()     ) { return 0 ; } ///< RETURN
    /// 
    return 1 ;
  };
  
protected:
  
  /** if cells belong to different areas
   *  one need to treat them in a different way
   *  @exception CaloException if detector information is not available
   *  @param  seed  the seed cell
   *  @param  cell  the cell to be tested
   *  @return fraction of area of the second cell which fits inside 
   *          3x3 matrix aroud the seed cell
   */
  double treatDifferentAreas( const LHCb::CaloCellID& seed ,
                              const LHCb::CaloCellID& cell ) const ;
  
};

// ============================================================================
// The End 
// ============================================================================
#endif // CALOUTILS_CELLMATRIX3X3_H
// ============================================================================
