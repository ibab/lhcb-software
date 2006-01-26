// $Id: MCParticles.h,v 1.1.1.1 2006-01-26 16:13:39 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.1.1.1 $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef LOKI_MCPARTICLES_H 
#define LOKI_MCPARTICLES_H 1
// ============================================================================
// Include files
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
#include "Event/MCVertex.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Kinematics.h"
#include "LoKi/Functions.h"
#include "LoKi/Operators.h"
#include "LoKi/Constants.h"
#include "LoKi/MCTypes.h"
#include "LoKi/MCPIDOperators.h"
// ============================================================================
// forward declarations 
// ============================================================================
class IMCDecayFinder ;
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
 *  with the campain of Dr.O.Callot at al.: 
 *  "No Vanya's lines are allowed in LHCb/Gaudi software."
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23 
 */
// ============================================================================

namespace LoKi 
{  
  /** @namespace MCParticles MCParticles.h LoKi/MCParticles.h  
   *
   *  @author Vanya  BELYAEV Ivan.Belyaev@itep.ru
   *  @date   2003-08-09
   */
  namespace  MCParticles 
  {
    /** @struct Momentum 
     *  evaluator of the momentum of the particle 
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    struct Momentum : public LoKi::Function<const LHCb::MCParticle*> 
    {
      /// clone method (mandatory!)
      virtual Momentum* clone() const  ; 
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    };
    
    /** @struct Energy 
     *  evaluator of the energy of the particle 
     *  
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    struct Energy : public LoKi::Function<const LHCb::MCParticle*>
    {
      /// clone method (mandatory!)
      virtual Energy* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    };
    
    /** @struct TransverseMomentum
     *  evaluator of the transverse momentum of the particle 
     *  
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    struct TransverseMomentum : public LoKi::Function<const LHCb::MCParticle*>
    {
      /// clone method (mandatory!)
      virtual TransverseMomentum* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    };
    
    /** @struct MomentumX
     *  evaluator of the x component of the particle's momentum 
     *  
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    struct MomentumX : public LoKi::Function<const LHCb::MCParticle*>
    {      
      /// clone method (mandatory!)
      virtual MomentumX* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    };
    
    /** @struct MomentumY
     *  evaluator of the y component of the particle's momentum 
     *  
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    struct MomentumY : public LoKi::Function<const LHCb::MCParticle*>
    {      
      /// clone method (mandatory!)
      virtual MomentumY* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    };
    
    /** @struct MomentumZ
     *  evaluator of the z component of the particle's momentum 
     *  
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    struct MomentumZ : public LoKi::Function<const LHCb::MCParticle*>
    {      
      /// clone method (mandatory!)
      virtual MomentumZ* clone() const  ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    };

    /** @struct PseudoRapidity
     *  evaluator of the seudorapidity of the particle 
     *  
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    struct PseudoRapidity : public LoKi::Function<const LHCb::MCParticle*>
    {      
      /// clone method (mandatory!)
      virtual PseudoRapidity* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    };
    
    /** @struct Theta
     *  evaluator of the pseudorapidity of the particle 
     *  
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2002-07-15
     */
    struct Theta : public LoKi::Function<const LHCb::MCParticle*>
    {      
      /// clone method (mandatory!)
      virtual Theta* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    };
    
    /** @struct Phi
     *  evaluator of the 'Phi' of the particle 
     *  
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    struct Phi : public LoKi::Function<const LHCb::MCParticle*>
    {      
      /// clone method (mandatory!)
      virtual Phi* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    };

    /** @struct Mass
     *  evaluator of the mass of the particle 
     *  
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    struct Mass : public LoKi::Function<const LHCb::MCParticle*>
    {      
      /// clone method (mandatory!)
      virtual Mass* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    };
    
    /** @struct Identifier
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

    /** @struct AbsIdentifier
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
    
    /** @struct ThreeCharge
     *  evaluator of the id of the particle 
     *  
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    struct ThreeCharge : public LoKi::Function<const LHCb::MCParticle*>
    {      
      /// clone method (mandatory!)
      virtual ThreeCharge* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    };

    /** @struct ProperLifeTime 
     *  evaluator of the proper lifetime of the particle 
     *  
     *  Properlife time formaly evaluated as the 
     *  distance between the first 'endVertex' 
     *  and 'originVertex' times m/p 
     *  
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2004-01-25
     */
    struct ProperLifeTime : public LoKi::Function<const LHCb::MCParticle*>
    {      
      /// clone method (mandatory!)
      virtual ProperLifeTime* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    };
    
    /** @class HasQuark 
     *  simple predicate , return true if particle has quark 'quark'
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2004-01-25
     */
    class  HasQuark  : public LoKi::Predicate<const LHCb::MCParticle*>
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

    /** @class IsCharged
     *  simple predicate , return true if the particle is charged 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2004-01-25
     */
    struct IsCharged : public LoKi::Predicate<const LHCb::MCParticle*>
    {
      /// clone method (mandatory!)
      virtual IsCharged* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    };
    
    /** @class IsNeutral
     *  simple predicate , return true if the particle is neutral
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2004-01-25
     */
    struct IsNeutral : public LoKi::Predicate<const LHCb::MCParticle*>
    {
      /// clone method (mandatory!)
      virtual IsNeutral* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    };
    
    /** @class IsLepton
     *  simple predicate , return true if the particle is a lepton
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2004-01-25
     */
    struct IsLepton : public LoKi::Predicate<const LHCb::MCParticle*>
    {
      /// clone method (mandatory!)
      virtual IsLepton* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    };
    
    /** @class IsMeson
     *  simple predicate , return true if the particle is a meson
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2004-01-25
     */
    struct IsMeson : public LoKi::Predicate<const LHCb::MCParticle*>
    {
      /// clone method (mandatory!)
      virtual IsMeson* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    };
    
    /** @class IsBaryon
     *  simple predicate , return true if the particle is a baryon
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2004-01-25
     */
    struct IsBaryon : public LoKi::Predicate<const LHCb::MCParticle*>
    {
      /// clone method (mandatory!)
      virtual IsBaryon* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    };
    
    /** @class IsHadron
     *  simple predicate , return true of particle is hadron
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2004-01-25
     */
    struct IsHadron : public LoKi::Predicate<const LHCb::MCParticle*>
    {
      /// clone method (mandatory!)
      virtual IsHadron* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    };
    
    /** @class IsNucleus
     *  simple predicate , return true of particle is nucleus
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2004-01-25
     */
    struct IsNucleus : public LoKi::Predicate<const LHCb::MCParticle*>
    {
      /// clone method (mandatory!)
      virtual IsNucleus* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    };

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
        m_cont.reserve ( m_particle.size() + last - first ) ;
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
      MCMotherPredicate ( const LoKi::Predicate<const LHCb::MCParticle*>& cut   , 
                          const bool                                val   ) ;
      /// copy constructor 
      MCMotherPredicate ( const MCMotherPredicate&                  right ) ;
      /// destructor 
      virtual ~MCMotherPredicate();
      /// clone method (virtual constructor) (mandatory!)
      virtual MCMotherPredicate* clone() const ;
      /// the only one essential method
      virtual result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    private:
      double               m_val ;
      LoKi::MCTypes::MCCut m_cut ;
    };
    
    /** @class IsParticle
     *  Trivial predicate to check 
     *  if particle is identical
     *  to some other Particle
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
     *  @date 2005-02-02
     */
    class IsParticle : 
      public LoKi::Predicate<const LHCb::MCParticle*> 
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
    
    /** @class IsContainedObject 
     *  Trivial predicate to check 
     *  if particle is identical
     *  to some ContainedObject
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
     *  @date 2005-02-02
     */
    class IsContainedObject : 
      public LoKi::Predicate<const LHCb::MCParticle*> 
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
    class MomentumDistance : 
      public LoKi::Function<const LHCb::MCParticle*> 
    {
    public:
      /** constructor from the given 4-momentum
       *  @param vct 4-momentum 
       */
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
    class TransverseMomentumRel :
      public LoKi::Function<const LHCb::MCParticle*> 
    {
    public:
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
    
    /** @class DeltaPhi 
     *  Trivial evaluator of difference in 
     *  asimuthal angle phi
     *  with respect to a given direction 
     *  
     *  @see LoKi::Cuts::MCDPHI 
     *  @author Vanya BELYAEV  Ivan.Belyaev@lapp.in2p3.fr 
     *  @date 2005-04-09
     */
    class DeltaPhi: 
      public LoKi::Function<const LHCb::MCParticle*> 
    {
    public:
      /** constructor from the angle 
       *  @param phi angle 
       */
      DeltaPhi ( const double               phi    ) ;
      /** constructor from the direction 
       *  @param vct 3D-vector 
       */
      DeltaPhi ( const LoKi::ThreeVector&   vct    ) ;
      /** constructor from the direction 
       *  @param vct 3D-vector 
       */
      DeltaPhi ( const LoKi::LorentzVector& vct   ) ;
      /** copy constructor 
       *  @param rigth object to be copied 
       */
      DeltaPhi ( const DeltaPhi&            right ) ;
      /// MANDATORY: virtual destructor 
      virtual ~DeltaPhi() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  DeltaPhi* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    private:
      // private ocnstructor is disabled 
      DeltaPhi() ;
    private:
      double m_phi ;
    };
    
    /** @class DeltaEta
     *  Trivial evaluator of difference in 
     *  pseudorapidity 
     *  with respect to a given 4-momentum
     *  
     *  @see LoKi::Cuts::MCDETA
     *  @author Vanya BELYAEV  Ivan.Belyaev@lapp.in2p3.fr 
     *  @date 2005-04-09
     */
    class DeltaEta: 
      public LoKi::Function<const LHCb::MCParticle*> 
    {
    public:
      /** constructor from pseudorapidity
       *  @param eta pseudorapidity
       */
      DeltaEta ( const double eta ) ;
      /** constructor from the momentum
       *  @param vct momentum
       */
      DeltaEta ( const LoKi::LorentzVector& vct ) ;
      /** copy constructor 
       *  @param rigth object to be copied 
       */
      DeltaEta ( const DeltaEta&            right ) ;
      /// MANDATORY: virtual destructor 
      virtual ~DeltaEta() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  DeltaEta* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ; 
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    private:
      // private constructor is disabled 
      DeltaEta() ;
    private:
      double m_eta ;
    };
    
    /** @struct ValidOrigin 
     *  Trivial predicat eto test teh valdity of "origin" vertex 
     *  for Monte Carlo particle
     *  @author Vanya BELYAEV Ivan.Belyaev@lapp.in2p3.fr
     *  @date 2005-05-16
     */
    struct ValidOrigin : 
      public LoKi::Predicate<const LHCb::MCParticle*>
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
    
    /** @class MCVertexFunAdapter 
     *  simple function adapter which forward the 
     *  real evaluation to "originVertex" 
     *  @author Vanya BELYAEV Ivan.Belyaev@lapp.in2p3.fr
     *  @date 2005-05-16
     */
    class MCVertexFunAdapter : 
      public LoKi::Function<const LHCb::MCParticle*> 
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
    class MCDecayPattern : 
      public LoKi::Predicate<const LHCb::MCParticle*>
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
      // assignement operator is disabled 
      MCDecayPattern& operator=
      ( const MCDecayPattern& right ) ;
    private:
      IMCDecayFinder* m_finder ;
      std::string     m_decay  ;
    };
    
  }; // end of namespace LHCb::MCParticles 
}; // end of namespace LoKi

// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_MCPARTICLES_H
// ============================================================================
