// $Id: PrimaryVertex2Vertex.h,v 1.3 2006-01-31 10:34:15 gcorti Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2003/10/09 09:02:29  witoldp
// added conversion of vertex type
//
// Revision 1.1  2002/05/20 13:36:17  ibelyaev
//  add conversion of primary vertices
// 
// ============================================================================
#ifndef GIGACNV_PRIMARYVERTEX2VERTEX_H 
#define GIGACNV_PRIMARYVERTEX2VERTEX_H 1
// STD & STL
#include <functional>
// Event 
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"
// G4 
#include "G4PrimaryVertex.hh"
#include "G4PrimaryParticle.hh"
// local
#include "GiGaCnvFunctors.h"

namespace GiGaCnvFunctors
{
  
  /** @class PrimaryVertex2Vertex PrimaryVertex2Vertex.h 
   *
   *  Simple functor which transforms G4PrimaryVertex class 
   *  to MCVertex class   
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-05-20
   */  
  class PrimaryVertex2Vertex
    : public std::unary_function<const G4PrimaryVertex*,LHCb::MCVertex*>
  {
  public:
    /** the only one essential method 
     *  @param vertex pointer to G4PrimaryVertex object 
     *  @return pointer to new MCVertex object 
     */
    inline LHCb::MCVertex* operator() 
      ( const G4PrimaryVertex* vertex ) const
    {
      if( 0 == vertex ) { return 0 ; }
      // create vertex 
      LHCb::MCVertex* mcv = new LHCb::MCVertex();
      // fill it!
      mcv->setPosition( Gaudi::XYZPoint( vertex->GetX0() , 
                                         vertex->GetY0() , 
                                         vertex->GetZ0() ) ) ;
      mcv->setTime( vertex->GetT0() );
      // setting the vertex type to ppCollision
      mcv->setType( LHCb::MCVertex::ppCollision );
      return mcv ;
    };  
  };
  
}; // end of namespace GiGaCnvFunctors 

// ============================================================================
// The END 
// ============================================================================
#endif // GIGACNV_PRIMARYVERTEX2VERTEX_H
// ============================================================================
