// $Id: HltL0MuonCandidates.cpp,v 1.4 2008-12-21 17:14:10 graven Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include <algorithm>
#include "boost/foreach.hpp"
#include "Event/L0DUBase.h"
#include "Event/L0DUReport.h"
#include "Event/L0MuonCandidate.h"


// local
#include "HltL0MuonCandidates.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : HltL0MuonCandidates
//
// 2008-12-06 : Gerhard Raven
// based on HltL0MuonPrepare by Jose Angel Hernando Morata
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltL0MuonCandidates );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltL0MuonCandidates::HltL0MuonCandidates( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator, false )
  , m_selection(*this)
  , m_maker(0)
{
  declareProperty("L0DULocation", m_l0Location = L0DUReportLocation::Default );
  declareProperty("L0Channel", m_l0Channel );
  m_selection.declareProperties( boost::assign::map_list_of( 1,std::string("TES:")+L0MuonCandidateLocation::Default) );
}
//=============================================================================
// Destructor
//=============================================================================
HltL0MuonCandidates::~HltL0MuonCandidates() {
} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltL0MuonCandidates::initialize() {
  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorith

  m_selection.retrieveSelections();
  m_selection.registerSelection();
  
  m_maker = tool<IMuonSeedTool>("MuonSeedTool");
  declareInfo("#accept","",&counter("#accept"),0,std::string("Events accepted by ") + name());
  declareInfo("#input","",&counter("#input"),0,std::string("Candidates seen by ") + name());
  declareInfo("#pass","",&counter("#pass"),0,std::string("Candidates passed by ") + name());

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltL0MuonCandidates::execute() {
  //@TODO: only update cuts on L0 TCK change...
  LHCb::L0DUReport* l0 = get<L0DUReport>(m_l0Location);
  const LHCb::L0DUChannel::Map& channels = l0->configuration()->channels();
  LHCb::L0DUChannel::Map::const_iterator channel  = channels.find(m_l0Channel);
  if (channel == channels.end()) {
            error() << "could not find requested l0 channel " << m_l0Channel<< endmsg;
            return StatusCode::FAILURE;
  }
  //@TODO: check if channel is enabled!!

  const LHCb::L0DUElementaryCondition::Map& conditions = channel->second->elementaryConditions();
  typedef std::vector<std::string> map_t;
  static map_t map;
  if (map.empty()) {
     map.push_back("Muon1(Pt)");
     map.push_back("DiMuon(Pt)");
  }
  std::vector<int> cuts;
  for (LHCb::L0DUElementaryCondition::Map::const_iterator condition = conditions.begin();
       condition!=conditions.end(); ++condition) {
         std::string data = condition->second->data()->name();
         for (map_t::const_iterator i = map.begin();i!=map.end();++i ) {
            if (data!=*i) continue;
            cuts.push_back( condition->second->threshold() );
         }
  }

  Tracks* muons = new Tracks();
  put(muons, "Hlt/Track/"+m_selection.output()->id().str());

  if (cuts.empty()) {
    warning() << " L0 channel " << m_l0Channel << " does not use any know type of l0MuonCandidate?? -- no candidates converted!" << endmsg;
    return StatusCode::SUCCESS;
  }

  assert(cuts.size()==1);


  BOOST_FOREACH( L0MuonCandidate* l0muon, *m_selection.input<1>()) {
    bool pass=( l0muon->encodedPt() > cuts[0] );
    if (!pass)  continue;

    debug() << "l0pt " << l0muon->pt()<< endmsg;
    if (checkClone(l0muon)) {
        debug() << "is clone " << endmsg;
        continue;
    }
    std::auto_ptr<Track> track( new Track() );
    StatusCode sc = m_maker->makeTrack(*l0muon,*track);
    if (sc.isFailure()) return sc;
    //TODO: pushes both into IHltDataSvc and into TES (muons)
    //      this should be possible to do through the IHltDataSvc...
    m_selection.output()->push_back(track.get());
    muons->insert(track.release());
  }

  counter("#input")  +=  m_selection.input<1>()->size();
  counter("#accept") += !m_selection.output()->empty();
  counter("#pass")   +=  m_selection.output()->size();

  if (m_debug) {
     debug() << "# Input: " << m_selection.input<1>()->size() 
             << " -> # Output: " << m_selection.output()->size() << endreq;
  }
  if (m_debug) printInfo(" Muons ",*m_selection.output()); //#($)*))@*)@ requires outputlevel debut, but prnts at INFO...

  return StatusCode::SUCCESS;
}

namespace {
    class isInMuonStation : public std::unary_function<bool,LHCb::LHCbID> {
    public:
       isInMuonStation(unsigned int station) : m_station (station) {}
       bool operator()(const LHCb::LHCbID& id) { return id.isMuon() && id.muonID().station()==m_station; }
    private:
       unsigned int m_station;
    };
}

//=============================================================================
bool HltL0MuonCandidates::checkClone(L0MuonCandidate* muon)
{
  const MuonTileID& tileM1 = muon->muonTileIDs(0).front();
  const MuonTileID& tileM2 = muon->muonTileIDs(1).front();

  BOOST_FOREACH(const Track* t, *m_selection.output() ) {
    const std::vector< LHCb::LHCbID >& ids= t->lhcbIDs();

    std::vector<LHCb::LHCbID>::const_iterator oldTileM1 = 
        std::find_if( ids.begin(), ids.end(), isInMuonStation(0));
    if (oldTileM1 == ids.end() || !(*oldTileM1 == tileM1) ) continue; // not found, or no match...

    std::vector<LHCb::LHCbID>::const_iterator oldTileM2 =
        std::find_if( ids.begin(), ids.end(), isInMuonStation(1));
    if (oldTileM2 != ids.end() &&   *oldTileM2 == tileM2 ) return true; // found, and match...
  }
  return false;
}
