// $Id$
// ============================================================================
// Include files 
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Objects.h"
#include "LoKi/PhysDump.h"
#include "LoKi/DecayChain.h"
#include "LoKi/ATypes.h"
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
 *  @author Vanya Belyaev Ivan.Belyaev@cern.ch
 *  @date   2011-06-03
 * 
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
// ============================================================================
/*  template specialization of ``dumper''
 *  @see LoKi::Fuctors::Dump_
 *  @author Vanya Belyaev Ivan.Belyaev@cern.ch
 *  @date   2011-06-03
 */  
// ============================================================================
template <>
LoKi::Functors::Dump_<const LHCb::Particle*>::result_type 
LoKi::Functors::Dump_<const LHCb::Particle*>::operator() 
  ( LoKi::Functors::Dump_<const LHCb::Particle*>::argument a ) const 
{
  //
  const unsigned int mx = std::min ( m_dump.nMax() , a.size() ) ;
  //
  m_stream << m_dump.open  () ;
  //
  LoKi::DecayChain printer ;
  printer.print
    ( a.begin ()                          , 
      a.begin () + mx                     , 
      m_stream                            , 
      '\n'                                , 
      LoKi::Objects::_VALID_              , 
      LoKi::Objects::_NONE_               , 
      " "                                 , 
      0                                   ) ;
  //
  m_stream << m_dump.close () ;
  //
  return a ;
}
// ============================================================================
/*  template specialization of ``dumper''
 *  @see LoKi::Fuctors::Dump_
 *  @author Vanya Belyaev Ivan.Belyaev@cern.ch
 *  @date   2011-06-03
 */  
// ============================================================================
template <>
LoKi::Functors::Dump1_<const LHCb::Particle*,bool>::result_type 
LoKi::Functors::Dump1_<const LHCb::Particle*,bool>::operator() 
  ( LoKi::Functors::Dump1_<const LHCb::Particle*,bool>::argument a ) const 
{
  //
  if  ( m_right ) 
  {
    m_stream << m_dump.open  () ;
    //
    LoKi::DecayChain printer ;
    printer.print
      ( a                                   ,
        m_stream                            , 
        '\n'                                , 
        LoKi::Objects::_VALID_              , 
        LoKi::Objects::_NONE_               , 
        " "                                 , 
        0                                   ) ;
    //
    m_stream << m_dump.close () ;
    //
    return m_fun ( a ) ;
  }
  //
  result_type result = m_fun ( a ) ;
  //
  m_stream << m_dump.open  () ;
  //
  LoKi::DecayChain printer ;
  printer.print
    ( a                                   ,
      m_stream                            , 
      '\n'                                , 
      LoKi::Objects::_VALID_              , 
      LoKi::Objects::_NONE_               , 
      " "                                 , 
      0                                   ) ;
    //
  m_stream << m_dump.close () ;
  //
  return result ;
}
// ============================================================================
/*  template specialization of ``dumper''
 *  @see LoKi::Fuctors::Dump_
 *  @author Vanya Belyaev Ivan.Belyaev@cern.ch
 *  @date   2011-06-03
 */  
// ============================================================================
template <>
LoKi::Functors::Dump1_<const LHCb::Particle*,double>::result_type 
LoKi::Functors::Dump1_<const LHCb::Particle*,double>::operator() 
  ( LoKi::Functors::Dump1_<const LHCb::Particle*,double>::argument a ) const 
{
  //
  if  ( m_right ) 
  {
    m_stream << m_dump.open  () ;
    //
    LoKi::DecayChain printer ;
    printer.print
      ( a                                   ,
        m_stream                            , 
        '\n'                                , 
        LoKi::Objects::_VALID_              , 
        LoKi::Objects::_NONE_               , 
        " "                                 , 
        0                                   ) ;
    //
    m_stream << m_dump.close () ;
    //
    return m_fun ( a ) ;
  }
  //
  result_type result = m_fun ( a ) ;
  //
  m_stream << m_dump.open  () ;
  //
  LoKi::DecayChain printer ;
  printer.print
    ( a                                   ,
      m_stream                            , 
      '\n'                                , 
      LoKi::Objects::_VALID_              , 
      LoKi::Objects::_NONE_               , 
      " "                                 , 
      0                                   ) ;
  //
  m_stream << m_dump.close () ;
  //
  return result ;
}
// ============================================================================
template <>
LoKi::Functors::Dump1_<LoKi::ATypes::Combination,double>::result_type 
LoKi::Functors::Dump1_<LoKi::ATypes::Combination,double>::operator() 
  ( LoKi::Functors::Dump1_<LoKi::ATypes::Combination,double>::argument a ) const 
{
  //
  if ( m_right ) 
  {
    m_stream << m_dump.open  () ;
    //
    LoKi::DecayChain printer ;
    printer.print
      ( a.begin ()                          , 
        a.end   ()                          , 
        m_stream                            , 
        '\n'                                , 
        LoKi::Objects::_VALID_              , 
        LoKi::Objects::_NONE_               , 
        " "                                 , 
        0                                   ) ;
    //
    m_stream << m_dump.close () ;
    //
    return m_fun ( a ) ;
  }
  //  
  result_type result = m_fun ( a ) ;
  //
  m_stream << m_dump.open  () ;
  //
  LoKi::DecayChain printer ;
  printer.print
    ( a.begin ()                          , 
      a.end   ()                          , 
      m_stream                            , 
      '\n'                                , 
      LoKi::Objects::_VALID_              , 
      LoKi::Objects::_NONE_               , 
      " "                                 , 
        0                                   ) ;
  //
  m_stream << m_dump.close () ;
  //
  return result ;
}
// ============================================================================
template <>
LoKi::Functors::Dump1_<LoKi::ATypes::Combination,bool>::result_type 
LoKi::Functors::Dump1_<LoKi::ATypes::Combination,bool>::operator() 
  ( LoKi::Functors::Dump1_<LoKi::ATypes::Combination,bool>::argument a ) const 
{
  //
  if ( m_right ) 
  {
    m_stream << m_dump.open  () ;
    //
    LoKi::DecayChain printer ;
    printer.print
      ( a.begin ()                          , 
        a.end   ()                          , 
        m_stream                            , 
        '\n'                                , 
        LoKi::Objects::_VALID_              , 
        LoKi::Objects::_NONE_               , 
        " "                                 , 
        0                                   ) ;
    //
    m_stream << m_dump.close () ;
    //
    return m_fun ( a ) ;
  }
  //  
  result_type result = m_fun ( a ) ;
  //
  m_stream << m_dump.open  () ;
  //
  LoKi::DecayChain printer ;
  printer.print
    ( a.begin ()                          , 
      a.end   ()                          , 
      m_stream                            , 
      '\n'                                , 
      LoKi::Objects::_VALID_              , 
      LoKi::Objects::_NONE_               , 
      " "                                 , 
      0                                   ) ;
  //
  m_stream << m_dump.close () ;
  //
  return result ;
}
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
