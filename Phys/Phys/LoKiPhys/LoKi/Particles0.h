// $Id: Particles0.h,v 1.19 2009-02-11 12:41:49 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_PARTICLES0_H 
#define LOKI_PARTICLES0_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKiPhys 
// ============================================================================
#include "LoKi/PhysTypes.h"
#include "LoKi/Kinematics.h"
// ============================================================================
namespace LHCb
{ 
  class IParticlePropertySvc ;
  class  ParticleProperty    ;
}
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
 *  @date 2006-01-28 
 */
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  /** @namespace LoKi::Particles 
   *  collection of Particle-related functions and classes 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-01-28 
   */   
  namespace Particles 
  {
    // ========================================================================
    /** @class HasKey 
     *  The trivial predicate, it relies on Particle::hasKey method 
     *
     *  @see LHCb::Particle
     *  @see Particle::hasKey
     *  @see KeyedObject::hasKey
     *  @see LoKi::Cuts::HASKEY 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-23
     */
    class HasKey
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Predicate
    {
    public:
      // ======================================================================
      /// clone method (mandatory!)
      virtual HasKey* clone() const { return new HasKey(*this); }
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;      
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Key 
     *  The trivial function, it relies on Particle::key method 
     *
     *  @see LHCb::Particle
     *  @see Particle::key
     *  @see KeyedObject::key
     *  @see LoKi::Cuts::KEY 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-23
     */
    class  Key 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {
    public:
      /** constructor from "bad" value, to be returned 
       *  in the case of invalid argument
       *  @param bad value to be returned for invalid argument 
       */
      Key ( const LHCb::Particle::key_type bad  = -1 ) ;
      /** constrtuctor from "bad" value, to be returned 
       *  in the case of invalid argument and "nokey"
       *  value to be returned in the case of "!hasKey"
       *  @param bad value to be returned for invalid argument 
       *  @param nokey value to be returned for invalid argument 
       */
      Key ( const LHCb::Particle::key_type bad   , 
            const LHCb::Particle::key_type nokey ) ;
      /// copy constructor 
      Key ( const Key& right ) ;
      /// MANDATORY virtual destrutor 
      virtual ~Key(){} ;
      /// clone method (mandatory!)
      virtual Key* clone() const { return new Key(*this); }
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;      
      // ======================================================================
    private :
      // ======================================================================
      LHCb::Particle::key_type m_bad    ;
      LHCb::Particle::key_type m_nokey  ;
      // ======================================================================
    } ;
    // ========================================================================    
    /** @class InTES 
     *
     *  The trivial predicate which evaluates 
     *  to true for particles, registered in TES 
     *  in the container with the given name 
     * 
     *  @see LHCb::Particle
     *  @see ContainedObject
     *  @see ContainedObject::parent 
     *  @see DataObject 
     *  @see DataObject::registry
     *  @see IRegistry
     *  @see IRegistry::ideintifier 
     *  @see LoKi::Cuts::InTES 
     *  
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-23
     */
    class InTES
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Predicate
    {
    public:
      /** constructor fomr the location and the flag
       *  @param location container name 
       *         (the leading "/Event" could be omitted 
       *  @param fullpath  the logical flag. if it set 
       *         to "true", the full path to be compared 
       */
      InTES
      ( const std::string& location        , 
        const bool         fullpath = true ) ;
      ///copy constructor 
      InTES ( const  InTES& right ) ;
      /// MANDATORY: virtual destrcutor 
      virtual ~InTES() {}
      /// clone method (mandatory!)
      virtual  InTES* clone() const 
      { return new InTES(*this); }
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;        
    public:
      /// location in TES 
      const  std::string& location () const { return m_location ; }
      /// operation mode flag 
      inline bool         fullpath () const{ return m_fullpath ; }        
    private:
      // ======================================================================
      // the default constructor is disabled 
      InTES() ;
      // ======================================================================
    private:
      // ======================================================================
      std::string m_location ;
      bool        m_fullpath  ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Identifier
     *  evaluator of the id of the particle 
     *  @see LoKi::Cuts::ID
     *  @see LHCb::ParticleID 
     *  @see LHCb::Particle
     *  
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class Identifier
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {  
    public:
      // ======================================================================
      /// clone method (mandatory!)
      virtual Identifier* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
      // ======================================================================
    } ;
    // ========================================================================
    /** @class AbsIdentifier
     *  evaluator of the id of the particle 
     *  @see LoKi::Cuts::ABSID
     *  @see LHCb::ParticleID 
     *  @see LHCb::Particle
     *  
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class AbsIdentifier
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {  
    public:
      // ======================================================================
      /// clone method (mandatory!)
      virtual AbsIdentifier* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
      // ======================================================================
    } ;
    // ========================================================================    
    /** @class Charge
     *  the trivial evaluator of particle charge 
     *  it relies on LHCb::Particle::charge 
     *  @see LoKi::Cuts::Q
     *  @see LHCb::Particle
     *  @see LHCb::Particle::charge
     *  
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2006-05-22
     */
    class Charge 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {
    public:
      /// clone method (mandatory!)
      virtual Charge* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
      /// get the actual charge 
      double charge ( const LHCb::Particle* p ) const { return p->charge() ; }
      // ======================================================================
    } ;
    // ========================================================================
    /** @class SumCharge
     *  the trivial evaluator of particle charge 
     *  it relies on LoKi::Particles::Charge for the 
     *  basic particles, and for recursive sum for 
     *  non-basic particles 
     *  @see LoKi::Cuts::SUMQ
     *  @see LoKi::Particles::Charge
     *  
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2006-05-22
     */
    class SumCharge : public Charge 
    {  
    public:
      /// clone method (mandatory!)
      virtual SumCharge* clone() const ;
      /// the only one essential method 
      virtual result_type operator() ( argument p ) const 
      { return sumCharge ( p ) ; }
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
      /// the actual evaluator 
      result_type sumCharge( argument p ) const ;
      // ======================================================================
    } ;
    // ========================================================================    
    /** @class Momentum 
     *  evaluator of the momentum of the particle 
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class Momentum 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function 
    {
    public:
      // ======================================================================
      /// clone method (mandatory!)
      virtual Momentum* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    };
    // ========================================================================    
    /** @class Momentum2
     *  evaluator of the squared momentum of the particle 
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class Momentum2 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function 
    {
    public:
      // ======================================================================
      /// clone method (mandatory!)
      virtual Momentum2* clone() const  ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    };
    // ========================================================================    
    /** @class Energy 
     *  evaluator of the energy of the particle 
     *  
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class Energy 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {
    public:
      /// clone method (mandatory!)
      virtual Energy* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    };
    // ========================================================================    
    /** @class TransverseMomentum
     *  evaluator of the transverse momentum of the particle 
     *  
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class TransverseMomentum 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {
    public:
      // ======================================================================
      /// clone method (mandatory!)
      virtual TransverseMomentum* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    };
    // ========================================================================
    /** @class MomentumX
     *  evaluator of the x component of the particle's momentum 
     *  
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class MomentumX 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {    
    public:
      // ======================================================================
      /// clone method (mandatory!)
      virtual MomentumX* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    };
    // ========================================================================    
    /** @class MomentumY
     *  evaluator of the y component of the particle's momentum 
     *  
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class MomentumY
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {     
    public:
      // ======================================================================
      /// clone method (mandatory!)
      virtual MomentumY* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    };
    // ========================================================================    
    /** @class MomentumZ
     *  evaluator of the z component of the particle's momentum 
     *  
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class MomentumZ 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {     
    public:
      // ======================================================================
      /// clone method (mandatory!)
      virtual MomentumZ* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    };
    // ========================================================================
    /** @class PseudoRapidity
     *  evaluator of the seudorapidity of the particle 
     *  
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class PseudoRapidity 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {    
    public:
      // ======================================================================
      /// clone method (mandatory!)
      virtual PseudoRapidity* clone() const 
      { return new PseudoRapidity ( *this ) ; }
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return s << "ETA" ; }
      // ======================================================================
      /// get eta
      result_type eta ( argument p ) const 
      { return p -> momentum() . Eta () ; }
      // ======================================================================
   };
    // ========================================================================    
    /** @class Phi
     *  evaluator of the 'Phi' of the particle 
     *  
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class Phi 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {    
    public:
      // ======================================================================
      /// clone method (mandatory!)
      virtual Phi* clone() const { return new Phi ( *this ) ; }
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return s << "PHI" ; }
      // ======================================================================
    public:
      // ======================================================================
      /// get phi 
      result_type phi ( argument p ) const 
      { return p -> momentum() . Phi () ; }
      /// adjust phi into the raneg of [-180:180]degrees 
      double adjust ( double angle ) const 
      {
        static const double s_180 = 180 * Gaudi::Units::degree ;
        static const double s_360 = 360 * Gaudi::Units::degree ;
        //
        while ( angle >      s_180 ) { angle -= s_360 ; }
        while ( angle < -1 * s_180 ) { angle += s_360 ; }
        //
        return angle ; 
      }
    // ======================================================================
    };
    // ========================================================================    
    /** @class Theta
     *  evaluator of the 'Theta' of the particle 
     *  
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class Theta 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {    
    public:
      // ======================================================================
      /// clone method (mandatory!)
      virtual Theta* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    };
    // ========================================================================    
    /** @class Mass
     *  evaluator of the mass of the particle 
     *  
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class Mass 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {    
    public:
      // ======================================================================
      /// clone method (mandatory!)
      virtual Mass* clone() const { return new Mass( *this ) ; }
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return s << "M" ; }
      // evaluate the mass  
      double mass ( argument p ) const { return p->momentum().M() ; }  
      // ======================================================================
    };
    // ========================================================================    
    /** @class MeasuredMass
     *  evaluator of the measured mass of the particle 
     *  
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class MeasuredMass 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {    
    public:
      // ======================================================================
      /// clone method (mandatory!)
      virtual MeasuredMass* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // evaluate the mass  
      double measuredMass ( argument p ) const { return p->measuredMass() ; }  
      // ======================================================================
    };
    // ========================================================================
    /** @class InvariantMass
     *  evaluator of invariant mass for different sub-combination of 
     *  daughter particles 
     *
     *  @see LoKi::Cuts::MASS
     *  @see LoKi::Cuts::M12
     *  @see LoKi::Cuts::M13
     *  @see LoKi::Cuts::M14
     *  @see LoKi::Cuts::M23
     *  @see LoKi::Cuts::M24
     *  @see LoKi::Cuts::M34
     *
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class InvariantMass 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {
    public:      
      /// type for container of indices 
      typedef std::vector<size_t> Indices   ;
      /** constructor with index of daughter particle 
       *  @attention <c>index==0</c> corresponds to the particle 
       */
      InvariantMass
      ( const size_t index = 0 ) ;
      /** constructor with pair of indices of daughter particle 
       *  @attention <c>index==0</c> corresponds to the particle 
       */
      InvariantMass
      ( const size_t index1 , 
        const size_t index2 ) ;
      /** constructor with triplet of indices of daughter particle 
       *  @attention <c>index==0</c> corresponds to the particle 
       */
      InvariantMass 
      ( const size_t index1 , 
        const size_t index2 , 
        const size_t index3 ) ;
      /** constructor with arbitrary number of indices of daughter particle 
       *  @attention <c>index==0</c> corresponds to the particle 
       */
      InvariantMass
      ( const Indices& indices ) ;
      /// clone method (mandatory!)
      virtual InvariantMass* clone() const  ;
      /** the only one essential method 
       *  @param  p mother particle 
       *  @return the invariant mass for selected 
       *          combination of daughter particles
       */
      result_type  operator() ( argument p ) const ;
      /// specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      Indices                     m_indices ;
      // ======================================================================
    };
    // ========================================================================    
    /** @class DeltaMass
     *  evaluator of the mass of the particle 
     *  
     *  @see LoKi::Cuts::DMASS
     *
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-09-09
     */
    class DeltaMass : public LoKi::Particles::Mass 
    {      
    public:
      // ======================================================================
      /** constructor  
       *  @param mass nominal mass 
       */
      DeltaMass
      ( const double mass ) 
        : LoKi::Particles::Mass() , m_mass ( mass ) {}
      /** constructor 
       *  @param pp particle property 
       */
      DeltaMass ( const LHCb::ParticleProperty& pp ) ;
      /** constructor 
       *  @param name particle name 
       *  @param ppsvc  LHCb::ParticleProperty service 
       */
      DeltaMass
      ( const std::string&          name       , 
        LHCb::IParticlePropertySvc* ppsvc  = 0 ) ;
      /** constructor 
       *  @param pid  particle ID 
       *  @param ppsvc LHCb::ParticleProperty service 
       */
      DeltaMass
      ( const LHCb::ParticleID&     pid       , 
        LHCb::IParticlePropertySvc* ppsvc = 0 ) ;
      /// virtual destructor 
      virtual ~DeltaMass(){};
      /// clone method (mandatory!)
      virtual DeltaMass* clone() const ; 
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// get the mass
      double m0() const { return m_mass ; }
      // ======================================================================
    private:
      // ======================================================================
      /// default constructor is disabled 
      DeltaMass();                           // default constructor is disabled 
      // ======================================================================      
    private:
      // ======================================================================      
      /// the reference mass 
      double                 m_mass ;
      // ======================================================================      
    };
    // ========================================================================
    /** @class AbsDeltaMass
     *  evaluator of the absolute value for delta mass of the particle 
     *  
     *  @see LoKi::Cuts::ADMASS
     *
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-09-09
     */
    class AbsDeltaMass : public DeltaMass 
    {      
    public:
      // ======================================================================      
      /** constructor  
       *  @param mass nominal mass 
       */
      AbsDeltaMass ( const double mass ) 
        : DeltaMass ( mass ) {}
      /** constructor 
       *  @param pp particle property 
       */
      AbsDeltaMass ( const LHCb::ParticleProperty& pp )
        : DeltaMass ( pp ) {}
      /** constructor 
       *  @param name particle name 
       *  @param ppsvc LHCb::ParticleProperty service 
       */
      AbsDeltaMass 
      ( const std::string&          name      , 
        LHCb::IParticlePropertySvc* ppsvc = 0 ) 
        : DeltaMass ( name , ppsvc ) {}
      /** constructor 
       *  @param pid  particle ID 
       *  @param ppsvc LHCb::ParticleProperty service 
       */
      AbsDeltaMass
      ( const LHCb::ParticleID&     pid       , 
        LHCb::IParticlePropertySvc* ppsvc = 0 )
        : DeltaMass ( pid , ppsvc ) {}
      /// virtual destructor 
      virtual ~AbsDeltaMass(){};
      /// clone method (mandatory!)
      virtual  AbsDeltaMass* clone() const 
      { return new AbsDeltaMass (*this ) ;}
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================      
    private:
      // ======================================================================      
      /// default constructor 
      AbsDeltaMass();
      // ======================================================================      
    };
    // ========================================================================
    /** @class DeltaMeasuredMass
     *  evaluator of the mass of the particle 
     *  
     *  @see LoKi::Cuts::DMMASS
     *
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-09-09
     */
    class DeltaMeasuredMass : public DeltaMass 
    {      
    public:
      /// constructor  
      DeltaMeasuredMass ( const double mass ) 
        : DeltaMass ( mass ) {}
      /** constructor 
       *  @param pp particle property 
       */
      DeltaMeasuredMass 
      ( const LHCb::ParticleProperty& pp )
        : DeltaMass ( pp ) {}
      /** constructor 
       *  @param name particle name 
       *  @param ppsvc LHCb::ParticleProperty service 
       */
      DeltaMeasuredMass 
      ( const std::string&          name      , 
        LHCb::IParticlePropertySvc* ppsvc = 0 )
        : DeltaMass ( name , ppsvc ) {}
      /** constructor 
       *  @param pid  particle ID 
       *  @param ppsvc LHCb::ParticleProperty service 
       */
      DeltaMeasuredMass 
      ( const LHCb::ParticleID&     pid       , 
        LHCb::IParticlePropertySvc* ppsvc = 0 ) 
        : DeltaMass ( pid , ppsvc ) {}
      /// virtual destructor 
      virtual ~DeltaMeasuredMass()  ;
      /// clone method (mandatory!)
      virtual DeltaMeasuredMass* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // =======================================================================
      // evaluate the mass  
      double measuredMass ( argument p ) const { return p->measuredMass() ; }  
      // ======================================================================
    private:
      // ======================================================================
      /// default constructor 
      DeltaMeasuredMass();
      // ======================================================================
    };
    // ========================================================================
    /** @class AbsDeltaMeasuredMass 
     *  evaluator of absolute value for measured mass 
     *
     *  @see LoKi::Cuts::ADMMASS
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2004-08-04
     */
    class AbsDeltaMeasuredMass : public DeltaMeasuredMass 
    {
    public :
      // ======================================================================
      /** constructor  from particle mass 
       *  @param mass particle mass 
       */
      AbsDeltaMeasuredMass 
      ( const double mass ) 
        : DeltaMeasuredMass ( mass ) {}
      /** constructor 
       *  @param pp particle property 
       */
      AbsDeltaMeasuredMass 
      ( const LHCb::ParticleProperty& pp )
        : DeltaMeasuredMass ( pp ) {}
      /** constructor 
       *  @param name particle name 
       *  @param ppsvc  LHCb::ParticleProperty service 
       */
      AbsDeltaMeasuredMass
      ( const std::string&          name      , 
        LHCb::IParticlePropertySvc* ppsvc = 0 )
        : DeltaMeasuredMass ( name , ppsvc  ) {}
      /** constructor 
       *  @param pid  particle ID 
       *  @param ppsvc LHCb::ParticleProperty service 
       */
      AbsDeltaMeasuredMass 
      ( const LHCb::ParticleID&     pid       , 
        LHCb::IParticlePropertySvc* ppsvc = 0 )       
        : DeltaMeasuredMass ( pid  , ppsvc  ) {}
      /// destructor (virtual)
      virtual ~AbsDeltaMeasuredMass() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual AbsDeltaMeasuredMass* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      AbsDeltaMeasuredMass() ;
      // ======================================================================
    };
    // ========================================================================    
    /** @class DeltaMeasuredMassChi2 
     *  evaluator of delta meadured mass in chi2 units 
     *
     *  @see LoKi::Cuts::CHI2MM
     *  @see LoKi::Cuts::CHI2MMASS
     *
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2003-01-17
     */
    class DeltaMeasuredMassChi2 : public DeltaMeasuredMass 
    {      
    public:
      // ======================================================================
      /// constructor  
      DeltaMeasuredMassChi2 
      ( const double mass   ) 
        : DeltaMeasuredMass ( mass ) {}
      /** constructor 
       *  @param pp particle property 
       */
      DeltaMeasuredMassChi2 
      ( const LHCb::ParticleProperty& pp ) 
        : DeltaMeasuredMass ( pp ) {}
      /** constructor 
       *  @param name particle name 
       *  @param ppsvc ParticleProperty service 
       */
      DeltaMeasuredMassChi2 
      ( const std::string&          name      , 
        LHCb::IParticlePropertySvc* ppsvc = 0 ) 
        : DeltaMeasuredMass ( name , ppsvc ) {}
      /** constructor 
       *  @param pid  particle ID 
       *  @param ppsvc ParticleProperty service 
       */
      DeltaMeasuredMassChi2 
      ( const LHCb::ParticleID&     pid       , 
        LHCb::IParticlePropertySvc* ppsvc = 0 ) 
        : DeltaMeasuredMass ( pid , ppsvc ) {}
      /// virtual destructor 
      ~DeltaMeasuredMassChi2(){};
      /// clone method (mandatory!)
      virtual DeltaMeasuredMassChi2* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// default constructor is disabled 
      DeltaMeasuredMassChi2();
      // ======================================================================
    };
    // ========================================================================    
    /** @class DeltaMassChi2 
     *  evaluator of delta mass in chi2 units 
     *
     *  @see LoKi::Cuts::CHI2M
     *  @see LoKi::Cuts::CHI2MASS
     *
     *  @author Vanya Belyaev Ivan.Belyav@nikhef.nl
     *  @date   2008-09-23
     */
    class DeltaMassChi2 : public DeltaMass 
    {      
    public:
      /// constructor  
      DeltaMassChi2 ( const double mass ) 
        : DeltaMass ( mass ) {}
      /** constructor 
       *  @param pp particle property 
       */
      DeltaMassChi2 ( const LHCb::ParticleProperty& pp ) 
        : DeltaMass ( pp ) {}
      /** constructor 
       *  @param name particle name 
       *  @param ppsvc ParticleProperty service 
       */
      DeltaMassChi2 
      ( const std::string&          name       , 
        LHCb::IParticlePropertySvc* ppsvc  = 0 ) 
        : DeltaMass ( name , ppsvc ) {}
      /** constructor 
       *  @param pid  particle ID 
       *  @param ppsvc ParticleProperty service 
       */
      DeltaMassChi2 
      ( const LHCb::ParticleID&     pid       , 
        LHCb::IParticlePropertySvc* ppsvc = 0 ) 
        : DeltaMass ( pid , ppsvc ) {}
      /// virtual destructor 
      ~DeltaMassChi2(){};
      /// clone method (mandatory!)
      virtual DeltaMassChi2* clone() const 
      { return new DeltaMassChi2(*this) ; }
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// default constructor 
      DeltaMassChi2();
      // ======================================================================
    };
    // ========================================================================    
    /** @class ConfidenceLevel
     *  Trivial function which evaluates "confidence level" for 
     *  the particle
     *
     *  It relies on the method LHCb::Particle::confLevel
     *
     *  @see LHCb::Particle
     *  @see LoKi::Cuts::CL
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    class ConfidenceLevel 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {
    public:
      /// clone method (mandatory!)
      virtual ConfidenceLevel* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    };
    // ========================================================================
    /** @class  Weight
     *  Trivial function which evaluates "weight" for 
     *  the particle
     *
     *  It relies on the method LHCb::Particle::weight
     *
     *  @see LHCb::Particle
     *  @see LoKi::Cuts::WEIGHT
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    class Weight
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {
    public:
      // ======================================================================
      /// clone method (mandatory!)
      virtual Weight* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    };
    // ========================================================================    
    /** @class IsBasic
     *  Trivial predicate which evaluates to "true" for 
     *  the "basic" particles 
     *
     *  It relies on the method LHCb::Particle::isBasicParticle
     *
     *  @see LHCb::Particle
     *  @see LoKi::Cuts::BASIC
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    class IsBasic 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Predicate
    {
    public:
      // ======================================================================
      /// clone method (mandatory!)
      virtual IsBasic* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    };
    // ========================================================================    
    /** @class NumberOfDaughters
     *  Trivial function which evaluates number of daughters for 
     *  the particle
     *
     *  It relies on the method LHCb::Particle::daughters().size()
     *
     *  @see LHCb::Particle
     *  @see LoKi::Cuts::WEIGHT
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    class NumberOfDaughters 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {
    public:
      // ======================================================================
      /// clone method (mandatory!)
      virtual NumberOfDaughters* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    };
    // ========================================================================
    /** @class HasProto
     *  Trivial predicate which evaluates to true 
     *  for particles with the valid protoparticle 
     *
     *  It relies on the method LHCb::Particle::proto()
     *
     *  @see LHCb::Particle
     *  @see LoKi::Cuts::HASPROTO
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    class HasProto 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Predicate
    {
    public:
      // ======================================================================
      /// clone method (mandatory!)
      virtual HasProto* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    };
    // ========================================================================    
    /** @class HasVertex
     *  Trivial predicate which evaluates to true 
     *  for particles with the valid endVertex
     *
     *  It relies on the method LHCb::Particle::endVertex
     *
     *  @see LHCb::Particle
     *  @see LoKi::Cuts::HASPROTO
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    class HasVertex 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Predicate
    {
    public:
      // ======================================================================
      /// clone method (mandatory!)
      virtual HasVertex* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    };
    // ========================================================================
    /** @class HasTrack
     *  Trivial predicate which evaluates to true 
     *  for particles with the valid track
     *
     *  It relies on the "chain":  LHCb::Particle::proto()->track()
     *
     *  @see LHCb::Particle
     *  @see LoKi::Cuts::HASTRACK
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-02-11
     */
    class HasTrack
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Predicate
    {
    public:
      // ======================================================================
      virtual ~HasTrack() {}
      /// clone method (mandatory!)
      virtual HasTrack* clone() const { return new HasTrack(*this) ; }
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    };
    // ========================================================================    
    /** @class TransverseMomentumRel 
     *  Evaluator of the particle's transverse momentum with respect to a 
     *  certain direction
     *
     *  @see LoKi::Cuts::PTDIR
     *  @see LoKi::ThreeVector
     *  @see LoKi::LorentzVector
     *     
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-03-01
     */
    class TransverseMomentumRel 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {
    public:
      // ======================================================================
      /// constructor from theta phi
      TransverseMomentumRel ( const double theta , 
                              const double phi   ) ;      
      /// constructor from 3-vector 
      TransverseMomentumRel ( const LoKi::ThreeVector&   v ) ;      
      /// constructor from 4-vector 
      TransverseMomentumRel ( const LoKi::LorentzVector& v ) ;
      /// constructor from the particle 
      TransverseMomentumRel ( const LHCb::Particle*      v ) ;
      /// copy constructor 
      TransverseMomentumRel ( const TransverseMomentumRel& right ) ;
      /// MANDATORY: clone method ("virtual constructor") 
      virtual  TransverseMomentumRel* clone() const ;
      /// MANDATORY: virtual destructor 
      virtual ~TransverseMomentumRel() ;
      /// MANDATORY: the only one essential method 
      result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// default constructor is disabled 
      TransverseMomentumRel ( ) ;
      // ======================================================================
    public:
      // ======================================================================
      /// get the momentum 
      const LoKi::ThreeVector& momentum  () const { return m_momentum   ; }
      /// get the momentum (alias)
      const LoKi::ThreeVector& direction () const { return   momentum() ; }      
      // ======================================================================
    public:
      // ======================================================================
      /// set new momentum 
      void setMomentum  ( const LoKi::ThreeVector&   vct ) 
      { m_momentum = vct ; }
      /// set new momentum 
      void setMomentum  ( const LoKi::LorentzVector& vct ) 
      { m_momentum = vct ; }
      /// set new momentum 
      template <class VECTOR>
      void setMomentum  ( const VECTOR&              vct ) 
      { m_momentum = LoKi::ThreeVector ( vct ) ; }
      /// set new momentum 
      void setMomentum  ( const LHCb::Particle*      p   ) 
      { 
        if ( 0 == p ) 
        { Exception ( "Invalid LHCb::Particle* for setMomentum(..)" ) ; }
        setMomentum ( p -> momentum() ) ;
      } 
      /// set new momentum 
      template <class PARTICLE>
      void setMomentum  ( const PARTICLE* p )
      {
        if ( 0 != p ) 
        { Exception ( "Invalid PARTICLE* for setMomentum(..)" ) ; }
        setMomentum ( p -> momentum() ) ;
      } 
      // ======================================================================
    private:
      // ======================================================================
      /// the direction 
      LoKi::ThreeVector m_momentum ;                           // the direction 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class TransverseMomentumQ 
     *  Simple evalautor of the value of the transvers emomentum of daughter 
     *  particle with respect to the direction of the mother particle. 
     *  It is useful e.g. as Y-variable for Armenteros-Podolanski plot or 
     *  for jet-studies 
     *  @attention indices start from 1
     *  @see LoKi::Cuts::QPT 
     *  @see LoKi::Cuts::QPT1 
     *  @see LoKi::Cuts::QPT2 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-09-21
     */
    class TransverseMomentumQ 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {
    public:
      // ======================================================================
      /// constructor from the daughter's index 
      TransverseMomentumQ ( const unsigned int index = 1 ) ;
      /// MANDATORY: virtual destrcutor 
      virtual ~TransverseMomentumQ () {}
      /// MANDATORY: clone methiod ("virtual constructor")
      virtual  TransverseMomentumQ* clone() const 
      { return new TransverseMomentumQ ( *this ) ; }
      /// MANDATORY: the only one essential method 
      result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the index of daughter particle
      const unsigned int m_index ; // the index pof daughter particle
      // ======================================================================
    } ;
    // ========================================================================
    /** @class NominalMass 
     *  trivial evaluato to return the nominal mass of the particle
     *  @see LoKi::Cuts::NMASS 
     *  @see LoKi::Cuts::PDGMASS 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-09-23
     */
    class NominalMass 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {
    public:
      // ======================================================================
      /// MANDATORY: virtual destructor 
      virtual ~NominalMass() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  NominalMass* clone() const { return new NominalMass ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return s << "NMASS" ; }
      // ======================================================================
    };    
    // ========================================================================
  } // end of namespace LoKi::Particles
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PARTICLES0_H
// ============================================================================
