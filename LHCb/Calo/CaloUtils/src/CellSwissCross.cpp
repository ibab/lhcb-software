// $Id: CellSwissCross.cpp,v 1.1 2001-11-07 17:24:49 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
// Include files
// CaloDet 
#include "CaloDet/DeCalorimeter.h"
// local
#include "CaloUtils/CellSwissCross.h"

// ============================================================================
/** @file CellSwissCross.cpp  
 *  
 *  Implementation file for class : CellSwissCross
 * 
 * @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 * @date 07/11/2001 
 */
// ============================================================================

// ============================================================================
/** destructor 
 */
// ============================================================================
CellSwissCross::~CellSwissCross() {};

// ============================================================================
/** if cells belong to different areas
 *  one need to treat them in a different way
 *  @param cell  the cell to be tested
 *  @return fraction of area of the second cell which fits inside 
 *          3x3 matrix aroud the seed cell
 */
// ============================================================================
double CellSwissCross::treatDifferentAreas( const CaloCellID& seed ,
                                            const CaloCellID& cell ) const 
{
  /// valid detector
  if( 0 == det() ) 
    { throw CaloException("CellMatrix3x3::treatDifferentAreas: DeCalo=0!");}
  /// both cells are valid? 
  if( !det()->valid( seed ) || !det()->valid( cell )  ) { return 0 ; }
  /// cell sizes 
  const double      sizeMtrx   =  0.5  * det()->cellSize   ( seed ) ;
  const double      sizeCell   =  0.5  * det()->cellSize   ( cell ) ;
  /// cell centers 
  const HepPoint3D& centerMtrx =         det()->cellCenter ( seed ) ;
  const HepPoint3D& centerCell =         det()->cellCenter ( cell ) ;
  /// effective halfsize 
  const double      sizeMin    =  0.01 * mini ( sizeMtrx , sizeCell ) ;
  /// 
  /// check the intersection with larger 3x3 matrix  
  const double AreaAux = 
    area( centerMtrx , 3 * sizeMtrx ,
          centerCell ,     sizeCell ) ;
  if( AreaAux <= sizeMin )                     { return 0 ; }  ///< RETURN 
  /// check the real intersection with swiss-cross 
  const double Area = 
    area( centerMtrx + HepPoint3D( -2 * sizeMtrx , 0             , 0 ) , 
          sizeMtrx                                                   ,
          centerCell                                                 , 
          sizeCell                                                   ) +    
    area( centerMtrx + HepPoint3D(  2 * sizeMtrx , 0             , 0 ) , 
          sizeMtrx                                                   ,
          centerCell                                                 , 
          sizeCell                                                   ) + 
    area( centerMtrx + HepPoint3D( 0             , -2 * sizeMtrx , 0 ) , 
          sizeMtrx                                                   ,
          centerCell                                                 , 
          sizeCell                                                   ) +
    area( centerMtrx + HepPoint3D( 0             ,  2 * sizeMtrx , 0 ) , 
          sizeMtrx   ,
          centerCell , 
          sizeCell   ) ;  
  ///
  if( Area <= sizeMin )       { return 0; }  ///< RETURN 
  ///
  return Area / sizeCell / sizeCell ;
};

// ============================================================================
// The End 
// ============================================================================

