
// $Id: HltEnd.cpp,v 1.2 2006-09-26 13:54:54 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "HltEnd.h"
#include "Event/HltSummary.h"
#include "Event/HltNames.h"
#include "HltBase/ESequences.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : HltEnd
//
// 2006-07-25 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltEnd );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltEnd::HltEnd( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{  
  declareProperty("OnTES", m_onTES = true);

  declareProperty("SummaryName", 
                  m_summaryName = LHCb::HltSummaryLocation::Default);

}
//=============================================================================
// Destructor
//=============================================================================
HltEnd::~HltEnd() {} 

//=============================================================================
// Endialization
//=============================================================================
StatusCode HltEnd::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  m_summaryTracksName = m_summaryName+"/Track";
  m_summaryVerticesName = m_summaryName+"/Vertex";
  
  debug() << "==> Endialize" << endmsg;
  IHltDataStore* store = tool<IHltDataStore>( "HltDataStore" );
  m_tracks     = &(store->tracks());
  m_summaryTracks = &(store->tracks(m_summaryTracksName));
                      
  m_vertices = &(store->vertices());
  m_summaryVertices = &(store->vertices(m_summaryVerticesName));
  release(store);
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltEnd::execute() {

  debug() << "==> Execute" << endmsg;

  // TODO what happens with the EFF
  if (m_onTES) saveAll();
  else saveSummary();


  return StatusCode::SUCCESS;  
}

void HltEnd::saveSummary() {

  
  debug() << " number of tracks in summary [1]" << 
    m_summaryTracks->size() << endreq;
  debug() << " number of vertices in summary [1]" << 
    m_summaryVertices->size() << endreq;
  std::unique(m_summaryTracks->begin(),m_summaryTracks->end());
  std::unique(m_summaryVertices->begin(),m_summaryVertices->end());


  debug() << " number of tracks in summary [2]" << 
    m_summaryTracks->size() << endreq;
  debug() << " number of vertices in summary [2]" << 
    m_summaryVertices->size() << endreq;

  saveTracks(m_summaryTracksName, *m_summaryTracks);
  saveVertices(m_summaryVerticesName, *m_summaryVertices);

}

  
void HltEnd::saveTracks(const std::string& name, 
                        const Hlt::TrackContainer& cont) {
  if (exist<Tracks>(name)) return;
  Tracks* tes = new Tracks();
  for (Hlt::TrackContainer::const_iterator it = cont.begin(); it != cont.end();
       ++it)
    tes->insert ( (*it)->clone() );
  put(tes,name);
  debug() << " stored on TES " << name << " " << tes->size() << endreq;
}

void HltEnd::saveVertices(const std::string& name, 
                          const Hlt::VertexContainer& cont) {
  if (exist<RecVertices>(name)) return;
  RecVertices* tes = new RecVertices();
  for (Hlt::VertexContainer::const_iterator it = cont.begin(); it != cont.end();
       ++it) {
    RecVertex* vt = new RecVertex();
    *vt = **it;
    tes->insert ( vt );
  }
  put(tes,name);
  debug() << " stored on TES " << name << " " << tes->size() << endreq;
}

void HltEnd::saveAll() {

  for (MapTracks::iterator it = m_tracks->begin(); it != m_tracks->end();
       ++it) 
    saveTracks(it->first,it->second);
  
  for (MapVertices::iterator it2 = m_vertices->begin(); it2 != m_vertices->end();
       ++it2)
    saveVertices(it2->first,(it2->second));
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltEnd::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
