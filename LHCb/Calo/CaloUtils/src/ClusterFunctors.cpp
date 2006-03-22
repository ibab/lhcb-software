// $Id: ClusterFunctors.cpp,v 1.5 2006-03-22 18:25:06 odescham Exp $ 
// ===========================================================================
// $Log: not supported by cvs2svn $
// Revision 1.4  2002/04/07 18:05:41  ibelyaev
//  bug fix
//
// Revision 1.3  2002/04/02 10:59:31  ibelyaev
//  update for new event model
//
// Revision 1.2  2001/12/02 14:59:49  ibelyaev
//  add new functor for z-position of cluster
//
// Revision 1.1.1.1  2001/11/02 14:39:53  ibelyaev
// New package: The first commit into CVS
//
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
#include "CaloKernel/CaloException.h"
#include "CaloDet/DeCalorimeter.h"
#include "Event/CaloCluster.h"
#include "Event/CaloDigit.h"
#include "CaloUtils/ClusterFunctors.h"

// ============================================================================
/** @file ClusterFunctors.cpp
 * 
 *  Implementation of non-inline method from ClusterFunctors namespace
 *  @see ClusterFunctors 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 04/07/2001 
 */
// ============================================================================

// ============================================================================
/**  Calculate the "energy" of the cluster as a sum of 
 *   energies of its digits, weighted with energy fractions
 *   @param   cl  pointer to cluster 
 *   @return      "energy" of cluster
 */
// ============================================================================
double  LHCb::ClusterFunctors::energy
( const LHCb::CaloCluster* cl )
{
  if( 0 == cl ||  cl->entries().empty() ) { return 0 ; }
  return 
	  ClusterFunctors::energy( cl->entries().begin() , cl->entries().end() ) ;
};

// ===========================================================================
/**    useful function to determine, if clusters have 
 *     at least one common cell.
 *
 *     For invalid arguments return "false" 
 *
 *     @param   cl1   pointer to first  cluster 
 *     @param   cl2   pointer to second cluster
 *     @return "true" if clusters have at least 1 common cell 
 */
// ===========================================================================
bool LHCb::ClusterFunctors::overlapped
( const LHCb::CaloCluster* cl1 ,
  const LHCb::CaloCluster* cl2 )
{ 
  if( 0 == cl1              || 0 == cl2              || 
      cl1->entries().empty() || cl2->entries().empty() ){ return false ; }
  ///
  const_iterator_pair p =
    LHCb::ClusterFunctors::commonDigit( cl1->entries().begin() , 
                                  cl1->entries().end  () ,
                                  cl2->entries().begin() , 
                                  cl2->entries().end  () ) ;
  ///
  return 
    ( cl1->entries().end() == p.first  ) ? false : true ;
};


// ===========================================================================
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
// ===========================================================================
StatusCode  LHCb::ClusterFunctors::calculateEXY
( const LHCb::CaloCluster*   cl ,
  const DeCalorimeter* de , 
  double& e               ,
  double& x               ,
  double& y               )
{
  e = 0 ;
  x = 0 ;
  y = 0 ;
  if( 0 == cl || cl->entries().empty() ){ return StatusCode::FAILURE; }  
  ///
  return ClusterFunctors::calculateEXY( cl->entries().begin() , 
                                        cl->entries().end  () , 
                                        de , e , x , y       );
};

// ===========================================================================
/** throw the exception
 *  @param message exception message
 *  @return status code (fictive)
 */
// ===========================================================================
StatusCode LHCb::ClusterFunctors::throwException 
( const std::string& message )
{
  // throw the exception
  if( true ) { throw CaloException(" ClusterFunctors::" + message ); }
  ///
  return StatusCode::FAILURE ;
};


// ===========================================================================
// The End 
// ===========================================================================

