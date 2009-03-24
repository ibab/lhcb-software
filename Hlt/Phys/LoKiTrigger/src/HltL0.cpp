// $Id: HltL0.cpp,v 1.1 2009-03-24 17:33:26 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <string>
#include <sstream>
// ============================================================================
// L0Event 
// ============================================================================
#include "Event/L0DUReport.h"
#include "Event/L0DUConfig.h"
#include "Event/L0CaloCandidate.h"
#include "Event/L0MuonCandidate.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/HltL0.h"
// ============================================================================
/** @file
 *  Implementation file for functions frn manespace Hlt::L0Utils 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date   2009-03-23
 */
// ============================================================================
// standard printout 
// ============================================================================
std::ostream& Hlt::L0Utils::L0CaloCut::fillStream ( std::ostream& s ) const 
{
  s << "L0CaloCut(" << m_type ;
  if ( m_hasThreshold ) { s << "," << m_threshold ; }
  return s << ")" ;
}
// ============================================================================
// conversion to string 
// ============================================================================
std::string Hlt::L0Utils::L0CaloCut::toString   () const 
{
  std::ostringstream s ;
  fillStream ( s ) ;
  return s.str() ;
}
// ============================================================================
// standard printout 
// ============================================================================
std::ostream& Hlt::L0Utils::L0MuonCut::fillStream ( std::ostream& s ) const 
{
  s << "L0MuonCut('" << m_type << "'" ;
  if ( m_hasThreshold ) { s << "," << m_threshold ; }
  return s << ")" ;
}
// ============================================================================
// conversion to string 
// ============================================================================
std::string Hlt::L0Utils::L0MuonCut::toString   () const 
{
  std::ostringstream s ;
  fillStream ( s ) ;
  return s.str() ;
}
// ============================================================================
/*  get the elementary data for the given channel by names 
 *  @param config  (INPUT)  L0DUConfig object
 *  @param channel (INPUT)  the channel under the interest 
 *  @param names   (INPUT)  the list of names 
 *  @param data    (OUTPUT) the output map of data 
 *  @return status code 
 */
// ============================================================================
StatusCode Hlt::L0Utils::getElementaryL0Data 
( const LHCb::L0DUConfig*    config  , 
  const std::string&         channel ,
  const Hlt::L0Utils::Names& names   , 
  Hlt::L0Utils::Data&        data    ) 
{
  data.clear() ;
  if ( 0 == config ) { return L0Config_Invalid ; }                   // RETURN 
  const LHCb::L0DUChannel::Map& channels = config->channels() ;
  LHCb::L0DUChannel::Map::const_iterator ichannel = channels.find(channel);
  if ( channels.end() == ichannel ) { return L0Channel_Unknown ; }    // RETURN 
  //
  return getElementaryL0Data 
    ( ichannel -> second , names , data ) ;
}
// ============================================================================
/*  get the elementary data for the given channel by names 
 *  @param channel (INPUT)  the channel under the interest
 *  @param names   (INPUT)  the list of names 
 *  @param data    (OUTPUT) the output map of data 
 *  @return status code 
 */
// ============================================================================
StatusCode Hlt::L0Utils::getElementaryL0Data 
( const LHCb::L0DUChannel* channel ,
  const Hlt::L0Utils::Names& names , 
  Hlt::L0Utils::Data&        data  ) 
{
  data.clear() ;
  if ( 0 == channel ) { return L0Channel_Invalid ; }                  // RETURN 
  // get the conditions 
  const LHCb::L0DUElementaryCondition::Map& conditions =
    channel->elementaryConditions() ;  
  // loop over elementary conditions:
  for ( LHCb::L0DUElementaryCondition::Map::const_iterator
          condition = conditions.begin();
        condition!=conditions.end(); ++condition )
  {
    // Retrieve const   L0DU Elementary data
    const LHCb::L0DUElementaryCondition* elementary = condition->second ;
    if ( 0 == elementary  ) { return L0Condition_Invalid ; }          // RETURN
    // get the data 
    const LHCb::L0DUElementaryData* edata = elementary->data() ;
    if ( 0 == edata       ) { return L0Data_Invalid      ; }          // RETURN 
    // find the corresponding data & fill the output map
    for ( Names::const_iterator iname = names.begin() ; 
          names.end() != iname ; ++iname ) 
    { if ( edata->name() == *iname ) { data[ *iname ] = elementary ; } }  // NB!
  }
  return StatusCode::SUCCESS ;                                        // RETURN
}
// ============================================================================
/*  get the cuts for L0 Calo channels 
 *  @param config the L0 configuration object 
 *  @param channel the channel name 
 *  @param types the map  { name : type } 
 *  @param cuts  (OUTPUT) the list of L0Calo cuts 
 *  @see Hlt::L0Utils::L0CaloCut
 */
// ============================================================================
StatusCode Hlt::L0Utils::getL0Cuts 
( const LHCb::L0DUConfig*        config  , 
  const std::string&             channel ,
  const Hlt::L0Utils::CaloTypes& types   , 
  Hlt::L0Utils::L0CaloCuts&      cuts    ) 
{
  cuts.clear() ;
  if ( 0 == config ) { return L0Config_Invalid ; }                    // RETURN 
  //
  const LHCb::L0DUChannel::Map& channels = config->channels() ;
  LHCb::L0DUChannel::Map::const_iterator ichannel = channels.find(channel);
  if ( channels.end() == ichannel ) { return L0Channel_Unknown ; }    // RETURN 
  //
  return getL0Cuts ( ichannel -> second , types , cuts ) ;            // RETURN 
}
// ============================================================================
/*  get the cuts for L0 Calo channels 
 *  @param config the L0 configuration object 
 *  @param channel the channel name 
 *  @param types the map  { name : type } 
 *  @param cuts  (OUTPUT) the list of LCalo cuts 
 *  @see Hlt::L0Utils::L0CaloCut
 */
// ============================================================================
StatusCode Hlt::L0Utils::getL0Cuts 
( const LHCb::L0DUChannel*       channel ,
  const Hlt::L0Utils::CaloTypes& types   , 
  Hlt::L0Utils::L0CaloCuts&      cuts    ) 
{
  cuts.clear() ;
  if ( 0 == channel ) { return L0Channel_Invalid ; }                  // RETURN
  // get all conditions:
  const LHCb::L0DUElementaryCondition::Map& conditions = 
    channel->elementaryConditions() ;
  // loop over elementary conditions:
  for ( LHCb::L0DUElementaryCondition::Map::const_iterator condition = conditions.begin();
        condition!=conditions.end(); ++condition )
  {
    // Retrieve const   L0DU Elementary data
    const LHCb::L0DUElementaryCondition* elementary = condition->second ;
    if ( 0 == elementary  ) { return L0Condition_Invalid ; }          // RETURN
    // get the data 
    const LHCb::L0DUElementaryData* edata = elementary->data() ;
    if ( 0 == edata       ) { return L0Data_Invalid      ; }          // RETURN 
    // find the corresponding data & fill the output map
    for ( CaloTypes::const_iterator itype = types.begin() ; 
          types.end() != itype ; ++itype ) 
    { 
      if ( edata->name() !=  itype->first ) { continue ; }          // CONTINUE 
      // 
      cuts.push_back  
        ( L0CaloCut ( itype -> second , elementary -> threshold () ) ) ; // NB!
    } 
  }
  return StatusCode::SUCCESS ;                                        // RETURN  
}
// ============================================================================
/*  get the cuts for L0 Calo channels 
 *  @param types the map  { name : type } 
 *  @param cuts  (OUTPUT) the list of LCalo cuts 
 *  @see Hlt::L0Utils::L0CaloCut
 */
// ============================================================================
StatusCode Hlt::L0Utils::getL0Cuts
( const std::string&             channel ,
  const Hlt::L0Utils::CaloTypes& types   , 
  Hlt::L0Utils::L0CaloCuts&      cuts    ) 
{
  cuts.clear() ;
  CaloTypes::const_iterator itype = types.find ( channel ) ;
  if ( types.end() == itype ) { return L0Channel_InvalidMap ; }       // RETURN 
  cuts.push_back ( L0CaloCut ( itype->second ) ) ;
  return StatusCode::SUCCESS ;                                        // RETURN 
}
// ============================================================================
/*  get L0Muon cuts from the L0 channel
 *  @param channel (INPUT)  the channel under the interest 
 *  @param names   (INPUT)  the list of names 
 *  @param cuts    (OUTPUT) the output list of cuts 
 *  @return status code 
 */
// ============================================================================
StatusCode Hlt::L0Utils::getL0Cuts 
( const LHCb::L0DUChannel*   channel ,
  const Hlt::L0Utils::Names& names   , 
  Hlt::L0Utils::L0MuonCuts&  cuts    )
{
  cuts.clear() ;
  if ( 0 == channel ) { return L0Channel_Invalid ; }                  // RETURN 
  // get the conditions 
  const LHCb::L0DUElementaryCondition::Map& conditions =
    channel->elementaryConditions() ;  
  // loop over elementary conditions:
  for ( LHCb::L0DUElementaryCondition::Map::const_iterator
          condition = conditions.begin();
        condition!=conditions.end(); ++condition )
  {
    // Retrieve const   L0DU Elementary data
    const LHCb::L0DUElementaryCondition* elementary = condition->second ;
    if ( 0 == elementary  ) { return L0Condition_Invalid ; }          // RETURN
    // get the data 
    const LHCb::L0DUElementaryData* edata = elementary->data() ;
    if ( 0 == edata       ) { return L0Data_Invalid      ; }          // RETURN 
    // find the corresponding data & fill the output map
    for ( Names::const_iterator iname = names.begin() ; 
          names.end() != iname ; ++iname ) 
    { 
      if ( edata->name() != *iname ) { continue ; }                 // CONTINUE
      // get L0 muon cut
      cuts.push_back 
        (  L0MuonCut ( *iname , elementary->threshold () ) ) ;           // NB!  
    }  
  }
  return StatusCode::SUCCESS ;                                        // RETURN
}
// ============================================================================
/*  get the cuts for L0 Calo channels 
 *  @param config the L0 configuration object 
 *  @param channel the channel name 
 *  @param types the map  { name : type } 
 *  @param cuts  (OUTPUT) the list of LCalo cuts 
 *  @see Hlt::L0Utils::L0CaloCut
 */
// ============================================================================ 
StatusCode Hlt::L0Utils::getL0Cuts
( const LHCb::L0DUConfig*    config  , 
  const std::string&         channel ,
  const Hlt::L0Utils::Names& names   , 
  Hlt::L0Utils::L0MuonCuts&  cuts    ) 
{
  cuts.clear() ;
  if ( 0 == config ) { return L0Config_Invalid ; }                    // RETURN 
  const LHCb::L0DUChannel::Map& channels = config->channels() ;
  LHCb::L0DUChannel::Map::const_iterator ichannel = channels.find(channel);
  if ( channels.end() == ichannel ) { return L0Channel_Unknown ; }    // RETURN 
  //
  return getL0Cuts ( ichannel -> second , names , cuts ) ;            // RETURN 
}
// ============================================================================
// The END 
// ============================================================================
