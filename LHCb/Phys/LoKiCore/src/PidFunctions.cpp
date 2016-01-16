// $Id$ 
// ============================================================================
// PartProp
// ============================================================================
#include "Kernel/ParticleID.h"
#include "Kernel/ParticleProperty.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/PidFunctions.h"
#include "LoKi/ParticleProperties.h"
#include "LoKi/Math.h"
#include "LoKi/ToCpp.h"
// ============================================================================
/** @file 
 *  Implementation file for functions from namespace LoKi::Pids 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2015-01-18 
 */
// ============================================================================
LoKi::Pids::GetPids::GetPids ( const long id )
  : LoKi::AuxFunBase ( std::tie ( id ) )
  , m_ints  ( 1 , id ) 
  , m_names ()
{}
// ============================================================================
LoKi::Pids::GetPids::GetPids ( const unsigned long id )
  : LoKi::AuxFunBase ( std::tie ( id ) )
  , m_ints  ( 1 , id ) 
  , m_names ()
{}
// ============================================================================
LoKi::Pids::GetPids::GetPids ( const LHCb::ParticleID& id )
  : LoKi::AuxFunBase ( std::tie ( id ) )
  , m_ints  ( 1 , id.pid()  ) 
  , m_names ()
{}
// ============================================================================
LoKi::Pids::GetPids::GetPids ( const std::string& id )
  : LoKi::AuxFunBase ( std::tie ( id ) )
  , m_ints  () 
  , m_names ( 1 , id )
{}
// ============================================================================
LoKi::Pids::GetPids::GetPids ( const std::vector<int>& id )
  : LoKi::AuxFunBase ( std::tie ( id ) )
  , m_ints  ( id.begin() , id.end() ) 
  , m_names ()
{}
// ============================================================================
LoKi::Pids::GetPids::GetPids ( const std::vector<unsigned int>& id )
  : LoKi::AuxFunBase ( std::tie ( id ) )
  , m_ints  ( id.begin() , id.end() ) 
  , m_names ()
{}
// ============================================================================
LoKi::Pids::GetPids::GetPids ( const std::vector<long>& id )
  : LoKi::AuxFunBase ( std::tie ( id ) )
  , m_ints  ( id.begin() , id.end() ) 
  , m_names ()
{}
// ============================================================================
LoKi::Pids::GetPids::GetPids ( const std::vector<unsigned long>& id )
  : LoKi::AuxFunBase ( std::tie ( id ) )
  , m_ints  ( id.begin() , id.end() ) 
  , m_names ()
{}
// ============================================================================
LoKi::Pids::GetPids::GetPids ( const std::vector<LHCb::ParticleID>& id )
  : LoKi::AuxFunBase ( std::tie ( id ) )
  , m_ints  () 
  , m_names ()
{
  m_ints.resize  ( id.size  () ) ;
  std::transform ( id.begin () , 
                   id.end   () , 
                   m_ints.begin () , 
                   std::mem_fun_ref(&LHCb::ParticleID::pid ) ) ;
}
// ============================================================================
LoKi::Pids::GetPids::GetPids ( const std::vector<std::string>& id )
  : LoKi::AuxFunBase ( std::tie ( id ) )
  , m_ints  () 
  , m_names ( id )
{
  std::vector<LHCb::ParticleID> pids ;
  if ( gaudi() && !id.empty() ) 
  { pids = LoKi::Particles::pidsFromNames ( m_names ) ; }
  //
  m_ints.resize  ( pids.size  () ) ;
  std::transform ( pids.begin () , 
                   pids.end   () , 
                   m_ints .begin () , 
                   std::mem_fun_ref(&LHCb::ParticleID::pid ) ) ;
}
// ============================================================================
bool LoKi::Pids::GetPids::in_list ( const long id ) const 
{
  //
  if ( m_ints.empty() && !m_names.empty() ) { getData() ; }
  //
  return m_ints.end() != std::find ( m_ints.begin() , m_ints.end  () , id ) ;  
}
// ============================================================================
namespace 
{
  struct AbsId_ : public std::unary_function<long,bool>
  {
    AbsId_ ( const long id ) : m_absid ( std::abs ( id ) ) {} ;
    inline bool operator() ( const long i ) const { return std::abs ( i ) == m_absid ; }
  private:
    long m_absid ;
  } ;
}
// ============================================================================
bool LoKi::Pids::GetPids::in_abs_list ( const long id ) const 
{
  //
  if ( m_ints.empty() && !m_names.empty() ) { getData() ; }
  //
  return m_ints.end() != std::find_if ( m_ints.begin() , 
                                        m_ints.end  () , AbsId_( id ) ) ;
}
// ============================================================================
// load data (if needed) 
// ============================================================================
StatusCode LoKi::Pids::GetPids::getData()  const 
{
  //
  if ( !m_ints.empty() && m_names.empty() ) { return StatusCode::SUCCESS ; }
  if (  m_ints.size () == m_names.size () ) { return StatusCode::SUCCESS ; }
  //
  std::vector<LHCb::ParticleID> pids ( LoKi::Particles::pidsFromNames ( m_names ) ) ;
  //
  m_ints.resize  ( pids.size  () ) ;
  std::transform ( pids.begin () , 
                   pids.end   () , 
                   m_ints .begin () , 
                   std::mem_fun_ref(&LHCb::ParticleID::pid ) ) ;
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// The END 
// ============================================================================
