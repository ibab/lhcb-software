// $Id: HltInit.cpp,v 1.4 2007-06-20 20:34:05 hernando Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "HltInit.h"
#include "Event/HltSummary.h"
#include "HltBase/ESequences.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : HltInit
//
// 2006-07-25 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltInit );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltInit::HltInit( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  // location of the summary and the summary box name
  declareProperty("summaryName",
                  m_summaryName = LHCb::HltSummaryLocation::Default);
  
}
//=============================================================================
// Destructor
//=============================================================================
HltInit::~HltInit() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltInit::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  m_hltSvc = NULL;
  std::string name = "HltDataSvc/EventDataSvc";
  sc = serviceLocator()->service(name,m_hltSvc,true);
  // m_hltSvc = svc<IDataManagerSvc>("HltDataSvc",true);  
  if (!m_hltSvc) error() << " not able to create Hlt Svc " << endreq;
  else debug() << " SUCCESSFULLY CREATED!! " << endreq;
  m_hltSvc->setRoot("/Event", new DataObject());

  IDataProviderSvc* hltsvc = NULL;
  sc = serviceLocator()->service("HltDataSvc/EventDataSvc",hltsvc);
  if (!hltsvc) error() << " not able to create Hlt Svc provider " << endreq;
  else debug() << " SUCCESSFULLY CREATED provider!! " << endreq;  

  info() << " hlt data svc " << (int) m_hltSvc << endreq;
  info() << " event data svc " << (int) evtSvc() << endreq;

  put(hltsvc, new Tracks(), "/Event/Hlt/Track/Caca");
  debug() << " put tracks " << sc.isSuccess() << endreq;
  Tracks* mtracks = get<Tracks>(hltsvc,"/Event/Hlt/Track/Caca");
  if (!mtracks) error() << " can not retrieve tracks :(" << endreq;
  debug() << " retrieved tracks :)" << endreq;
  


  
 //  sc = serviceLocator()->getService(name,IDataManagerSvc::interfaceID(),m_hltSvc);
//   if (!sc) error() << " Not able to create HltDataSvc" << endreq;
//  

  debug() << "==> Initialize" << endmsg;
  IHltDataStore* store = tool<IHltDataStore>( "HltDataStore" );
  m_tracks     = &(store->tracks());
  m_vertices = &(store->vertices());
  release(store);

  m_patStore = tool<PatDataStore>( "PatDataStore" );
  release(m_patStore);
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltInit::execute() {

  debug() << "==> Execute" << endmsg;

  for (MapTracks::iterator it = m_tracks->begin(); 
       it != m_tracks->end(); ++it)
    (it->second).clear();
      
  for (MapVertices::iterator it2 = m_vertices->begin(); 
       it2 != m_vertices->end(); ++it2)
    (it2->second).clear();

  HltSummary* sum = new HltSummary();
  put( sum, m_summaryName );

 //  for (MapTracks::iterator it = m_tracks->begin(); it != m_tracks->end(); 
//        ++it) {
//     const std::string& name = it->first;
//     if (Estd::has_key(m_patStore->beginTracks(),
//                       m_patStore->endTracks(),name)) {
//       Hlt::TrackContainer& tracks = it->second;
//       PatTrackContainer* patTracks = m_patStore->tracks(name);
//       if (patTracks->size()>0)
//         std::copy(patTracks->begin(),patTracks->end(),
//                   std::back_inserter(tracks));
//       debug() << " pat loaded " << name << " " << tracks.size() << endreq;
//       if (tracks.size() > 0) continue;
//       if (!exist<Tracks>(name)) continue;
//       Tracks* tesTracks = get<Tracks>(name);
//       if (!tesTracks) continue;
//       if (tesTracks->size() >0) 
//         std::copy(tesTracks->begin(),tesTracks->end(),
//                   std::back_inserter(tracks));
//       debug() << " tes loaded " << name << " " << tracks.size() << endreq;
//     }      
//   } 

//   for (MapVertices::iterator it = m_vertices->begin(); it != m_vertices->end(); 
//        ++it) {
//     const std::string& name = it->first;
//     if (Estd::has_key(m_patStore->beginVertices(),
//                       m_patStore->endVertices(),name)) {
//       Hlt::VertexContainer& vertices = it->second;
//       PatVertexContainer* patVertices = m_patStore->vertices(name);
//       if (patVertices->size()>0)
//         std::copy(patVertices->begin(),patVertices->end(),
//                   std::back_inserter(vertices));
//       debug() << " pat loaded " << name << " " << vertices.size() << endreq;
//       if (vertices.size() > 0) continue;
//       if (!exist<RecVertices>(name)) continue;
//       RecVertices* tesVertices = get<RecVertices>(name);
//       if (!tesVertices) continue;
//       if (tesVertices->size() >0) 
//         std::copy(tesVertices->begin(),tesVertices->end(),
//                   std::back_inserter(vertices));
//       debug() << " tes loaded " << name << " " << vertices.size() << endreq;
//     }      
//   } 

      
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltInit::finalize() {

  m_hltSvc->clearStore();
  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
