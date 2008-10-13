// $Id: Particles23.h,v 1.1 2008-10-13 16:32:23 ibelyaev Exp $
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
      virtual result_type operator() ( argument a ) const ;
      /// OPTIONAL  : the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// decode the masses 
      StatusCode decode () const ;
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
  } // end of namespace LoKi::Particles 
  // ==========================================================================
} // end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PARTICLES23_H
// ============================================================================
