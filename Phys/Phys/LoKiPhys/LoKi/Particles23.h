// $Id: Particles23.h,v 1.2 2008-12-03 12:37:55 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_PARTICLES23_H 
#define LOKI_PARTICLES23_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi 
// ============================================================================
#include "LoKi/BasicFunctors.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
#include "LoKi/Objects.h"
#include "LoKi/WrongMass.h"
// ============================================================================
/** @file 
 *  Collection of functors to evaluate "wrong" mass 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-10-13
 */
namespace LoKi 
{
  // ==========================================================================
  namespace Particles 
  {
    // ========================================================================
    /** @class WrongMass 
     *  Simple class to evaluate the mass of the mother particle 
     *  using "wrong" mass hypotheses for daughter particles 
     *  @see LoKi::Cuts::WM
     *  @see LoKi::Cuts::WMASS 
     *  @see LoKi::Cuts::AWMASS 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-10-13
     */
    class WrongMass 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function 
    {
    public:
      // ======================================================================
      /// constructor from masses 
      WrongMass ( const double m1 , 
                  const double m2 ) ;
      WrongMass ( const double m1 , 
                  const double m2 , 
                  const double m3 ) ;
      WrongMass ( const double m1 , 
                  const double m2 , 
                  const double m3 ,
                  const double m4 ) ;
      WrongMass ( const std::vector<double>& masses ) ;
      // ======================================================================
      /// constructor from pids 
      WrongMass ( const LHCb::ParticleID& p1 , 
                  const LHCb::ParticleID& p2 ) ;
      WrongMass ( const LHCb::ParticleID& p1 , 
                  const LHCb::ParticleID& p2 ,
                  const LHCb::ParticleID& p3 ) ;
      WrongMass ( const LHCb::ParticleID& p1 , 
                  const LHCb::ParticleID& p2 ,
                  const LHCb::ParticleID& p3 ,
                  const LHCb::ParticleID& p4 ) ;
      WrongMass ( const std::vector<LHCb::ParticleID>& pids ) ;
      // ======================================================================
      /// constructor form names 
      WrongMass ( const std::string& m1 , 
                  const std::string& m2 ) ;
      WrongMass ( const std::string& m1 , 
                  const std::string& m2 ,
                  const std::string& m3 ) ;
      WrongMass ( const std::string& m1 , 
                  const std::string& m2 ,
                  const std::string& m3 ,
                  const std::string& m4 ) ;
      WrongMass ( const std::vector<std::string>& names );  
      // ======================================================================
      /// MANDATORY : virtual destructor 
      virtual ~WrongMass() {}
      /// MANDATORY : clone method ("virtual constructor")
      virtual  WrongMass* clone() const { return new WrongMass(*this) ; }
      /// MANDATORY : the only one essential method 
      virtual result_type operator() ( argument a ) const 
      { return wmass ( a ) ; }
      /// OPTIONAL  : the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    protected:
      // ======================================================================
      /// decode the masses 
      StatusCode decode () const ;
      // ======================================================================
    public:
      // ======================================================================
      /// evaluate the wrong mass 
      double wmass ( argument p ) const ;
      /// evaluate the wrong mass 
      double wmass ( const LHCb::Particle::ConstVector&    p ) const 
      { return wmass ( p.begin() , p.end() ) ; }
      /// evaluate the wrong mass 
      double wmass ( const SmartRefVector<LHCb::Particle>& p ) const 
      { return wmass ( p.begin() , p.end() ) ; }
      // ======================================================================
    public:
      // ======================================================================
      template <class DAUGHTER>
      double wmass ( DAUGHTER first , DAUGHTER last ) const 
      {
        StatusCode sc = check ( first , last ) ;
        if ( sc.isFailure() ) 
        {
          Error ( "Invalid configuration, return InvalidMass" , sc ) ; 
          return LoKi::Constants::InvalidMass ;
        }
        // evaluate the 'wrong mass'
        const LoKi::LorentzVector wm = 
          LoKi::Kinematics::wrongMass 
          ( first , 
            last  , 
            m_masses.begin  () , LoKi::Objects::_VALID_ ) ;
        // 
        return wm.M() ; 
      }
      // ======================================================================
      /// check the configuration 
      StatusCode check () const 
      {
        if ( !m_masses.empty() ) { return StatusCode::SUCCESS ; }
        return decode() ;
      }
      /// check the argument 
      StatusCode check ( argument p ) const  
      {
        if ( 0 == p ) { return Error ( "LHCb::Particle* points to NULL") ; }
        StatusCode sc = check() ;
        if ( sc.isFailure() ) { return Error ("Invalid status", sc ) ; }
        return StatusCode::SUCCESS ;
      }
      /// check the daughters 
      template <class DAUGHTER>
      StatusCode 
      check ( DAUGHTER first , DAUGHTER last ) const 
      {
        StatusCode sc = check() ;
        if ( sc.isFailure() ) { return Error ( "Invalid status" , sc ) ; }
        if ( last - first != (long) m_masses.size() ) 
        { return Error ( " #masses != #daughters" ) ; }
        return StatusCode::SUCCESS ;
      }
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      WrongMass() ; // the default constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      /// the actual list of masses 
      mutable std::vector<double>   m_masses ; // the actual list of masses 
      /// the list of PIDs 
      std::vector<LHCb::ParticleID> m_pids   ; // the list of PIDs 
      /// the list of names 
      std::vector<std::string>      m_names  ; // the list of names 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class DeltaWrongMass 
     *  Simple class to evaluate the the 
     *  delta mass of the mother particle 
     *  using "wrong" mass hypotheses for daughter particles 
     *  with respect to soem "reference" mass 
     *  @see LoKi::Cuts::DWMASS 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-10-13
     */
    class DeltaWrongMass : public WrongMass 
    {
    public:
      // ======================================================================
      /// constructor from masses 
      DeltaWrongMass ( const double            m0 , const WrongMass& wm ) ;
      // ======================================================================
      /// constructor from pids 
      DeltaWrongMass ( const LHCb::ParticleID& m0 , const WrongMass& wm ) ;
      // ======================================================================
      /// constructor form names 
      DeltaWrongMass ( const std::string&      m0 , const WrongMass& wm ) ;
      // ======================================================================
      /// MANDATORY : virtual destructor 
      virtual ~DeltaWrongMass() {}
      /// MANDATORY : clone method ("virtual constructor")
      virtual  DeltaWrongMass* clone() const { return new DeltaWrongMass(*this) ; }
      /// MANDATORY : the only one essential method 
      virtual result_type operator() ( argument a ) const { return dwmass ( a ) ; }
      /// OPTIONAL  : the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// evaluate delta wrong mass 
      double dwmass ( argument p ) const ;
      /// evaluate the wrong mass 
      double dwmass ( const LHCb::Particle::ConstVector&    p ) const  
      { return dwmass ( p.begin() , p.end() ) ; }
      /// evaluate the wrong mass 
      double dwmass ( const SmartRefVector<LHCb::Particle>& p ) const 
      { return dwmass ( p.begin() , p.end() ) ; }
      // ======================================================================
    public:
      // ======================================================================
      template <class DAUGHTER>
      double dwmass ( DAUGHTER first , DAUGHTER last ) const 
      {
        if ( m0() < 0 ) { return m0 () ; }
        const double dm = wmass ( first , last  ) ;
        if ( dm   < 0 ) { return m0 () ; }
        return dm - m0() ;
      }
      // ======================================================================
    public:
      // ======================================================================
      double                  m0    () const { return m_m0    ; }
      const std::string&      name0 () const { return m_name0 ; }
      const LHCb::ParticleID& pid0  () const { return m_pid0  ; }
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      DeltaWrongMass() ;                 // the default constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      /// the reference mass 
      double           m_m0    ;                   //        the reference mass 
      /// the reference mass 
      std::string      m_name0 ;                   //      the nominal particle 
      /// the reference mass 
      LHCb::ParticleID m_pid0  ;                   //           the nominal pid 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class AbdDeltaWrongMass 
     *  Simple class to evaluate the the 
     *  absolute value of delta mass of the mother particle 
     *  using "wrong" mass hypotheses for daughter particles 
     *  with respect to soem "reference" mass 
     *  @see LoKi::Cuts::ADWMASS 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-10-13
     */
    class AbsDeltaWrongMass : public DeltaWrongMass 
    {
    public:
      // ======================================================================
      /// constructor from masses 
      AbsDeltaWrongMass ( const double            m0 , const WrongMass& wm ) ;
      // ======================================================================
      /// constructor from pids 
      AbsDeltaWrongMass ( const LHCb::ParticleID& p1 , const WrongMass& wm ) ;
      // ======================================================================
      /// constructor form names 
      AbsDeltaWrongMass ( const std::string&      m0 , const WrongMass& wm ) ;
      // ======================================================================
      /// constructor from delta mass
      AbsDeltaWrongMass ( const DeltaWrongMass& wm ) ;
      // ======================================================================
      /// MANDATORY : virtual destructor 
      virtual ~AbsDeltaWrongMass() {}
      /// MANDATORY : clone method ("virtual constructor")
      virtual  AbsDeltaWrongMass* clone() const 
      { return new AbsDeltaWrongMass(*this) ; }
      /// MANDATORY : the only one essential method 
      virtual result_type operator() ( argument a ) const 
      { return adwmass ( a ) ; }
      /// OPTIONAL  : the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// evaluate delta wrong mass 
      double adwmass ( argument p ) const ;
      /// evaluate the wrong mass 
      double adwmass ( const LHCb::Particle::ConstVector&    p ) const 
      { return adwmass ( p.begin() , p.end() ) ; }
      /// evaluate the wrong mass 
      double adwmass ( const SmartRefVector<LHCb::Particle>& p ) const 
      { return adwmass ( p.begin() , p.end() ) ; }
      // ======================================================================
    public:
      // ======================================================================
      template <class DAUGHTER>
      double adwmass ( DAUGHTER first , DAUGHTER last ) const 
      { return m0 () < 0 ? m0() :   ::fabs ( dwmass ( first , last ) ) ; }
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      AbsDeltaWrongMass() ;                 // the default constructor is disabled 
      // ======================================================================
    } ;
    // ========================================================================    
  } // end of namespace LoKi::Particles 
  // ==========================================================================
} // end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PARTICLES23_H
// ============================================================================
