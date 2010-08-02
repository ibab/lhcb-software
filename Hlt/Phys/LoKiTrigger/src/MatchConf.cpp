// $Id$
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/MatchConf.h"
// ============================================================================
/** @file 
 *  Implementation file for class LoKi::Hlt1::MatchConf
 *  @date 2008-11-14 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 */
// ============================================================================
/*  constructor 
 *  @param mTool ITrackMatch tool name 
 *  @param address  TES location of "matched" tracks 
 *  @param trType   track type
 *  @param moveIDs  transfer IDs ?
 *  @param moveAncs transfer ancestors ? 
 *  @param moveInfo transfer extra info ? 
 *  @param quality  matching quality 
 *  @param quality2 matching quality2
 */
// ============================================================================
LoKi::Hlt1::MatchConf::MatchConf 
( const std::string&       mTool    ,   //                ITrackMatch tool name  
  const std::string&       address  ,   //       TES location of matched tracks
  const LHCb::Track::Types trType   ,   //                           Track type 
  const bool               moveIDs  ,   //                       transfer IDs ? 
  const bool               moveAncs ,   //                 transfer ancestors ? 
  const bool               moveInfo ,   //                transfer Extra Info ? 
  const std::string&       quality  ,   //                     matching quality 
  const std::string&       quality2 ,   //                    mathcing quality2 
  const double             maxQCut  ,   //             max matching quality cut 
  const double             maxQ2Cut )   //            max matching quality2 cut  
  : m_mTool    ( mTool    ) 
  , m_address  ( address  ) 
  , m_trType   ( trType   ) 
  , m_moveIDs  ( moveIDs  ) 
  , m_moveAncs ( moveAncs ) 
  , m_moveInfo ( moveInfo ) 
  , m_quality  ( quality  ) 
  , m_quality2 ( quality2 ) 
  , m_maxQualityCut  ( maxQCut   ) 
  , m_maxQuality2Cut ( maxQ2Cut  ) 
{} 
// =============================================================================
// printout to ostream
// =============================================================================
std::ostream& LoKi::Hlt1::MatchConf::fillStream ( std::ostream& s ) const 
{
  return 
    s << "LoKi.Hlt1.MatchConf("
      << "'" << mTool  () << "',"
      << "'" << address() << "',"
      << "LHCb.Track." << trType() << ","
      << ( moveIDs  () ? "True," : "False," ) 
      << ( moveAncs () ? "True," : "False," ) 
      << ( moveInfo () ? "True," : "False," ) 
      << "'" << quality  () << "',"
      << "'" << quality2 () << "'," 
      <<     maxQualityCut  () << "," 
      <<     maxQuality2Cut () << ")" ;
}
// ============================================================================
// conversion to the string 
// ============================================================================
std::string LoKi::Hlt1::MatchConf::toString () const 
{
  std::ostringstream s ;
  fillStream ( s ) ;
  return s.str() ;
}
// ============================================================================
// output operator to ostream  
// ============================================================================
std::ostream& operator<<( std::ostream&                s , 
                          const LoKi::Hlt1::MatchConf& o ) 
{ return o.fillStream ( s ) ; }
// ============================================================================




// ============================================================================
// The END 
// ============================================================================
