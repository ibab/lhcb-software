// Include files 

// from Gaudi
#include "GaudiKernel/IIncidentSvc.h" 
// local
#include "MuonFakeClustering.h"
#include "MuonInterfaces/MuonLogPad.h"
#include "MuonInterfaces/MuonHit.h"
#include "MuonDet/IMuonFastPosTool.h"
#include "MuonDet/DeMuonDetector.h"
using namespace LHCb;
using namespace std;

//-----------------------------------------------------------------------------
// Implementation file for class : MuonFakeClustering
//
// produce fake clusters by just copying the list of MuonLogPads to MuonHits
// 2009-10-15 : Giacomo GRAZIANI
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( MuonFakeClustering )



MuonFakeClustering::MuonFakeClustering( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent )
  : base_class ( type, name , parent ) 
{
  declareInterface<IMuonClusterRec>(this);
  declareProperty( "PosTool"          , m_posToolName = "MuonDetPosTool");
  m_clustersDone = false;
}

MuonFakeClustering::~MuonFakeClustering() {
  clear();
} 



StatusCode MuonFakeClustering::initialize () {
  StatusCode sc = GaudiTool::initialize() ;
  
  if (!sc) return sc;
  m_muonDetector = getDet<DeMuonDetector>(DeMuonLocation::Default);
  if(!m_muonDetector){
    err()<<"error retrieving the Muon detector element "<<endmsg;
    return StatusCode::FAILURE;
  }
  m_posTool = tool<IMuonFastPosTool>(m_posToolName);
  if(!m_posTool) {
    error()<<"error retrieving the muon position tool "<<endmsg;
    return StatusCode::FAILURE;
  }
  
  incSvc()->addListener( this, IncidentType::EndEvent );
  return sc;
}

void MuonFakeClustering::handle ( const Incident& incident )
{ 
  if ( IncidentType::EndEvent == incident.type() ) {
    verbose() << "End Event: clear everything"<<endmsg;
    clear() ;
  }
}

void MuonFakeClustering::clear() {
  for(auto ih=m_clusters.begin() ; ih !=m_clusters.end() ; ih++)
    delete (*ih); // delete all the allocated MuonHit's
  m_clusters.clear();
  m_clustersDone=false;
}


StatusCode MuonFakeClustering::finalize ()

{
  debug()<<" MuonFakeClustering::finalize"<<endmsg;
  return   GaudiTool::finalize() ;
}


const std::vector<MuonHit*>* MuonFakeClustering::clusters(const std::vector<MuonLogPad*>* pads, 
                                                          bool force) {

  if( (m_clustersDone == false || force==true) && pads) {
    int nhits=0;
    clear();
    
    if (pads) {
      for (const auto& pad : *pads) {
        if(! pad->truepad() ) continue;

        // fill the hit array
        // create a MuonHit object
        m_clusters.emplace_back( new MuonHit(m_muonDetector, pad, 
                                             m_posTool ) );
        // store a progressive hit number for debugging purposes
        m_clusters.back()->setHitID(++nhits).ignore();
      }
    }
  }
  if (pads) m_clustersDone = true;
  return &m_clusters;
}
