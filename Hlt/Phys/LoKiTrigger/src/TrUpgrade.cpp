// $Id:$
// ============================================================================
// $URL$
// ============================================================================
// Include Files 
// ============================================================================
// GaudiAlg 
// ============================================================================
#include "GaudiAlg/GaudiAlgorithm.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/Track.h"
// ============================================================================
// HltBase 
// ============================================================================
#include "HltBase/HltUtils.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/TrUpgrade.h"
// ============================================================================
// Local
// ============================================================================
#include "LTTools.h"
// ============================================================================
/** @file
 *  The implementation file for class LoKi:Hlt::TrUpgrade 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-11-12
 */
// ============================================================================
/*  constructor from all configuration parameters 
 *  @param output  the output selection name 
 *  @param config  the tool configuration 
 */
// ============================================================================
LoKi::Hlt1::Upgrade::Upgrade 
( const std::string&             output  ,         // output selection name/key 
  const LoKi::Hlt1::UpgradeConf& config  )         //             configuration 
  : LoKi::BasicFunctors<const Hlt::Candidate*>::Pipe () 
  , LoKi::Hlt1::UpgradeTool ( config ) 
  , m_sink      ( output ) 
{}
// ============================================================================
/*  constructor from all configuration parameters 
 *  @param output  the output selection name 
 *  @param config  the tool configuration 
 */
// ============================================================================
LoKi::Hlt1::Upgrade::Upgrade 
( const std::string&             output  ,         // output selection name/key 
  const LoKi::Hlt1::UpgradeTool& config  )         //             configuration 
  : LoKi::BasicFunctors<const Hlt::Candidate*>::Pipe () 
  , LoKi::Hlt1::UpgradeTool ( config ) 
  , m_sink      ( output ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Hlt1::Upgrade::~Upgrade(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Hlt1::Upgrade* 
LoKi::Hlt1::Upgrade::clone() const 
{ return new LoKi::Hlt1::Upgrade ( *this ) ; }
// ============================================================================
// OPTIONAL:: nice printout 
// ============================================================================
std::ostream& LoKi::Hlt1::Upgrade::fillStream ( std::ostream& s ) const 
{
  return
    s  << "TC_UPGRADE('" << selName() 
       << "',"           << config () << ")";  
}
// ============================================================================
// MANDATORY : theonly essentiam method 
// ============================================================================
LoKi::Hlt1::Upgrade::result_type 
LoKi::Hlt1::Upgrade::operator() 
  ( LoKi::Hlt1::Upgrade::argument a ) const
{
  result_type output ;
  StatusCode sc = upgrade ( a , output ) ;
  if ( sc.isFailure() ) { Error(" error from upgrade" , sc ) ; }
  // register the selection 
  return m_sink ( output ) ;
}
// ============================================================================
/*  constructor from all configuration parameters 
 *  @param output  the output selection name 
 *  @param config  the tool configuration 
 */
// ============================================================================
LoKi::Hlt1::TrUpgrade::TrUpgrade 
( const std::string&             output  ,         // output selection name/key 
  const LoKi::Hlt1::UpgradeConf& config  )         //             configuration 
  : LoKi::BasicFunctors<const LHCb::Track*>::Pipe () 
  , LoKi::Hlt1::UpgradeTool ( config ) 
  , m_sink      ( output ) 
{}
// ============================================================================
/*  constructor from all configuration parameters 
 *  @param output  the output selection name 
 *  @param config  the tool configuration 
 */
// ============================================================================
LoKi::Hlt1::TrUpgrade::TrUpgrade 
( const std::string&             output  ,         // output selection name/key 
  const LoKi::Hlt1::UpgradeTool& config  )         //             configuration 
  : LoKi::BasicFunctors<const LHCb::Track*>::Pipe () 
  , LoKi::Hlt1::UpgradeTool ( config ) 
  , m_sink      ( output ) 
{}
// ============================================================================
// OPTIONAL:: nice printout 
// ============================================================================
std::ostream& LoKi::Hlt1::TrUpgrade::fillStream ( std::ostream& s ) const 
{
  return
    s  << "TrUPGRADE('" << selName() 
       << "',"          << config () << ")";  
}
// ============================================================================
// MANDATORY : theonly essentiam method 
// ============================================================================
LoKi::Hlt1::TrUpgrade::result_type 
LoKi::Hlt1::TrUpgrade::operator() 
  ( LoKi::Hlt1::TrUpgrade::argument a ) const
{
  result_type output ;
  StatusCode sc = upgrade ( a , output ) ;
  if ( sc.isFailure() ) { Error(" error from upgrade" , sc ) ; }
  // register the selection 
  return m_sink ( output ) ;
}
// ============================================================================
// The END 
// ============================================================================
