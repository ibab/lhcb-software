// $Id: AKinematics.h,v 1.4 2008-04-16 11:33:30 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_AKINEMATICS_H 
#define LOKI_AKINEMATICS_H 1
// ============================================================================
// Include files
// ============================================================================
// STD& STL 
// ============================================================================
#include <iterator>
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
    /** get the four momentum of the particle from the combination
     *
     *  @attention All indices starts from one. Zero corresponds to the 
     *              whole combination!
     *
     *  @see LHCb::Particle
     *  @see LoKi::LorentzVector
     *  @param  begin the begin-iterator of the sequence of the particles 
     *  @param  end   the end-iterator of the sequence of the particles 
     *  @param  index index of the daughetr particle 
     *  @return 4-momentum
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-15
     */
    template <class PARTICLE>
    inline LoKi::LorentzVector 
    momentum 
    ( PARTICLE begin          , 
      PARTICLE end            , 
      const std::size_t index ) 
    {
      if      ( 0 == index                            ) 
      { return LoKi::Kinematics::addMomenta ( begin , end ) ; } // RETURN 
      const int dist = std::distance ( begin , end ) ;
      if  ( 0 >= dist  ) 
      { return LoKi::LorentzVector () ; }                       // RETURN 
      else if ( static_cast<std::size_t> ( dist ) < index )  
      { return LoKi::LorentzVector () ; }                       // RETURN 
      // get the daughter: 
      PARTICLE ichild = begin ;
      std::advance ( ichild , index - 1 )   ;          // ATTENTION!!!
      const LHCb::Particle* child = *ichild ;          // ATTENTION!!!
      if ( 0 == child                ) { return LoKi::LorentzVector() ; }
      // get the final result:
      return child->momentum () ;
    }
    // ========================================================================
    /** get the four momentum of the particle from the combination
     *
     *  @attention All indices starts from one. Zero corresponds to the 
     *              whole combination!
     *
     *  @see LoKi::ATypes::Combination
     *  @param  comb combination 
     *  @param  index index of the daughetr particle 
     *  @return 4-momentum
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-15
     */
    inline LoKi::LorentzVector 
    momentum 
    ( const LoKi::ATypes::Combination& comb      , 
      const std::size_t                index = 0 ) 
    { return momentum ( comb.begin() , comb.end() , index ) ; }
    // ========================================================================
    /** get the four momentum of the particle from the combination
     *
     *  @attention All indices starts from one. Zero corresponds to the 
     *              whole combination!
     *
     *  @see LHCb::Particle::ConstVector
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
    { return momentum ( comb.begin() , comb.end() , index ) ; }
    // ========================================================================
    /** get the four momentum of the 2-particle sub-combination
     *  form the combination
     *
     *  @attention All indices starts from one. Zero corresponds to the 
     *              whole combination!
     *
     *  @param  begin the begin-iterator of the sequence of the particles 
     *  @param  end   the end-iterator of the sequence of the particles 
     *  @param  index1 index of the first particle in the sub-combination
     *  @param  index2 index of the second particle in the sub-combination
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-15
     */
    template <class PARTICLE>
    inline LoKi::LorentzVector 
    momentum 
    ( PARTICLE          begin  , 
      PARTICLE          end    , 
      const std::size_t index1 , 
      const std::size_t index2 ) 
    {
      return 
        momentum ( begin , end , index1 ) + 
        momentum ( begin , end , index2 ) ;
    }
    // ========================================================================    
    /** get the four momentum of the 2-particle sub-combination
     *  from the combination
     *
     *  @attention All indices starts from one. Zero corresponds to the 
     *              whole combination!
     *
     *  @see LoKi::ATypes::Combination
     *  @param  comb   combination 
     *  @param  index1 index of the first particle in the sub-combination
     *  @param  index2 index of the second particle in the sub-combination
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-15
     */
    inline LoKi::LorentzVector 
    momentum 
    ( const LoKi::ATypes::Combination& comb      , 
      const std::size_t                index1    , 
      const std::size_t                index2    ) 
    {
      return momentum ( comb.begin () , comb.end() , index1 , index2 ) ;
    }
    // ========================================================================    
    /** get the four momentum of the 2-particle sub-combination
     *  from the combination
     *
     *  @attention All indices starts from one. Zero corresponds to the 
     *              whole combination!
     *
     *  @see LHCb::Particle::ConstVector
     *  @param  comb   combination 
     *  @param  index1 index of the first particle in the sub-combination
     *  @param  index2 index of the second particle in the sub-combination
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-15
     */
    inline LoKi::LorentzVector 
    momentum 
    ( const LHCb::Particle::ConstVector& comb   ,
      const std::size_t                  index1 , 
      const std::size_t                  index2 ) 
    {
      return momentum ( comb.begin () , comb.end() , index1 , index2 ) ;
    }
    // ========================================================================
    /** get the four momentum of the 3-particle sub-combination
     *  from the combination
     *
     *  @attention All indices starts from one. Zero corresponds to the 
     *              whole combination!
     *
     *  @param  begin the begin-iterator of the sequence of the particles 
     *  @param  end   the end-iterator of the sequence of the particles 
     *  @param  index1 index of the first particle in the sub-combination
     *  @param  index2 index of the second particle in the sub-combination
     *  @param  index3 index of the third particle in the sub-combination
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-15
     */
    template <class PARTICLE>
    inline LoKi::LorentzVector 
    momentum 
    ( PARTICLE          begin  , 
      PARTICLE          end    , 
      const std::size_t index1 , 
      const std::size_t index2 , 
      const std::size_t index3 ) 
    {
      return 
        momentum ( begin , end , index1 , index2 ) + 
        momentum ( begin , end ,          index3 ) ;
    }
    // ========================================================================
    /** get the four momentum of the 3-particle sub-combination
     *  from the combination
     *
     *  @attention All indices starts from one. Zero corresponds to the 
     *              whole combination!
     *
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
    ( const LoKi::ATypes::Combination&   comb      , 
      const std::size_t                  index1    , 
      const std::size_t                  index2    , 
      const std::size_t                  index3    ) 
    {
      return momentum ( comb.begin() , comb.end () , index1 , index2 , index3 ) ;
    }
    // ========================================================================
    /** get the four momentum of the 3-particle sub-combination
     *  from the combination
     *
     *  @attention All indices starts from one. Zero corresponds to the 
     *              whole combination!
     *
     *  @see LHCb::Particle::ConstVector
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
      return momentum ( comb.begin() , comb.end () , index1 , index2 , index3 ) ;
    }
    // ========================================================================
    /** get the four momentum of the 4-particle sub-combination
     *  from the combination
     *
     *  @attention All indices starts from one. Zero corresponds to the 
     *              whole combination!
     *
     *  @param  begin the begin-iterator of the sequence of the particles 
     *  @param  end   the end-iterator of the sequence of the particles 
     *  @param  index1 index of the first particle in the sub-combination
     *  @param  index2 index of the second particle in the sub-combination
     *  @param  index3 index of the third particle in the sub-combination
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-15
     */
    template <class PARTICLE>
    inline LoKi::LorentzVector 
    momentum 
    ( PARTICLE          begin  , 
      PARTICLE          end    ,
      const std::size_t index1 , 
      const std::size_t index2 , 
      const std::size_t index3 , 
      const std::size_t index4 ) 
    {
      return 
        momentum ( begin , end , index1 , index2 ) + 
        momentum ( begin , end , index3 , index4 ) ;
    }
    // ========================================================================
    /** get the four momentum of the 4-particle sub-combination
     *  from the combination
     *
     *  @attention All indices starts from one. Zero corresponds to the 
     *              whole combination!
     *
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
    ( const LoKi::ATypes::Combination&   comb      , 
      const std::size_t                  index1    , 
      const std::size_t                  index2    , 
      const std::size_t                  index3    , 
      const std::size_t                  index4    ) 
    {
      return momentum 
        ( comb.begin() , comb.end() , index1 , index2 , index3 , index4 ) ;
    }
    // ========================================================================
    /** get the four momentum of the 4-particle sub-combination
     *  from the combination
     *
     *  @attention All indices starts from one. Zero corresponds to the 
     *              whole combination!
     *
     *  @see LHCb::Particle::ConstVector
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
      return momentum 
        ( comb.begin() , comb.end() , index1 , index2 , index3 , index4 ) ;
    }
    // ========================================================================
    /** get the four momentum of the "N"-particle sub-combination
     *  from the combination
     *
     *  @attention All indices starts from one. Zero corresponds to the 
     *              whole combination!
     *
     *  @param  begin the begin-iterator of the sequence of the particles 
     *  @param  end   the end-iterator of the sequence of the particles 
     *  @param  comb   combination 
     *  @param  indices vector of indices 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-15
     */
    template <class PARTICLE>
    inline LoKi::LorentzVector 
    momentum 
    ( PARTICLE                 begin     , 
      PARTICLE                 end       , 
      std::vector<std::size_t> indices   ) 
    {
      LoKi::LorentzVector result = LoKi::LorentzVector() ;
      while ( !indices.empty() ) 
      {
        result += momentum ( begin , end , indices.back() ) ; // recursion 
        indices.pop_back() ;                            // NB!
      }
      return result ;
    }
    // ========================================================================    
    /** get the four momentum of the "N"-particle sub-combination
     *  from the combination
     *
     *  @attention All indices starts from one. Zero corresponds to the 
     *              whole combination!
     *
     *  @see LoKi::ATypes::Combination
     *  @param  comb   combination 
     *  @param  indices vector of indices 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-15
     */
    inline LoKi::LorentzVector 
    momentum 
    ( const LoKi::ATypes::Combination& comb    , 
      const std::vector<std::size_t>&  indices ) 
    { return momentum ( comb.begin() , comb.end() , indices ) ; }
    // ========================================================================    
    /** get the four momentum of the "N"-particle sub-combination
     *  from the combination
     *
     *  @attention All indices starts from one. Zero corresponds to the 
     *              whole combination!
     *
     *  @see LHCb::Particle::ConstVector
     *  @param  comb   combination 
     *  @param  indices vector of indices 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-15
     */
    inline LoKi::LorentzVector 
    momentum 
    ( const LHCb::Particle::ConstVector& comb    , 
      const std::vector<std::size_t>&    indices ) 
    { return momentum ( comb.begin() , comb.end() , indices ) ; }
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
