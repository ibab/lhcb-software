// $Id: MCParticles.h,v 1.14 2007-07-23 17:27:31 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_MCPARTICLES_H 
#define LOKI_MCPARTICLES_H 1
// ============================================================================
// Include files
// ============================================================================
// STL & STD 
// ============================================================================
#include <string>
#include <math.h>
// ============================================================================
// Event 
// ============================================================================
#include "Event/MCParticle.h"
// ============================================================================
// MCInterfaces
// ============================================================================
#include "MCInterfaces/IMCDecayFinder.h"
#include "MCInterfaces/IMCParticleSelector.h"
#include "MCInterfaces/IMCReconstructible.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Interface.h"
#include "LoKi/Kinematics.h"
#include "LoKi/Functions.h"
#include "LoKi/Operators.h"
#include "LoKi/Constants.h"
#include "LoKi/MCTypes.h"
#include "LoKi/MCPIDOperators.h"
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
 *  @date 2001-01-23 
 */
// ============================================================================
namespace LoKi 
{  
  /** @namespace LoKi::MCParticles MCParticles.h LoKi/MCParticles.h  
   *
   *  @author Vanya  BELYAEV Ivan.Belyaev@itep.ru
   *  @date   2003-08-09
   */
  namespace  MCParticles 
  {
    // ========================================================================
    /** @class Momentum 
     *  evaluator of the momentum of the particle 
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class Momentum : public LoKi::Function<const LHCb::MCParticle*> 
    {
    public:
      /// clone method (mandatory!)
      virtual Momentum* clone() const  ; 
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    };
    // ========================================================================    
    /** @class Energy 
     *  evaluator of the energy of the particle 
     *  
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class Energy : public LoKi::Function<const LHCb::MCParticle*>
    {
    public:
      /// clone method (mandatory!)
      virtual Energy* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    };
    // ========================================================================    
    /** @class TransverseMomentum
     *  evaluator of the transverse momentum of the particle 
     *  
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class TransverseMomentum : public LoKi::Function<const LHCb::MCParticle*>
    {
    public:
      /// clone method (mandatory!)
      virtual TransverseMomentum* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    };
    // ========================================================================    
    /** @class MomentumX
     *  evaluator of the x component of the particle's momentum 
     *  
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class MomentumX : public LoKi::Function<const LHCb::MCParticle*>
    { 
    public:
      /// clone method (mandatory!)
      virtual MomentumX* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    };
    // ========================================================================    
    /** @class MomentumY
     *  evaluator of the y component of the particle's momentum 
     *  
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class MomentumY : public LoKi::Function<const LHCb::MCParticle*>
    {  
    public:
      /// clone method (mandatory!)
      virtual MomentumY* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    };
    // ========================================================================    
    /** @class MomentumZ
     *  evaluator of the z component of the particle's momentum 
     *  
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class MomentumZ : public LoKi::Function<const LHCb::MCParticle*>
    { 
    public:
      /// clone method (mandatory!)
      virtual MomentumZ* clone() const  ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    };
    // ========================================================================    
    /** @class PseudoRapidity
     *  evaluator of the seudorapidity of the particle 
     *  
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class PseudoRapidity : public LoKi::Function<const LHCb::MCParticle*>
    {    
    public:
      /// clone method (mandatory!)
      virtual PseudoRapidity* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    };
    // ========================================================================    
    /** @class Theta
     *  evaluator of the pseudorapidity of the particle 
     *  
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2002-07-15
     */
    class Theta : public LoKi::Function<const LHCb::MCParticle*>
    {  
    public:
      /// clone method (mandatory!)
      virtual Theta* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    };
    // ========================================================================    
    /** @class Phi
     *  evaluator of the 'Phi' of the particle 
     *  
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class Phi : public LoKi::Function<const LHCb::MCParticle*>
    { 
    public:
      /// clone method (mandatory!)
      virtual Phi* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    };
    // ========================================================================    
    /** @class Mass
     *  evaluator of the mass of the particle 
     *  
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class Mass : public LoKi::Function<const LHCb::MCParticle*>
    { 
    public:
      /// clone method (mandatory!)
      virtual Mass* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    };
    // ========================================================================    
    /** @class Identifier
     *  evaluator of the id of the particle 
     *  
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class Identifier : public LoKi::Function<const LHCb::MCParticle*>
    {    
    public:
      /// clone method (mandatory!)
      virtual Identifier* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    };
    // ========================================================================    
    /** @class AbsIdentifier
     *  evaluator of the id of the particle 
     *  
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class AbsIdentifier : public LoKi::Function<const LHCb::MCParticle*>
    {      
    public:
      /// clone method (mandatory!)
      virtual AbsIdentifier* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    }; 
    // ========================================================================    
    /** @class Oscillated
     *
     *  It evaluates to 'true' for oscillated particles 
     *  
     *  @author Vanya Belyaev ibelyaev@physics.syr.edu
     *  @date   2006-02-05
     */
    class Oscillated : public LoKi::Predicate<const LHCb::MCParticle*>
    {      
    public:
      /// clone method (mandatory!)
      virtual Oscillated* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    };
    // ========================================================================    
    /** @class ThreeCharge
     *  evaluator of the id of the particle 
     *  
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class ThreeCharge : public LoKi::Function<const LHCb::MCParticle*>
    { 
    public:
      /// clone method (mandatory!)
      virtual ThreeCharge* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    };
    // ========================================================================    
    /** @class ProperLifeTime 
     *  evaluator of the proper lifetime of the particle 
     *  
     *  Properlife time formaly evaluated as the 
     *  distance between the first 'endVertex' 
     *  and 'originVertex' times m/p 
     *  
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2004-01-25
     */
    class ProperLifeTime : public LoKi::Function<const LHCb::MCParticle*>
    { 
    public:
      /// clone method (mandatory!)
      virtual ProperLifeTime* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    };
    // ========================================================================    
    /** @class HasQuark 
     *  simple predicate , return true if particle has quark 'quark'
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2004-01-25
     */
    class  HasQuark : public LoKi::Predicate<const LHCb::MCParticle*>
    {
    public:
      HasQuark (  const LHCb::ParticleID::Quark quark ) ;
      // copy constructor 
      HasQuark (  const HasQuark& right ) ;
      /// clone method (mandatory!)
      virtual HasQuark* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
      //
      LHCb::ParticleID::Quark quark() const { return m_quark ; }
    private:
      HasQuark();
    private :
      LHCb::ParticleID::Quark m_quark ;
    };
    // ========================================================================    
    /** @class IsCharged
     *  simple predicate , return true if the particle is charged 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2004-01-25
     */
    class IsCharged : public LoKi::Predicate<const LHCb::MCParticle*>
    {
    public:
      /// clone method (mandatory!)
      virtual IsCharged* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    };
    // ========================================================================    
    /** @class IsNeutral
     *  simple predicate , return true if the particle is neutral
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2004-01-25
     */
    class IsNeutral : public LoKi::Predicate<const LHCb::MCParticle*>
    {
    public:
      /// clone method (mandatory!)
      virtual IsNeutral* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    };
    // ========================================================================    
    /** @class IsLepton
     *  simple predicate , return true if the particle is a lepton
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2004-01-25
     */
    class IsLepton : public LoKi::Predicate<const LHCb::MCParticle*>
    {
    public:
      /// clone method (mandatory!)
      virtual IsLepton* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    };
    // ========================================================================    
    /** @class IsMeson
     *  simple predicate , return true if the particle is a meson
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2004-01-25
     */
    class IsMeson : public LoKi::Predicate<const LHCb::MCParticle*>
    {
    public:
      /// clone method (mandatory!)
      virtual IsMeson* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    };
    // ========================================================================    
    /** @class IsBaryon
     *  simple predicate , return true if the particle is a baryon
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2004-01-25
     */
    class IsBaryon : public LoKi::Predicate<const LHCb::MCParticle*>
    {
    public:
      /// clone method (mandatory!)
      virtual IsBaryon* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    };
    // ========================================================================    
    /** @class IsHadron
     *  simple predicate , return true of particle is hadron
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2004-01-25
     */
    class IsHadron : public LoKi::Predicate<const LHCb::MCParticle*>
    {
    public:
      /// clone method (mandatory!)
      virtual IsHadron* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    };
    // ========================================================================    
    /** @class IsNucleus
     *  simple predicate , return true of particle is nucleus
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2004-01-25
     */
    class IsNucleus : public LoKi::Predicate<const LHCb::MCParticle*>
    {
    public:
      /// clone method (mandatory!)
      virtual IsNucleus* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    };
    // ========================================================================    
    /** @class FromMCDecayTree 
     *  simple predicate whoch evaluates if the MC particle belons to 
     *  MC decay tree of other MC particle
     *  @author Vanya BELYAEV  Ivan.Belyaev@itep.ru
     *  @date   2004-07-07
     */
    class FromMCDecayTree : public LoKi::Predicate<const LHCb::MCParticle*>
    {
    protected:
      typedef std::vector<const LHCb::MCParticle*> MCCont ;
    public:
      /** constructor from 1 MC particle 
       *  @param mcp pointer to MC particle
       */
      FromMCDecayTree ( const LHCb::MCParticle*                 mcp   ) ;
      /** constructor from range of MC particles
       *  @param cnt range of MC particles 
       */
      FromMCDecayTree ( const LoKi::MCTypes::MCRange&     cnt   ) ;
      /** constructor from container of MC particles
       *  @param cnt container of MC particles 
       */
      FromMCDecayTree ( const LoKi::MCTypes::MCContainer& cnt   ) ;
      /** constructor from container of MC particles
       *  @param cnt container of MC particles 
       */
      FromMCDecayTree ( const ::LHCb::MCParticles*              cnt   ) ;
      /** templated constructor from any sequence of MC particles 
       *  @param first begin of the sequence 
       *  @param last  end   of the sequence 
       */
      template <class MCPARTICLE>
      FromMCDecayTree ( MCPARTICLE first , 
                        MCPARTICLE last  ) 
        : LoKi::Predicate<const LHCb::MCParticle*>()
          , m_cont ( first , last ) {}
      // copy constructor 
      FromMCDecayTree( const FromMCDecayTree& ) ;
      /// virtual destructor 
      ~FromMCDecayTree();
      /// clone method (mandatory!)
      virtual FromMCDecayTree* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    public: 
      /** the add MCparticle into the list of tested particles 
       *  @param p pointe to the particle 
       *  @return status code 
       */
      StatusCode  add    ( const LHCb::MCParticle* p ) ;
      /** add particles into the lst of particles 
       *  @param first begin of sequence 
       *  @param last  end of the sequence  
       *  @return status code 
       */
      template <class MCPARTICLE> 
      StatusCode add    ( MCPARTICLE first , 
                          MCPARTICLE last  ) 
      { 
        m_cont.reserve ( m_cont.size() + last - first ) ;
        StatusCode sc = StatusCode::SUCCESS ;
        for ( ; first != last ; ++first ) 
        { 
          StatusCode code = add     ( *first ) ; 
          if ( code.isFailure() ) { sc = code ; }
        } 
        return sc ;
      };
      /** remove LHCb::MCParticle from the list of tested LHCb::MCParticles 
       *  @param p pointe to the LHCb::MCParticle 
       *  @return status code 
       */
      StatusCode remove ( const LHCb::MCParticle* p ) ;
      /** remove LHCb::MCParticles from the list of tested MCarticles 
       *  @param first begin of sequence 
       *  @param last  end of the sequence 
       */
      template <class MCPARTICLE> 
      StatusCode remove ( MCPARTICLE first , 
                          MCPARTICLE last  ) 
      { 
        StatusCode sc = StatusCode::SUCCESS ;
        for ( ; first != last ; ++first ) 
        { 
          StatusCode code = remove ( *first ) ; 
          if ( code.isFailure() ) { sc = code ; }
        } 
        return sc ;
      }; 
      /** set new LHCb::MCParticles 
       *  @param first begin of sequence 
       *  @param last  end of the sequence  
       *  @return status code 
       */
      template <class MCPARTICLE> 
      StatusCode setParticles ( MCPARTICLE first , 
                                MCPARTICLE last  ) 
      { 
        m_cont.clear() ;
        return add ( first , last ) ;
      };
    private:
      FromMCDecayTree();
    private:
      MCCont m_cont ;
    };
    // ========================================================================    
    /** @class NinMCdownTree 
     *  Simple function to count teh appearence 
     *  of predicate over the decay tree (down!)
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2004-11-17 
     */
    class NinMCdownTree: public LoKi::Function<const LHCb::MCParticle*>
    {
    public:
      /** constructor 
       *  @param cut cut to be inspected 
       */
      NinMCdownTree ( const LoKi::Predicate<const LHCb::MCParticle*>& cut ) ;
      /// destructor
      virtual ~NinMCdownTree() ;
      /// clone method/"virtual constructor" (mandatory!)
      virtual NinMCdownTree* clone() const ;
      /// the main method 
      virtual result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    private:
      LoKi::MCTypes::MCCut m_cut ;
    };
    // ========================================================================    
    /** @class MCMotherFunction
     *  simple adapter function which 
     *  delegates the evaluation of 
     *  function to LHCb::MCParticle::mother() 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2004-11-17 
     */
    class MCMotherFunction: public LoKi::Function<const LHCb::MCParticle*>
    {
    public:
      /** constructor 
       *  @param fun function to be evaluated 
       *  @param val value to be returned for 'invalid' mother 
       */
      MCMotherFunction ( const LoKi::Function<const LHCb::MCParticle*>& fun   , 
                         const double                             val   ) ;
      /// copy constructor 
      MCMotherFunction ( const MCMotherFunction&                  right ) ;
      /// destructor 
      virtual ~MCMotherFunction();
      /// clone method (virtual constructor) (mandatory!)
      virtual MCMotherFunction* clone() const ;
      /// the only one essential method
      virtual result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    private:
      double               m_val ;
      LoKi::MCTypes::MCFun m_fun ;
    };
    // ========================================================================    
    /** @class MCMotherPredicate 
     *  simple adapter function which 
     *  delegates the evaluation of 
     *  function to LHCb::MCParticle::mother() 
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr 
     *  @date   2005-02-02 
     */
    class MCMotherPredicate: public LoKi::Predicate<const LHCb::MCParticle*>
    {
    public:
      /** constructor 
       *  @param fun predicate to be evaluated 
       *  @param val value to be returned for 'invalid' mother 
       */
      MCMotherPredicate
      ( const LoKi::Predicate<const LHCb::MCParticle*>& cut   , 
        const bool                                      val   ) ;
      /// copy constructor 
      MCMotherPredicate 
      ( const MCMotherPredicate&                  right ) ;
      /// destructor 
      virtual ~MCMotherPredicate();
      /// clone method (virtual constructor) (mandatory!)
      virtual MCMotherPredicate* clone() const ;
      /// the only one essential method
      virtual result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    private:
      bool                 m_val ;
      LoKi::MCTypes::MCCut m_cut ;
    };
    // ========================================================================    
    /** @class IsParticle
     *  Trivial predicate to check 
     *  if particle is identical
     *  to some other Particle
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
     *  @date 2005-02-02
     */
    class IsParticle : public LoKi::Predicate<const LHCb::MCParticle*> 
    {
    public:
      typedef std::vector<const LHCb::MCParticle*> Objects;
    public:
      /** constructor from 1 particle 
       *  @param object object to be compared 
       */
      IsParticle( const LHCb::MCParticle* object ) ;
      /** constructor from range of particles 
       *  @param range range of object to be compared 
       */
      IsParticle( const LoKi::MCTypes::MCRange& obj ) ;
      /** constructor from range of particles 
       *  @param range range object to be compared 
       */
      IsParticle( const LoKi::MCTypes::MCRange::Base& obj ) ;
      /** templated constructor
       *  @param begin 'begin'-iterator for sequence of objects 
       *  @param end   'end'-iterator for sequence of objects 
       */
      template<class OBJECT>
      IsParticle
      ( OBJECT begin , 
        OBJECT end   ) 
        : LoKi::Predicate<const LHCb::MCParticle*>() 
        , m_objects ( begin , end ) 
      {};
      /** copy constructor
       *  @param right  object to be copied 
       */
      IsParticle( const IsParticle& right ) ;
      /// destructor (virtual)
      virtual ~IsParticle();
      /// MANDATORY: clone method ('virtual constructor')
      virtual IsParticle* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    private:
      Objects m_objects ; 
    };
    // ========================================================================    
    /** @class IsContainedObject 
     *  Trivial predicate to check 
     *  if particle is identical
     *  to some ContainedObject
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
     *  @date 2005-02-02
     */
    class IsContainedObject : public LoKi::Predicate<const LHCb::MCParticle*> 
    {
    public:
      typedef std::vector<const ContainedObject*> Objects;
    public:
      /** constructor from contained object
       *  @param object object to be compared 
       */
      IsContainedObject
      ( const ContainedObject* object ) ;
      /** templated constructor
       *  @param begin 'begin'-iterator for sequence of objects 
       *  @param end   'end'-iterator for sequence of objects 
       */
      template<class OBJECT>
      IsContainedObject
      ( OBJECT begin , 
        OBJECT end   ) 
        : LoKi::Predicate<const LHCb::MCParticle*>() 
        , m_objects ( begin , end ) 
      {};
      /** copy constructor
       *  @param right  object to be copied 
       */
      IsContainedObject( const IsContainedObject& right ) ;
      /// destructor (virtual)
      virtual ~IsContainedObject();
      /// MANDATORY: clone method ('virtual constructor')
      virtual IsContainedObject* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    private:
      Objects m_objects ; 
    };    
    // ========================================================================    
    /** @class MomentumDistance 
     *  Trivial evaluator of EUCLIDIAN distance 
     *  of 4-momentums; it could be useful e.g. 
     *  for trivial "kinematical matching": 
     *  One could find the particle with "closest" 
     *  4-momentum to the given one:
     *
     *  @code 
     *
     *  SEQUENCE            particles = ... ;
     *  LoKi::LorentzVector vct       = ... ; 
     *
     *  // find "best match" 
     *  SEQUENCE::const_iterator imin = 
     *    LoKi::select_min( particles.begin () , 
     *                      particles.end   () , 
     *                      MCMOMDIST ( vct )  ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Cuts::MCMOMDIST
     *  @see LoKi::LorentzVector
     *
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
     *  @date 2005-03-23 
     */
    class MomentumDistance : public LoKi::Function<const LHCb::MCParticle*> 
    {
    public:
      /// constructor from four components
      MomentumDistance
      ( const double px , 
        const double py , 
        const double pz , 
        const double e  ) ;
      /// constructor from the given 4-momentum
      MomentumDistance ( const LoKi::LorentzVector& vct ) ;
      /** copy constructor
       *  @param right object to be copied 
       */
      MomentumDistance ( const MomentumDistance& right ) ;
      /// MANDATORY: virtual destructor 
      virtual ~MomentumDistance() ;
      /// MANDATORY: clone function ("virtual constructor")
      virtual MomentumDistance* clone() const ;      
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
      private:
      /// default constructor is private
      MomentumDistance() ;
    private:
      LoKi::LorentzVector m_vct ;
    };
    // ========================================================================    
    /** @class TransverseMomentumRel
     *  Trivial evaluator of the transverse momenrum, 
     *  relatve to the given direction 
     *
     *  @code 
     *
     *  const LHCb::MCParticle*  particle = ... ;
     *  LoKi::ThreeVector  vct  = ... ; 
     *
     *  MCFun pt = MCPTDIR( vct ) ;
     *  const double pt = pt ( particle ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Cuts::MCPTDIR
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
     *  @date 2005-03-23 
     */
    class TransverseMomentumRel : public LoKi::Function<const LHCb::MCParticle*> 
    {
    public:
      /// constructor from theta & phi 
      TransverseMomentumRel ( const double theta , const double phi ) ;
      /** constructor
       *  @param vct direction vertor 
       *  @see LoKi::ThreeVector 
       */
      TransverseMomentumRel 
      ( const LoKi::ThreeVector& vct ) ;
      /** copy constructor 
       *  @param right object to be copied 
       */
      TransverseMomentumRel 
      ( const TransverseMomentumRel& right ) ;
      /// MANDATORY: virtual destructor 
      virtual ~TransverseMomentumRel();
      /// MANDATORY: clone function ("virtual constructor")
      virtual TransverseMomentumRel* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    private:
      /// default constructor is private
      TransverseMomentumRel() ;
    private:
      LoKi::ThreeVector m_vct ;
    };
    // ========================================================================    
    /** @class DeltaPhi
     *  Simple evaluator of "DeltaPhi" for the particle
     *
     *  @see LoKi::Cuts::MCDPHI 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-03-03
     */
    class DeltaPhi : public LoKi::Function<const LHCb::MCParticle*>
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
        : LoKi::Function<const LHCb::MCParticle*> () 
        , m_eval (         )
        , m_phi  ( v.phi() )
      { 
        m_phi = adjust ( m_phi ) ;
      } ;
      /// constructor from the particle
      DeltaPhi ( const LHCb::MCParticle* p ) ;
      /// templated constructor from particle
      template <class PARTICLE> 
      DeltaPhi ( const PARTICLE* p ) 
        : LoKi::Function<const LHCb::MCParticle*> () 
          , m_eval (  )
          , m_phi  (  )
      { 
        if ( 0 == p ) { Exception("Invalid PARTICLE*") ;}
        m_phi = p->momentum().phi() ;
        m_phi = adjust ( m_phi ) ;
      } ;
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
      /// adjust delta phi into the raneg of [-180:180]degrees 
      const double adjust ( double phi ) const ;
    public:
      // accessor to phi0
      double phi0() const { return m_phi ; }      
    private:
      // the default constructor is disabled 
      DeltaPhi ();
    private:
      // the actual evaluator of phi
      LoKi::MCParticles::Phi  m_eval ; ///< the actual evaluator of phi
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
    class DeltaEta : public LoKi::Function<const LHCb::MCParticle*>
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
        : LoKi::Function<const LHCb::MCParticle*> () 
        , m_eval (         )
        , m_eta  ( v.Eta() )
      {} ;
      /// constructor from the particle
      DeltaEta ( const LHCb::MCParticle* p ) ;
      /// templated constructor from particle
      template <class PARTICLE> 
      DeltaEta ( const PARTICLE* p ) 
        : LoKi::Function<const LHCb::MCParticle*> () 
        , m_eval (  )
        , m_eta  (  )
      { 
        if ( 0 == p ) { Exception("Invalid PARTICLE*") ;}
        m_eta = p->momentum().Eta() ;
      } ;
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
      // accessor to eta0
      double eta0() const { return m_eta ; }
    private:
      // the default constructor is disabled 
      DeltaEta ();
    private:
      // the actual evaluator of eta
      LoKi::MCParticles::PseudoRapidity m_eval ; ///< the actual evaluator of eta
      // the angle itself 
      double                             m_eta  ; ///< the angle itself 
    } ;
    // ========================================================================    
    /** @class DeltaR2
     *  Simple evaluator of "DeltaPhi**2+ DeltaEta**2"
     *
     *  @see LoKi::Cuts::MCDR2 
     *  @see LoKi::Cuts::MCDELTAR2 
     *  @see LoKi::Cuts::MCDPHI
     *  @see LoKi::Cuts::MCDETA
     *  @see LoKi::MCParticles::DeltaPhi
     *  @see LoKi::MCParticles::DeltaEta
     *  @see LoKi::MCParticles::Phi
     *  @see LoKi::MCParticles::PseudoRapidity
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-03-03
     */
    class DeltaR2 : public LoKi::Function<const LHCb::MCParticle*>
    {
    public:
      /// constructor from eta and phi 
      DeltaR2 ( const double eta , const double phi ) ;
      /// constructor from the vector 
      DeltaR2 ( const LoKi::ThreeVector&   v ) ;
      /// constructor from the vector 
      DeltaR2 ( const LoKi::LorentzVector& v ) ;
      /// templated constructor from vector 
      template <class VECTOR> 
      DeltaR2 ( const VECTOR& v ) 
        : LoKi::Function<const LHCb::MCParticle*> () 
        , m_dphi ( v )
        , m_deta ( v )
      {} ;
      /// constructor from the particle
      DeltaR2 ( const LHCb::MCParticle* p ) ;
      /// templated constructor from particle
      template <class PARTICLE> 
      DeltaR2 ( const PARTICLE* p ) 
        : LoKi::Function<const LHCb::MCParticle*> () 
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
      LoKi::MCParticles::DeltaPhi m_dphi ; ///< the actual evaluator of delta phi
      // the actual evaluator of delta eta
      LoKi::MCParticles::DeltaEta m_deta ; ///< the actual evaluator of delta eta
    } ;    
    // ========================================================================    
    /** @class ValidOrigin 
     *  Trivial predicat eto test teh valdity of "origin" vertex 
     *  for Monte Carlo particle
     *  @author Vanya BELYAEV Ivan.Belyaev@lapp.in2p3.fr
     *  @date 2005-05-16
     */
    class ValidOrigin : public LoKi::Predicate<const LHCb::MCParticle*>
    {
    public:
      /// MANDATORY : virtual destructor 
      virtual ~ValidOrigin();
      /// MANDATORY : clone method ("virtual constructor")
      virtual  ValidOrigin* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ; 
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    };
    // ========================================================================    
    /** @class MCVertexFunAdapter 
     *  simple function adapter which forward the 
     *  real evaluation to "originVertex" 
     *  @author Vanya BELYAEV Ivan.Belyaev@lapp.in2p3.fr
     *  @date 2005-05-16
     */
    class MCVertexFunAdapter : public LoKi::Function<const LHCb::MCParticle*> 
    {
    public  :
      /** constructor from vertex funtion 
       *  @param vfunc vertex function 
       *  @param err1  value to be returned 
       *         for invalid argument or 
       */
      MCVertexFunAdapter 
      ( const LoKi::MCTypes::MCVFunc& vfunc            , 
        const double                  err   = -1000000 ) ;
      /// MANDATORY : virtual destructor 
      virtual ~MCVertexFunAdapter() ;
      /// MANDATORY : clone method ("virtual constructor")
      virtual  MCVertexFunAdapter* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ; 
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    private :
      LoKi::MCTypes::MCVFun m_fun ;
      double                m_err ;
    };
    // ========================================================================    
    /** @class MCDecayPattern
     *  Simple predicate to test certain decay pattern, using
     *  nice tool IMCDecayFinder, developed by Olivier Dormond 
     *
     *  The idea of this predicate belongs to Luis Fernandez
     *
     *  @code 
     *
     *   // create the predicate which evaluated to true 
     *   // for B0 -> + pi- and all its ancestors 
     *   MCCut b2pp = MCDECAY ( "B0 -> pi+ pi-"   , 
     *                          mcDecayFinder () ) ;
     * 
     *   // use this predicate :  
     *  MCRange B0 = mcselect ( "B0" , b2pp ) ;
     *
     *  @endcode 
     * 
     *  @attention The "decay" string of IMCDecayFinder tool is redefined!
     *
     *  @see IMCDecayFinder 
     *  @see LoKi::Cuts::MCDECAY 
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@lapp.in2p3.fr
     *  @date 2005-05-16
     */
    class MCDecayPattern : public LoKi::Predicate<const LHCb::MCParticle*>
    {
    public:
      /** constructor 
       *  @param decay  decay descriptor 
       *  @param finder decay finder tool 
       *  @see IMCDecayFinder
       */
      MCDecayPattern
      ( const std::string& decay  ,
        IMCDecayFinder*    finder ) ;
      /** constructor 
       *  @param decay  decay descriptor 
       *  @param finder decay finder tool 
       *  @see IMCDecayFinder
       */
      MCDecayPattern
      ( const std::string&                     decay  ,
        const LoKi::Interface<IMCDecayFinder>& finder ) ;
      /** copy constructor
       *  @param right object to be copied 
       */
      MCDecayPattern 
      ( const  MCDecayPattern& right ) ;
      /// MANDATORY: virtual destructor 
      virtual ~MCDecayPattern() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  MCDecayPattern* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ; 
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    private:
      // default constructor is disabled 
      MCDecayPattern() ;
    private:
      LoKi::Interface<IMCDecayFinder> m_finder ;
      std::string                     m_decay  ;
    };
    // ========================================================================    
    /** @class MCFilter
     *  The simple class-adapter to IMCParticleSelector tool
     *  @see IMCParticleSelector
     *  @see LoKi::Cuts::MCFILTER
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-03-20
     */
    class MCFilter : public LoKi::Predicate<const LHCb::MCParticle*>
    {
    public:
      /// constructor from selector 
      MCFilter ( const IMCParticleSelector*                  selector ) ;
      /// constructor from selector 
      MCFilter ( const LoKi::Interface<IMCParticleSelector>& selector ) ;
      /// copy constructor 
      MCFilter ( const MCFilter& right ) ;
      /// MANDATORY: virtual destructor
      virtual ~MCFilter() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual MCFilter* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    private:
      // default constructor is disabled 
      MCFilter() ;
    private:
      // the selector itself
      LoKi::Interface<IMCParticleSelector> m_selector ; ///< the selector itself
    };
    // ========================================================================    
    /** @class MCReconstructuble
     *  Simple function which evaluates the "MCRecontructible" category
     *  @see IMCRecontructible
     *  @see LoKi::Cuts::MCREC
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-01
     */
    class MCReconstructible : public LoKi::Function<const LHCb::MCParticle*>
    {
    public:
      /// constructor from the tool 
      MCReconstructible ( const IMCReconstructible*                  tool ) ;
      /// constructor from the holder 
      MCReconstructible ( const LoKi::Interface<IMCReconstructible>& tool ) ;
      /// copy constructor
      MCReconstructible ( const MCReconstructible&  copy ) ;
      /// MANDATORY: virtual dectructor 
      virtual ~MCReconstructible (){} ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  MCReconstructible* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual  result_type operator() ( argument p ) const ;
      /// OPTIONAL: "short representation"
      virtual  std::ostream& fillStream ( std::ostream& s ) const ;
    public:
      /// cast operator to the underlying tool 
      operator const LoKi::Interface<IMCReconstructible>& () const
      { return m_eval ; }
    private:
      // default constructor is disabled 
      MCReconstructible () ; ///< default constructor is disabled
    private:
      // the underlying tool
      LoKi::Interface<IMCReconstructible> m_eval ; ///< the underlying tool
    } ;
    // ========================================================================    
    /** @class MCReconstructubleAs
     *  Simple function which checks 
     *     the "MCRecontructible" category
     *  @see IMCRecontructible
     *  @see LoKi::Cuts::MCRECAS
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-01
     */
    class MCReconstructibleAs : public LoKi::Predicate<const LHCb::MCParticle*>
    {
    public:
      /// constructor from the tool and category 
      MCReconstructibleAs 
      ( const IMCReconstructible*             tool , 
        const IMCReconstructible::RecCategory cat  ) ;
      /// constructor from the tool and category 
      MCReconstructibleAs 
      ( const IMCReconstructible::RecCategory cat  ,
        const IMCReconstructible*             tool ) ;
      /// constructor from the holder and category
      MCReconstructibleAs 
      ( const LoKi::Interface<IMCReconstructible>& tool , 
        const IMCReconstructible::RecCategory      cat  ) ;
      /// constructor from the holder and category
      MCReconstructibleAs 
      ( const IMCReconstructible::RecCategory      cat  ,
        const LoKi::Interface<IMCReconstructible>& tool ) ;
      /// copy constructor
      MCReconstructibleAs ( const MCReconstructibleAs&  copy ) ;
      /// MANDATORY: virtual dectructor 
      virtual ~MCReconstructibleAs (){} ;
      /// MAND ATORY: clone method ("virtual constructor")
      virtual  MCReconstructibleAs* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual  result_type operator() ( argument p ) const ;
      /// OPTIONAL: "short representation"
      virtual  std::ostream& fillStream ( std::ostream& s ) const ;
    private:
      /// cast operator to the underlying tool 
      operator const LoKi::Interface<IMCReconstructible>& () const { return m_eval ; }
    private:
      // default constructor is disabled 
      MCReconstructibleAs () ; ///< default constructor is disabled
    private:
      // the underlying tool
      LoKi::Interface<IMCReconstructible> m_eval ; ///< the underlying tool
      // the recontruction category 
      IMCReconstructible::RecCategory     m_cat  ; ///< the recontruction category
    } ;    
    // ========================================================================    
    /** @class ChildFunction
     *  Simple adapter function which 
     *  apply the function to a daughter MC-particle
     * 
     *  @code
     *
     *  const LHCb::MCParticle* B = ... ;
     *
     *  // the transverse momentum of the first daughter particle:
     *  MCFun fun = MCCHILD( MCPT , 1 ) ;
     *  const double pt1 = fun( B ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Cuts::MCCHILD 
     *  @see LoKi::Child::child 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-02
     */
    class ChildFunction : public LoKi::Function<const LHCb::MCParticle*>
    {
    public:
      /** constructor from the function and daughter index 
       *  @param fun    the function to be used 
       *  @param index  the index of daughter particle
       *  @param bad    the value to be returned for invalid particle 
       */
      ChildFunction 
      ( const LoKi::MCTypes::MCFunc& fun                     , 
        const size_t                                   index , 
        const double bad = LoKi::Constants::NegativeInfinity ) ;
      /** constructor from the function and daughter index 
       *  @param index  the index of daughter particle
       *  @param fun    the function to be used 
       *  @param bad    the value to be returned for invalid particle 
       */
      ChildFunction
      ( const size_t                 index ,
        const LoKi::MCTypes::MCFunc& fun   ,
        const double bad = LoKi::Constants::NegativeInfinity ) ;
      /// copy 
      ChildFunction ( const ChildFunction& right ) ;
      /// MANDATORY: virtual destructor
      virtual ~ChildFunction(){};
      /// MANDATORY: clone method ("virtual constructor")
      virtual  ChildFunction*  clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL:  specific printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
    private:
      // no default constructor 
      ChildFunction(); ///< no default constructor
    private:
      // the function itself 
      LoKi::MCTypes::MCFun m_fun   ; ///< the function itself 
      // index of daughter particle 
      size_t               m_index ; ///< index of daughter particle 
      // return value for invalid particle 
      double               m_bad   ; ///< return value for invalid particle 
    } ;
    // ========================================================================    
    /** @class ChildPredicate
     *  Simple adapter predicate  which 
     *  apply the predicate to a daughter  particle
     * 
     *  @code
     *
     *  const LHCb::MCParticle* B = ... ;
     *
     *  // ask if the first daughter is pi+
     *  MCCut cut = MCCHILDCUT( "pi+" == MCID  , 1 ) ;
     *  const bool pion = cut ( B ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Cuts::MCCHILDCUT
     *  @see LoKi::Child::child 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-02
     */
    class ChildPredicate : public LoKi::Predicate<const LHCb::MCParticle*>
    {
    public:
      /** constructor from the function and daughter index 
       *  @param fun    the function to be used 
       *  @param index  the index of daughter particle
       *  @param bad    the return valeu for invalid particle 
       */
      ChildPredicate 
      ( const LoKi::MCTypes::MCCuts& cut         , 
        const size_t                 index       ,
        const bool                   bad = false ) ;
      /** constructor from the function and daughter index 
       *  @param index  the index of daughter particle
       *  @param fun    the function to be used 
       *  @param bad    the return valeu for invalid particle 
       */
      ChildPredicate
      ( const size_t                 index       ,
        const LoKi::MCTypes::MCCuts& cut         ,
        const bool                   bad = false ) ;
      /// copy 
      ChildPredicate ( const ChildPredicate& right ) ;
      /// MANDATORY: virtual destructor
      virtual ~ChildPredicate(){};
      /// MANDATORY: clone method ("virtual constructor")
      virtual  ChildPredicate*  clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL:  specific printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
    private:
      // no default constructor 
      ChildPredicate(); ///< no default constructor
    private:
      // the function itself 
      LoKi::MCTypes::MCCut m_cut   ; ///< the function itself 
      // index of daughter particle 
      size_t               m_index ; ///< index of daughter particle 
      // bad value to be returned for invalid particle 
      bool                 m_bad   ;  ///< bad value
    };
    // ========================================================================    
    /** @class InTree
     *  The trivial predicate which evaluates to true 
     *  if there is at least one particle in the decay 
     *  tree of the given particle which satisfies the 
     *  certain criteria
     *
     *  The function uses the algorithm LoKi::MCAlgs::found 
     *
     *  @see LoKi::MCAlgs::found 
     *  @see LoKi::Cuts::MCINTREE 
     *  @see LHCb::MCParticle
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2004-05-05
     */
    class InTree : public LoKi::Predicate<const LHCb::MCParticle*> 
    {
    public:
      /** standard constructor 
       *  @param cut cut to be checked 
       *  @param decayOnly flag to indicat the search through decay products only
       */
      InTree  ( const LoKi::MCTypes::MCCuts& cut               , 
                const bool                   decayOnly = false ) ;
      /// copy constructor 
      InTree  ( const InTree& right ) ;
      /// MANDATORY: virtual destructor 
      virtual ~InTree (){};
      /// MANDATORY: clone function ("virtual constructor")
      virtual  InTree*       clone() const ;
      /// MANDATORY: the only one essential method 
      virtual  result_type   operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
    private:
      // default constructor is disabled 
      InTree () ; ///< default constructor is disabled 
    private:
      LoKi::MCTypes::MCCut m_cut       ;
      bool                 m_decayOnly ;
    } ;
    // ========================================================================    
    /** @class NinTree
     *  The simple funtion which evaluates the number 
     *  of particle in decay tree which satisfies the certan criteria 
     * 
     *  The function uses the algorithm LoKi::MCAlgs::count_if 
     *
     *  @see LoKi::MCAlgs::count_if 
     *  @see LoKi::Cuts::MCNINTREE 
     *  @see LHCb::MCParticle
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2004-05-05
     */
    class NinTree : public LoKi::Function<const LHCb::MCParticle*>
    {
    public:
      /** standard constructor 
       *  @param cut cut to be applied 
       */
      NinTree  ( const LoKi::MCTypes::MCCuts& cut                , 
                 const bool                   decayOnly = false  ) ;
      /// copy constructor 
      NinTree  ( const NinTree& right ) ;
      /// destructor 
      virtual ~NinTree (){} ;
      /// MANDATORY: clone function (virtual destructor)
      virtual  NinTree* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual  result_type   operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    private:
      // the default constructor is disabled 
      NinTree () ; ///< the default constructor is disabled 
    private:
      LoKi::MCTypes::MCCut m_cut       ;
      bool                 m_decayOnly ;
    };
    // ========================================================================    
    /** @class SumTree
     *
     *  The simple function which accumulated the 
     *  value of certain function over the decay 
     *  tree of the particle for the articles which 
     *  satisfy the certain selection criteria
     *  Accumulation is done throught summation
     *
     *  The function uses the algorithm LoKi::MCAlgs::accumulate
     *
     *  @see LoKi::MCAlgs::accumulate
     *  @see LoKi::Cuts::MCSUMTREE 
     *  @see LHCb::MCParticle
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2004-05-05
     */
    class SumTree : public LoKi::Function<const LHCb::MCParticle*>
    {
    public:
      /** standard constructor 
       *  @param fun the function to be evaluated 
       *  @param cut cut to be applied 
       *  @param decayObnly flag to traverse only decay tree 
       *  @param res initial value 
       */
      SumTree
      ( const LoKi::MCTypes::MCFunc& fun               ,
        const LoKi::MCTypes::MCCuts& cut               ,
        const bool                   decayOnly = false , 
        const double                 res       = 0.0   ) ;
      /** standard constructor 
       *  @param fun the function to be evaluated 
       *  @param cut cut to be applied 
       *  @param decayObnly flag to traverse only decay tree 
       *  @param res initial value 
       */
      SumTree
      ( const LoKi::MCTypes::MCCuts& cut               ,
        const LoKi::MCTypes::MCFunc& fun               ,
        const bool                   decayOnly = false , 
        const double                 res       = 0.0   ) ;
      /// copy constructor 
      SumTree ( const SumTree& right ) ;
      /// MANDATORY: virtual destructor 
      virtual ~SumTree (){} ;
      /// MANDATORY: clone function (virtual destructor)
      virtual  SumTree* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual  result_type   operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    private:
      // the default constructor is disabled 
      SumTree() ; // the default constructor is disabled 
    private:
      LoKi::MCTypes::MCFun m_fun       ;
      LoKi::MCTypes::MCCut m_cut       ;
      bool                 m_decayOnly ;
      double               m_res       ;
    };
    // ========================================================================    
    /** @class MultTree
     *
     *  The simple function which accumulated the 
     *  value of certain function over the decay 
     *  tree of the particle for the articles which 
     *  satisfy the certain selection criteria
     *  Accumulation is done throught summation
     *
     *  The function uses the algorithm LoKi::MCAlgs::accumulate
     *
     *  @see LoKi::MCAlgs::accumulate
     *  @see LoKi::Cuts::MCMULTTREE 
     *  @see LHCb::MCParticle
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2004-05-05
     */
    class MultTree : public LoKi::Function<const LHCb::MCParticle*>
    {
    public:
      /** standard constructor 
       *  @param fun the function to be evaluated 
       *  @param cut cut to be applied 
       *  @param decayObnly flag to traverse only decay tree 
       *  @param res initial value 
       */
      MultTree
      ( const LoKi::MCTypes::MCFunc& fun               ,
        const LoKi::MCTypes::MCCuts& cut               ,
        const bool                   decayOnly = false , 
        const double                 res       = 1.0   ) ;
      /** standard constructor 
       *  @param fun the function to be evaluated 
       *  @param cut cut to be applied 
       *  @param decayObnly flag to traverse only decay tree 
       *  @param res initial value 
       */
      MultTree
      ( const LoKi::MCTypes::MCCuts& cut               ,
        const LoKi::MCTypes::MCFunc& fun               ,
        const bool                   decayOnly = false , 
        const double                 res       = 1.0   ) ;
      /// copy constructor 
      MultTree ( const MultTree& right ) ;
      /// MANDATORY: virtual destructor 
      virtual ~MultTree (){} ;
      /// MANDATORY: clone function (virtual destructor)
      virtual  MultTree* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual  result_type   operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    private:
      // the default constructor is disabled 
      MultTree() ; ///< the default constructor is disabled 
    private:
      LoKi::MCTypes::MCFun m_fun       ;
      LoKi::MCTypes::MCCut m_cut       ;
      bool                 m_decayOnly ;
      double               m_res       ;
    };
    // ========================================================================    
    /** @class MinTree
     *
     *  The trivial algorithm which scans the decay 
     *  tree of the particle and searches for the the 
     *  minimal value for some functions for
     *  particles which satisfy the certain criteria 
     *
     *  The function uses the algorithm LoKi::MCAlgs::min_value 
     *
     *  @see LoKi::MCAlgs::min_value  
     *  @see LoKi::Cuts::MCMINTREE 
     *  @see LHCb::Particle
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2004-05-05
     */
    class MinTree : public LoKi::Function<const LHCb::MCParticle*>
    {
    public:
      /** standard constructor 
       *  @param fun the function to be evaluated 
       *  @param cut cut to be applied 
       *  @param decayObnly flag to traverse only decay tree 
       *  @param res initial value 
       */
      MinTree
      ( const LoKi::MCTypes::MCFunc& fun  ,
        const LoKi::MCTypes::MCCuts& cut  ,
        const bool                   decayOnly = false , 
        const double                 res  = 
        LoKi::Constants::PositiveInfinity ) ;
      /** standard constructor 
       *  @param fun the function to be evaluated 
       *  @param cut cut to be applied 
       *  @param decayObnly flag to traverse only decay tree 
       *  @param res initial value 
       */
      MinTree
      ( const LoKi::MCTypes::MCCuts& cut ,
        const LoKi::MCTypes::MCFunc& fun ,
        const bool                   decayOnly = false , 
        const double                 res = 
        LoKi::Constants::PositiveInfinity ) ;
      /// copy constructor 
      MinTree ( const MinTree& right ) ;
      /// MANDATORY: virtual destructor 
      virtual ~MinTree (){} ;
      /// MANDATORY: clone function (virtual destructor)
      virtual  MinTree* clone() const  ;
      /// MANDATORY: the only one essential method 
      virtual  result_type   operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    private:
      // the default constructor is disabled 
      MinTree() ; ///< the default constructor is disabled 
    private:
      LoKi::MCTypes::MCFun m_fun       ;
      LoKi::MCTypes::MCCut m_cut       ;
      bool                 m_decayOnly ;
      double               m_res       ;
    };
    // ========================================================================    
    /** @class MaxTree
     *
     *  The trivial algorithm which scans the decay 
     *  tree of the particle and searches for the the 
     *  minimal value for some functions for
     *  particles which satisfy the certain criteria 
     *
     *  The function uses the algorithm LoKi::MCAlgs::max_value 
     *
     *  @see LoKi::MCAlgs::max_value  
     *  @see LoKi::Cuts::MCMAXTREE 
     *  @see LHCb::Particle
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2004-05-05
     */
    class MaxTree : public LoKi::Function<const LHCb::MCParticle*>
    {
    public:
      /** standard constructor 
       *  @param fun the function to be evaluated 
       *  @param cut cut to be applied 
       *  @param decayObnly flag to traverse only decay tree 
       *  @param res initial value 
       */
      MaxTree
      ( const LoKi::MCTypes::MCFunc& fun  ,
        const LoKi::MCTypes::MCCuts& cut  ,
        const bool                   decayOnly = false , 
        const double                 res  = 
        LoKi::Constants::NegativeInfinity ) ;
      /** standard constructor 
       *  @param fun the function to be evaluated 
       *  @param cut cut to be applied 
       *  @param decayObnly flag to traverse only decay tree 
       *  @param res initial value 
       */
      MaxTree
      ( const LoKi::MCTypes::MCCuts& cut ,
        const LoKi::MCTypes::MCFunc& fun ,
        const bool                   decayOnly = false , 
        const double                 res = 
        LoKi::Constants::NegativeInfinity ) ;
      /// copy constructor 
      MaxTree ( const MaxTree& right ) ;
      /// MANDATORY: virtual destructor 
      virtual ~MaxTree (){} ;
      /// MANDATORY: clone function (virtual destructor)
      virtual  MaxTree* clone() const  ;
      /// MANDATORY: the only one essential method 
      virtual  result_type   operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    private:
      // the default constructor is disabled 
      MaxTree() ; ///< the default constructor is disabled 
    private:
      LoKi::MCTypes::MCFun m_fun       ;
      LoKi::MCTypes::MCCut m_cut       ;
      bool                 m_decayOnly ;
      double               m_res       ;
    };
    // ========================================================================    
  } // end of namespace LHCb::MCParticles 
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_MCPARTICLES_H
// ============================================================================
