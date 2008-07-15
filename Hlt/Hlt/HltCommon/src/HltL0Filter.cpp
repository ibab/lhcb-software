// $Id: HltL0Filter.cpp,v 1.4 2008-07-15 09:55:50 graven Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 


// local
#include "HltL0Filter.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : HltL0Filter
//
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltL0Filter );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltL0Filter::HltL0Filter( const std::string& name,
                    ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
{
  declareProperty("L0DULocation", m_l0Location = L0DUReportLocation::Default );
  m_l0Channels.declareUpdateHandler( &HltL0Filter::updateChannels , this);
  declareProperty("L0Channels",   m_l0Channels);
}
//=============================================================================
// Destructor
//=============================================================================
HltL0Filter::~HltL0Filter() {}; 

//=============================================================================
// Restart -- we explicitly support restart... woho!
//=============================================================================
StatusCode HltL0Filter::restart() 
{
    return StatusCode::SUCCESS;
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltL0Filter::initialize() {
  debug() << "==> Initialize" << endmsg;
  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  // no selection as input, but we do have an output selection,
  // of 'no candidate' type..
  registerSelection();

  //TODO: maybe at some point we start filling in part of the TCK 
  //      dependent L0 mapping already at initialize...
  
  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltL0Filter::execute() {

  LHCb::L0DUReport* l0 = get<L0DUReport>(m_l0Location);

  long L0TCK = l0->tck();

  // make sure we have the correct mapping for this L0 configuration
  TCKMap_t::const_iterator map =  m_map.find(L0TCK);
  if (map == m_map.end()) {
	if (msgLevel(MSG::DEBUG)) {
        debug() << " adding new map for L0 TCK " << L0TCK << endmsg;
    }
    map = m_map.insert( L0TCK, getL0Map(l0->configuration()->channels()) ).first;
  }

  bool pass = l0->decision();
  if (msgLevel(MSG::DEBUG)) debug() << " L0 decision " << pass << endreq;

  //TODO: right now we always require the 'or' of a set of channels, maybe
  //      we should add the possibility of adding the 'and' of a set?

  //TODO CHECK: is it possible l0->decision is false, but a channel is true?
  //    if not, we can abandon anything with the global L0 negative here...
  //    but then again, why would we ever see those events?
  if (!map->second.empty() /* && pass */) {
    pass = false;
    ChannelMap_t::const_iterator i = map->second.begin(), 
                               end = map->second.end();
    while (!pass && i!= end) pass = l0->channelDecision((i++)->second);
  }
  
  setDecision(pass);
  if (msgLevel(MSG::DEBUG)) 
    debug() << "HltL0Filter: " << (pass?"accept":"reject") << endreq;
  return StatusCode::SUCCESS;
};

void
HltL0Filter::updateChannels(Property&) {
    m_map.clear(); // invalidate all our channel mappings
}

HltL0Filter::ChannelMap_t 
HltL0Filter::getL0Map( const LHCb::L0DUChannel::Map& channels) const {
    ChannelMap_t m;
    typedef std::vector<std::string>::const_iterator iter_t;
    for (iter_t i  = m_l0Channels.value().begin();
                i != m_l0Channels.value().end(); ++i ) {
        LHCb::L0DUChannel::Map::const_iterator chan = channels.find(*i);
        Assert(chan != channels.end(),"Could not find requested L0channel");
        m.insert( *i, chan->second->id() );
    }
    return m;
}
