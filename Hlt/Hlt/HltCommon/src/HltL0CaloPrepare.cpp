// $Id: HltL0CaloPrepare.cpp,v 1.6 2008-06-25 20:11:02 graven Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "HltL0CaloPrepare.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : HltL0CaloPrepare
//
// 2006-07-28 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltL0CaloPrepare );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltL0CaloPrepare::HltL0CaloPrepare( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
{
  
  declareProperty("MinEt", m_minEt = 3500.);
  declareProperty("CaloType", m_caloType = 2);

  declareProperty("CaloCandidatesLocation", m_caloCandidatesLocation = 
                  L0CaloCandidateLocation::Full);

  declareProperty("CaloMakerTool", m_caloMakerName = "");
}
//=============================================================================
// Destructor
//=============================================================================
HltL0CaloPrepare::~HltL0CaloPrepare() {
} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltL0CaloPrepare::initialize() {
  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorith

  m_outputTracks = &(registerTSelection<LHCb::Track>());

  m_histoEt = initializeHisto("Et",0.,6000.,100);
  m_histoEt1 = initializeHisto("Et1",0.,6000.,100);

  debug() << " calo candidates location " 
          << m_caloCandidatesLocation << endreq;

  m_caloMaker = 0;
  if (m_caloMakerName != "")
    m_caloMaker = tool<ICaloSeedTool>("HadronSeedTool");

  saveConfiguration();  
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltL0CaloPrepare::execute() {

  StatusCode sc = StatusCode::SUCCESS;

  // get calo candidates
  L0CaloCandidates* calos = get<L0CaloCandidates>(m_caloCandidatesLocation);
  Tracks* output = new Tracks();
  put(output,"Hlt/Track/"+m_outputTracks->id().str());

  for (L0CaloCandidates::const_iterator it = calos->begin(); 
       it != calos->end(); ++it) {
    const L0CaloCandidate& calo = **it;
    if (calo.type() == m_caloType && calo.et() >= m_minEt) {
        Track* tcalo = new Track();
        if (m_caloMaker) m_caloMaker->makeTrack(calo,*tcalo);
        else             makeTrack(calo,*tcalo);
        addExtras(calo,*tcalo);
        output->insert(tcalo);
        m_outputTracks->push_back(tcalo);
    }
  }

  debug() << " number of calos above et " << m_outputTracks->size() 
          << " candidates " << m_outputSelection->ncandidates() << endreq;
  if (m_debug) printInfo(" Calos ",*m_outputTracks);

  return sc;
}


StatusCode HltL0CaloPrepare::finalize() {

  debug() << "==> Finalize" << endmsg;
  return HltAlgorithm::finalize();  
}

void HltL0CaloPrepare::makeTrack(const L0CaloCandidate& calo,
                                  LHCb::Track& track) {
  
  // Get energy and position of L0 calo candidate:
  double x      = calo.position().x();
  double y      = calo.position().y();
  double z      = calo.position().z();
  double ex     = calo.posTol()*(4./sqrt(12.0));
  double ey     = calo.posTol()*(4./sqrt(12.0));
  double et     = calo.et();

  double sintheta = sqrt(x*x + y*y)/(sqrt(x*x + y*y + z*z));
  double e = fabs(et)/sintheta;

  debug() << " position " << calo.position() << " et " << et
          << " sintheta " << sintheta << " e " << e << endreq;  

  State state;
  state.setLocation(State::MidHCal);
  state.setState(x,y,z,ex,ey,1./e);
  track.addToStates(state);
  
}

void HltL0CaloPrepare::addExtras(const L0CaloCandidate& calo,
                                  LHCb::Track& track) {

  double ex     = calo.posTol()*(4./sqrt(12.0));
  double ey     = calo.posTol()*(4./sqrt(12.0));

  const std::vector<State*>& states = track.states();
  for (std::vector<State*>::const_iterator it = states.begin();
       it != states.end(); ++it) {
    State& state = *(*it);
    if (state.location() == State::MidHCal){
      state.setTx(ex);
      state.setTy(ey);
      debug() << " changed slopes! " << state.slopes() << endreq;
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
