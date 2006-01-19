// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "HltTrackIPSelection.h"

#include "HltUtils.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HltTrackIPSelection
//
// 2005-11-17 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<HltTrackIPSelection>          s_factory ;
const        IAlgFactory& HltTrackIPSelectionFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltTrackIPSelection::HltTrackIPSelection( const std::string& name,
                                          ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty("InputTracksName", 
                  m_inputTracksName = TrackLocation::HltSpaceVelo);
  
  declareProperty( "InputVerticesName" ,
                   m_inputVerticesName = VertexLocation::HltPrimaryVertices);
  
  declareProperty("IPMin", m_ipmin = 0.1 *mm);
  declareProperty("IPMax", m_ipmax = 3.0 *mm);
  declareProperty("IPAbs", m_ipabs = true);

  declareProperty("SelectVertex",m_selectVertex = false);
  declareProperty("FlagTracks", m_doFlag = true);

}
//=============================================================================
// Destructor
//=============================================================================
HltTrackIPSelection::~HltTrackIPSelection() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltTrackIPSelection::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endreq;
  PatDataStore* store     = tool<PatDataStore>( "PatDataStore" );
  if (store == NULL) 
    warning() << " error not pat Data store " << endreq;

  m_itracks = store->tracks(m_inputTracksName);
  m_vertices = store->vertices(m_inputVerticesName);

  release(store);

  return StatusCode::SUCCESS;
}


//=============================================================================
// Initialization
//=============================================================================
StatusCode HltTrackIPSelection::execute() {

  debug() << "==> Execute" << endmsg;

  // load the input tracks and vertices
  StatusCode sc = loadEvent();
  if (sc.isFailure()) {
    warning() << " No vertices or Input Tracks";
    return StatusCode::SUCCESS;
  }
  
  // loop in tracks and compute the smalest IP per track
  computeTrackSmallestIP();

  // select one of the primary vertices with more SIP in window
  if (m_selectVertex) selectVertex();
  
  // flag as selected the track with IP in window
  if (m_doFlag) flagSelectedTracks();
  
  return StatusCode::SUCCESS;
};

StatusCode HltTrackIPSelection::loadEvent() 
{

  StatusCode sc = StatusCode::SUCCESS;
  if (m_vertices == NULL || m_itracks == NULL) sc = StatusCode::FAILURE;

  debug() << " load event status " << sc << endreq;
  if (sc == StatusCode::SUCCESS) {
    debug() << " number of tracks " << m_itracks->size() << endreq;
    debug() << " number of vertices " << m_vertices->size() << endreq;
  }
  
  return sc;
}

void HltTrackIPSelection::computeTrackSmallestIP() 
{

  m_sip.clear();
  m_kver.clear();
  m_ipsel.clear();
  
  unsigned int ntracks = m_itracks->size();
  m_sip.reserve(ntracks);
  m_kver.reserve(ntracks);
  m_ipsel.reserve(ntracks);
  for (unsigned int i = 0; i < ntracks; i++) {
    m_sip[i] = 0.;
    m_kver[i] = 0;
    m_ipsel[i] = 0;
  }

  unsigned int i = 0;
  for (PatTrackContainer::const_iterator it = m_itracks->begin(); 
       it != m_itracks->end(); it++, i++) {
    const Track& track = *(*it);
    if (track.checkFlag(Track::Backward)) continue;
    double sip = 10e6;
    unsigned kvertex = 999;
    int k = 0;
    for (PatVertexContainer::const_iterator itv = m_vertices->begin(); 
         itv != m_vertices->end(); ++itv, k++) {
      TVertex& vertex = *(*itv);
      double ip = HltUtils::impactParameter(vertex,track);
      if (fabs(ip)< fabs(sip)) {
        sip = ip;
        kvertex = k;  
      }      
    }
    if (m_ipabs) sip = fabs(sip);
    m_sip[i] = sip;
    m_kver[i] = kvertex;
    if ((sip>= m_ipmin) && (sip < m_ipmax)) m_ipsel[i] = 1;
  }
  
  if (msgLevel(MSG::DEBUG)) {
    for (unsigned int i = 0; i < ntracks; i++) 
      info() << " track i " << i << ", ip " << m_sip[i]
             <<", kver " << m_kver[i] << ", sel? "<< m_ipsel[i] << endreq;
  }

}

void HltTrackIPSelection::selectVertex() {

  m_vertexKey = 0;

  info() << " selecting vertex " << endreq;
  
  m_canver.clear();
  unsigned int nver = m_vertices->size();
  m_canver.reserve(nver);
  for (unsigned int i = 0; i < nver; i++) m_canver[i] = 0;
  
  unsigned int ntracks = m_itracks->size();
  for (unsigned int i = 0; i < ntracks; i++) 
    if (m_ipsel[i]) m_canver[m_kver[i]] += 1;
  
  unsigned int kver = 0;
  int ncan = 0;
  for (unsigned int i = 0; i < nver; i++) {
    if (m_canver[i] > ncan) {
      kver = i; 
      ncan = m_canver[i];
    }
  }
  
  // now unselect tracks that has an ip not to the selected vertex
  for (unsigned int i = 0; m_ipsel.size(); i++) 
    if (m_kver[i] != m_vertexKey) m_ipsel[i] = 0;
  
  
  m_vertexKey = kver;
  if (msgLevel(MSG::DEBUG)) {
    info() << " n vertices " << nver
           << " selected vertex " << m_vertexKey 
           << " candidates " << ncan << endreq;
    for (unsigned int i = 0; i <nver; i++)
      info() << " vertex " << i << " candidates " 
             << m_canver[i] << endreq;
  }
}


void HltTrackIPSelection::flagSelectedTracks() 
{
  unsigned int i = 0;
  int nflags = 0;
  for (PatTrackContainer::iterator it = m_itracks->begin(); 
       it != m_itracks->end(); ++it, i++) {
    if (m_ipsel[i]) { 
      (*it)->setFlag(Track::IPSelected,true);
      nflags++;
    }
  }
  
  if (msgLevel(MSG::DEBUG))
    info() << " number of flaged tracks " << nflags << endreq;
  
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltTrackIPSelection::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}


