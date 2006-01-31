// $Id: Point2Vertex.h,v 1.6 2006-01-31 10:34:15 gcorti Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.5  2002/05/20 13:36:17  ibelyaev
//  add conversion of primary vertices
//
// ============================================================================
#ifndef GIGACNV_POINT2VERTEX_H 
#define GICACNV_POINT2VERTEX_H 1
// ============================================================================
// STD & STL 
#include <functional>
// LHCbEvent 
#include "Event/MCVertex.h"
// GiGa
#include "GiGa/GiGaTrajectoryPoint.h"

namespace GiGaCnvFunctors
{
  
  /** @class Point2Vertex Point2Vertex.h 
   *  
   *  a simpler helper class to convert GiGaTrajectoryPoint object
   *  into MCVertex object
   *
   *  @author Ivan Belyaev
   *  @date   22/07/2001
   */
  
  class Point2Vertex:
    public std::unary_function<const GiGaTrajectoryPoint*,LHCb::MCVertex*>
  {
  public:
    
    /** create MCVertex from GiGaTrajectoryPoint object 
     *  @param point  pointer to GiGaTrajectoryPoint Object 
     *  @return pointer to converted MCVertex object
     */
    inline LHCb::MCVertex* operator() 
      ( const GiGaTrajectoryPoint* point ) const 
    {
      /// skip artificial NULLs 
      if( 0 == point ) { return 0 ; }
      /// create new vertex 
      LHCb::MCVertex* vertex = new LHCb::MCVertex();
      /// fill it! 
      vertex->setPosition( Gaudi::XYZPoint(  point->GetPosition() ) ) ;
      vertex->setTime( point->GetTime () ) ;
      ///
      return vertex ;
    };   
  };
  
}; // end of namespace GiGaCnvFunctors
// ============================================================================


// ============================================================================
#endif ///< GIGACNV_POINT2VERTEX_H
// ============================================================================
