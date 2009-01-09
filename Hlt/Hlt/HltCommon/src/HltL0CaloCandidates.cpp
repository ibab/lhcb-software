// $Id: HltL0CaloCandidates.cpp,v 1.14 2009-01-09 10:34:47 graven Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "boost/foreach.hpp"
#include <memory>
#include "Event/L0DUBase.h"
#include "Event/L0DUReport.h"
#include "Event/L0CaloCandidate.h"

// local
#include "HltL0CaloCandidates.h"

using namespace LHCb;


//-----------------------------------------------------------------------------
// Implementation file for class : HltL0CaloCandidates
// 2008-12-05 : Gerhard Raven
// based on HltL0CaloPrepare by Jose Angel Hernando Morata
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltL0CaloCandidates );



//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltL0CaloCandidates::HltL0CaloCandidates( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator, false 
  // vector of default histo defs...
  //   list_of( Gaudi::Histo1DDef( "Et", 0., 6000., 100 ) )
  //            Gaudi::Histo1DDef( "EtBest", 0., 6000., 100 ) )
    ) 
  , m_selection(*this)
  , m_caloMaker(0)
  , m_et(0)
  , m_etMax(0)
{
  declareProperty("L0DULocation", m_l0Location = L0DUReportLocation::Default );
  declareProperty("L0Channel", m_l0Channel );
  declareProperty("CaloMakerTool", m_caloMakerName = "");
  // (re)set property from HltAlgorithm
  setProperty("HistoDescriptor","{ 'Et'    : ('Et',   0.,6000.,100)"
                                ", 'EtMax' : ('EtMax',0.,6000.,100)"
                                "}");

  //FIXME/TODO: check whether Full should not be Default...
  m_selection.declareProperties( boost::assign::map_list_of(1,std::string("TES:")+L0CaloCandidateLocation::Full)  );
}
//=============================================================================
// Destructor
//=============================================================================
HltL0CaloCandidates::~HltL0CaloCandidates() {
} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltL0CaloCandidates::initialize() {
  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorith

  m_selection.retrieveSelections();
  m_selection.registerSelection();

  m_caloMaker = (m_caloMakerName.empty() 
                    ? (ICaloSeedTool*)0
                    :tool<ICaloSeedTool>(m_caloMakerName));

  // force creation of counters, and declare them to monitoring...
  declareInfo("#accept","",&counter("#accept"),0,std::string("Events accepted by ") + name());
  declareInfo("#input","",&counter("#input"),0,std::string("Candidates seen by ") + name());
  declareInfo("#candidated accepted","",&counter("#candidated accepted"),0,std::string("Candidates accepted by ") + name());
  
  m_et    = book("Et");
  m_etMax = book("EtMax");
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================

HltL0CaloCandidates::CutList_t 
HltL0CaloCandidates::generateCutList(const LHCb::L0DUChannel& channel) {
  typedef GaudiUtils::VectorMap<std::string,L0DUBase::CaloType::Type> map_t;
  static map_t map;
  if (map.empty()) {
     map.insert("Electron(Et)", L0DUBase::CaloType::Electron);
     map.insert("Photon(Et)",   L0DUBase::CaloType::Photon);
     map.insert("Hadron(Et)",   L0DUBase::CaloType::Hadron);
     map.insert("LocalPi0(Et)", L0DUBase::CaloType::Pi0Local);
     map.insert("GlobalPi0(Et)",L0DUBase::CaloType::Pi0Global);
  }
  CutList_t cuts;
  const LHCb::L0DUElementaryCondition::Map& conditions = channel.elementaryConditions();
  for (LHCb::L0DUElementaryCondition::Map::const_iterator condition = conditions.begin();
       condition!=conditions.end(); ++condition) {
         std::string data = condition->second->data()->name();
         for (map_t::const_iterator i = map.begin();i!=map.end();++i ) {
            if (data!=i->first) continue;
            cuts.push_back( L0CaloCandidateCut( i->second, condition->second->threshold() ) );
         }
  }
  return cuts;
}

HltL0CaloCandidates::CutList_t 
HltL0CaloCandidates::generateCutList(const std::string& whichType) {
  typedef GaudiUtils::VectorMap<std::string,L0DUBase::CaloType::Type> map_t;
  static map_t map;
  if (map.empty()) {
     map.insert("AllElectron", L0DUBase::CaloType::Electron);
     map.insert("AllPhoton",   L0DUBase::CaloType::Photon);
     map.insert("AllHadron",   L0DUBase::CaloType::Hadron);
     map.insert("AllLocalPi0", L0DUBase::CaloType::Pi0Local);
     map.insert("AllGlobalPi0",L0DUBase::CaloType::Pi0Global);
  }
  map_t::const_iterator i = map.find(whichType);
  CutList_t cuts;
  if (i!=map.end()) cuts.push_back( L0CaloCandidateCut( i->second ) );
  return cuts;
}

StatusCode HltL0CaloCandidates::execute() {

  HltL0CaloCandidates::CutList_t cuts;
  if (m_l0Channel.substr(0,3)!="All") {
      LHCb::L0DUReport* l0 = get<L0DUReport>(m_l0Location);

      //@TODO: only update cuts on L0 TCK change, and cache the result...
      const LHCb::L0DUChannel::Map& channels = l0->configuration()->channels();
      LHCb::L0DUChannel::Map::const_iterator channel  = channels.find(m_l0Channel);
      if (channel == channels.end()) {
            error() << "could not find requested l0 channel " << m_l0Channel << endmsg;
            return StatusCode::FAILURE;
      }
      //@TODO: check if channel is actually enabled!!
      cuts = generateCutList( *(channel->second) );
  } else {
      cuts = generateCutList( m_l0Channel );
  }

  Tracks* output = new Tracks();
  put(output,"Hlt/Track/"+m_selection.output()->id().str());

  if (cuts.empty()) {
    warning() << " L0 channel " << m_l0Channel << " does not use any know type of l0CaloCandidate?? -- no candidates converted!" << endmsg;
    return StatusCode::SUCCESS;
  }

  double etMax = -1.;
  BOOST_FOREACH(const L0CaloCandidate* calo, *m_selection.input<1>()) {
    if (       calo->type()!=L0DUBase::CaloType::Electron
            && calo->type()!=L0DUBase::CaloType::Photon
            && calo->type()!=L0DUBase::CaloType::Hadron
            && calo->type()!=L0DUBase::CaloType::Pi0Local
            && calo->type()!=L0DUBase::CaloType::Pi0Global ) {
             warning() << " got candidate with unexpected type " << calo->type() << endmsg;
    }
    bool pass=true;
    CutList_t::const_iterator i = cuts.begin();
    while ( pass && i!=cuts.end() ) pass = (*i++)(calo);
    if (!pass)  continue;
    fill(m_et,calo->et(),1.);
    if (calo->et()>etMax) etMax = calo->et();
    debug() << " accepted calo cand with type = " << calo->type()  << " and et = " << calo->et() << " and etcode = " << calo->etCode() << endmsg;
    //TODO: split creating subset of L0 candidates and conversion into track....
    std::auto_ptr<Track> tcalo( new Track() );
    if (m_caloMaker) {
        StatusCode sc = m_caloMaker->makeTrack(*calo,*tcalo);
        if (sc.isFailure()) return sc;
    } else                           makeTrack(*calo,*tcalo);
    addExtras(*calo,*tcalo);
    m_selection.output()->push_back(tcalo.get());
    output->insert(tcalo.release());
  }
  if (!m_selection.output()->empty()) fill(m_etMax,etMax,1.);

  counter("#input")  +=  m_selection.input<1>()->size();
  counter("#accept") += !m_selection.output()->empty();
  counter("#candidated accepted") +=  m_selection.output()->size();

  if (msgLevel(MSG::DEBUG)) {
    debug()  << "# Input: " << m_selection.input<1>()->size() 
             << " -> # Output: " << m_selection.output()->size() << endreq;
    printInfo(" Calos ",*m_selection.output()); //#($)*))@*)@ requires outputlevel debug, but prints at INFO...
  }
  return StatusCode::SUCCESS;
}


void HltL0CaloCandidates::makeTrack(const L0CaloCandidate& calo,
                                  LHCb::Track& track) {
  
  // Get energy and position of L0 calo candidate:
  double x      = calo.position().x();
  double y      = calo.position().y();
  double z      = calo.position().z();
  double ex     = calo.posTol()*(4./sqrt(12.0));
  double ey     = ex;
  double et     = calo.et();

  double sintheta = sqrt(x*x + y*y)/(sqrt(x*x + y*y + z*z));
  double e = fabs(et)/sintheta;

  debug() << " position " << calo.position() << " et " << et
          << " sintheta " << sintheta << " e " << e << endreq;  

  State state;
  state.setLocation(State::MidHCal);
  state.setState(x,y,z,ex,ey,1./e); // ??? slopeX = ex, slopY = ey ???
  track.addToStates(state);
  
}

void HltL0CaloCandidates::addExtras(const L0CaloCandidate& calo,
                                  LHCb::Track& track) {

  double ex     = calo.posTol()*(4./sqrt(12.0));
  double ey     = ex;

  track.addInfo(hltInfoID("L0ET"),calo.et());

  BOOST_FOREACH( State* state, track.states() ) {
    if (state->location() == State::MidHCal){
      state->setTx(ex); // ???
      state->setTy(ey); // ???
      debug() << " changed slopes! " << state->slopes() << endreq;
    }
  }  

  CaloCellID id = calo.id();
  LHCb::CaloCellID id1(id.calo(), id.area(), id.row()+1, id.col()   );
  LHCb::CaloCellID id2(id.calo(), id.area(), id.row()+1, id.col()+1 );
  LHCb::CaloCellID id3(id.calo(), id.area(), id.row()  , id.col()+1 );

  track.addToLhcbIDs( LHCbID(id) );
  track.addToLhcbIDs( LHCbID(id1) );
  track.addToLhcbIDs( LHCbID(id2) );
  track.addToLhcbIDs( LHCbID(id3) );
}


//=============================================================================
