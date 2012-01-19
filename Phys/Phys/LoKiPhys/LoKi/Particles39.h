// $Id: $
// ============================================================================
#ifndef LOKI_PARTICLES39_H 
#define LOKI_PARTICLES39_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKiCore 
// ============================================================================
// #include "LoKi/Listener.h"
#include "LoKi/UniqueKeeper.h"
// ============================================================================
// LoKiPhys 
// ============================================================================
#include "LoKi/PhysTypes.h"
// ============================================================================ 
/** @file LoKi/Particles39.h
 *
 *  This file is a part of LoKi project -
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas,
 *  contributions and advices from G.Raven, J.van Tilburg,
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *   By usage of this code one clearly states the disagreement 
 *    with the smear campaign of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 * 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2012-01-17
 */
namespace LoKi
{
  // ==========================================================================
  namespace Particles
  {
    // ========================================================================
    /** @class MinMaxDistance 
     *  simple class to calculate min/max "distance" between 
     *  the particle and set of particles 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2012-01-17
     */
    class MinMaxDistance 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
      , public LoKi::UniqueKeeper<LHCb::Particle>
    {
    protected:
      // ======================================================================
      /// the actual type of "distance"-function
      typedef double (*dist_func) ( const LHCb::Particle* , 
                                    const LHCb::Particle* ) ;
      // ======================================================================
    public:
      // ======================================================================
      MinMaxDistance
      ( const bool                   minval                           ,  
        const LHCb::Particle::Range& parts  = LHCb::Particle::Range() ,
        dist_func                    dist   = 0                       ) ;
      /// MANDATORY: destructor 
      virtual ~MinMaxDistance() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  MinMaxDistance* clone() const ;
      /// MANDATORY: the only essential method
      virtual result_type operator() ( argument p ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      MinMaxDistance () ;                // the default constructor is disabled 
      // ======================================================================
    protected:
      // ======================================================================
      double distance 
      ( const LHCb::Particle* p1 , 
        const LHCb::Particle* p2 ) const { return (*m_distance)( p1 , p2 ) ; }
      // ======================================================================
      /// the actual computation
      double distance ( const LHCb::Particle* p ) const ;
      bool   minimum  () const { return m_minimum ;}
      // ======================================================================
    private:
      // ======================================================================
      /// minmax flag 
      bool      m_minimum  ;                           // minmax flag 
      /// the distance function 
      dist_func m_distance ;                           // the distance function 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class MinMaxDistanceWithSource 
     *  simple class to calculate min/max "distance" between 
     *  the particle and set of particles 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2012-01-17
     */
    class MinMaxDistanceWithSource
      : public LoKi::Particles::MinMaxDistance 
    {
    public:
      // ======================================================================
      MinMaxDistanceWithSource
      ( const bool                                                minval     ,  
        const LoKi::BasicFunctors<const LHCb::Particle*>::Source& source     , 
        dist_func                                                 dist   = 0 ) ;
      /// MANDATORY: destructor 
      virtual ~MinMaxDistanceWithSource() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  MinMaxDistanceWithSource* clone() const ;
      /// MANDATORY: the only essential method
      virtual result_type operator() ( argument p ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      MinMaxDistanceWithSource () ;      // the default constructor is disabled 
      // ======================================================================
    protected:
      // ======================================================================
      typedef LoKi::BasicFunctors<const LHCb::Particle*>::Source   iSource ;
      typedef LoKi::Assignable<iSource>::Type                       Source ;
      // ======================================================================
      const iSource& source() const { return m_source ; }
      // ======================================================================      
    private:
      // ======================================================================
      /// the source 
      Source m_source ;                                           // the source 
      // ======================================================================      
    } ;
    // ========================================================================
    /** @class MinDR2 
     *  Evalue the minimal \f$\left(\Delta R\right)^2\f$ with respect
     *  to set of particles 
     *  @see LoKi::Cuts::MINDR2 
     *  @author Vanya BELYAEV Ivan.BElyaev@itep.ru
     *  @date 2012-01-18
     */
    class MinDR2 : public MinMaxDistanceWithSource
    {
    public:
      // ======================================================================
      /// constructor from the source 
      MinDR2
      ( const LoKi::BasicFunctors<const LHCb::Particle*>::Source& source ) ;
      /// constructor from the location & filter  
      MinDR2
      ( const std::string& location , 
        const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate& cut = 
        LoKi::BasicFunctors<const LHCb::Particle*>::BooleanConstant( true ) ) ;
      /// constructor from the locations & filter  
      MinDR2
      ( const std::vector<std::string>& locations , 
        const  LoKi::BasicFunctors<const LHCb::Particle*>::Predicate& cut = 
        LoKi::BasicFunctors<const LHCb::Particle*>::BooleanConstant( true ) ) ;
      /// MANDATORY: virtual destructor 
      virtual ~MinDR2() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  MinDR2* clone() const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      MinDR2 () ;                        // the default constructor is disabled 
      // ======================================================================
    };
    // ========================================================================
    /** @class MaxDR2 
     *  Evalue the maximal \f$\left(\Delta R\right)^2\f$ with respect
     *  to set of particles 
     *  @see LoKi::Cuts::MAXDR2 
     *  @author Vanya BELYAEV Ivan.BElyaev@itep.ru
     *  @date 2012-01-18
     */
    class MaxDR2 : public MinMaxDistanceWithSource
    {
    public:
      // ======================================================================
      /// constructor from the source 
      MaxDR2
      ( const LoKi::BasicFunctors<const LHCb::Particle*>::Source& source ) ;
      /// constructor from the location & filter  
      MaxDR2
      ( const std::string& location , 
        const  LoKi::BasicFunctors<const LHCb::Particle*>::Predicate& cut = 
        LoKi::BasicFunctors<const LHCb::Particle*>::BooleanConstant ( true ) ) ;
      /// constructor from the locations & filter  
      MaxDR2
      ( const std::vector<std::string>& locations , 
        const  LoKi::BasicFunctors<const LHCb::Particle*>::Predicate& cut = 
        LoKi::BasicFunctors<const LHCb::Particle*>::BooleanConstant ( true ) ) ;
      /// MANDATORY: virtual destructor 
      virtual ~MaxDR2() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  MaxDR2* clone() const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      MaxDR2 () ;                        // the default constructor is disabled 
      // ======================================================================
    };
    // ======================================================================
    /** @class MinKL 
     *  Evaluate the minimal  KK-divergency with respect
     *  to set of particles 
     *  @see LoKi::Cuts::MINKL 
     *  @author Vanya BELYAEV Ivan.BElyaev@itep.ru
     *  @date 2012-01-18
     */
    class MinKL : public MinMaxDistanceWithSource
    {
    public:
      // ======================================================================
      /// constructor from the source 
      MinKL
      ( const LoKi::BasicFunctors<const LHCb::Particle*>::Source& source ) ;
      /// constructor from the location & filter  
      MinKL
      ( const std::string& location , 
        const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate& cut = 
        LoKi::BasicFunctors<const LHCb::Particle*>::BooleanConstant( true ) ) ;
      /// constructor from the locations & filter  
      MinKL
      ( const std::vector<std::string>& locations , 
        const  LoKi::BasicFunctors<const LHCb::Particle*>::Predicate& cut = 
        LoKi::BasicFunctors<const LHCb::Particle*>::BooleanConstant( true ) ) ;
      /// MANDATORY: virtual destructor 
      virtual ~MinKL () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  MinKL* clone() const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      MinKL () ;                        // the default constructor is disabled 
      // ======================================================================
    };
    // ========================================================================
    /** @class MinAngle 
     *  Evaluate the minimal angle with respect
     *  to set of particles 
     *  @see LoKi::Cuts::MINANGLE 
     *  @see LoKi::PhysKinematics::deltaAlpha
     *  @author Vanya BELYAEV Ivan.BElyaev@itep.ru
     *  @date 2012-01-18
     */
    class MinAngle : public MinMaxDistanceWithSource
    {
    public:
      // ======================================================================
      /// constructor from the source 
      MinAngle
      ( const LoKi::BasicFunctors<const LHCb::Particle*>::Source& source ) ;
      /// constructor from the location & filter  
      MinAngle
      ( const std::string& location , 
        const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate& cut = 
        LoKi::BasicFunctors<const LHCb::Particle*>::BooleanConstant( true ) ) ;
      /// constructor from the locations & filter  
      MinAngle
      ( const std::vector<std::string>& locations , 
        const  LoKi::BasicFunctors<const LHCb::Particle*>::Predicate& cut = 
        LoKi::BasicFunctors<const LHCb::Particle*>::BooleanConstant( true ) ) ;
      /// MANDATORY: virtual destructor 
      virtual ~MinAngle () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  MinAngle* clone() const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      MinAngle () ;                      // the default constructor is disabled 
      // ======================================================================
    };
    // ========================================================================
    /** @class MinM2 
     *  Evaluate the minnmal mass-difference with respect to set of particles 
     *  @see LoKi::Cuts::MINM2
     *  @see LoKi::PhysKinematics::minM2
     *  @author Vanya BELYAEV Ivan.BElyaev@itep.ru
     *  @date 2012-01-18
     */
    class MinM2 : public MinMaxDistanceWithSource
    {
    public:
      // ======================================================================
      /// constructor from the source 
      MinM2
      ( const LoKi::BasicFunctors<const LHCb::Particle*>::Source& source ) ;
      /// constructor from the location & filter  
      MinM2
      ( const std::string& location , 
        const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate& cut = 
        LoKi::BasicFunctors<const LHCb::Particle*>::BooleanConstant( true ) ) ;
      /// constructor from the locations & filter  
      MinM2
      ( const std::vector<std::string>& locations , 
        const  LoKi::BasicFunctors<const LHCb::Particle*>::Predicate& cut = 
        LoKi::BasicFunctors<const LHCb::Particle*>::BooleanConstant( true ) ) ;
      /// MANDATORY: virtual destructor 
      virtual ~MinM2() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  MinM2* clone() const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      MinM2 () ;                        // the default constructor is disabled 
      // ======================================================================
    };
    // ========================================================================
    /** @class MaxIDs 
     *  Evaluate the maximal overlap (in term of LHCIDs fraction)
     *  to set of particles 
     *  @see LoKi::Cuts::MAXIDS 
     *  @author Vanya BELYAEV Ivan.BElyaev@itep.ru
     *  @date 2012-01-18
     */
    class MaxIDs : public MinMaxDistanceWithSource
    {
    public:
      // ======================================================================
      /// constructor from the source 
      MaxIDs
      ( const LoKi::BasicFunctors<const LHCb::Particle*>::Source& source ) ;
      /// constructor from the location & filter  
      MaxIDs
      ( const std::string& location , 
        const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate& cut = 
        LoKi::BasicFunctors<const LHCb::Particle*>::BooleanConstant( true ) ) ;
      /// constructor from the locations & filter  
      MaxIDs
      ( const std::vector<std::string>& locations , 
        const  LoKi::BasicFunctors<const LHCb::Particle*>::Predicate& cut = 
        LoKi::BasicFunctors<const LHCb::Particle*>::BooleanConstant( true ) ) ;
      /// MANDATORY: virtual destructor 
      virtual ~MaxIDs () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  MaxIDs* clone() const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      MaxIDs () ;                        // the default constructor is disabled 
      // ======================================================================
    };
  } //                                         end of namespace LoKi::Particles
  // ==========================================================================
  namespace Cuts 
  {
    // ========================================================================
    /** @typedef MINDR2
     *  simple functor to evaluate \f$ \min \left( \Delta R \right)^2 \f$
     *  @see LoKi::Particles::MinDR2
     *  @see LoKi::Particles::MinMaxDistanceWithSource 
     *  @see LoKi::Particles::MinMaxDistance 
     *  @see LoKi::PhysKinematics::deltaR2
     *  @see LoKi::Cuts::MAXDR2
     *  @author Vanya BEYAEV  Ivan.BElyaev@itep.ru
     *  @date 2012-01-18
     */
    typedef LoKi::Particles::MinDR2                                   MINDR2 ;
    // ========================================================================
    /** @typedef MAXDR2
     *  simple functor to evaluate \f$ \max \left( \Delta R \right)^2 \f$
     *  @see LoKi::Particles::MaxDR2
     *  @see LoKi::Particles::MinMaxDistanceWithSource 
     *  @see LoKi::Particles::MinMaxDistance 
     *  @see LoKi::PhysKinematics::deltaR2
     *  @see LoKi::Cuts::MINDR2
     *  @author Vanya BEYAEV  Ivan.BElyaev@itep.ru
     *  @date 2012-01-18
     */
    typedef LoKi::Particles::MaxDR2                                   MAXDR2 ;
    // ========================================================================
    /** @typedef MINKL
     *  simple functor to evaluate minimal KL-divergency 
     *  @see LoKi::Particles::MinKL
     *  @see LoKi::PhysKinematics::kullback
     *  @author Vanya BEYAEV  Ivan.BElyaev@itep.ru
     *  @date 2012-01-18
     */
    typedef LoKi::Particles::MinKL                                      MINKL ;
    // ========================================================================
    /** @typedef MAXIDS
     *  simple functor to evaluate the maximal overlap (interms of LHCbIDs)
     *  @see LoKi::Particles::MaxIDs
     *  @see LHCb::HashIDs
     *  @see LHCb::HashIDs::overlap
     *  @author Vanya BEYAEV  Ivan.BElyaev@itep.ru
     *  @date 2012-01-18
     */
    typedef LoKi::Particles::MaxIDs                                   MAXIDS ;
    // ========================================================================
    /** @typedef MINANGLE
     *  simple functor to evaluate the minimal angle between particles 
     *  @see LoKi::Particles::MinAngle
     *  @see LoKi::PhysKinematics::deltaAlpha
     *  @author Vanya BEYAEV  Ivan.BElyaev@itep.ru
     *  @date 2012-01-18
     */
    typedef LoKi::Particles::MinAngle                                MINANGLE ;
    // ========================================================================
    /** @typedef MINM2
     *  simple functor to evaluate the minimal angle between particles
     *  @see LoKi::Particles::MinM2
     *  @see LoKi::PhysKinematics::minM2
     *  @author Vanya BEYAEV  Ivan.BElyaev@itep.ru
     *  @date 2012-01-18
     */
    typedef LoKi::Particles::MinM2                                      MINM2 ;
    // ========================================================================
  } //                                              end of namespace LoKi::Cuts 
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PARTICLES39_H
// ============================================================================
