// $Id: Vertices2.h,v 1.5 2008-11-02 20:13:32 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_VERTICES2_H 
#define LOKI_VERTICES2_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Particles4.h"
// ============================================================================
/** @file 
 *  
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-11-25
 */
// ============================================================================
namespace LoKi 
{
  namespace Vertices 
  {
    // ========================================================================
    /** @class  ImpPar
     *
     *  evaluator of the impact parameter of vertex 
     *  with respect to a particle
     *
     *  @see LHCb::VertexBase 
     *  @see LHCb::Particle
     *  @see LoKi::Particles::ImpPar
     *  @see LoKi::Cuts::VIP
     *  
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2003-03-17
     */
    class ImpPar 
      : public LoKi::BasicFunctors<const LHCb::VertexBase*>::Function 
    {
    public:
      /// constructor from the particle and tool 
      ImpPar
      ( const LHCb::Particle*                     particle , 
        const LoKi::Vertices::ImpactParamTool&    tool     ) ;
      /// constructor from the particle and tool 
      ImpPar
      ( const LHCb::Particle*                     particle , 
        const LoKi::Vertices::ImpParBase&         base      ) ;
      /// constructor from the particle and tool 
      ImpPar
      ( const LoKi::Vertices::ImpactParamTool&    tool     ,
        const LHCb::Particle*                     particle ) ;
      /// constructor from the particle and tool 
      ImpPar
      ( const LoKi::Vertices::ImpParBase&         base      ,
        const LHCb::Particle*                     particle  ) ;
      /// copy constructor 
      ImpPar ( const ImpPar& right ) ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual ImpPar* clone() const  ;
      /// MANDATORY: virtual destructor 
      virtual ~ImpPar() ;
      /// MANDATORY: the only one essential method 
      result_type operator() ( argument ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    public:
      // set the particle 
      void setParticle ( const LHCb::Particle* p ) 
      { m_particle = p ; }
    protected:
      /// accessor to the particle 
      const LHCb::Particle* particle() const 
      { return m_particle ; }
    private:
      // ======================================================================
      // default constructor is private 
      ImpPar(); 
      // ======================================================================
    private:
      LoKi::Particles::ImpPar   m_evaluator ;
      const LHCb::Particle*     m_particle  ;
    };
    // ========================================================================
    /** @class  ImpParChi2
     *
     *  evaluator of the impact parameter of vertex 
     *  with respect to a particle
     *
     *  @see LHCb::VertexBase 
     *  @see LHCb::Particle
     *  @see LoKi::Particles::ImpParChi2
     *  @see LoKi::Cuts::VIPCHI2
     *  
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2003-03-17
     */
    class ImpParChi2 
      : public LoKi::BasicFunctors<const LHCb::VertexBase*>::Function 
    {
    public:
      /// constructor from the particle and tool 
      ImpParChi2
      ( const LHCb::Particle*                     particle , 
        const LoKi::Vertices::ImpactParamTool&    tool     ) ;
      /// constructor from the particle and tool 
      ImpParChi2
      ( const LHCb::Particle*                     particle , 
        const LoKi::Vertices::ImpParBase&         base      ) ;
      /// constructor from the particle and tool 
      ImpParChi2
      ( const LoKi::Vertices::ImpactParamTool&    tool     ,
        const LHCb::Particle*                     particle ) ;
      /// constructor from the particle and tool 
      ImpParChi2
      ( const LoKi::Vertices::ImpParBase&         base      ,
        const LHCb::Particle*                     particle  ) ;
      /// copy constructor 
      ImpParChi2 ( const ImpParChi2& right ) ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual ImpParChi2* clone() const  ;
      /// MANDATORY: virtual destructor 
      virtual ~ImpParChi2() ;
      /// MANDATORY: the only one essential method 
      result_type operator() ( argument ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    public:
      // set the particle 
      void setParticle ( const LHCb::Particle* p ) 
      { m_particle = p ; }
    protected:
      /// accessor to the particle 
      const LHCb::Particle* particle() const 
      { return m_particle ; }
    private:
      // ======================================================================
      // default constructor is private 
      ImpParChi2();
      // ======================================================================
    private:
      // ======================================================================
      LoKi::Particles::ImpParChi2 m_evaluator ;
      const LHCb::Particle*       m_particle  ;
      // ======================================================================
    };
    // ========================================================================    
  } // end of namespace LoKi::Vertices 
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_VERTICES2_H
// ============================================================================
