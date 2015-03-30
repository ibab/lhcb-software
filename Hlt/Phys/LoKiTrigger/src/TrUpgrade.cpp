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
#include "LoKi/TrgToCpp.h"
#include "LoKi/TrUpgrade.h"
// ============================================================================
// Local
// ============================================================================
#include "LTTools.h"
// ============================================================================
/** @file
 *  The implementation file for 'upgrade' functors 
 *  @see LoKi:Hlt::UpgradeTr 
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
LoKi::Hlt1::UpgradeTracks::UpgradeTracks 
( const std::string&             output  ,         // output selection name/key 
  const LoKi::Hlt1::UpgradeConf& config  )         //             configuration 
  : LoKi::AuxFunBase ( std::tie ( output, config ) ) 
  , LoKi::BasicFunctors<const Hlt::Candidate*>::Pipe () 
  , LoKi::Hlt1::UpgradeTool ( config ) 
  , m_sink                  ( output )
{}
// ============================================================================
/*  constructor from all configuration parameters 
 *  the location is used to search for pre-upgraded tracks
 *  @param output  the output selection name 
 *  @param config  the tool configuration 
 */
// ============================================================================
LoKi::Hlt1::UpgradeTracks::UpgradeTracks 
( const std::string&             output ,          // output selection name/key 
  const LoKi::Hlt1::UpgradeConf& config ,          //             configuration 
  const std::string&         complement )          // complement cache location
  : LoKi::AuxFunBase ( std::tie ( output, config, complement ) ) 
  , LoKi::BasicFunctors<const Hlt::Candidate*>::Pipe () 
  , LoKi::Hlt1::UpgradeTool ( config, complement )
  , m_sink                  ( output )
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
   s  << "TC_UPGRADE_TR('" << selName() 
      << "',"              << config ();
   if (!complement().empty())
     s  << "',"            << complement();
  s  << "')";
  return s;
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
