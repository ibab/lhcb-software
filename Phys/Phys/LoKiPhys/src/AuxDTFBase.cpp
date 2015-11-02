// $Id$
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <set>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ToStream.h"
// ============================================================================
// DaVinciInterface 
// ============================================================================
#include "Kernel/IDecayTreeFit.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/GetTools.h"
#include "LoKi/AuxDTFBase.h"
#include "LoKi/ParticleProperties.h"
// ============================================================================
/** @file 
 *
 * Implementation file for class LoKi::AuxDTFBase
 *
 *  This file is a part of LoKi project -
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas,
 *  contributions and advices from G.Raven, J.van Tilburg,
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2010-06-03
 */
// ============================================================================
// constructor from the fitter 
// ============================================================================
LoKi::AuxDTFBase::AuxDTFBase ( const IDecayTreeFit* fitter ) 
  : m_fitter      ( fitter ) 
{}
// ============================================================================
// constructor from the fitter 
// ============================================================================
LoKi::AuxDTFBase::AuxDTFBase ( const std::string& fitter ) 
  : m_fitterName  ( fitter ) 
{}
// ============================================================================
// constructor from the fitter 
// ============================================================================
LoKi::AuxDTFBase::AuxDTFBase
( const std::vector<std::string>&      constraints , 
  const LoKi::AuxDTFBase::MASSES&      masses      ,
  const IDecayTreeFit*                 fitter      ) 
  : m_fitter      ( fitter ) 
{
  setConstraint ( constraints  ) ;
  setConstraint ( masses       ) ;
}
// ============================================================================
// constructor from the fitter 
// ============================================================================
LoKi::AuxDTFBase::AuxDTFBase
( const std::vector<std::string>&      constraints , 
  const LoKi::AuxDTFBase::MASSES2&     masses      ,
  const IDecayTreeFit*                 fitter      ) 
  : m_fitter      ( fitter ) 
{
  setConstraint ( constraints  ) ;
  setConstraint ( masses       ) ;
}
// ============================================================================
// constructor from the fitter 
// ============================================================================
LoKi::AuxDTFBase::AuxDTFBase
( const std::vector<LHCb::ParticleID>& constraints , 
  const LoKi::AuxDTFBase::MASSES&      masses      ,
  const IDecayTreeFit*                 fitter      ) 
  : m_fitter      ( fitter ) 
{
  setConstraint ( constraints  ) ;
  setConstraint ( masses       ) ;
}
// ============================================================================
// constructor from the fitter 
// ============================================================================
LoKi::AuxDTFBase::AuxDTFBase
( const std::vector<LHCb::ParticleID>& constraints , 
  const LoKi::AuxDTFBase::MASSES2&     masses      ,
  const IDecayTreeFit*                 fitter      ) 
  : m_fitter      ( fitter ) 
{
  setConstraint ( constraints  ) ;
  setConstraint ( masses       ) ;
}
// ============================================================================
// constructor from the fitter 
// ============================================================================
LoKi::AuxDTFBase::AuxDTFBase
( const std::vector<std::string>&      constraints , 
  const std::string&                   fitter      ,
  const LoKi::AuxDTFBase::MASSES&      masses      )
  : m_fitterName  ( fitter ) 
{
  setConstraint ( constraints  ) ;
  setConstraint ( masses       ) ;
}
// ============================================================================
// constructor from the fitter 
// ============================================================================
LoKi::AuxDTFBase::AuxDTFBase
( const std::vector<std::string>&      constraints , 
  const std::string&                   fitter      ,
  const LoKi::AuxDTFBase::MASSES2&     masses      )
  : m_fitterName  ( fitter ) 
{
  setConstraint ( constraints  ) ;
  setConstraint ( masses       ) ;
}
// ============================================================================
// constructor from the fitter 
// ============================================================================
LoKi::AuxDTFBase::AuxDTFBase
( const std::vector<LHCb::ParticleID>& constraints , 
  const std::string&                   fitter      , 
  const LoKi::AuxDTFBase::MASSES&      masses      )
  : m_fitterName  ( fitter ) 
{
  setConstraint ( constraints  ) ;
  setConstraint ( masses       ) ;
}
// ============================================================================
// constructor from the fitter 
// ============================================================================
LoKi::AuxDTFBase::AuxDTFBase
( const std::vector<LHCb::ParticleID>& constraints , 
  const std::string&                   fitter      ,
  const LoKi::AuxDTFBase::MASSES2&     masses      )
  : m_fitterName  ( fitter ) 
{
  setConstraint ( constraints  ) ;
  setConstraint ( masses       ) ;
}
// ============================================================================
// copy constructor 
// ============================================================================
LoKi::AuxDTFBase::AuxDTFBase
( const LoKi::AuxDTFBase& right ) 
  : LoKi::AuxFunBase     ( right )
  , LoKi::AuxDesktopBase ( right ) 
  , m_fitterName  ( right.m_fitterName  ) 
  , m_fitter      ( right.m_fitter      ) 
  , m_constraints ( right.m_constraints ) 
  , m_masses      ( right.m_masses      )
{}
// ============================================================================
// set vector of constraints 
// ============================================================================
unsigned int LoKi::AuxDTFBase::setConstraint
( const std::vector<std::string>& pids ) 
{
  m_constraints.clear() ;
  //
  std::set<LHCb::ParticleID> _pids ;
  for  ( const auto & ip : pids )
  {
    LHCb::ParticleID pid = LoKi::Particles::pidFromName ( ip ) ;
    Assert ( LHCb::ParticleID() != pid , 
             "Invalid mass constraint: '" + ip + "'" ) ;
    { _pids.insert ( LHCb::ParticleID( pid.abspid() ) ) ; }
  }
  //
  m_constraints.insert
    ( m_constraints.end () ,
      _pids.begin       () , 
      _pids.end         () ) ;
  //
  return m_constraints.size() ;
}
// ============================================================================
// virtual destructor 
// ============================================================================
LoKi::AuxDTFBase::~AuxDTFBase()
{
  if ( m_fitter && !gaudi() ) 
  {
    // Warning("IDecayTreeFit: manual reset") ;
    m_fitter.reset() ;
  }  
}
// ============================================================================
// set constraint
// ============================================================================
unsigned int LoKi::AuxDTFBase::setConstraint
( const             std::string &      pids ) 
{ return setConstraint ( std::vector<std::string> ( 1 , pids ) ) ; } 
// ============================================================================
// set vector of constraints 
// ============================================================================
unsigned int LoKi::AuxDTFBase::setConstraint
( const std::vector<LHCb::ParticleID>& pids ) 
{
  m_constraints.clear() ;
  //
  std::set<LHCb::ParticleID> _pids ;
  //
  static const LHCb::ParticleID s_ID{};
  //
  for  ( const auto& ip : pids )
  {
    if ( ip != s_ID ) 
    { _pids.emplace( ip.abspid() ) ; }
  }
  //
  m_constraints.insert
    ( m_constraints.end () ,
      _pids.begin       () , 
      _pids.end         () ) ;
  //
  return m_constraints.size() ;
}
// ============================================================================
// set constraint
// ============================================================================
unsigned int LoKi::AuxDTFBase::setConstraint
( const LHCb::ParticleID&      pids ) 
{ return setConstraint ( std::vector<LHCb::ParticleID> ( 1 , pids ) ) ; } 
// ============================================================================
// set vector of constraints 
// ============================================================================
unsigned int LoKi::AuxDTFBase::setConstraint
( const  LoKi::AuxDTFBase::MASSES& pids ) 
{
  m_masses.clear() ;
  //
  static const LHCb::ParticleID s_ID{};
  //
  for  (  const auto &ip : pids ) 
  {
    //
    LHCb::ParticleID pid = LoKi::Particles::pidFromName ( ip.first ) ;
    Assert ( s_ID != pid , 
             "Invalid mass constraint: '" + ip.first + "'" ) ;
    //
    m_masses [ pid ] = ip.second ;
  }
  //
  return m_masses.size() ;
}
// ============================================================================
// set vector of constraints 
// ============================================================================
unsigned int LoKi::AuxDTFBase::setConstraint
( const  LoKi::AuxDTFBase::MASSES2& pids ) 
{
  m_masses.clear() ;
  //
  static const LHCb::ParticleID s_ID = LHCb::ParticleID() ;
  //
  for  (  const auto& ip : pids ) 
  {
    if ( s_ID == ip.first ) { continue ; } // skip invalids  
    //
    m_masses [ LHCb::ParticleID ( ip.first.abspid() ) ] = ip.second ;
  }
  //
  return m_masses.size() ;
}
// ============================================================================
void LoKi::AuxDTFBase::loadFitter ( const std::string& name ) const 
{
  if ( !m_fitter ) 
  { m_fitter = LoKi::GetTools::decayTreeFitter ( *this , name ) ; }
  Assert ( !(!m_fitter) , "Invalid DecayTreeFitter" ) ;
}
// ============================================================================
// get constraints 
// ============================================================================
std::vector<std::string> LoKi::AuxDTFBase::constraints ()  const 
{
  //
  std::set<std::string> pids ;
  //
  for ( const auto& ipid : m_constraints )
  { pids.insert( LoKi::Particles::nameFromPID( ipid ) ) ; }
  //
  return std::vector<std::string>( pids.begin() , pids.end() );
}
// ============================================================================
// get constraints 
// ============================================================================
LoKi::AuxDTFBase::MASSES 
LoKi::AuxDTFBase::masses ()  const 
{
  //
  MASSES mass  ;
  //
  for ( const auto& ipid : m_masses )
  { 
    mass [ LoKi::Particles::nameFromPID( ipid.first ) ] = ipid.second ;
  }
  //
  return mass ;
}
// ============================================================================
// apply mass-constraints  
// ============================================================================
void LoKi::AuxDTFBase::applyConstraints () const 
{
  IDecayTreeFit* _fitter = fitter() ;
  Assert ( !(!_fitter) , "applyConstraints: Invalid Fitter!" ) ;
  //
  for ( const auto& ipid : m_constraints ) 
  { _fitter->addConstraint ( ipid ) ; }  
  //
  for ( const auto& imass : m_masses )
  { _fitter->addConstraint ( imass.first , imass.second ) ; }  
  //
}
// ============================================================================
// get the fitter  
// ============================================================================
IDecayTreeFit* LoKi::AuxDTFBase::fitter() const 
{ 
  if  (!m_fitter) { loadFitter ( fitterName() ) ; }
  return m_fitter ; 
}
// =============================================================================
// print constraints 
// =============================================================================
std::ostream& LoKi::AuxDTFBase::printConstraints ( std::ostream& s ) const 
{
  if ( m_constraints.empty() ) { return s ; }                          // RETURN
  //
  auto ss = constraints () ;
  if ( ss.empty ()         ) { return s ; }                            // RETURN 
  //
  if ( 1 == ss.size()      ) { return s << ", '"  << ss[0] << "' " ; } // RETURN
  //
  s << ", " ;
  Gaudi::Utils::toStream ( ss , s ) ;
  //
  if ( !m_masses.empty() ) 
  {
    s << ", " ;
    Gaudi::Utils::toStream ( masses() , s ) ;
  }
  //
  return s ;                                                           // RETURN
}



 
// ============================================================================
// The END 
// ============================================================================
