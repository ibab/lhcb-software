// $Id: Particles1.h,v 1.1 2006-02-19 21:49:12 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef LOKI_PARTICLES1_H 
#define LOKI_PARTICLES1_H 1
// ============================================================================
// Include files
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
// ============================================================================
// LoKiPhys 
// ============================================================================
#include "LoKi/PhysTypes.h"
#include "LoKi/Vertices1.h"
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
 *  By usage of this code one clearly states the disagreement 
 *  with the campain of Dr.O.Callot et al.: 
 *  "No Vanya's lines are allowed in LHCb/Gaudi software."
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-02-19 
 */
// ============================================================================

namespace LoKi
{
  namespace Particles 
  {
    
    /** @class VertexDistance
     *
     *  evaluator of the GEOMETRY 
     *  distance between the particle 
     *  "endVertex" and "the vertex". 
     *
     *  The actual evaluation 
     *  is delegated to the function 
     *  LoKi::Vertices::VertexDistance 
     * 
     *  @see LoKi::Vertices::VertexDistance 
     *  @see LHCb::Vertex
     *  @see LHCb::Particle
     * 
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class VertexDistance : 
      public    LoKi::Function<const LHCb::Particle*> 
    {
    public:
      /// constructor 
      VertexDistance 
      ( const LHCb::Vertex*                   vertex ) ;
      /// constructor 
      VertexDistance 
      ( const LoKi::Point3D&                  vertex ) ;
      /// constructor 
      VertexDistance 
      ( const LoKi::Vertices::VertexHolder&   base   ) ;
      /// constructor 
      VertexDistance 
      ( const LoKi::Vertices::VertexDistance& base   ) ;
      /// copy constructor 
      VertexDistance 
      ( const LoKi::Particles::VertexDistance& base  ) ;
      /// MANDATORY: virtual destructor 
      virtual ~VertexDistance();
      /// MANDATORY: clone method ("virtual constructor")
      virtual VertexDistance* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      /// the actual computation 
      result_type distance           ( argument p ) const ;
    private:
      /// default constructor is private
      VertexDistance();
    private:
      // the actual evaluator 
      LoKi::Vertices::VertexDistance m_fun ;
    } ;
    
    /** @class VertexSignedDistance
     *
     *  evaluator of the GEOMETRY 
     *  distance between the particle 
     *  "endVertex" and "the vertex". 
     *  The distance is signed according 
     *  to the sign of (Zv-Z0)
     *
     *  The actual evaluation 
     *  is delegated to the function 
     *  LoKi::Vertices::VertexSignedDistance 
     * 
     *  @see LoKi::Vertices::VertexSignedDistance 
     *  @see LHCb::Vertex
     *  @see LHCb::Particle
     * 
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class VertexSignedDistance : 
      public    LoKi::Function<const LHCb::Particle*> 
    {
    public:
      /// constructor 
      VertexSignedDistance 
      ( const LHCb::Vertex*                   vertex ) ;
      /// constructor 
      VertexSignedDistance 
      ( const LoKi::Point3D&                  vertex ) ;
      /// constructor 
      VertexSignedDistance 
      ( const LoKi::Vertices::VertexHolder&   base   ) ;
      /// constructor 
      VertexSignedDistance       
      ( const LoKi::Vertices::VertexDistance& base   ) ;
      /// constructor 
      VertexSignedDistance       
      ( const LoKi::Vertices::VertexSignedDistance& base   ) ;
      /// copy constructor 
      VertexSignedDistance 
      ( const LoKi::Particles::VertexSignedDistance& base  ) ;
      /// MANDATORY: virtual destructor 
      virtual ~VertexSignedDistance();
      /// MANDATORY: clone method ("virtual constructor")
      virtual VertexSignedDistance* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      /// the actual computation 
      result_type distance           ( argument p ) const ;
    private:
      /// default constructor is private
      VertexSignedDistance();
    private:
      // the actual evaluator 
      LoKi::Vertices::VertexSignedDistance m_fun ;
    } ;

    /** @class VertexDotDistance
     *
     *  evaluator of the
     *  distance between the particle 
     *  "endVertex" and "the vertex" along 
     *  the particle momentum
     *
     *  @see LHCb::Vertex
     *  @see LHCb::Particle
     * 
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class VertexDotDistance : 
      public    LoKi::Function<const LHCb::Particle*> ,
      public    LoKi::Vertices::VertexHolder 
    {
    public:
      /// constructor 
      VertexDotDistance 
      ( const LHCb::Vertex*                   vertex ) ;
      /// constructor 
      VertexDotDistance 
      ( const LoKi::Point3D&                  vertex ) ;
      /// constructor 
      VertexDotDistance 
      ( const LoKi::Vertices::VertexHolder&   base   ) ;
      /// copy constructor 
      VertexDotDistance 
      ( const LoKi::Particles::VertexDotDistance& base  ) ;
      /// MANDATORY: virtual destructor 
      virtual ~VertexDotDistance();
      /// MANDATORY: clone method ("virtual constructor")
      virtual  VertexDotDistance* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      /// the actual computation 
      result_type distance           ( argument p ) const ;
    private:
      /// default constructor is private
      VertexDotDistance();
    } ;
    
    /** @class VertexChi2Distance
     *
     *  evaluator of the chi2 of GEOMEETRY 
     *  distance between the particle 
     *  "endVertex" and "the vertex" 
     *
     *  The actual evaluation 
     *  is delegated to the function 
     *  LoKi::Vertices::VertexChi2Distance 
     *  
     *  @see LoKi::Vertices::VertexChi2Distance 
     *  @see LHCb::Vertex
     *  @see LHCb::Particle
     * 
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class VertexChi2Distance : 
      public    LoKi::Function<const LHCb::Particle*> 
    {
    public:
      /// constructor 
      VertexChi2Distance 
      ( const LHCb::Vertex*                   vertex ) ;
      /// constructor 
      VertexChi2Distance 
      ( const LoKi::Point3D&                  vertex ) ;
      /// constructor 
      VertexChi2Distance 
      ( const LoKi::Vertices::VertexHolder&   base   ) ;
      /// constructor 
      VertexChi2Distance 
      ( const LoKi::Vertices::VertexChi2Distance& base   ) ;
      /// copy constructor 
      VertexChi2Distance 
      ( const LoKi::Particles::VertexChi2Distance& base  ) ;
      /// MANDATORY: virtual destructor 
      virtual ~VertexChi2Distance();
      /// MANDATORY: clone method ("virtual constructor")
      virtual VertexChi2Distance* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      /// the actual computation 
      result_type chi2 ( argument p ) const ;
    private:
      /// default constructor is private
      VertexChi2Distance();
    private:
      // the actual evaluator 
      LoKi::Vertices::VertexChi2Distance m_fun ;
    } ;
    
  } ; // end of namespace LoKi::Particles
} ; // end of namespace LoKi


// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PARTICLES1_H
// ============================================================================
