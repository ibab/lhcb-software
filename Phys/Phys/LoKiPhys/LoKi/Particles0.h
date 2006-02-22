// $Id: Particles0.h,v 1.2 2006-02-22 20:53:47 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2006/02/19 21:49:12  ibelyaev
//  restructirisation + new funtions
//
// ============================================================================
#ifndef LOKI_PARTICLES0_H 
#define LOKI_PARTICLES0_H 1
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
// ============================================================================
class IParticlePropertySvc ;
class  ParticleProperty    ;
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
 *  @date 2006-01-28 
 */
// ============================================================================

namespace LoKi
{
  namespace Particles 
  {
    
    /** @struct Momentum 
     *  evaluator of the momentum of the particle 
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    struct Momentum 
      : public LoKi::Function<const LHCb::Particle*> 
    {
      /// clone method (mandatory!)
      virtual Momentum* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    };
    
    /** @struct Momentum2
     *  evaluator of the squared momentum of the particle 
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    struct Momentum2 
      : public LoKi::Function<const LHCb::Particle*> 
    {
      /// clone method (mandatory!)
      virtual Momentum2* clone() const  ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    };
    
    /** @struct Energy 
     *  evaluator of the energy of the particle 
     *  
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    struct Energy 
      : public LoKi::Function<const LHCb::Particle*>
    {
      /// clone method (mandatory!)
      virtual Energy* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    };
    
    /** @struct TransverseMomentum
     *  evaluator of the transverse momentum of the particle 
     *  
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    struct TransverseMomentum 
      : public LoKi::Function<const LHCb::Particle*>
    {
      /// clone method (mandatory!)
      virtual TransverseMomentum* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    };
    
    /** @struct MomentumX
     *  evaluator of the x component of the particle's momentum 
     *  
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    struct MomentumX 
      : public LoKi::Function<const LHCb::Particle*>
    {      
      /// clone method (mandatory!)
      virtual MomentumX* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    };
    
    /** @struct MomentumY
     *  evaluator of the y component of the particle's momentum 
     *  
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    struct MomentumY 
      : public LoKi::Function<const LHCb::Particle*>
    {      
      /// clone method (mandatory!)
      virtual MomentumY* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    };
    
    /** @struct MomentumZ
     *  evaluator of the z component of the particle's momentum 
     *  
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    struct MomentumZ 
      : public LoKi::Function<const LHCb::Particle*>
    {      
      /// clone method (mandatory!)
      virtual MomentumZ* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    };
    
    /** @struct PseudoRapidity
     *  evaluator of the seudorapidity of the particle 
     *  
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    struct PseudoRapidity 
      : public LoKi::Function<const LHCb::Particle*>
    {      
      /// clone method (mandatory!)
      virtual PseudoRapidity* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    };
    
    /** @struct Phi
     *  evaluator of the 'Phi' of the particle 
     *  
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    struct Phi 
      : public LoKi::Function<const LHCb::Particle*>
    {      
      /// clone method (mandatory!)
      virtual Phi* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    };
    
    /** @struct Theta
     *  evaluator of the 'Theta' of the particle 
     *  
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    struct Theta 
      : public LoKi::Function<const LHCb::Particle*>
    {      
      /// clone method (mandatory!)
      virtual Theta* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    };
    
    /** @struct Mass
     *  evaluator of the mass of the particle 
     *  
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    struct Mass 
      : public LoKi::Function<const LHCb::Particle*>
    {      
      /// clone method (mandatory!)
      virtual Mass* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    };
    
    /** @struct MeasuredMass
     *  evaluator of the measured mass of the particle 
     *  
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    struct MeasuredMass 
      : public LoKi::Function<const LHCb::Particle*>
    {      
      /// clone method (mandatory!)
      virtual MeasuredMass* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    };
    
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
      : public LoKi::Function<const LHCb::Particle*>
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

    /** @class DeltaMass
     *  evaluator of the mass of the particle 
     *  
     *  @see LoKi::Cuts::DMASS
     *
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-09-09
     */
    class DeltaMass 
      : public LoKi::Function<const LHCb::Particle*>
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

    /** @class AbsDeltaMass
     *  evaluator of the absolute value for delta mass of the particle 
     *  
     *  @see LoKi::Cuts::ADMASS
     *
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-09-09
     */
    class AbsDeltaMass 
      : public LoKi::Function<const LHCb::Particle*>
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
    
    /** @class DeltaMeasuredMass
     *  evaluator of the mass of the particle 
     *  
     *  @see LoKi::Cuts::DMMASS
     *
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-09-09
     */
    class DeltaMeasuredMass 
      : public LoKi::Function<const LHCb::Particle*>
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
    
    /** @class AbsDeltaMeasuredMass 
     *  evaluator of absolute value for measured mass 
     *
     *  @see LoKi::Cuts::ADMMASS
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2004-08-04
     */
    class AbsDeltaMeasuredMass 
      : public  LoKi::Function<const LHCb::Particle*>
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
    
    /** @class DeltaMeasuredMassChi2 
     *  evaluator of delta meadured mass in chi2 units 
     *
     *  @see LoKi::Cuts::CHI2M
     *  @see LoKi::Cuts::CHI2MASS
     *
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2003-01-17
     */
    class DeltaMeasuredMassChi2 
      : public LoKi::Function<const LHCb::Particle*>
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
    
    /** @struct ConfidenceLevel
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
    struct ConfidenceLevel 
      : public LoKi::Function<const LHCb::Particle*>
    {
      /// clone method (mandatory!)
      virtual ConfidenceLevel* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    };

    /** @struct Weight
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
    struct Weight
      : public LoKi::Function<const LHCb::Particle*>
    {
      /// clone method (mandatory!)
      virtual Weight* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    };

    /** @struct IsBasic
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
    struct IsBasic
      : public LoKi::Predicate<const LHCb::Particle*>
    {
      /// clone method (mandatory!)
      virtual IsBasic* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    };
    
    /** @struct NumberOfDaughters
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
    struct NumberOfDaughters
      : public LoKi::Function<const LHCb::Particle*>
    {
      /// clone method (mandatory!)
      virtual NumberOfDaughters* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    };


    /** @class HasInfo
     *  Trivial predicate which evaluates LHCb::Particle::hasInfo
     *  function
     *
     *  It relies on the method LHCb::Particle::hasInfo
     *
     *  @see LHCb::Particle
     *  @see LoKi::Cuts::HASINFO 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    class HasInfo
      : public LoKi::Predicate<const LHCb::Particle*>
    {
    public:
      /** constructor from "info"
       *  @param key info index/mark/key
       */
      HasInfo ( const int key ) ;
      /// copy constructor 
      HasInfo ( const HasInfo& right ) ;
      /// destructor 
      virtual ~HasInfo();
      /// clone method (mandatory!)
      virtual HasInfo* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    private:
      // the defautl constructor is disabled 
      HasInfo();
    private:
      int m_info ;
    };
    
    /** @class Info
     *  Trivial function which evaluates LHCb::Particle::info
     *  
     *  It relies on the method LHCb::Particle::info
     *
     *  @see LHCb::Particle
     *  @see LoKi::Cuts::INFO 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    class Info
      : public LoKi::Function<const LHCb::Particle*>
    {
    public:
      /** constructor from "info"
       *  @param key info index/mark/key
       *  @param def default valeu for info 
       *  @param bad bad value to be retured for invalid particle 
       */
      Info 
      ( const int    key , 
        const double def , 
        const double bad ) ;
      /** constructor from "info"
       *  @param key info index/mark/key
       *  @param def default valeu for info 
       */
      Info 
      ( const int    key , 
        const double def ) ;
      /// copy constructor 
      Info ( const Info& right ) ;
      /// destructor 
      virtual ~Info();
      /// clone method (mandatory!)
      virtual Info* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    private:
      // the default constructor is disabled 
      Info();
    private:
      int    m_key ;
      double m_def ;
      double m_bad ;
    };

    /** @struct HasProto
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
    struct HasProto
      : public LoKi::Predicate<const LHCb::Particle*>
    {
      /// clone method (mandatory!)
      virtual HasProto* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    };

    /** @struct HasVertex
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
    struct HasVertex
      : public LoKi::Predicate<const LHCb::Particle*>
    {
      /// clone method (mandatory!)
      virtual HasVertex* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    };
      
  } ; // end of namespace LoKi::Particles
} ; // end of namespace LoKi 


// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PARTICLES0_H
// ============================================================================
