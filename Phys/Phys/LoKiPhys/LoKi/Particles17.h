// $Id: Particles17.h,v 1.5 2007-11-28 14:39:29 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_PARTICLES17_H 
#define LOKI_PARTICLES17_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Particles0.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
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
 *  @date 2007-03-01
 */
// ============================================================================
namespace LoKi
{
  namespace  Particles
  {
    // ========================================================================
    /** @class DeltaPhi
     *  Simple evaluator of "DeltaPhi" for the particle
     *
     *  @see LoKi::Cuts::DPHI 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-03-03
     */
    class DeltaPhi 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {
    public:
      /// constructor from the angle
      DeltaPhi ( const double phi ) ;
      /// constructor from the vector 
      DeltaPhi ( const LoKi::ThreeVector& v ) ;
      /// constructor from the vector 
      DeltaPhi ( const LoKi::LorentzVector& v ) ;
      /// templated constructor from vector 
      template <class VECTOR> 
      DeltaPhi ( const VECTOR& v ) 
        : LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
        , m_eval (         )
        , m_phi  ( v.phi() )
      { 
        m_phi = adjust ( m_phi ) ;
      } 
      /// constructor from the particle
      DeltaPhi ( const LHCb::Particle* p ) ;
      /// templated constructor from particle
      template <class PARTICLE> 
      DeltaPhi ( const PARTICLE* p ) 
        : LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
        , m_eval (  )
        , m_phi  (  )
      { 
        if ( 0 == p ) { Exception("Invalid PARTICLE*") ;}
        m_phi = p->momentum().phi() ;
        m_phi = adjust ( m_phi ) ;
      } 
      /// copy constructor
      DeltaPhi ( const DeltaPhi& right ) ;
      /// MANDATORY: virtual destructor 
      virtual ~DeltaPhi() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  DeltaPhi* clone() const ; 
      /// MANDATORY: the only essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: "SHORT" representation
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
    public:
      /// get phi_0
      double phi() const { return  m_phi ; }
    public:
      /// adjust delta phi into the raneg of [-180:180]degrees 
      const double adjust ( double phi ) const ;
    private:
      // the default constructor is disabled 
      DeltaPhi ();
    private:
      // the actual evaluator of phi
      LoKi::Particles::Phi    m_eval ; ///< the actual evaluator of phi
      // the angle itself 
      double                  m_phi  ; ///< the angle itself 
    } ;
    // ========================================================================    
    /** @class DeltaEta
     *  Simple evaluator of "DeltaEta" for the particle
     *
     *  @see LoKi::Cuts::DETA
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-03-03
     */
    class DeltaEta 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {
    public:
      /// constructor from the eta
      DeltaEta ( const double eta ) ;
      /// constructor from the vector 
      DeltaEta ( const LoKi::ThreeVector&   v ) ;
      /// constructor from the vector 
      DeltaEta ( const LoKi::LorentzVector& v ) ;
      /// templated constructor from vector 
      template <class VECTOR> 
      DeltaEta ( const VECTOR& v ) 
        : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
        , m_eval (         )
        , m_eta  ( v.Eta() )
      {} 
      /// constructor from the particle
      DeltaEta ( const LHCb::Particle* p ) ;
      /// templated constructor from particle
      template <class PARTICLE> 
      DeltaEta ( const PARTICLE* p ) 
        : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
        , m_eval (  )
        , m_eta  (  )
      { 
        if ( 0 == p ) { Exception("Invalid PARTICLE*") ;}
        m_eta = p->momentum().Eta() ;
      } 
      /// copy constructor
      DeltaEta ( const DeltaEta& right ) ;
      /// MANDATORY: virtual destructor 
      virtual ~DeltaEta() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  DeltaEta* clone() const ; 
      /// MANDATORY: the only essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: "SHORT" representation
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
    public:
      /// get eta_0
      double eta() const { return  m_eta ; }
    private:
      // the default constructor is disabled 
      DeltaEta ();
    private:
      // the actual evaluator of eta
      LoKi::Particles::PseudoRapidity m_eval ; ///< the actual evaluator of eta
      // the angle itself 
      double                          m_eta  ; ///< the angle itself 
    } ;
    // ========================================================================
    /** @class DeltaR2
     *  Simple evaluator of "DeltaPhi**2+ DeltaEta**2"
     *
     *  @see LoKi::Cuts::DR2 
     *  @see LoKi::Cuts::DELTAR2 
     *  @see LoKi::Cuts::DPHI
     *  @see LoKi::Cuts::DETA
     *  @see LoKi::Particles::DeltaPhi
     *  @see LoKi::Particles::DeltaEta
     *  @see LoKi::Particles::Phi
     *  @see LoKi::Particles::PseudoRapidity
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-03-03
     */
    class DeltaR2 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {
    public:
      /// constructor from the eta, phi
      DeltaR2 ( double eta , double phi ) ;
      /// constructor from the vector 
      DeltaR2 ( const LoKi::ThreeVector&   v ) ;
      /// constructor from the vector 
      DeltaR2 ( const LoKi::LorentzVector& v ) ;
      /// templated constructor from vector 
      template <class VECTOR> 
      DeltaR2 ( const VECTOR& v ) 
        : LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
        , m_dphi ( v )
        , m_deta ( v )
      {} ;
      /// constructor from the particle
      DeltaR2 ( const LHCb::Particle* p ) ;
      /// templated constructor from particle
      template <class PARTICLE> 
      DeltaR2 ( const PARTICLE* p ) 
        : LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
        , m_dphi ( p )
        , m_deta ( p )
      {} ;
      /// copy constructor
      DeltaR2 ( const DeltaR2& right ) ;
      /// MANDATORY: virtual destructor 
      virtual ~DeltaR2() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  DeltaR2* clone() const ; 
      /// MANDATORY: the only essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: "SHORT" representation
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
    private:
      // the default constructor is disabled 
      DeltaR2 ();
    private:
      // the actual evaluator of delta phi
      LoKi::Particles::DeltaPhi m_dphi ; ///< the actual evaluator of delta phi
      // the actual evaluator of delta eta
      LoKi::Particles::DeltaEta m_deta ; ///< the actual evaluator of delta eta
    } ;
    // ========================================================================
  } // end of namespace LoKi::Particles
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PARTICLES17_H
// ============================================================================
