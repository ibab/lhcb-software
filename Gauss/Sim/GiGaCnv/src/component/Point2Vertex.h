// $Id: Point2Vertex.h,v 1.4 2002-04-23 11:23:40 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2001/08/12 17:24:55  ibelyaev
// improvements with Doxygen comments
//
// Revision 1.2  2001/07/25 17:19:32  ibelyaev
// all conversions now are moved from GiGa to GiGaCnv
//
// Revision 1.1  2001/07/24 11:13:56  ibelyaev
// package restructurization(III) and update for newer GiGa
// 
// ============================================================================
#ifndef GIGACNV_POINT2VERTEX_H 
#define GICACNV_POINT2VERTEX_H 1
// ============================================================================
/// STD & STL 
#include <functional>
/// LHCbEvent 
#include "Event/MCVertex.h"
/// GiGa
#include "GiGa/GiGaTrajectoryPoint.h"

/** @class Point2Vertex Point2Vertex.h 
 *  
 *  a simpler helper class to convert GiGaTrajectoryPoint object
 *  into MCVertex object
 *
 *  @author Ivan Belyaev
 *  @date   22/07/2001
 */

class Point2Vertex:
  public std::unary_function<const GiGaTrajectoryPoint*,MCVertex*>
{
public:
  
  /** create MCVertex from GiGaTrajectoryPoint object 
   *  @param point  pointer to GiGaTrajectoryPoint Object 
   *  @return pointer to converted MCVertex object
   */
  inline MCVertex* operator() 
    ( const GiGaTrajectoryPoint* point ) const 
  {
    /// skip artificial NULLs 
    if( 0 == point ) { return 0 ; }
    /// create new vertex 
    MCVertex* vertex = new MCVertex();
    /// fill it! 
    vertex->setPosition     ( point->GetPosition () ) ;
    vertex->setTimeOfFlight ( point->GetTime     () ) ;
    ///
    return vertex ;
  };
  
};

// ============================================================================
#endif ///< GIGACNV_POINT2VERTEX_H
// ============================================================================
