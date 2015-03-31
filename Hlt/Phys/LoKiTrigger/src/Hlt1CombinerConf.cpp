// $Id: Hlt1Combiner.cpp 180655 2014-11-25 10:38:48Z mkenzie $
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Hlt1CombinerConf.h"
#include "LoKi/ToCpp.h"
#include "LoKi/AuxFunBase.h"
// ============================================================================
/** @file
 *
 *  Implementation file for class LoKi::Hlt1::Hlt1Combiner
 *  This file is part of LoKi project:
 *   ``C++ ToolKit for Smart and Friendly Physics Analysis''
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas,
 *  contributions and advices from G.Raven, J.van Tilburg,
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Matthew KENZIE matthew.kenzie@cern.ch
 *  @date   2014-11-27
 *
 *                    $Revision: 180655 $
 *  Last Modification $Date: 2014-11-25 11:38:48 +0100 (Tue, 25 Nov 2014) $
 *                 by $Author: mkenzie $
 */
// ============================================================================
namespace 
{
  // ==========================================================================
  /** @var s_combiner 
   *  default particle combiner 
   *  @see IParticleCombiner
   */
  const std::string s_combiner = "LoKi::FastVertexFitter:PUBLIC" ;
  // ==========================================================================
}
// ============================================================================
// CONSTRUCTORS FOR 2 body combinations
// ============================================================================
//  consructor from the decay, combination, mother cuts and combiner
// ============================================================================
LoKi::Hlt1::Hlt1CombinerConf::Hlt1CombinerConf
( const std::string&                                                decay      ,
  const LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate&  combcut    ,
  const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate&      mothcut    ,
  const std::string&                                                combiner   )
  : m_decstrings ( 1 , decay )
  , m_acut    ( combcut )
  , m_acut12  ( LoKi::BasicFunctors<LoKi::ATypes::Combination>::BooleanConstant ( true ) )
  , m_acut123 ( LoKi::BasicFunctors<LoKi::ATypes::Combination>::BooleanConstant ( true ) )
  , m_cut     ( mothcut )
  , m_combinertool ( combiner.empty() ? s_combiner : combiner )
  , m_toCpp   ( Gaudi::Utils::toCpp_lst ( std::tie (  decay , combcut , mothcut , combiner ) ) ) 
{}
// ============================================================================
//  consructor from the decay, combination and mother cuts
// ============================================================================
LoKi::Hlt1::Hlt1CombinerConf::Hlt1CombinerConf
( const std::vector<std::string>&                                   decay      ,
  const LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate&  combcut    ,
  const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate&      mothcut    ,
  const std::string&                                                combiner   )
  : m_decstrings   ( decay ) 
  , m_acut         ( combcut )
  , m_acut12       ( LoKi::BasicFunctors<LoKi::ATypes::Combination>::BooleanConstant ( true ) )
  , m_acut123      ( LoKi::BasicFunctors<LoKi::ATypes::Combination>::BooleanConstant ( true ) )
  , m_cut          ( mothcut )
  , m_combinertool ( combiner.empty() ? s_combiner : combiner )
  , m_toCpp   ( Gaudi::Utils::toCpp_lst ( std::tie (  decay , combcut , mothcut , combiner ) ) ) 
{}
// ============================================================================
// CONSTRUCTORS FOR 3 body combinations
// ============================================================================
//  consructor from the decay, combination, mother cuts and combiner
// ============================================================================
LoKi::Hlt1::Hlt1CombinerConf::Hlt1CombinerConf
( const std::string&                                                decay      ,
  const LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate&  combcut    ,
  const LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate&  combcut12  ,
  const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate&      mothcut    ,
  const std::string&                                                combiner   )
  : m_decstrings   ( 1 , decay )
  , m_acut         ( combcut )
  , m_acut12       ( combcut12  )
  , m_acut123      ( LoKi::BasicFunctors<LoKi::ATypes::Combination>::BooleanConstant ( true ) )
  , m_cut          ( mothcut )
  , m_combinertool ( combiner.empty() ? s_combiner : combiner )
  , m_toCpp   ( Gaudi::Utils::toCpp_lst ( std::tie (  decay     , combcut , 
                                                     combcut12 , 
                                                     mothcut   , combiner ) ) ) 
{}
// ============================================================================
//  consructor from the decay, combination and mother cuts
// ============================================================================
LoKi::Hlt1::Hlt1CombinerConf::Hlt1CombinerConf
( const std::vector<std::string>&                                   decay      ,
  const LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate&  combcut    ,
  const LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate&  combcut12  ,
  const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate&      mothcut    ,
  const std::string&                                                combiner   )
  : m_decstrings   ( decay      ) 
  , m_acut         ( combcut    )
  , m_acut12       ( combcut12  )
  , m_acut123      ( LoKi::BasicFunctors<LoKi::ATypes::Combination>::BooleanConstant ( true ) )
  , m_cut          ( mothcut    )
  , m_combinertool ( combiner.empty() ? s_combiner : combiner )
  , m_toCpp   ( Gaudi::Utils::toCpp_lst ( std::tie (  decay     , combcut ,
                                                     combcut12 , 
                                                     mothcut   , combiner ) ) ) 
{}
// ============================================================================
// CONSTRUCTORS FOR 4 body combinations
// ============================================================================
//  consructor from the decay, combination, mother cuts and combiner
// ============================================================================
LoKi::Hlt1::Hlt1CombinerConf::Hlt1CombinerConf
( const std::string&                                                decay      ,
  const LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate&  combcut    ,
  const LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate&  combcut12  ,
  const LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate&  combcut123 ,
  const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate&      mothcut    ,
  const std::string&                                                combiner   )
  : m_decstrings   ( 1 , decay  )
  , m_acut         ( combcut    )
  , m_acut12       ( combcut12  )
  , m_acut123      ( combcut123 )
  , m_cut          ( mothcut    )
  , m_combinertool ( combiner.empty() ? s_combiner : combiner )
  , m_toCpp   ( Gaudi::Utils::toCpp_lst ( std::tie (  decay     , combcut    , 
                                                     combcut12 , combcut123 , 
                                                     mothcut   , combiner   ) ) ) 
{}
// ============================================================================
//  consructor from the decay, combination and mother cuts
// ============================================================================
LoKi::Hlt1::Hlt1CombinerConf::Hlt1CombinerConf
( const std::vector<std::string>&                                   decay      ,
  const LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate&  combcut    ,
  const LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate&  combcut12  ,
  const LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate&  combcut123 ,
  const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate&      mothcut    ,
  const std::string&                                                combiner   )
  : m_decstrings   ( decay      ) 
  , m_acut         ( combcut    )
  , m_acut12       ( combcut12  )
  , m_acut123      ( combcut123 )
  , m_cut          ( mothcut    )
  , m_combinertool ( combiner.empty() ? s_combiner : combiner )
  , m_toCpp   ( Gaudi::Utils::toCpp_lst ( std::tie (  decay     , combcut    , 
                                                     combcut12 , combcut123 , 
                                                     mothcut   , combiner   ) ) ) 
{}
// ============================================================================
// virtual destructor
// ============================================================================
LoKi::Hlt1::Hlt1CombinerConf::~Hlt1CombinerConf(){}
// ============================================================================
// printout
// ============================================================================
std::ostream& LoKi::Hlt1::Hlt1CombinerConf::fillStream
( std::ostream& s ) const
{
  s << "LoKi.Hlt1.Hlt1CombinerConf(";
  if ( 1 == m_decstrings.size() ) 
  { Gaudi::Utils::toStream ( m_decstrings[0] , s ) ; }
  else 
  { Gaudi::Utils::toStream ( m_decstrings    , s ) ; }
  s << "," << m_acut 
    << "," << m_acut12  
    << "," << m_acut123 
    << "," << m_cut     ;
  if ( s_combiner != m_combinertool )
  {
    s << "," ;
    Gaudi::Utils::toStream ( m_combinertool , s ) ;
  }
  return s << ")" ;
}
// ============================================================================
// conversion to the string
// ============================================================================
std::string LoKi::Hlt1::Hlt1CombinerConf::toString () const
{
  std::ostringstream s ;
  fillStream ( s ) ;
  return s.str() ;
}
// ============================================================================
std::string LoKi::Hlt1::Hlt1CombinerConf::toCpp() const
{ return "LoKi::Hlt1::Hlt1CombinerConf(" + m_toCpp + ")" ; }
// ============================================================================
std::string Gaudi::Utils::toCpp ( const LoKi::Hlt1::Hlt1CombinerConf& o )
{  return o.toCpp() ; }
// ============================================================================
// The END
// ============================================================================
