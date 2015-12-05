// $Id$
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
#include "LoKi/PidFunctions.h"
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
 *  By usage of this code one clearly states the disagreement 
 *  with the smear campaign of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 *  
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-01-28 
 *  
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
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
    class GAUDI_API HasKey
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Predicate
    {
    public:
      // ======================================================================
      /// Default Constructor
      HasKey() { }
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
    class  GAUDI_API Key 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {
    public:
      // ======================================================================
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
    class GAUDI_API InTES
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
    class GAUDI_API Identifier
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {  
    public:
      // ======================================================================
      /// Default Contructor
      Identifier() { }
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
    class GAUDI_API AbsIdentifier
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {  
    public:
      // ======================================================================
      /// Default Constructor
      AbsIdentifier() { }
      /// clone method (mandatory!)
      virtual AbsIdentifier* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
      // ======================================================================
    } ;
    // ========================================================================
    /** @class IsID 
     *  new verison of PID-comparison 
     */
    // ========================================================================
    class GAUDI_API IsID  
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Predicate
      , public LoKi::Pids::GetPids 
    {
    public:
      // ======================================================================
      IsID ( const int                            id  ) ;
      IsID ( const long                           id  ) ;
      IsID ( const unsigned long                  id  ) ;
      IsID ( const LHCb::ParticleID&              id  ) ;
      IsID ( const std::string&                   id  ) ;
      IsID ( const std::vector<int>&              ids ) ;
      IsID ( const std::vector<long>&             ids ) ;
      IsID ( const std::vector<unsigned int>&     ids ) ;
      IsID ( const std::vector<unsigned long>&    ids ) ;
      IsID ( const std::vector<LHCb::ParticleID>& ids ) ;
      IsID ( const std::vector<std::string>&      ids ) ;
      IsID ( const LoKi::Pids::GetPids&           ids ) ;
      // ======================================================================
      virtual IsID* clone() const ;
      /// the only one essential method
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================      
      IsID() ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class IsNotID 
     *  new verison of PID-comparison 
     */
    // ========================================================================
    class GAUDI_API IsNotID : public LoKi::Particles::IsID 
    {
    public:
      // ======================================================================
      IsNotID ( const int                            id  ) ;
      IsNotID ( const long                           id  ) ;
      IsNotID ( const unsigned long                  id  ) ;
      IsNotID ( const LHCb::ParticleID&              id  ) ;
      IsNotID ( const std::string&                   id  ) ;
      IsNotID ( const std::vector<int>&              ids ) ;
      IsNotID ( const std::vector<long>&             ids ) ;
      IsNotID ( const std::vector<unsigned int>&     ids ) ;
      IsNotID ( const std::vector<unsigned long>&    ids ) ;
      IsNotID ( const std::vector<LHCb::ParticleID>& ids ) ;
      IsNotID ( const std::vector<std::string>&      ids ) ;
      IsNotID ( const LoKi::Pids::GetPids&           ids ) ;
      // ======================================================================
      virtual IsNotID* clone() const ;
      /// the only one essential method
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================      
      IsNotID() ;
      // ======================================================================
    } ;
    // ========================================================================
    class GAUDI_API IsAbsID : public LoKi::Particles::IsID   
    {
    public:
      // ======================================================================
      IsAbsID ( const int                            id  ) ;
      IsAbsID ( const long                           id  ) ;
      IsAbsID ( const unsigned long                  id  ) ;
      IsAbsID ( const LHCb::ParticleID&              id  ) ;
      IsAbsID ( const std::string&                   id  ) ;
      IsAbsID ( const std::vector<int>&              ids ) ;
      IsAbsID ( const std::vector<long>&             ids ) ;
      IsAbsID ( const std::vector<unsigned int>&     ids ) ;
      IsAbsID ( const std::vector<unsigned long>&    ids ) ;
      IsAbsID ( const std::vector<LHCb::ParticleID>& ids ) ;
      IsAbsID ( const std::vector<std::string>&      ids ) ;
      IsAbsID ( const LoKi::Pids::GetPids&           ids ) ;
      // ======================================================================
      virtual IsAbsID* clone() const ;
      /// the only one essential method
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================      
      IsAbsID() ;
      // ======================================================================
    } ;
    // ========================================================================
    class GAUDI_API IsNotAbsID : public LoKi::Particles::IsAbsID
    {
    public:
      // ======================================================================
      IsNotAbsID ( const int                            id  ) ;
      IsNotAbsID ( const long                           id  ) ;
      IsNotAbsID ( const unsigned long                  id  ) ;
      IsNotAbsID ( const LHCb::ParticleID&              id  ) ;
      IsNotAbsID ( const std::string&                   id  ) ;
      IsNotAbsID ( const std::vector<int>&              ids ) ;
      IsNotAbsID ( const std::vector<long>&             ids ) ;
      IsNotAbsID ( const std::vector<unsigned int>&     ids ) ;
      IsNotAbsID ( const std::vector<unsigned long>&    ids ) ;
      IsNotAbsID ( const std::vector<LHCb::ParticleID>& ids ) ;
      IsNotAbsID ( const std::vector<std::string>&      ids ) ;
      IsNotAbsID ( const LoKi::Pids::GetPids&           ids ) ;
      // ======================================================================
      virtual IsNotAbsID* clone() const ;
      /// the only one essential method
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================      
      IsNotAbsID() ;
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
    class GAUDI_API Charge 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {
    public:
      // ======================================================================
      /// Default Constructor
      Charge() { }
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
    class GAUDI_API SumCharge : public Charge 
    {  
    public:
      // ======================================================================
      /// Default Constructor
      SumCharge() { }
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
    class GAUDI_API Momentum 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function 
    {
    public:
      // ======================================================================
      /// Default Contructor
      Momentum() { }
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
    class GAUDI_API Momentum2 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function 
    {
    public:
      // ======================================================================
      /// Default Contructor
      Momentum2() { }
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
    class GAUDI_API Energy 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {
    public:
      // ======================================================================
      /// Default Contructor
      Energy() { }
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
    class GAUDI_API TransverseMomentum 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {
    public:
      // ======================================================================
      /// Default Contructor
      TransverseMomentum() { }
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
    class GAUDI_API MomentumX 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {    
    public:
      // ======================================================================
      /// Default Contructor
      MomentumX() { }
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
    class GAUDI_API MomentumY
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {     
    public:
      // ======================================================================
      /// Default Contructor
      MomentumY() { }
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
    class GAUDI_API MomentumZ 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {     
    public:
      // ======================================================================
      /// Default Contructor
      MomentumZ() { }
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
     *  evaluator of the pseudorapidity of the particle 
     *
     *  @see LoKi::Cuts::ETA 
     *  @see LoKi::Cuts::Y
     *  @see LoKi::Cuts::Y0 
     *
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class GAUDI_API PseudoRapidity 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {    
    public:
      // ======================================================================
      /// Default Contructor
      PseudoRapidity() { }
      /// clone method (mandatory!)
      virtual PseudoRapidity* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
      /// get eta
      double eta ( argument                    p ) const 
      { return eta ( p -> momentum() ) ; }
      /// get eta
      double eta ( const Gaudi::LorentzVector& v ) const 
      { return v . Eta () ; }
      /// get rapidity 
      double y  ( argument                    p  ) const 
      { return y ( p->momentum() ) ; }
      /// get rapidity 
      double y  ( const Gaudi::LorentzVector& v  ) const ;
      /// get rapidity0 
      double y0 ( argument                    p  ) const 
      { return y ( p->momentum() ) ; }
      /// get rapidity 
      double y0 ( const Gaudi::LorentzVector& v  ) const ;
      // ======================================================================
    };
    // ========================================================================
    /** @class Rapidity
     *  evaluator of the rapidity of the particle 
     *
     *  \f$ y = \frac{1}{2}\log \frac{ E - p_z }{ E + p_z } \f$ 
     *
     *  @see LoKi::Cuts::Y 
     *
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class GAUDI_API Rapidity : public PseudoRapidity 
    {    
    public:
      // ======================================================================
      /// Default Contructor
      Rapidity() { }
      /// clone method (mandatory!)
      virtual Rapidity* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    };
    // ========================================================================
    /** @class Rapidity0
     *  evaluator of the rapidity of the particle 
     *
     *  \f$ y_0 = \frac{1}{2}\log \frac{ E - p }{ E + p } \f$ 
     *
     *  @see LoKi::Cuts::Y0
     *
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class GAUDI_API Rapidity0 : public Rapidity 
    {    
    public:
      // ======================================================================
      /// Default Contructor
      Rapidity0() { }
      /// clone method (mandatory!)
      virtual Rapidity0* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    };
    // ========================================================================    
    /** @class Phi
     *  evaluator of the 'Phi' of the particle 
     *  
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class GAUDI_API Phi 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {    
    public:
      // ======================================================================
      /// Default Contructor
      Phi() { }
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
    class GAUDI_API Theta 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {    
    public:
      // ======================================================================
      /// Default Contructor
      Theta() { }
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
    class GAUDI_API Mass 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {    
    public:
      // ======================================================================
      /// Default Constructor
      Mass() { }
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
    class GAUDI_API MeasuredMass 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {    
    public:
      // ======================================================================
      /// Default Constructor
      MeasuredMass() { }
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
    class GAUDI_API InvariantMass 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {
    public:      
      // ======================================================================
      /// type for container of indices 
      typedef std::vector<unsigned int> Indices   ;
      /** constructor with index of daughter particle 
       *  @attention <c>index==0</c> corresponds to the particle 
       */
      InvariantMass
      ( const unsigned int index = 0 ) ;
      /** constructor with pair of indices of daughter particle 
       *  @attention <c>index==0</c> corresponds to the particle 
       */
      InvariantMass
      ( const unsigned int index1 , 
        const unsigned int index2 ) ;
      /** constructor with triplet of indices of daughter particle 
       *  @attention <c>index==0</c> corresponds to the particle 
       */
      InvariantMass 
      ( const unsigned int index1 , 
        const unsigned int index2 , 
        const unsigned int index3 ) ;
      /** constructor with four indices of daughter particle 
       *  @attention <c>index==0</c> corresponds to the particle 
       */
      InvariantMass 
      ( const unsigned int index1 , 
        const unsigned int index2 , 
        const unsigned int index3 ,
        const unsigned int index4 ) ;
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
    class GAUDI_API DeltaMass : public LoKi::Particles::Mass 
    {      
    public:
      // ======================================================================
      /** constructor  
       *  @param mass nominal mass 
       */
      DeltaMass ( const double mass ) ;
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
    class GAUDI_API AbsDeltaMass : public DeltaMass 
    {      
    public:
      // ======================================================================      
      /** constructor  
       *  @param mass nominal mass 
       */
      AbsDeltaMass ( const double mass ) ;
      /** constructor 
       *  @param pp particle property 
       */
      AbsDeltaMass ( const LHCb::ParticleProperty& pp ) ;
      /** constructor 
       *  @param name particle name 
       *  @param ppsvc LHCb::ParticleProperty service 
       */
      AbsDeltaMass 
      ( const std::string&          name      , 
        LHCb::IParticlePropertySvc* ppsvc = 0 ) ;
      /** constructor 
       *  @param pid  particle ID 
       *  @param ppsvc LHCb::ParticleProperty service 
       */
      AbsDeltaMass
      ( const LHCb::ParticleID&     pid       , 
        LHCb::IParticlePropertySvc* ppsvc = 0 ) ;
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
    class GAUDI_API DeltaMeasuredMass : public DeltaMass 
    {      
    public:
      // ======================================================================
      /// constructor  
      DeltaMeasuredMass ( const double mass ) ;
      /** constructor 
       *  @param pp particle property 
       */
      DeltaMeasuredMass ( const LHCb::ParticleProperty& pp ) ;
      /** constructor 
       *  @param name particle name 
       *  @param ppsvc LHCb::ParticleProperty service 
       */
      DeltaMeasuredMass 
      ( const std::string&          name      , 
        LHCb::IParticlePropertySvc* ppsvc = 0 ) ;
      /** constructor 
       *  @param pid  particle ID 
       *  @param ppsvc LHCb::ParticleProperty service 
       */
      DeltaMeasuredMass 
      ( const LHCb::ParticleID&     pid       , 
        LHCb::IParticlePropertySvc* ppsvc = 0 ) ;
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
    class GAUDI_API AbsDeltaMeasuredMass : public DeltaMeasuredMass 
    {
    public :
      // ======================================================================
      /** constructor  from particle mass 
       *  @param mass particle mass 
       */
      AbsDeltaMeasuredMass ( const double mass ) ;
      /** constructor 
       *  @param pp particle property 
       */
      AbsDeltaMeasuredMass ( const LHCb::ParticleProperty& pp ) ;
      /** constructor 
       *  @param name particle name 
       *  @param ppsvc  LHCb::ParticleProperty service 
       */
      AbsDeltaMeasuredMass
      ( const std::string&          name      , 
        LHCb::IParticlePropertySvc* ppsvc = 0 ) ;
      /** constructor 
       *  @param pid  particle ID 
       *  @param ppsvc LHCb::ParticleProperty service 
       */
      AbsDeltaMeasuredMass 
      ( const LHCb::ParticleID&     pid       , 
        LHCb::IParticlePropertySvc* ppsvc = 0 ) ;
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
    class GAUDI_API DeltaMeasuredMassChi2 : public DeltaMeasuredMass 
    {      
    public:
      // ======================================================================
      /// constructor  
      DeltaMeasuredMassChi2 ( const double mass   ) ;
      /** constructor 
       *  @param pp particle property 
       */
      DeltaMeasuredMassChi2 ( const LHCb::ParticleProperty& pp ) ;
      /** constructor 
       *  @param name particle name 
       *  @param ppsvc ParticleProperty service 
       */
      DeltaMeasuredMassChi2 
      ( const std::string&          name      , 
        LHCb::IParticlePropertySvc* ppsvc = 0 ) ;
      /** constructor 
       *  @param pid  particle ID 
       *  @param ppsvc ParticleProperty service 
       */
      DeltaMeasuredMassChi2 
        ( const LHCb::ParticleID&     pid       , 
          LHCb::IParticlePropertySvc* ppsvc = 0 ) ;
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
    class GAUDI_API DeltaMassChi2 : public DeltaMass 
    {      
    public:
      // ======================================================================
      /// constructor  
      DeltaMassChi2 ( const double mass ) ;
      /** constructor 
       *  @param pp particle property 
       */
      DeltaMassChi2 ( const LHCb::ParticleProperty& pp ) ;
      /** constructor 
       *  @param name particle name 
       *  @param ppsvc ParticleProperty service 
       */
      DeltaMassChi2 
      ( const std::string&          name       , 
        LHCb::IParticlePropertySvc* ppsvc  = 0 ) ;
      /** constructor 
       *  @param pid  particle ID 
       *  @param ppsvc ParticleProperty service 
       */
      DeltaMassChi2 
      ( const LHCb::ParticleID&     pid       , 
        LHCb::IParticlePropertySvc* ppsvc = 0 ) ;
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
    class GAUDI_API ConfidenceLevel 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {
    public:
      // ======================================================================
      /// Default Constructor
      ConfidenceLevel() { }
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
    class GAUDI_API Weight
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {
    public:
      // ======================================================================
      /// Default Constructor
      Weight() { }
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
    class GAUDI_API IsBasic 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Predicate
    {
    public:
      // ======================================================================
      /// Default Constructor
      IsBasic() { }
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
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    class GAUDI_API NumberOfDaughters 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {
    public:
      // ======================================================================
      /// Default Constructor
      NumberOfDaughters() { }
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
    class GAUDI_API HasProto 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Predicate
    {
    public:
      // ======================================================================
      /// Default Constructor
      HasProto() { }
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
    class GAUDI_API HasVertex 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Predicate
    {
    public:
      // ======================================================================
      /// Default Constructor
      HasVertex() { }
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
    class GAUDI_API HasTrack
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Predicate
    {
    public:
      // ======================================================================
      /// Default Constructor
      HasTrack() { }
      /// Destructor
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
    class GAUDI_API TransverseMomentumRel 
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
      /** evaluate the transverse momentum versus the direction 
       *  @param p (INPUT) the 4-momentum
       *  @param d (INPUT) the 3-direction 
       *  @reutrn the transverse momentum versus the direction 
       */
      double ptDir 
      ( const LoKi::LorentzVector& p , 
        const LoKi::ThreeVector&   d ) const ;
      // ======================================================================
      /** evaluate the 'corrected' mass of the particle 
       *  \f[  \vec{i} = \sqrt{ M^2 + 
       *            \left|p_{T}^{\prime}\right|^2 } +
       *            \left|p_{T}^{\prime}\right|, \f] 
       *  where \f$ \left|p_{T}^{\prime}\right|\f$ stands for the 
       *  transverse momentum relative to the fligth direction 
       *
       *  @param p  (INPUT) the 4-momentum
       *  @param d  (INPUT) the 3-direction 
       *  @return the corrected mass
       *  @see LoKi::TransverseMomentumFlight::ptDir 
       *  @see LoKi::TransverseMomentumRel::ptDir 
       *  @thanks Mike Williams
       */
      double mCorrDir
      ( const LoKi::LorentzVector& p  , 
        const LoKi::ThreeVector&   d  ) const ;
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
    class GAUDI_API TransverseMomentumQ 
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
    class GAUDI_API NominalMass 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {
    public:
      // ======================================================================
      /// Default Constructor
      NominalMass() { }
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
    /** @class Cov2 
     *  trivial accessor to elements of particle covariance 7x7-matrix
     *  @see LoKi::Cuts::PCOV2
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2010-12-09
     */
    class GAUDI_API Cov2 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {
    public:
      // ======================================================================
      /// constructor from indices 
      Cov2 ( const unsigned short i , 
             const unsigned short j ) ;
      /// MANDATORY: virtual destructor 
      virtual ~Cov2 () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Cov2* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      Cov2 () ;                          // the default constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      unsigned short m_i ;
      unsigned short m_j ;      
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Perr2
     *  trivial accessor to get estimaet for momentum dispersion 
     *  \f$ \sigma^2(\left| \vec{p} \right|) \f$ 
     *  @see LoKi::Cuts::PERR2 
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2010-12-09
     */
    class GAUDI_API Perr2
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {
    public:
      // ======================================================================
      /// Default Constructor
      Perr2() { }
      /// MANDATORY: virtual destructor 
      virtual ~Perr2 () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Perr2* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class PTerr2
     *  trivial accessor to get estimate for transverse momentum dispersion 
     *  \f$ \sigma^2( p_T) \f$ 
     *  @see LoKi::Cuts::PTERR2 
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2010-12-09
     */
    class GAUDI_API PTerr2
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {
    public:
      // ======================================================================
      /// Default Constructor
      PTerr2() { }
      /// MANDATORY: virtual destructor 
      virtual ~PTerr2 () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  PTerr2* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class M2err2
     *  trivial accessor to get estimate for M^2 dispersion 
     *  \f$ \sigma^2( M^2 ) \f$ 
     *  @see LoKi::Cuts::M2ERR2
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2010-12-09
     */
    class GAUDI_API M2err2
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {
    public:
      // ======================================================================
      /// Default Constructor
      M2err2() { }
      /// MANDATORY: virtual destructor 
      virtual ~M2err2 () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  M2err2* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Chi2VX
     *  trivial accessor to get estimate chi2 of the vertex 
     *  @see LoKi::Cuts::CHI2VX 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2015-12-05
     */
    class GAUDI_API Chi2VX
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {
    public:
      // ======================================================================
      /// Default Constructor
      Chi2VX(){} ;
      /// MANDATORY: virtual destructor 
      virtual ~Chi2VX () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Chi2VX* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Chi2VXnDOF
     *  trivial accessor to get estimate chi2 of the vertex per degree of freedom 
     *  @see LoKi::Cuts::CHI2VXNDOF
     *  @see LoKi::Cuts::CHI2VXNDF
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2015-12-05
     */
    class GAUDI_API Chi2VXnDOF
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {
    public:
      // ======================================================================
      /// Default Constructor
      Chi2VXnDOF(){} ;
      /// MANDATORY: virtual destructor 
      virtual ~Chi2VXnDOF () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Chi2VXnDOF* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    } ;
    // ========================================================================
  } //                                         end of namespace LoKi::Particles
  // ==========================================================================
  namespace Cuts 
  {
    // ========================================================================
    // Suppress Intel compiler warnings about missing default constructor
    // In this case the compiler generated constructor is fine, since there are
    // no member data to be initialised
#ifdef __INTEL_COMPILER
#pragma warning(disable:854)
#pragma warning(push)
#endif
    // ========================================================================
    /** @var CHI2VX
     *  get chi2 of vertex 
     * 
     *  @code 
     * 
     *   const LHCb::Particle* p = ... ;
     *
     *   const double chi2 = CHI2VX ( p ) ;
     *
     *  @endcode 
     *  @see LoKi::Particles::Chi2VX
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2010-12-09     
     */ 
    const LoKi::Particles::Chi2VX                                      CHI2VX ;
    // ========================================================================
    /** @var CHI2VXNDF
     *  get  reduced chi2 of vertex  (per degree of freedon)
     * 
     *  @code 
     * 
     *   const LHCb::Particle* p = ... ;
     *
     *   const double chi2ndf = CHI2VXNDF ( p ) ;
     *
     *  @endcode 
     *  @see LoKi::Particles::Chi2VXnDOF
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2010-12-09     
     */ 
    const LoKi::Particles::Chi2VXnDOF                               CHI2VXNDF ;
    // ========================================================================
    /** @var CHI2VXNDOF
     *  get  reduced chi2 of vertex  (per degree of freedon)
     * 
     *  @code 
     * 
     *   const LHCb::Particle* p = ... ;
     *
     *   const double chi2ndf = CHI2VXNDOF ( p ) ;
     *
     *  @endcode 
     *  @see LoKi::Particles::Chi2VXnDOF
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2010-12-09     
     */ 
    const LoKi::Particles::Chi2VXnDOF                              CHI2VXNDOF ;
    // ========================================================================
    /** @typedef PCOV2 
     *  Trivial accessor toe elemenst of particel covarinacte matrix :
     *  @code 
     * 
     *   const PCPOV2 pXpX = PCCOV2 ( 0 , 0 ) ;
     *   const PCPOV2  xPx = PCCOV2 ( 4 , 0 ) ;
     *
     *   const LHCb::Particle* p = ... ;
     *
     *
     *   const double cov2_PxPx = pXpX ( p ) ;
     *   const double cov2_xPx  =  xPx ( p ) ;
     *
     *  @endcode 
     *  @see LoKi::Particles::Cov2 
     *  @see LHCb::Particle::momCovMatrix 
     *  @see LHCb::Particle::posCovMatrix 
     *  @see LHCb::Particle::posMomCovMatrix 
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2010-12-09
     */
    typedef LoKi::Particles::Cov2                                       PCOV2 ;
    // ========================================================================
    /** @var PERR2 
     *  Simple estimator of 3-momentum dispersion
     *  \f$ \sigma^2 ( \left| \vec{p} \right| ) \f$
     * 
     *  @code 
     * 
     *   const LHCb::Particle* p = ... ;
     *
     *   const double sigma2p = PERR2 ( p ) ;
     *
     *  @endcode 
     *  @see Gaudi::Math::sigma2p 
     *  @see LoKi::Particles::Perr2 
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2010-12-09     
     */ 
    const LoKi::Particles::Perr2                                        PERR2 ;
    // ========================================================================
    /** @var PTERR2 
     *  Simple estimator of 3-momentum dispersion
     *  \f$ \sigma^2 ( p_T ) \f$
     * 
     *  @code 
     * 
     *   const LHCb::Particle* p = ... ;
     *
     *   const double sigma2pt = PTERR2 ( p ) ;
     *
     *  @endcode 
     *  @see Gaudi::Math::sigma2pt
     *  @see LoKi::Particles::PTerr2 
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2010-12-09     
     */ 
    const LoKi::Particles::PTerr2                                      PTERR2 ;
    // ========================================================================
    /** @var M2ERR2 
     *  Simple estimator of 3-momentum dispersion
     *  \f$ \sigma^2 ( M^2 ) \f$
     * 
     *  @code 
     * 
     *   const LHCb::Particle* p = ... ;
     *
     *   const double sigma2m2 = M2ERR2 ( p ) ;
     *
     *  @endcode 
     *  @see Gaudi::Math::sigma2mass2
     *  @see LoKi::Particles::M2err2 
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2010-12-09     
     */ 
    const LoKi::Particles::M2err2                                      M2ERR2 ;
    // ========================================================================
#ifdef __INTEL_COMPILER
#pragma warning(pop) 
#endif    
    // ========================================================================
  } //                                              end of namespace LoKi::Cuts  
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_PARTICLES0_H
// ============================================================================
