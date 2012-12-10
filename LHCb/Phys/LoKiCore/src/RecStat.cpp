// ============================================================================
// Include files 
// ============================================================================
// Event 
// ============================================================================
#include "Event/RecSummary.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/RecStat.h"
// ============================================================================
/** @file
 *  Implementation file for 
 *  class LoKi::TES::HasRecSummary and 
 *  class LoKi::TES::RecSummary 
 *  @see LoKi::TES::HasRecSummary and 
 *  @see LoKi::TES::RecSummary 
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
 *  @date 2011-02-02
 */
// ============================================================================
/*  Standard constructor from the key, location and 'rootInTES'
 *  @param key          the key to use 
 *  @param location     TES location of LHCb::RecSummary obejct
 *  @param useRootInTES flato to use/ignore root-in-tes 
 */
// ============================================================================
LoKi::TES::HasRecSummary::HasRecSummary 
( const int          key          , 
  const std::string& location     , 
  const bool         useRootInTES ) 
  : LoKi::TES::Exists ( location , useRootInTES ) 
  , m_key ( key )
{
  if  ( location.empty () ) 
  { setLocation( LHCb::RecSummaryLocation::Default ) ; }
}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::TES::HasRecSummary::~HasRecSummary () {}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::TES::HasRecSummary*
LoKi::TES::HasRecSummary::clone () const
{ return new LoKi::TES::HasRecSummary ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::TES::HasRecSummary::result_type 
LoKi::TES::HasRecSummary::operator() 
  ( /* LoKi::TES::HasRecSummary::argument a */ ) const 
{
  // check the algorithm
  Assert ( !(!m_algorithm) , "GaudiAlgorithm* points to NULL" ) ;
  //
  const LHCb::RecSummary* summary = 
    m_algorithm->getIfExists<LHCb::RecSummary> ( location () , useRootInTes () );
  if ( NULL == summary ) 
  {
    Error ( "No LHCb::RecSummary is available at '" + location () + "'") ;
    return false ;
  }
  //
  return summary -> hasInfo ( m_key ) ;
}    
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& 
LoKi::TES::HasRecSummary::fillStream ( std::ostream& s ) const 
{
  //
  s << " HASRECSUMMARY( " << m_key ;
  if ( !useRootInTes() || LHCb::RecSummaryLocation::Default != location() ) 
  { s << ",'" << location() << "'" ; }
  if ( !useRootInTes() ) { s << ", False " ; }
  //
  return s << " ) " ;
}
// ============================================================================
/*  Standard constructor from the key, location and 'rootInTES'
 *  @param key          the key to use 
 *  @param bad          the bad value 
 *  @param location     TES location of LHCb::RecSummary obejct
 *  @param useRootInTES flato to use/ignore root-in-tes 
 */
// ============================================================================
LoKi::TES::RecSummary::RecSummary 
( const int          key          , 
  const int          bad         , 
  const std::string& location     , 
  const bool         useRootInTES ) 
  : LoKi::TES::Contains ( location , useRootInTES ) 
  , m_key ( key )
  , m_bad ( bad )
{
  if  ( location.empty () ) 
  { setLocation( LHCb::RecSummaryLocation::Default ) ; }
}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::TES::RecSummary::~RecSummary () {}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::TES::RecSummary*
LoKi::TES::RecSummary::clone () const
{ return new LoKi::TES::RecSummary ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::TES::RecSummary::result_type 
LoKi::TES::RecSummary::operator() 
  ( /* LoKi::TES::HasRecSummary::argument a */ ) const 
{
  // check the algorithm
  Assert ( !(!m_algorithm) , "GaudiAlgorithm* points to NULL" ) ;
  //
  const LHCb::RecSummary* summary = 
    m_algorithm->getIfExists<LHCb::RecSummary> ( location () , useRootInTes () );
  if ( NULL == summary ) 
  {
    Error ( "No LHCb::RecSummary is available at '" + location () + "'") ;
    return m_bad ;
  }
  //
  return summary -> info ( m_key , m_bad ) ;
}    
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& 
LoKi::TES::RecSummary::fillStream ( std::ostream& s ) const 
{
  s << " RECSUMMARY( " << m_key << "," << m_bad ;
  if ( !useRootInTes() || LHCb::RecSummaryLocation::Default != location() ) 
  { s << ",'" << location() << "'" ; }
  if ( !useRootInTes() ) { s << ", False " ; }
  //
  return s << " ) " ;
}
// ============================================================================



// ============================================================================
// The END 
// ============================================================================
