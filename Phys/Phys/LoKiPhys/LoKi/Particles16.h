// $Id: Particles16.h,v 1.2 2007-04-16 16:16:26 pkoppenb Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.2 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2006/12/16 13:22:26  ibelyaev
//  add Particles16: DIRA&DANG
//
// ============================================================================
#ifndef LOKI_PARTICLES16_H 
#define LOKI_PARTICLES16_H 1
// ============================================================================
// Include files
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
#include "Event/Vertex.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Functions.h"
#include "LoKi/VertexHolder.h"
// ============================================================================

// ============================================================================
/** @file
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-03-20 
 */
// ============================================================================


namespace LoKi
{
  namespace Particles 
  {
    /** @class CosineDirectionAngle
     *  evaluator of the angle between particle momentum   
     *  and the direction from fererence vertex/point 
     *  to the particle decay vertex
     *  @see LoKi::Cuts::DIRA
     *  @see LoKi::Cuts::DANG
     *  @author Vanya BELYAEV ibelyave@physics.syr.edu
     *  @date   2002-07-15
     */
    class CosineDirectionAngle 
      : public LoKi::Function<const LHCb::Particle*> 
      , public LoKi::Vertices::VertexHolder
    {
    public:
      /// constructor form vertex 
      CosineDirectionAngle ( const LHCb::VertexBase* vertex ) ;
      /// constructor form the point 
      CosineDirectionAngle ( const LoKi::Point3D&    point  ) ;
      /// constructor from the holder 
      CosineDirectionAngle ( const LoKi::Vertices::VertexHolder& holder ) ;
      /// copy constructor 
      CosineDirectionAngle ( const CosineDirectionAngle& right ) ;
      /// MANDATORY: virtual destructor
      virtual ~CosineDirectionAngle() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  CosineDirectionAngle* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    private:
      /// default constructor is private 
      CosineDirectionAngle();
    } ;
  }  // end of namespace LoKi::Particles
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PARTICLES16_H
// ============================================================================
