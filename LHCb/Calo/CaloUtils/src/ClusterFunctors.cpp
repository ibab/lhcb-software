// ===========================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2001/07/12 21:54:22  ibelyaev
// adaptation for Win2K
//
// Revision 1.1  2001/07/04 18:15:54  ibelyaev
// Shared cell algorimth implementation
//
// ===========================================================================
#define CALOUTILS_CLUSTERFUNCTORS_CPP 1
// ===========================================================================
// Include files
// GaudiKernel
#include "GaudiKernel/SmartRef.h"
// CaloDEt 
#include "CaloDet/DeCalorimeter.h"
// CaloEvent 
#include "CaloEvent/CaloCluster.h"
#include "CaloEvent/CaloDigit.h"
// CaloUtils
#include "CaloUtils/ClusterFunctors.h"

// ============================================================================
/** @file ClusterFunctors.cpp
 * 
 *  Implementation of non-inline method from ClusterFunctors namespace
 *  @date 04/07/2001 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 */
// ============================================================================

/// ===========================================================================
/**  Calculate the "energy" of the cluster as a sum of 
     energies of its digits, weighted with energy fractions
     @param   cl  pointer to cluster 
     @return      "energy" of cluster
*/
/// ===========================================================================
double  ClusterFunctors::energy( const CaloCluster* cl )
{
  if( 0 == cl ||  cl->digits().empty() ) { return 0 ; }
  return 
	  ClusterFunctors::energy( cl->digits().begin() , cl->digits().end() ) ;
};

/// ===========================================================================
/**    useful function to determine, if clusters have 
 *     at least one common cell.
 *
 *     For invalid arguments return "false" 
 *
 *     @param   cl1   pointer to first  cluster 
 *     @param   cl2   pointer to second cluster
 *     @return "true" if clusters have at least 1 common cell 
 */
/// ===========================================================================
bool ClusterFunctors::overlapped( const CaloCluster* cl1 ,
                                  const CaloCluster* cl2 )
{ 
  if( 0 == cl1              || 0 == cl2              || 
      cl1->digits().empty() || cl2->digits().empty() ){ return false ; }
  ///
  const_iterator_pair p =
    ClusterFunctors::commonDigit( cl1->digits().begin() , 
                                  cl1->digits().end  () ,
                                  cl2->digits().begin() , 
                                  cl2->digits().end  () ) ;
  ///
  return 
    ( cl1->digits().end() == p.first  ) ? false : true ;
};


/// ===========================================================================
/**  Calculate the "energy", X and Y position 
 *   of the cluster as a sum of 
 *   energies/x/y of its digits, 
 *   weighted with energy fractions
 *   @param   cl  pointer to cluster object
 *   @param   de  pointer to DeCalorimeter object
 *   @param   e   energy 
 *   @param   x   x-position 
 *   @param   y   y-position
 *   @return    status code
 */
/// ===========================================================================
StatusCode  ClusterFunctors::calculateEXY( const CaloCluster*   cl ,
                                           const DeCalorimeter* de , 
                                           double& e               ,
                                           double& x               ,
                                           double& y               )
{
  e = 0 ;
  x = 0 ;
  y = 0 ;
  if( 0 == cl || cl->digits().empty() ){ return StatusCode::FAILURE; }  
  ///
  return ClusterFunctors::calculateEXY( cl->digits().begin() , 
                                        cl->digits().end  () , 
                                        de , e , x , y       );
};

/// ===========================================================================

