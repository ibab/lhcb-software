// $Id: Hlt1Combiner.cpp 180655 2014-11-25 10:38:48Z mkenzie $
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Hlt1CombinerConf.h"
#include "LoKi/ToCpp.h"
#include "LoKi/AuxFunBase.h"
#include "LTTools.h"
#include "GaudiKernel/IToolSvc.h"
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
// CONSTRUCTORS FOR 2 body combinations
// ============================================================================
// ============================================================================
//  consructor from the decay, combination and mother cuts
// ============================================================================
LoKi::Hlt1::Hlt1CombinerConf::Hlt1CombinerConf
( std::string                                                       decay      ,
  const LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate&  combcut    ,
  const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate&      mothcut    )
  : m_decstrings { decay }
  , m_acut ( combcut )
  , m_acut12 ( LoKi::BasicFunctors<LoKi::ATypes::Combination>::BooleanConstant ( true ) )
  , m_acut123 ( LoKi::BasicFunctors<LoKi::ATypes::Combination>::BooleanConstant ( true ) )
  , m_cut  ( mothcut )
  , m_combinertool ( "LoKi::FastVertexFitter:PUBLIC" )
{
  setup();
}
// ============================================================================
//  consructor from the decay, combination, mother cuts and combiner
// ============================================================================
LoKi::Hlt1::Hlt1CombinerConf::Hlt1CombinerConf
( std::string                                                       decay      ,
  const LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate&  combcut    ,
  const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate&      mothcut    ,
  std::string                                                       combiner   )
  : m_decstrings { decay }
  , m_acut ( combcut )
  , m_acut12 ( LoKi::BasicFunctors<LoKi::ATypes::Combination>::BooleanConstant ( true ) )
  , m_acut123 ( LoKi::BasicFunctors<LoKi::ATypes::Combination>::BooleanConstant ( true ) )
  , m_cut  ( mothcut )
  , m_combinertool ( combiner )
{
  setup();
}
// ============================================================================
//  consructor from the decay, combination and mother cuts
// ============================================================================
LoKi::Hlt1::Hlt1CombinerConf::Hlt1CombinerConf
( std::vector<std::string>                                          decay      ,
  const LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate&  combcut    ,
  const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate&      mothcut    ,
  std::string                                                       combiner   )
  : m_decstrings { decay }
  , m_acut ( combcut )
  , m_acut12 ( LoKi::BasicFunctors<LoKi::ATypes::Combination>::BooleanConstant ( true ) )
  , m_acut123 ( LoKi::BasicFunctors<LoKi::ATypes::Combination>::BooleanConstant ( true ) )
  , m_cut  ( mothcut )
  , m_combinertool ( combiner )
{
  setup();
}
// ============================================================================
//  consructor from the decay, combination and mother cuts
// ============================================================================
LoKi::Hlt1::Hlt1CombinerConf::Hlt1CombinerConf
( std::vector<std::string>                                          decay      ,
  const LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate&  combcut    ,
  const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate&      mothcut    )
  : m_decstrings { decay }
  , m_acut ( combcut )
  , m_acut12 ( LoKi::BasicFunctors<LoKi::ATypes::Combination>::BooleanConstant ( true ) )
  , m_acut123 ( LoKi::BasicFunctors<LoKi::ATypes::Combination>::BooleanConstant ( true ) )
  , m_cut  ( mothcut )
  , m_combinertool ( "LoKi::FastVertexFitter:PUBLIC" )
{
  setup();
}
// ============================================================================
// CONSTRUCTORS FOR 3 body combinations
// ============================================================================
// ============================================================================
//  consructor from the decay, combination and mother cuts
// ============================================================================
LoKi::Hlt1::Hlt1CombinerConf::Hlt1CombinerConf
( std::string                                                       decay      ,
  const LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate&  combcut    ,
  const LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate&  combcut12  ,
  const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate&      mothcut    )
  : m_decstrings { decay }
  , m_acut ( combcut )
  , m_acut12 ( combcut12 )
  , m_acut123 ( LoKi::BasicFunctors<LoKi::ATypes::Combination>::BooleanConstant ( true ) )
  , m_cut  ( mothcut )
  , m_combinertool ( "LoKi::FastVertexFitter:PUBLIC" )
{
  setup();
}
// ============================================================================
//  consructor from the decay, combination, mother cuts and combiner
// ============================================================================
LoKi::Hlt1::Hlt1CombinerConf::Hlt1CombinerConf
( std::string                                                       decay      ,
  const LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate&  combcut    ,
  const LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate&  combcut12  ,
  const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate&      mothcut    ,
  std::string                                                       combiner   )
  : m_decstrings { decay }
  , m_acut ( combcut )
  , m_acut12 ( combcut12 )
  , m_acut123 ( LoKi::BasicFunctors<LoKi::ATypes::Combination>::BooleanConstant ( true ) )
  , m_cut  ( mothcut )
  , m_combinertool ( combiner )
{
  setup();
}
// ============================================================================
//  consructor from the decay, combination and mother cuts
// ============================================================================
LoKi::Hlt1::Hlt1CombinerConf::Hlt1CombinerConf
( std::vector<std::string>                                          decay      ,
  const LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate&  combcut    ,
  const LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate&  combcut12  ,
  const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate&      mothcut    ,
  std::string                                                       combiner   )
  : m_decstrings { decay }
  , m_acut ( combcut )
  , m_acut12 ( combcut12 )
  , m_acut123 ( LoKi::BasicFunctors<LoKi::ATypes::Combination>::BooleanConstant ( true ) )
  , m_cut  ( mothcut )
  , m_combinertool ( combiner )
{
  setup();
}
// ============================================================================
//  consructor from the decay, combination and mother cuts
// ============================================================================
LoKi::Hlt1::Hlt1CombinerConf::Hlt1CombinerConf
( std::vector<std::string>                                          decay      ,
  const LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate&  combcut    ,
  const LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate&  combcut12  ,
  const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate&      mothcut    )
  : m_decstrings { decay }
  , m_acut ( combcut )
  , m_acut12 ( combcut12 )
  , m_acut123 ( LoKi::BasicFunctors<LoKi::ATypes::Combination>::BooleanConstant ( true ) )
  , m_cut  ( mothcut )
  , m_combinertool ( "LoKi::FastVertexFitter:PUBLIC" )
{
  setup();
}
// ============================================================================
// CONSTRUCTORS FOR 4 body combinations
// ============================================================================
// ============================================================================
//  consructor from the decay, combination and mother cuts
// ============================================================================
LoKi::Hlt1::Hlt1CombinerConf::Hlt1CombinerConf
( std::string                                                       decay      ,
  const LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate&  combcut    ,
  const LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate&  combcut12  ,
  const LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate&  combcut123 ,
  const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate&      mothcut    )
  : m_decstrings { decay }
  , m_acut ( combcut )
  , m_acut12 ( combcut12 )
  , m_acut123 ( combcut123 )
  , m_cut  ( mothcut )
  , m_combinertool ( "LoKi::FastVertexFitter:PUBLIC" )
{
  setup();
}
// ============================================================================
//  consructor from the decay, combination, mother cuts and combiner
// ============================================================================
LoKi::Hlt1::Hlt1CombinerConf::Hlt1CombinerConf
( std::string                                                       decay      ,
  const LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate&  combcut    ,
  const LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate&  combcut12  ,
  const LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate&  combcut123 ,
  const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate&      mothcut    ,
  std::string                                                       combiner   )
  : m_decstrings { decay }
  , m_acut ( combcut )
  , m_acut12 ( combcut12 )
  , m_acut123 ( combcut123 )
  , m_cut  ( mothcut )
  , m_combinertool ( combiner )
{
  setup();
}
// ============================================================================
//  consructor from the decay, combination and mother cuts
// ============================================================================
LoKi::Hlt1::Hlt1CombinerConf::Hlt1CombinerConf
( std::vector<std::string>                                          decay      ,
  const LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate&  combcut    ,
  const LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate&  combcut12  ,
  const LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate&  combcut123 ,
  const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate&      mothcut    ,
  std::string                                                       combiner   )
  : m_decstrings { decay }
  , m_acut ( combcut )
  , m_acut12 ( combcut12 )
  , m_acut123 ( combcut123 )
  , m_cut  ( mothcut )
  , m_combinertool ( combiner )
{
  setup();
}
// ============================================================================
//  consructor from the decay, combination and mother cuts
// ============================================================================
LoKi::Hlt1::Hlt1CombinerConf::Hlt1CombinerConf
( std::vector<std::string>                                          decay      ,
  const LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate&  combcut    ,
  const LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate&  combcut12  ,
  const LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate&  combcut123 ,
  const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate&      mothcut    )
  : m_decstrings { decay }
  , m_acut ( combcut )
  , m_acut12 ( combcut12 )
  , m_acut123 ( combcut123 )
  , m_cut  ( mothcut )
  , m_combinertool ( "LoKi::FastVertexFitter:PUBLIC" )
{
  setup();
}
// ============================================================================
// setup
// ============================================================================
StatusCode LoKi::Hlt1::Hlt1CombinerConf::setup()
{
  LoKi::ILoKiSvc* svc = LoKi::AuxFunBase::lokiSvc() ;
  SmartIF<IToolSvc> tsvc ( svc ) ;
  IDecodeSimpleDecayString *dsds ;
  StatusCode sc = tsvc->retrieveTool("DecodeSimpleDecayString:PUBLIC", dsds) ;
  m_decays = Decays::decays ( m_decstrings, dsds ) ;
  release(dsds);
  Assert ( !m_decays.empty(), "Unable to decode DecayDescriptor" );
  return sc;
}
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
  s << " [ ";
  for (unsigned int i = 0; i < m_decstrings.size()-1; i++) {
    s << m_decstrings[i] << ",";
  }
  s << m_decstrings[m_decstrings.size()-1] << " ] " ;
  s << "," ; 
  m_acut.fillStream( s );
  s << "," ;
  m_cut.fillStream( s ) ;
  s << "," ;
  combiner() ;
  s << ")" ;
  return s;
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
std::string Gaudi::Utils::toCpp ( const LoKi::Hlt1::Hlt1CombinerConf& o )
{
  std::string s = "LoKi::Hlt1::Hlt1CombinerConf( ";
  s += toCpp ( o.decaystrs() ) + ", " ;
  s += toCpp ( o.acut() )  + ", " ;
  s += toCpp ( o.cut() )   + ", " ;
  s += toCpp ( o.combiner() )     ;
  s += ")" ;
  return s ;
}
