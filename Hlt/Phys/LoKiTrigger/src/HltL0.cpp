// $Id$
// ============================================================================
// $URL$
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
 *  Implementation file for functions frn manespace LoKi::L0 
 *
 *  This file is part of LoKi project: 
 *   ``C++ ToolKit for Smart and Friendly Physics Analysis''
 * 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date   2009-03-23
 *
 *  $Revision$
 *  Last Modification $Date$ 
 *                 by $Author$ 
 */
// ============================================================================
// constructor 
// ============================================================================
LoKi::L0::L0CaloCut::L0CaloCut( const L0DUBase::CaloType::Type type )
    : LoKi::BasicFunctors<const LHCb::L0CaloCandidate*>::Predicate()
    , m_type{type}
    , m_hasThreshold{false}
    , m_threshold{-1}
{
}
// ============================================================================
// constructor 
// ============================================================================
LoKi::L0::L0CaloCut::L0CaloCut( const L0DUBase::CaloType::Type type,
                                const int threshold )
    : LoKi::BasicFunctors<const LHCb::L0CaloCandidate*>::Predicate()
    , m_type{type}
    , m_hasThreshold{true}
    , m_threshold{threshold}
{
}
// ============================================================================
// constructor 
// ============================================================================
LoKi::L0::L0CaloCut::L0CaloCut()
    : LoKi::BasicFunctors<const LHCb::L0CaloCandidate*>::Predicate()
    // , m_type         (       ) Visual C++ does not like this line...
    , m_hasThreshold{false}
    , m_threshold{-1}
{
}
// ============================================================================
// assignement 
// ============================================================================
LoKi::L0::L0CaloCut& LoKi::L0::L0CaloCut::
operator=( const LoKi::L0::L0CaloCut& right )
{
    if ( &right == this ) return *this;
    //
    m_type = right.m_type;
    m_hasThreshold = right.m_hasThreshold;
    m_threshold = right.m_threshold;
    //
    return *this;
}
// ============================================================================
// MANDATORY: virtual desctructor 
// ============================================================================
LoKi::L0::L0CaloCut::~L0CaloCut()
{
}
// ============================================================================
// MANDATORY: clone method ("virtual construtor")
// ============================================================================
LoKi::L0::L0CaloCut* LoKi::L0::L0CaloCut::clone() const
{
    return new LoKi::L0::L0CaloCut( *this );
}
// ============================================================================
// the only essential method 
// ============================================================================
LoKi::L0::L0CaloCut::result_type LoKi::L0::L0CaloCut::
operator()( LoKi::L0::L0CaloCut::argument calo ) const
{
    return calo && ( m_type == calo->type() ) &&
           ( !m_hasThreshold || m_threshold <= calo->etCode() );
}
// ============================================================================
// standard printout 
// ============================================================================
std::ostream& LoKi::L0::L0CaloCut::fillStream( std::ostream& s ) const
{
    s << "L0CaloCut(" << m_type;
    if ( m_hasThreshold ) s << "," << m_threshold;
    return s << ")";
}

// ============================================================================
// constructor 
// ============================================================================
LoKi::L0::L0MuonCut::L0MuonCut( std::string t )
    : LoKi::BasicFunctors<const LHCb::L0MuonCandidate*>::Predicate()
    , m_type{std::move( t )}
    , m_hasThreshold{false}
    , m_threshold{-1}
{}
// ============================================================================
// constructor 
// ============================================================================
LoKi::L0::L0MuonCut::L0MuonCut( std::string t, const int threshold )
    : LoKi::BasicFunctors<const LHCb::L0MuonCandidate*>::Predicate()
    , m_type{std::move( t )}
    , m_hasThreshold{true}
    , m_threshold{threshold}
{}
// ============================================================================
// MANDATORY: virtual desctructor 
// ============================================================================
LoKi::L0::L0MuonCut::~L0MuonCut()
{
}
// ============================================================================
// MANDATORY: clone method ("virtual construtor")
// ============================================================================
LoKi::L0::L0MuonCut* LoKi::L0::L0MuonCut::clone() const
{
    return new LoKi::L0::L0MuonCut{*this};
}
// ============================================================================
// assignement 
// ============================================================================
LoKi::L0::L0MuonCut& LoKi::L0::L0MuonCut::
operator=( const LoKi::L0::L0MuonCut& right )
{
    if ( &right == this ) return *this;
    //
    m_type = right.m_type;
    m_hasThreshold = right.m_hasThreshold;
    m_threshold = right.m_threshold;
    //
    return *this;
}
// ============================================================================
// the only essential method 
// ============================================================================
LoKi::L0::L0MuonCut::result_type LoKi::L0::L0MuonCut::
operator()( LoKi::L0::L0MuonCut::argument muon ) const
{
    return muon && ( !m_hasThreshold || m_threshold < ( muon->encodedPt() & 0x7F ) );
}
// ============================================================================
// standard printout 
// ============================================================================
std::ostream& LoKi::L0::L0MuonCut::fillStream( std::ostream& s ) const
{
    s << "L0MuonCut('" << m_type << "'";
    if ( m_hasThreshold ) {
        s << "," << m_threshold;
    }
    return s << ")";
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
StatusCode LoKi::L0::getElementaryL0Data( const LHCb::L0DUConfig* config,
                                          const std::string& channel,
                                          const LoKi::L0::Names& names,
                                          LoKi::L0::Data& data )
{
    data.clear();
    if ( !config ) return L0Config_Invalid; // RETURN
    const LHCb::L0DUChannel::Map& channels = config->channels();
    auto ichannel = channels.find( channel );
    //
    return channels.end() != ichannel
               ? getElementaryL0Data( ichannel->second, names, data )
               : L0Channel_Unknown; // RETURN
}
// ============================================================================
/*  get the elementary data for the given channel by names 
 *  @param channel (INPUT)  the channel under the interest
 *  @param names   (INPUT)  the list of names 
 *  @param data    (OUTPUT) the output map of data 
 *  @return status code 
 */
// ============================================================================
StatusCode LoKi::L0::getElementaryL0Data( const LHCb::L0DUChannel* channel,
                                          const LoKi::L0::Names& names,
                                          LoKi::L0::Data& data )
{
    data.clear();
    if ( !channel ) return L0Channel_Invalid; // RETURN
    // loop over elementary conditions:
    for ( const auto& condition : channel->elementaryConditions() ) {
        // Retrieve const   L0DU Elementary data
        const LHCb::L0DUElementaryCondition* elementary = condition.second;
        if ( !elementary ) return L0Condition_Invalid; // RETURN
        // get the data
        const LHCb::L0DUElementaryData* edata = elementary->data();
        if ( !edata ) return L0Data_Invalid;  // RETURN
        // find the corresponding data & fill the output map
        for ( const auto& name : names ) {
            if ( edata->name() == name ) data[name] = elementary;
        } // NB!
    }
    return StatusCode::SUCCESS; // RETURN
}
// ============================================================================
/*  get the cuts for L0 Calo channels 
 *  @param config the L0 configuration object 
 *  @param channel the channel name 
 *  @param types the map  { name : type } 
 *  @param cuts  (OUTPUT) the list of L0Calo cuts 
 *  @see LoKi::L0::L0CaloCut
 */
// ============================================================================
StatusCode LoKi::L0::getL0Cuts( const LHCb::L0DUConfig* config,
                                const std::string& channel,
                                const LoKi::L0::CaloTypes& types,
                                LoKi::L0::L0CaloCuts& cuts )
{
    cuts.clear();
    if ( !config ) return L0Config_Invalid; // RETURN
    const LHCb::L0DUChannel::Map& channels = config->channels();
    auto ichannel = channels.find( channel );
    return ( channels.end() != ichannel )
               ? getL0Cuts( ichannel->second, types, cuts )
               : L0Channel_Unknown;
}
// ============================================================================
/*  get the cuts for L0 Calo channels 
 *  @param config the L0 configuration object 
 *  @param channel the channel name 
 *  @param types the map  { name : type } 
 *  @param cuts  (OUTPUT) the list of LCalo cuts 
 *  @see LoKi::L0::L0CaloCut
 */
// ============================================================================
StatusCode LoKi::L0::getL0Cuts( const LHCb::L0DUChannel* channel,
                                const LoKi::L0::CaloTypes& types,
                                LoKi::L0::L0CaloCuts& cuts )
{
    cuts.clear();
    if ( !channel ) return L0Channel_Invalid;  // RETURN
    // get all conditions and loop over the elementary conditions:
    for ( const auto& condition : channel->elementaryConditions() ) {
        // Retrieve const   L0DU Elementary data
        auto  elementary = condition.second;
        if ( !elementary ) return L0Condition_Invalid; // RETURN
        // get the data
        auto edata = elementary->data();
        if ( !edata ) return L0Data_Invalid; // RETURN
        // find the corresponding data & fill the output map
        for ( const auto& type : types ) {
            if ( edata->name() == type.first ) {
                cuts.emplace_back( type.second, elementary->threshold() ); // NB!
            }
        }
    }
    return StatusCode::SUCCESS; // RETURN
}
// ============================================================================
/*  get the cuts for L0 Calo channels 
 *  @param types the map  { name : type } 
 *  @param cuts  (OUTPUT) the list of LCalo cuts 
 *  @see LoKi::L0::L0CaloCut
 */
// ============================================================================
StatusCode LoKi::L0::getL0Cuts( const std::string& channel,
                                const LoKi::L0::CaloTypes& types,
                                LoKi::L0::L0CaloCuts& cuts )
{
    cuts.clear();
    auto itype = types.find( channel );
    if ( types.end() == itype ) return L0Channel_InvalidMap; // RETURN
    cuts.emplace_back( itype->second );
    return StatusCode::SUCCESS; // RETURN
}
// ============================================================================
/*  get L0Muon cuts from the L0 channel
 *  @param channel (INPUT)  the channel under the interest 
 *  @param names   (INPUT)  the list of names 
 *  @param cuts    (OUTPUT) the output list of cuts 
 *  @return status code 
 */
// ============================================================================
StatusCode LoKi::L0::getL0Cuts( const LHCb::L0DUChannel* channel,
                                const LoKi::L0::Names& names,
                                LoKi::L0::L0MuonCuts& cuts )
{
    cuts.clear();
    if ( !channel ) return L0Channel_Invalid; // RETURN
    // get the conditions and loop over elementary conditions:
    for ( const auto& condition : channel->elementaryConditions() ) {
        // Retrieve const   L0DU Elementary data
        auto elementary = condition.second;
        if ( !elementary ) return L0Condition_Invalid; // RETURN
        // get the data
        auto edata = elementary->data();
        if ( !edata ) return L0Data_Invalid; // RETURN
        // find the corresponding data & fill the output map
        for ( const auto& name : names ) {
            if ( edata->name() != name ) continue; // CONTINUE
            // get L0 muon cut
            cuts.emplace_back( name, elementary->threshold() ); // NB!
        }
    }
    return StatusCode::SUCCESS; // RETURN
}
// ============================================================================
/*  get the cuts for L0 Calo channels 
 *  @param config the L0 configuration object 
 *  @param channel the channel name 
 *  @param types the map  { name : type } 
 *  @param cuts  (OUTPUT) the list of LCalo cuts 
 *  @see LoKi::L0::L0CaloCut
 */
// ============================================================================
StatusCode LoKi::L0::getL0Cuts( const LHCb::L0DUConfig* config,
                                const std::string& channel,
                                const LoKi::L0::Names& names,
                                LoKi::L0::L0MuonCuts& cuts )
{
    cuts.clear();
    if ( !config ) return L0Config_Invalid; // RETURN
    auto& channels = config->channels();
    auto ichannel = channels.find( channel );
    return channels.end() != ichannel ? getL0Cuts( ichannel->second, names, cuts )
                                      : L0Channel_Unknown;
}
// ============================================================================
// The END 
// ============================================================================
