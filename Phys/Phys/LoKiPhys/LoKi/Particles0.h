// $Id: Particles0.h,v 1.11 2007-07-23 17:35:42 ibelyaev Exp $
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
class IParticlePropertySvc ;
class  ParticleProperty    ;
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
    class HasKey : public LoKi::Predicate<const LHCb::Particle*>
    {
    public:
      /// clone method (mandatory!)
      virtual HasKey* clone() const { return new HasKey(*this); }
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;      
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
    class  Key : public LoKi::Function<const LHCb::Particle*>
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
    private :
      LHCb::Particle::key_type m_bad    ;
      LHCb::Particle::key_type m_nokey  ;
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
    class InTES : public LoKi::Predicate<const LHCb::Particle*>
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
      // the defuult constructor is disabled 
      InTES() ;
    private:
      std::string m_location ;
      bool        m_fullpath  ;
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
    class Identifier : public LoKi::Function<const LHCb::Particle*>
    {  
    public:
      /// clone method (mandatory!)
      virtual Identifier* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
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
    class AbsIdentifier : public LoKi::Function<const LHCb::Particle*>
    {  
    public:
      /// clone method (mandatory!)
      virtual AbsIdentifier* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
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
    class Charge : public LoKi::Function<const LHCb::Particle*>
    {
    public:
      /// clone method (mandatory!)
      virtual Charge* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
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
    class SumCharge : public LoKi::Function<const LHCb::Particle*>
    {  
    public:
      /// clone method (mandatory!)
      virtual SumCharge* clone() const ;
      /// the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
      /// the actual evaluator 
      result_type _charge( argument p ) const ;
    private:
      // evaluator for basic particles 
      LoKi::Particles::Charge m_charge ;
    } ;
    // ========================================================================    
    /** @class Momentum 
     *  evaluator of the momentum of the particle 
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class Momentum : public LoKi::Function<const LHCb::Particle*> 
    {
    public:
      /// clone method (mandatory!)
      virtual Momentum* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    };
    // ========================================================================    
    /** @class Momentum2
     *  evaluator of the squared momentum of the particle 
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class Momentum2 : public LoKi::Function<const LHCb::Particle*> 
    {
    public:
      /// clone method (mandatory!)
      virtual Momentum2* clone() const  ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    };
    // ========================================================================    
    /** @class Energy 
     *  evaluator of the energy of the particle 
     *  
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class Energy : public LoKi::Function<const LHCb::Particle*>
    {
    public:
      /// clone method (mandatory!)
      virtual Energy* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    };
    // ========================================================================    
    /** @class TransverseMomentum
     *  evaluator of the transverse momentum of the particle 
     *  
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class TransverseMomentum : public LoKi::Function<const LHCb::Particle*>
    {
    public:
      /// clone method (mandatory!)
      virtual TransverseMomentum* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    };
    // ========================================================================
    /** @class MomentumX
     *  evaluator of the x component of the particle's momentum 
     *  
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class MomentumX : public LoKi::Function<const LHCb::Particle*>
    {    
    public:
      /// clone method (mandatory!)
      virtual MomentumX* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    };
    // ========================================================================    
    /** @class MomentumY
     *  evaluator of the y component of the particle's momentum 
     *  
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class MomentumY : public LoKi::Function<const LHCb::Particle*>
    {     
    public:
      /// clone method (mandatory!)
      virtual MomentumY* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    };
    // ========================================================================    
    /** @class MomentumZ
     *  evaluator of the z component of the particle's momentum 
     *  
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class MomentumZ : public LoKi::Function<const LHCb::Particle*>
    {     
    public:
      /// clone method (mandatory!)
      virtual MomentumZ* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    };
    // ========================================================================
    /** @class PseudoRapidity
     *  evaluator of the seudorapidity of the particle 
     *  
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class PseudoRapidity : public LoKi::Function<const LHCb::Particle*>
    {    
    public:
      /// clone method (mandatory!)
      virtual PseudoRapidity* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    };
    // ========================================================================    
    /** @class Phi
     *  evaluator of the 'Phi' of the particle 
     *  
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class Phi : public LoKi::Function<const LHCb::Particle*>
    {    
    public:
      /// clone method (mandatory!)
      virtual Phi* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    };
    // ========================================================================    
    /** @class Theta
     *  evaluator of the 'Theta' of the particle 
     *  
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class Theta : public LoKi::Function<const LHCb::Particle*>
    {    
    public:
      /// clone method (mandatory!)
      virtual Theta* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    };
    // ========================================================================    
    /** @class Mass
     *  evaluator of the mass of the particle 
     *  
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class Mass : public LoKi::Function<const LHCb::Particle*>
    {    
    public:
      /// clone method (mandatory!)
      virtual Mass* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    };
    // ========================================================================    
    /** @class MeasuredMass
     *  evaluator of the measured mass of the particle 
     *  
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class MeasuredMass : public LoKi::Function<const LHCb::Particle*>
    {    
    public:
      /// clone method (mandatory!)
      virtual MeasuredMass* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
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
    class InvariantMass : public LoKi::Function<const LHCb::Particle*>
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
    private:
      Indices                     m_indices ;
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
    class DeltaMass : public LoKi::Function<const LHCb::Particle*>
    {      
    public:
      /** constructor  
       *  @param mass nominal mass 
       */
      DeltaMass
      ( const double mass ) : m_mass ( mass ) , m_eval () {};
      /** constructor 
       *  @param pp particle property 
       */
      DeltaMass
      ( const ParticleProperty& pp     ) ;
      /** constructor 
       *  @param name particle name 
       *  @param svp  LHCb::ParticleProperty service 
       */
      DeltaMass
      ( const std::string&      name   , 
        IParticlePropertySvc*   ppsvc  ) ;
      /** constructor 
       *  @param pid  particle ID 
       *  @param svp  LHCb::ParticleProperty service 
       */
      DeltaMass
      ( const LHCb::ParticleID& pid    , 
        IParticlePropertySvc*   ppsvc  ) ;
      /** constructor 
       *  @param name particle name 
       */
      DeltaMass 
      ( const std::string&      name   ) ;
      /** constructor 
       *  @param pid  particle ID 
       */
      DeltaMass 
      ( const LHCb::ParticleID& pid    ) ;      
      /// copy constructor 
      DeltaMass 
      ( const DeltaMass& copy ) ;
      /// virtual destructor 
      virtual ~DeltaMass(){};
      /// clone method (mandatory!)
      virtual DeltaMass* clone() const ; 
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    public:
      /// get the mass
      double m0() const { return m_mass ; }
    private:
      /// default constructor 
      DeltaMass();
    private:
      double                 m_mass ;
      LoKi::Particles::Mass  m_eval ;
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
    class AbsDeltaMass : public LoKi::Function<const LHCb::Particle*>
    {      
    public:
      /** constructor  
       *  @param mass nominal mass 
       */
      AbsDeltaMass 
      ( const double mass ) ;
      /** constructor 
       *  @param pp particle property 
       */
      AbsDeltaMass 
      ( const ParticleProperty& pp     ) ;
      /** constructor 
       *  @param name particle name 
       *  @param svp  LHCb::ParticleProperty service 
       */
      AbsDeltaMass 
      ( const std::string&      name   , 
        IParticlePropertySvc*   ppsvc  ) ;
      /** constructor 
       *  @param pid  particle ID 
       *  @param svp  LHCb::ParticleProperty service 
       */
      AbsDeltaMass
      ( const LHCb::ParticleID&       pid    , 
        IParticlePropertySvc*   ppsvc  ) ;
      /** constructor 
       *  @param name particle name 
       */
      AbsDeltaMass 
      ( const std::string&      name   ) ;
      /** constructor 
       *  @param pid  particle ID 
       */
      AbsDeltaMass ( const LHCb::ParticleID&       pid    ) ;      
      /// constructor from evaluator 
      AbsDeltaMass ( const DeltaMass& right ) ;
      /// copy constructor 
      AbsDeltaMass ( const AbsDeltaMass& right ) ;
      /// virtual destructor 
      virtual ~AbsDeltaMass(){};
      /// clone method (mandatory!)
      virtual  AbsDeltaMass* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    public:
      /// get the mass
      double m0() const { return m_eval.m0() ; }
    private:
      /// default constructor 
      AbsDeltaMass();
    private:
      LoKi::Particles::DeltaMass  m_eval ;
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
    class DeltaMeasuredMass : public LoKi::Function<const LHCb::Particle*>
    {      
    public:
      /// constructor  
      DeltaMeasuredMass 
      ( const double mass ) ;
      /** constructor 
       *  @param pp particle property 
       */
      DeltaMeasuredMass 
      ( const ParticleProperty& pp     );
      /** constructor 
       *  @param name particle name 
       *  @param svp  LHCb::ParticleProperty service 
       */
      DeltaMeasuredMass 
      ( const std::string&      name   , 
        IParticlePropertySvc*   ppsvc  );      
      /** constructor 
       *  @param pid  particle ID 
       *  @param svp  LHCb::ParticleProperty service 
       */
      DeltaMeasuredMass 
      ( const LHCb::ParticleID&       pid    , 
        IParticlePropertySvc*   ppsvc  ) ;      
      /** constructor 
       *  @param name particle name 
       */
      DeltaMeasuredMass 
      ( const std::string&      name   ) ;
      /** constructor 
       *  @param pid  particle ID 
       */
      DeltaMeasuredMass 
      ( const LHCb::ParticleID&       pid    ) ;
      /** copy constructor 
       *  @param rigth object to be copied 
       */
      DeltaMeasuredMass 
      ( const DeltaMeasuredMass&right  ) ;
      /// virtual destructor 
      virtual ~DeltaMeasuredMass()  ;
      /// clone method (mandatory!)
      virtual DeltaMeasuredMass* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    public:
      /// get the mass
      double m0() const { return m_mass ; }
    private:
      /// default constructor 
      DeltaMeasuredMass();
    private:
      double                         m_mass      ;
      LoKi::Particles::MeasuredMass  m_eval;
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
    class AbsDeltaMeasuredMass : public  LoKi::Function<const LHCb::Particle*>
    {
    public :
      /** constructor  from particle mass 
       *  @param mass particle mass 
       */
      AbsDeltaMeasuredMass 
      ( const double mass ) ;
      /** constructor 
       *  @param pp particle property 
       */
      AbsDeltaMeasuredMass 
      ( const ParticleProperty& pp     );
      /** constructor 
       *  @param name particle name 
       *  @param svp  LHCb::ParticleProperty service 
       */
      AbsDeltaMeasuredMass
      ( const std::string&      name   , 
        IParticlePropertySvc*   ppsvc  );      
      /** constructor 
       *  @param pid  particle ID 
       *  @param svp  LHCb::ParticleProperty service 
       */
      AbsDeltaMeasuredMass 
      ( const LHCb::ParticleID&       pid    , 
        IParticlePropertySvc*   ppsvc  ) ;      
      /** constructor 
       *  @param name particle name 
       */
      AbsDeltaMeasuredMass 
      ( const std::string&      name   ) ;
      /** constructor 
       *  @param pid  particle ID 
       */
      AbsDeltaMeasuredMass 
      ( const LHCb::ParticleID&       pid    ) ;
      /** constructor from evaluator 
       *  @param measured mass evaluator 
       */
      AbsDeltaMeasuredMass 
      ( const DeltaMeasuredMass& right ) ;      
      /** copy constructor
       *  @param right instance to be copied
       */
      AbsDeltaMeasuredMass 
      ( const AbsDeltaMeasuredMass& right ) ;
      /// destructor (virtual)
      virtual ~AbsDeltaMeasuredMass() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual AbsDeltaMeasuredMass* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    public:
      /// get the mass
      double m0() const { return m_eval.m0() ; }
    private:
      /// evaluator 
      LoKi::Particles::DeltaMeasuredMass m_eval ;
    };
    // ========================================================================    
    /** @class DeltaMeasuredMassChi2 
     *  evaluator of delta meadured mass in chi2 units 
     *
     *  @see LoKi::Cuts::CHI2M
     *  @see LoKi::Cuts::CHI2MASS
     *
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2003-01-17
     */
    class DeltaMeasuredMassChi2 : public LoKi::Function<const LHCb::Particle*>
    {      
    public:
      /// constructor  
      DeltaMeasuredMassChi2 
      ( const double           mass   ) ;
      /** constructor 
       *  @param pp particle property 
       */
      DeltaMeasuredMassChi2 
      ( const ParticleProperty& pp     ) ;
      /** constructor 
       *  @param name particle name 
       *  @param svp  ParticleProperty service 
       */
      DeltaMeasuredMassChi2 
      ( const std::string&      name   , 
        IParticlePropertySvc*   ppsvc  ) ;
      /** constructor 
       *  @param pid  particle ID 
       *  @param svp  ParticleProperty service 
       */
      DeltaMeasuredMassChi2 
      ( const LHCb::ParticleID&       pid    , 
        IParticlePropertySvc*   ppsvc  ) ;
      /** constructor 
       *  @param name particle name 
       */
      DeltaMeasuredMassChi2 
      ( const std::string&      name   ) ;
      /** constructor 
       *  @param pid  particle ID 
       */
      DeltaMeasuredMassChi2 
      ( const LHCb::ParticleID& pid    ) ;
      /// constructor 
      DeltaMeasuredMassChi2 
      ( const DeltaMeasuredMass& pid    ) ;     
      /// copy constructor 
      DeltaMeasuredMassChi2 
      ( const DeltaMeasuredMassChi2& pid    ) ;
      /// virtual destructor 
      ~DeltaMeasuredMassChi2(){};
      /// clone method (mandatory!)
      virtual DeltaMeasuredMassChi2* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    public:
      /// get the mass
      double m0() const { return m_eval.m0() ; }
    private:
      /// default constructor 
      DeltaMeasuredMassChi2();
    private:
      LoKi::Particles::DeltaMeasuredMass  m_eval;
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
    class ConfidenceLevel : public LoKi::Function<const LHCb::Particle*>
    {
    public:
      /// clone method (mandatory!)
      virtual ConfidenceLevel* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
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
    class Weight : public LoKi::Function<const LHCb::Particle*>
    {
    public:
      /// clone method (mandatory!)
      virtual Weight* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
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
    class IsBasic : public LoKi::Predicate<const LHCb::Particle*>
    {
    public:
      /// clone method (mandatory!)
      virtual IsBasic* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
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
    class NumberOfDaughters : public LoKi::Function<const LHCb::Particle*>
    {
    public:
      /// clone method (mandatory!)
      virtual NumberOfDaughters* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
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
    class HasProto : public LoKi::Predicate<const LHCb::Particle*>
    {
    public:
      /// clone method (mandatory!)
      virtual HasProto* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
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
    class HasVertex : public LoKi::Predicate<const LHCb::Particle*>
    {
    public:
      /// clone method (mandatory!)
      virtual HasVertex* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
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
    class TransverseMomentumRel : public LoKi::Function<const LHCb::Particle*>
    {
    public:
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
    private:
      /// default constructor is disabled 
      TransverseMomentumRel ( ) ;
    public:
      /// get the momentum 
      const LoKi::ThreeVector& momentum  () const { return m_momentum   ; }
      /// get the momentum (alias)
      const LoKi::ThreeVector& direction () const { return   momentum() ; }      
    public:
      /// set new momentum 
      void setMomentum  ( const LoKi::ThreeVector&   vct ) { m_momentum = vct ; }
      /// set new momentum 
      void setMomentum  ( const LoKi::LorentzVector& vct ) { m_momentum = vct ; }
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
      } ;
      /// set new momentum 
      template <class PARTICLE>
      void setMomentum  ( const PARTICLE* p )
      {
        if ( 0 != p ) 
        { Exception ( "Invalid PARTICLE* for setMomentum(..)" ) ; }
        setMomentum ( p -> momentum() ) ;
      } ;
    private:
      // the direction 
      LoKi::ThreeVector m_momentum ; ///< the direction 
    } ;
    // ========================================================================
  }  // end of namespace LoKi::Particles
}  // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PARTICLES0_H
// ============================================================================
