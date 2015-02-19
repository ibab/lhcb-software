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
//  consructor from the decay, combination and mother cuts
// ============================================================================
LoKi::Hlt1::Hlt1CombinerConf::Hlt1CombinerConf
( std::string                                                       decay      ,
  const LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate&  combcut    ,
  const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate&      mothcut    )
  : m_decstring ( decay )
  , m_acut ( combcut )
  , m_cut  ( mothcut )
{
  LoKi::ILoKiSvc* svc = LoKi::AuxFunBase::lokiSvc() ;
  SmartIF<IToolSvc> tsvc ( svc ) ;
  StatusCode sc = tsvc->retrieveTool("DecodeSimpleDecayString:PUBLIC", m_dsds);
  m_decays = Decays::decays ( m_decstring, m_dsds ) ;
  Assert ( !m_decays.empty(), "Unable to decode DecayDescriptor" );
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
  s << "LoKi.Hlt1.Hlt1CombinerConf(" << decay() << "," ;
  m_acut.fillStream( s );
  s << "," ;
  m_cut.fillStream( s ) ;
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
  s += toCpp ( o.decay() ) + ", " ;
  s += toCpp ( o.acut() )  + ", " ;
  s += toCpp ( o.cut() )          ;
  s += ")" ;
  return s ;
}
