/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log: not supported by cvs2svn $ 
/// ===========================================================================
#ifndef GIGACNV_POINT2VERTEX_H 
#define GICACNV_POINT2VERTEX_H 1
/// ===========================================================================
/// STD & STL 
#include <functional>
/// LHCbEvent 
#include "LHCbEvent/MCVertex.h"
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
  inline MCVertex* operator() ( const GiGaTrajectoryPoint* point ) const
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
  }
};

/// ===========================================================================
#endif ///< COMPONENT_POINT2VERTEX_H
/// ===========================================================================
