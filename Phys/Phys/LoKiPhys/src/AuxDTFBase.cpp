// $Id: AuxDTFBase.cpp,v 1.3 2010-06-09 17:54:27 ibelyaev Exp $
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
  : LoKi::AuxDesktopBase () 
  , m_fitterName  (        ) 
  , m_fitter      ( fitter ) 
  , m_constraints (        )
{}
// ============================================================================
// constructor from the fitter 
// ============================================================================
LoKi::AuxDTFBase::AuxDTFBase ( const std::string& fitter ) 
  : LoKi::AuxDesktopBase () 
  , m_fitterName  ( fitter ) 
  , m_fitter      () 
  , m_constraints ()
{}
// ============================================================================
// constructor from the fitter 
// ============================================================================
LoKi::AuxDTFBase::AuxDTFBase
( const std::vector<std::string>&      constraints , 
  const IDecayTreeFit*                 fitter      ) 
  : LoKi::AuxDesktopBase () 
  , m_fitterName  (        ) 
  , m_fitter      ( fitter ) 
  , m_constraints (        )
{
  setConstraint ( constraints  ) ;
}
// ============================================================================
// constructor from the fitter 
// ============================================================================
LoKi::AuxDTFBase::AuxDTFBase
( const std::vector<LHCb::ParticleID>& constraints , 
  const IDecayTreeFit*                 fitter      ) 
  : LoKi::AuxDesktopBase () 
  , m_fitterName  (        ) 
  , m_fitter      ( fitter ) 
  , m_constraints (        )
{
  setConstraint ( constraints  ) ;
}
// ============================================================================
// constructor from the fitter 
// ============================================================================
LoKi::AuxDTFBase::AuxDTFBase
( const std::vector<std::string>&      constraints , 
  const std::string&                   fitter      ) 
  : LoKi::AuxDesktopBase () 
  , m_fitterName  ( fitter ) 
  , m_fitter      (        ) 
  , m_constraints (        )
{
  setConstraint ( constraints  ) ;
}
// ============================================================================
// constructor from the fitter 
// ============================================================================
LoKi::AuxDTFBase::AuxDTFBase
( const std::vector<LHCb::ParticleID>& constraints , 
  const std::string&                   fitter      ) 
  : LoKi::AuxDesktopBase () 
  , m_fitterName  ( fitter ) 
  , m_fitter      (        ) 
  , m_constraints (        )
{
  setConstraint ( constraints  ) ;
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
  for  ( std::vector<std::string>::const_iterator ip = 
           pids.begin() ; pids.end() != ip ; ++ip ) 
  {
    LHCb::ParticleID pid = LoKi::Particles::pidFromName ( *ip ) ;
    Assert ( LHCb::ParticleID() != pid , 
             "Invalid mass constraint: '" + (*ip) + "'" ) ;
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
LoKi::AuxDTFBase::~AuxDTFBase(){}
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
  static const LHCb::ParticleID s_ID = LHCb::ParticleID() ;
  //
  for  ( std::vector<LHCb::ParticleID>::const_iterator ip = 
           pids.begin() ; pids.end() != ip ; ++ip ) 
  {
    if ( s_ID != *ip ) 
    { _pids.insert ( LHCb::ParticleID( ip->abspid() ) ) ; }
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
  for ( std::vector<LHCb::ParticleID>::const_iterator ipid = 
          m_constraints.begin() ; m_constraints.end()  != ipid ; ++ipid ) 
  { pids.insert( LoKi::Particles::nameFromPID( *ipid ) ) ; }
  //
  return std::vector<std::string>( pids.begin() , pids.end() );
}
// ============================================================================
// apply mass-constraints  
// ============================================================================
void LoKi::AuxDTFBase::applyConstraints () const 
{
  IDecayTreeFit* _fitter = fitter() ;
  Assert ( !(!_fitter) , "applyConstraints: Invalid Fitter!" ) ;
  //
  for ( std::vector<LHCb::ParticleID>::const_iterator ipid = 
          m_constraints.begin() ; m_constraints.end() != ipid ; ++ipid ) 
  { _fitter->addConstraint ( *ipid ) ; }  
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
  std::vector<std::string> ss = constraints () ;
  if ( ss.empty ()         ) { return s ; }                            // RETURN 
  //
  if ( 1 == ss.size()      ) { return s << ", '"  << ss[0] << "' " ; } // RETURN
  //
  s << ", " ;
  Gaudi::Utils::toStream ( ss , s ) ;  
  return s ;                                                           // RETURN 
}



 
// ============================================================================
// The END 
// ============================================================================
