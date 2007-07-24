// $Id: AKinematics.h,v 1.2 2007-07-24 05:20:09 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_AKINEMATICS_H 
#define LOKI_AKINEMATICS_H 1
// ============================================================================
// Include files
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Kinematics.h"
// ============================================================================
namespace LoKi 
{
  namespace AKinematics 
  {
    // ========================================================================
    /** get the four momentum of the particle form the combination
     *  @attention All indices starts from one. Zero corresponds to the 
     *              whole combination!
     *  @see LHCb::Particle::ConstVector
     *  @see LoKi::ATypes::Combination
     *  @param  comb combination 
     *  @param  index index of the daughetr particle 
     *  @return 4-momentum
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-15
     */
    inline LoKi::LorentzVector 
    momentum 
    ( const LHCb::Particle::ConstVector& comb      , 
      const std::size_t                  index = 0 ) 
    {
      if      ( 0 == index             ) 
      { return LoKi::Kinematics::addMomenta ( comb.begin() , comb.end() ) ; }
      else if ( comb.size()< index ) { return LoKi::LorentzVector() ; } 
      // get the daughter: 
      const LHCb::Particle* child = comb[index-1] ;          // ATTENTION!!
      if ( 0 == child                ) { return LoKi::LorentzVector() ; }
      // get the final result:
      return child->momentum () ;
    }
    // ========================================================================
    /** get the four momentum of the 2-particle sub-combination
     *  form the combination
     *  @attention All indices starts from one. Zero corresponds to the 
     *              whole combination!
     *  @see LHCb::Particle::ConstVector
     *  @see LoKi::ATypes::Combination
     *  @param  comb   combination 
     *  @param  index1 index of the first particle in the sub-combination
     *  @param  index2 index of the second particle in the sub-combination
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-15
     */
    inline LoKi::LorentzVector 
    momentum 
    ( const LHCb::Particle::ConstVector& comb      , 
      const std::size_t                  index1    , 
      const std::size_t                  index2    ) 
    {
      return momentum ( comb , index1 ) + momentum ( comb , index2 ) ;
    }
    // ========================================================================
    /** get the four momentum of the 3-particle sub-combination
     *  form the combination
     *  @attention All indices starts from one. Zero corresponds to the 
     *              whole combination!
     *  @see LHCb::Particle::ConstVector
     *  @see LoKi::ATypes::Combination
     *  @param  comb   combination 
     *  @param  index1 index of the first particle in the sub-combination
     *  @param  index2 index of the second particle in the sub-combination
     *  @param  index3 index of the third particle in the sub-combination
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-15
     */
    inline LoKi::LorentzVector 
    momentum 
    ( const LHCb::Particle::ConstVector& comb      , 
      const std::size_t                  index1    , 
      const std::size_t                  index2    , 
      const std::size_t                  index3    ) 
    {
      return momentum ( comb , index1 ) + 
        momentum ( comb , index2 , index3 ) ;
    }
    // ========================================================================
    /** get the four momentum of the 4-particle sub-combination
     *  form the combination
     *  @attention All indices starts from one. Zero corresponds to the 
     *              whole combination!
     *  @see LHCb::Particle::ConstVector
     *  @see LoKi::ATypes::Combination
     *  @param  comb   combination 
     *  @param  index1 index of the first particle in the sub-combination
     *  @param  index2 index of the second particle in the sub-combination
     *  @param  index3 index of the third particle in the sub-combination
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-15
     */
    inline LoKi::LorentzVector 
    momentum 
    ( const LHCb::Particle::ConstVector& comb      , 
      const std::size_t                  index1    , 
      const std::size_t                  index2    , 
      const std::size_t                  index3    , 
      const std::size_t                  index4    ) 
    {
      return momentum ( comb , index1 ) + 
        momentum ( comb , index2 , index3 , index4 ) ;
    }
    // ========================================================================
    /** get the four momentum of the "N"-particle sub-combination
     *  form the combination
     *  @attention All indices starts from one. Zero corresponds to the 
     *              whole combination!
     *  @see LHCb::Particle::ConstVector
     *  @see LoKi::ATypes::Combination
     *  @param  comb   combination 
     *  @param  indices vector of indices 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-15
     */
    inline LoKi::LorentzVector 
    momentum 
    ( const LHCb::Particle::ConstVector& comb      , 
      std::vector<std::size_t>           indices   ) 
    {
      LoKi::LorentzVector result = LoKi::LorentzVector() ;
      while ( !indices.empty() ) 
      {
        result += momentum ( comb , indices.back() ) ;
        indices.pop_back() ;
      }
      return result ;
    }
    // ========================================================================    
  } // end of namespace LoKi::AKinematics 
  namespace Kinematics 
  {
    // import namespace 
    using namespace LoKi::AKinematics ;
  } // end of namespace LoKi::Kinematics
} // end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_AKINEMATICS_H
// ============================================================================
