// $Id$
// ============================================================================
// Include files 
// ============================================================================
// STD & STL
// ============================================================================
#include <sstream>
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/UpgradeConf.h"
// ============================================================================
/** @file 
 *  Implementation file for class LoKi::Hlt1::UpgradeConf
 *  @date 2008-11-12 
 *  @author Vanya  BELYAEV Ivan.Belyaev@nikhef.nl
 */
// ============================================================================
// constructor 
// ============================================================================
LoKi::Hlt1::UpgradeConf::UpgradeConf 
( const std::string&        trTool   ,     //        ITrackFromTrack  tool name
  const std::string&        address  ,     //   TES location of upgraded tracks 
  const LHCb::Track::Types  trType   ,     //                        track type 
  const bool                owner    ,     //                           owner ? 
  const bool                moveIDs  ,     //                    transfer IDs ? 
  const bool                moveAncs ,     //            transfer anscendents ? 
  const bool                moveInfo ,     //             transfer extra info ?
  const bool                ptOrder  )     //                         order pt?
  : m_trTool   ( trTool   ) 
  , m_address  ( address  )
  , m_trType   ( trType   )
  , m_owner    ( owner    )
  , m_moveIDs  ( moveIDs  ) 
  , m_moveAncs ( moveAncs ) 
  , m_moveInfo ( moveInfo ) 
  , m_ptOrder  ( ptOrder  ) 
{}  
// ============================================================================
// output operator 
// ============================================================================
std::ostream& LoKi::Hlt1::UpgradeConf::fillStream ( std::ostream& s ) const 
{
  return 
    s << "LoKi.Hlt1.UpgradeConf('" << trTool  () << "'"
      << ",'"                      << address () << "'"
      << ",LHCb.Track."            << trType  ()
      << "," <<  ( owner    () ? "True" : "False" ) 
      << "," <<  ( moveIDs  () ? "True" : "False" ) 
      << "," <<  ( moveAncs () ? "True" : "False" ) 
      << "," <<  ( moveInfo () ? "True" : "False" ) 
      << "," <<  ( ptOrder  () ? "True" : "False" ) << ")" ;
}
// ============================================================================
// conversion to string 
// ============================================================================
std::string LoKi::Hlt1::UpgradeConf::toString () const 
{ 
  std::ostringstream s ;
  fillStream ( s ) ;
  return s.str() ;
}
// ============================================================================
// output operator to ostream  
// ============================================================================
std::ostream& operator<<( std::ostream&                  s , 
                          const LoKi::Hlt1::UpgradeConf& o ) 
{ return o.fillStream ( s ) ; }
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
