// $Id: TrUpgrade.cpp,v 1.2 2008-11-13 22:11:03 ibelyaev Exp $
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
LoKi::Hlt1::TrUpgrade::TrUpgrade 
( const std::string&             output  ,         // output selection name/key 
  const LoKi::Hlt1::UpgradeConf& config  )         //             configuration 
  : LoKi::BasicFunctors<LHCb::Track*>::Pipe () 
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
  : LoKi::BasicFunctors<LHCb::Track*>::Pipe () 
  , LoKi::Hlt1::UpgradeTool ( config ) 
  , m_sink      ( output ) 
{}
// ============================================================================
/*  constructor from all configuration parameters 
 *  @param output    the output selection name 
 *  @param trTool    the name of ITracksFromTrack tool 
 *  @param trType    the track type for upgrade 
 *  @param moveIDs   transfer IDs ?
 *  @param moveAncs  transfer anscendents ?
 *  @param moveInfo  transfer Extra Info ?
 *  @param ptOrder   order in pt ?
 */
// ============================================================================
LoKi::Hlt1::TrUpgrade::TrUpgrade 
( const std::string&        output   ,          //    output selection name/key 
  const std::string&        trTool   ,          //   ITrackFromTrack  tool name
  const std::string&        address  ,          //   TES location of the tracks 
  const LHCb::Track::Types  trType   ,          //                   track type 
  const bool                owner    ,          //                      owner ?
  const bool                moveIDs  ,          //               transfer IDs ? 
  const bool                moveAncs ,          //       transfer anscendents ? 
  const bool                moveInfo ,          //        transfer extra info ?
  const bool                ptOrder  )          //                   order pt ?
  : LoKi::BasicFunctors<LHCb::Track*>::Pipe () 
  , LoKi::Hlt1::UpgradeTool ( LoKi::Hlt1::UpgradeConf ( trTool   , 
                                                        address  , 
                                                        trType   , 
                                                        owner    , 
                                                        moveIDs  , 
                                                        moveAncs , 
                                                        moveInfo , 
                                                        ptOrder  ) )
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
