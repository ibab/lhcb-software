// $Id$
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
 *  The implementation file for 'upgrade' functors 
 *  @see LoKi:Hlt::Upgrade 
 *  @see LoKi:Hlt::UpgradeTr 
 *  @see LoKi:Hlt::UpgradeMTr 
 *  @see LoKi:Hlt::TrUpgradeTr 
 * 
 *  This file is a part of 
 *  <a href="http://cern.ch/lhcb-comp/Analysis/LoKi/index.html">LoKi project:</a>
 *   ``C++ ToolKit for Smart and Friendly Physics Analysis''
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  Version           $Revision$
 *  Last modification $Date$
 *                 by $Author$
 *
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
( std::string                    output  ,         // output selection name/key 
  const LoKi::Hlt1::UpgradeConf& config  )         //             configuration 
  : LoKi::BasicFunctors<const Hlt::Candidate*>::Pipe () 
  , LoKi::Hlt1::UpgradeTool ( config ) 
  , m_sink      { std::move(output) } 
{}
// ============================================================================
/*  constructor from all configuration parameters 
 *  @param output  the output selection name 
 *  @param config  the tool configuration 
 */
// ============================================================================
LoKi::Hlt1::Upgrade::Upgrade 
( std::string                    output  ,         // output selection name/key 
  const LoKi::Hlt1::UpgradeTool& config  )         //             configuration 
  : LoKi::BasicFunctors<const Hlt::Candidate*>::Pipe () 
  , LoKi::Hlt1::UpgradeTool ( config ) 
  , m_sink      { std::move(output) } 
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
{ return new LoKi::Hlt1::Upgrade { *this } ; }
// ============================================================================
// OPTIONAL:: nice printout 
// ============================================================================
std::ostream& LoKi::Hlt1::Upgrade::fillStream ( std::ostream& s ) const 
{
  return
    s  << "TC_UPGRADE_ALL('" << selName() 
       << "',"               << config () << ")";  
}
// ============================================================================
// MANDATORY : the only essential method 
// ============================================================================
LoKi::Hlt1::Upgrade::result_type 
LoKi::Hlt1::Upgrade::operator() 
  ( LoKi::Hlt1::Upgrade::argument a ) const
{
  result_type output ;
  if ( !a.empty() ) 
  {
    // StatusCode sc = upgrade ( a , output ) ;
    StatusCode sc = upgradeAll ( a , output ) ;
    if ( sc.isFailure() ) { Error(" error from upgrade" , sc, 0 ) ; }
  }
  // register the selection 
  return !m_sink ? output : m_sink ( output ) ;
}
// ============================================================================
/*  constructor from all configuration parameters 
 *  @param output  the output selection name 
 *  @param config  the tool configuration 
 */
// ============================================================================
LoKi::Hlt1::UpgradeTracks::UpgradeTracks 
( std::string                    output  ,         // output selection name/key 
  const LoKi::Hlt1::UpgradeConf& config  )         //             configuration 
  : LoKi::Hlt1::Upgrade( std::move(output) , config ) 
{}
// ============================================================================
/*  constructor from all configuration parameters 
 *  @param output  the output selection name 
 *  @param config  the tool configuration 
 */
// ============================================================================
LoKi::Hlt1::UpgradeTracks::UpgradeTracks 
( std::string                    output  ,         // output selection name/key 
  const LoKi::Hlt1::UpgradeTool& config  )         //             configuration 
  : LoKi::Hlt1::Upgrade( std::move(output) , config ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Hlt1::UpgradeTracks::~UpgradeTracks(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Hlt1::UpgradeTracks* 
LoKi::Hlt1::UpgradeTracks::clone() const 
{ return new LoKi::Hlt1::UpgradeTracks { *this } ; }
// ============================================================================
// OPTIONAL:: nice printout 
// ============================================================================
std::ostream& LoKi::Hlt1::UpgradeTracks::fillStream ( std::ostream& s ) const 
{
  return
    s  << "TC_UPGRADE_TR('" << selName() 
       << "',"              << config () << ")";  
}
// ============================================================================
// MANDATORY : the only essential method 
// ============================================================================
LoKi::Hlt1::UpgradeTracks::result_type 
LoKi::Hlt1::UpgradeTracks::operator() 
  ( LoKi::Hlt1::UpgradeTracks::argument a ) const
{
  result_type output ;
  if ( !a.empty() ) 
  {
    // NB: upgrade tracks only!!!
    StatusCode sc = upgradeTracks ( a , output ) ;
    if ( sc.isFailure() ) { Error ( "Error from upgrade" , sc, 0 ) ; }
  }
  // register the selection 
  return !m_sink ? output : m_sink ( output ) ;
}
// ============================================================================

// ============================================================================
/*  constructor from all configuration parameters 
 *  @param output  the output selection name 
 *  @param config  the tool configuration 
 */
// ============================================================================
LoKi::Hlt1::UpgradeMultiTracks::UpgradeMultiTracks 
( std::string                    output  ,         // output selection name/key 
  int                            index   ,         //   track index for upgrade  
  const LoKi::Hlt1::UpgradeConf& config  )         //             configuration 
  : LoKi::Hlt1::Upgrade( std::move(output) , config ) 
  , m_index ( index ) 
{}
// ============================================================================
/*  constructor from all configuration parameters 
 *  @param output  the output selection name 
 *  @param config  the tool configuration 
 */
// ============================================================================
LoKi::Hlt1::UpgradeMultiTracks::UpgradeMultiTracks 
( std::string                    output  ,         // output selection name/key 
  int                            index   ,   //   track index for upgrade  
  const LoKi::Hlt1::UpgradeTool& config  )         //             configuration 
  : LoKi::Hlt1::Upgrade( std::move(output) , config ) 
  , m_index ( index ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Hlt1::UpgradeMultiTracks::~UpgradeMultiTracks(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Hlt1::UpgradeMultiTracks* 
LoKi::Hlt1::UpgradeMultiTracks::clone() const 
{ return new LoKi::Hlt1::UpgradeMultiTracks { *this } ; }
// ============================================================================
// OPTIONAL:: nice printout 
// ============================================================================
std::ostream& LoKi::Hlt1::UpgradeMultiTracks::fillStream ( std::ostream& s ) const 
{
  return
    s  << "TC_UPGRADE_MT('" << selName() << "'"
       << ","               << m_index   
       << ","               << config () << ")";  
}
// ============================================================================
// MANDATORY : the only essential method 
// ============================================================================
LoKi::Hlt1::UpgradeMultiTracks::result_type 
LoKi::Hlt1::UpgradeMultiTracks::operator() 
  ( LoKi::Hlt1::UpgradeMultiTracks::argument a ) const
{
  result_type output ;
  if ( !a.empty() ) 
  {
    // NB: upgrade multi tracks only!!!
    StatusCode sc = 
      m_index < 0 ?  
      upgradeMultiTracks ( a ,           output ) : 
      upgradeMultiTracks ( a , m_index , output ) ;
    if ( sc.isFailure() ) { Error ( "Error from upgrade" , sc, 0 ) ; }
  }
  // register the selection 
  return !m_sink ? output : m_sink ( output ) ;
}
// ============================================================================
// The END 
// ============================================================================
