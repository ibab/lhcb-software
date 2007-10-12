
// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "HltTrackIPSelection.h"
#include "HltBase/HltFunctions.h"
#include "HltBase/HltSequences.h"

#include "Event/HltNames.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : HltTrackIPSelection
//
// 2005-11-17 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltTrackIPSelection );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltTrackIPSelection::HltTrackIPSelection( const std::string& name,
                                          ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
{

  declareProperty("SelectPrimaryVertex", m_selectPrimaryVertex = false  );

  declareProperty("IPType", m_iptype = "empty"  );
  declareProperty("MinIP", m_ipmin = 0. ); 
  declareProperty("MaxIP", m_ipmax = 0. ); 
  declareProperty("IPAbs", m_ipabs = false);

  declareCondition("MinIP", m_ipmin = 0. ); 
  declareCondition("MaxIP", m_ipmax = 0. );

}
//=============================================================================
// Destructor
//=============================================================================
HltTrackIPSelection::~HltTrackIPSelection() {

  // delete _nobackwards;
  // delete _kvertex;
  // delete _ipWindow;
  // delete _ipFun;
}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltTrackIPSelection::initialize() {
  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return stop("HltAlgorithm::initialize failure");

  initializeHisto(m_histoIP,"IP",-3.5,3.5,100);

  // crreate a filter against the backward tracks
  Hlt::CheckFlag back(Track::Backward);
  _nobackwards = (!back).clone();

  // create a function to retrieve the Info of which vertex this track
  // it is associated to.
  m_PVKey = HltNames::particleInfoID("PVKey");
  Hlt::Info<Track> kver(m_PVKey);
  _kvertex = ((kver == 0)).clone();

  // create a filter to select tracks with IP info inside a window
  m_IPKey = HltNames::particleInfoID("IP");
  if (m_ipabs) {
    Hlt::AbsInfo<Track> ip(m_IPKey);
    _ipWindow = (  (ip > m_ipmin) && (ip  < m_ipmax) ).clone();
  } else {
    Hlt::Info<Track> ip(m_IPKey);
    _ipWindow = (  (ip > m_ipmin) && (ip  < m_ipmax) ).clone();
  }

  // create the function to compute the immpact parameter
  _ipFun = NULL;
  if (m_iptype == "2DIP") _ipFun = new Hlt::rIP();
  else if (m_iptype == "3DIP") _ipFun = new Hlt::IP();
  else {
    error() << " Set option 'IPType' to '2DIP' or '3DIP' " << endreq;
    return StatusCode::FAILURE;
  }
  
  if (m_inputTracksName != "")
    checkInput(m_inputTracks," no input tracks");
  if (m_primaryVerticesName != "")
    checkInput(m_primaryVertices," no primary Vertices");
  if (m_outputTracksName != "")
    checkInput(m_outputTracks," no output tracks");
  if (m_selectPrimaryVertex)
    checkInput(m_outputVertices," no output vertices");

  m_otracks.reserve(200);
  m_otracks2.reserve(200);
  m_otracks.clear();
  m_otracks2.clear();

  return sc;
}


//=============================================================================
// Initialization
//=============================================================================
StatusCode HltTrackIPSelection::execute() {
  StatusCode sc = StatusCode::SUCCESS;
  
  // loop in tracks and compute the smalest IP per track
  computeTracksMinIP();

  // select one of the primary vertices with more SIP in window
  // if (m_selectVertex) 
  if (m_selectPrimaryVertex) selectVertex();

  if (m_outputTracks) candidateFound(m_outputTracks->size());
  
  return sc;
};


void HltTrackIPSelection::computeTracksMinIP() 
{

  // select the input tracks that are non-backwards
  m_otracks2.clear();
  if (m_inputTracks)
    Hlt::select(*m_inputTracks,*_nobackwards,m_otracks2);
  else if (m_patInputTracks)
    Hlt::select(*m_patInputTracks,*_nobackwards,m_otracks2);
  debug() << " number of non-backwards tracks " 
          << m_otracks2.size() << endreq;

  // compute the min impact parameter of the tracks and
  // store it in the track, with the information to which vertex
  // the tracks has its min impact parameter
  if (m_primaryVertices) 
    Hlt::map_abs_min_value(m_otracks2,*m_primaryVertices,
			   *_ipFun, m_IPKey,m_PVKey);
  else if (m_patInputVertices) {
    m_nPrimaryVertices = m_patInputVertices->size();
    Hlt::map_abs_min_value(m_otracks2,*m_patInputVertices,
			   *_ipFun,m_IPKey,m_PVKey);
  }
  // select the input tracks that are non-backwards
  m_otracks2.clear();
  if (m_inputTracks)
    Hlt::select(*m_inputTracks,*_nobackwards,m_otracks2);
  else if (m_patInputTracks)
    Hlt::select(*m_patInputTracks,*_nobackwards,m_otracks2);
  debug() << " number of non-backwards tracks " 
          << m_otracks2.size() << endreq;

  // compute the min impact parameter of the tracks and
  // store it in the track, with the information to which vertex
  // the tracks has its min impact parameter
  if (m_primaryVertices) 
    Hlt::map_abs_min_value(m_otracks2,*m_primaryVertices,
			   *_ipFun, m_IPKey,m_PVKey);
  else if (m_patInputVertices) {
    m_nPrimaryVertices = m_patInputVertices->size();
    Hlt::map_abs_min_value(m_otracks2,*m_patInputVertices,
			   *_ipFun,m_IPKey,m_PVKey);
  }

  // monitor the min impact parameter of all the tracks
  if (m_monitor) {
    std::vector<double> ips = infos(m_otracks2,m_IPKey,-1.e6);
    fillHisto(m_histoIP,ips,1.);
  }

  // check if there is one one vertex
  bool ok = (m_selectPrimaryVertex && (m_nPrimaryVertices >1));
  Hlt::TrackContainer& otracks = 
    ( ok? m_otracks : *m_outputTracks);
  otracks.clear();

  // select the tracks that are inside an impact paremter window
  Hlt::select(m_otracks2,*_ipWindow,otracks);

  debug() << " number of tracks in IPWindow " << otracks.size() << endreq;  
  if (m_verbose) debugOutput( otracks);
}

void HltTrackIPSelection::debugOutput(const Hlt::TrackContainer& tracks) const {

  for (Tracks::const_iterator it = tracks.begin(); 
       it != tracks.end(); ++it) 
    debug() << " quantity IP " << (**it).info(m_IPKey,-1) 
            << " key " << (int) (**it).info(m_PVKey,-1) << endreq;
}

void HltTrackIPSelection::selectVertex() {


  // if there is one vertex, we are done, select it.
  if (m_nPrimaryVertices == 1) {
    m_outputVertices->clear();
    if (m_outputVertices) {
      if (m_patInputVertices)
        m_outputVertices->push_back(*m_patInputVertices->begin());
      else
        m_outputVertices->push_back(*m_primaryVertices->begin());
    }
    return;
  }

  // initialize objects
  m_outputTracks->clear();
  m_kver.clear();
  m_kver.reserve(m_nPrimaryVertices);
  std::fill_n(m_kver.begin(),m_nPrimaryVertices,0);

  // count how many tracks are pointing to each primary vertex
  for (Hlt::TrackContainer::iterator it = m_otracks.begin() ; 
       it != m_otracks.end() ; ++it ) {
    int k = (int) (**it).info(m_PVKey,-1);
    m_kver[k] +=1;
  }

  // select the vertex with more tracks associated to it
  int ktracks = 0;
  int k = 0;
  for (int i = 0; i < m_nPrimaryVertices; i++) {
    if (ktracks < m_kver[i]) {ktracks = m_kver[i], k = i;}
    debug() << " vertex " << i << " tracks " << m_kver[i] << endreq;
  }

  // select as output tracks those one associated to the selected vertex
  m_outputTracks->clear();
  for (Track::Vector::iterator it = m_otracks.begin(); 
       it != m_otracks.end(); ++it) 
    if ((*it)->info(m_PVKey,-1) == k)  m_outputTracks->push_back(*it);

  
  // locate in the output the selected vertex
  if (m_outputVertices) {
    m_outputVertices->clear();
    if (m_patInputVertices)
      m_outputVertices->push_back(*(m_patInputVertices->begin()+k));
    else if (m_primaryVertices)
      m_outputVertices->push_back(*(m_primaryVertices->begin()+k));
  }

  if (m_debug) {
    debug() << " number of primary vertex " << m_nPrimaryVertices << endreq;
    debug() << " number of selected tracks " << m_otracks.size() << endreq;
    debug() << " selected vertex " << k << " tracks " << ktracks << endreq;
    if (m_outputVertices) 
      debug() << " number of output vertices " 
              << m_outputVertices->size() << endreq;
    if (m_verbose) debugOutput( *m_outputTracks);
  }
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltTrackIPSelection::finalize() {

  debug() << "==> Finalize" << endmsg;

  return HltAlgorithm::finalize();  
}


