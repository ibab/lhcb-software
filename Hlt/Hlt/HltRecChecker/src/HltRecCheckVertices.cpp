// $Id: HltRecCheckVertices.cpp,v 1.1 2006-10-24 09:50:49 hernando Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// from Event
#include "Linker/LinkedTo.h"
// local
#include "HltRecCheckVertices.h"
#include "HltRecCheckUtils.h"
#include "HltBase/ESequences.h"
#include "HltBase/HltFunctions.h"

using namespace LHCb;

//---------------------------------------------------------------------------
// Implementation file for class : HltRecCheckVertices
//
// 2006-05-24 : Hugo Ruiz Perez
//---------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltRecCheckVertices );

HltRecCheckVertices::HltRecCheckVertices( const std::string& name,
                                  ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
{
  
  declareProperty( "LinkName" ,    m_linkName     = "" );
  declareProperty( "IPType", m_ipType = "");
  
}

HltRecCheckVertices::~HltRecCheckVertices() {
  delete m_ipFun;
} 

StatusCode HltRecCheckVertices::initialize() {

  StatusCode sc = HltAlgorithm::initialize(); // must be executed first

  initializeHisto(m_histoNRCV,"NRCV",0.,10.,10);
  initializeHisto(m_histoNMCV,"NMCV",0.,10.,10);
  initializeHisto(m_histoNDV,"NDV",-10.,10.,20);

  initializeHisto(m_histoX,"VX",-1.,1.,100);
  initializeHisto(m_histoY,"VY",-1.,1.,100);
  initializeHisto(m_histoZ,"VZ",-200.,200.,100);

  initializeHisto(m_histoDX,"DeltaVX",-0.2,0.2,100);
  initializeHisto(m_histoDY,"DeltaVY",-0.2,0.2,100);
  initializeHisto(m_histoDZ,"DeltaVZ",-1.,1.,100);

  if (m_ipType == "2DIP") m_ipFun = new Hlt::rIP();
  else if (m_ipType == "3DIP") m_ipFun = new  Hlt::IP();
  else 
    error() << " please select an option IPType = 2DIP,3DIP " << endreq;

  return sc;
}

StatusCode HltRecCheckVertices::execute() {

  bool ok = HltAlgorithm::beginExecute();
  if (!ok) return stop(" No input tracks ");
  
  if (m_inputTracks) {
    relateVertices();
    checkVertices();
  }

  HltAlgorithm::endExecute();

  return StatusCode::SUCCESS;  
}

void HltRecCheckVertices::relateVertices() {

  LinkedTo<MCParticle> link(evtSvc(), msgSvc(), m_linkName);

  m_relTrackMCVertex.clear();
  m_relVertexMCVertex.clear();
  for (Hlt::TrackContainer::iterator it = m_inputTracks->begin();
       it != m_inputTracks->end(); ++it) {
    const Track& track = *(*it);
    MCParticle* mcpar = link.first( track.key() );

    size_t index = 0;
    double ip = Estd::map_compare_value(track,m_patInputVertices->begin(),
                                        m_patInputVertices->end(),
                                        *m_ipFun,Estd::abs_min(),index);
    verbose() << " ip " << ip << " index " << index << endreq;
    RecVertex& vertex = **(m_patInputVertices->begin()+index);
    
    if (!mcpar) continue;    

    const MCParticle& mother = MCHlt::ancestor(*mcpar);
    const MCVertex& mcvertex = *(mother.originVertex());

    m_relVertexMCVertex.relate(vertex,mcvertex);
    m_relTrackMCVertex.relate(track,mcvertex);
  }
  // m_relTrackMCVertex.info();

  m_relMCVertexTrack.clear();  
  m_relMCVertexTrack = m_relTrackMCVertex.reverse();
  // m_relMCVertexTrack.info();

  // m_relVertexMCVertex.info();

  m_relMCVertexVertex.clear();  
  m_relMCVertexVertex = m_relVertexMCVertex.reverse();
  // m_relMCVertexVertex.info();
  

}    



void HltRecCheckVertices::checkVertices() {

  int nrcver = m_relVertexMCVertex.size();
  int nmcver = m_relMCVertexVertex.size();
  fillHisto(m_histoNRCV,nrcver,1.);
  fillHisto(m_histoNMCV,nmcver,1.);
  fillHisto(m_histoNDV,nrcver-nmcver,1.);
  
  debug() << " n rec vertices " << nrcver << endreq;
  debug() << " n mc vertices " << nmcver << endreq;
  

  double w = 0.;
  const std::vector<RecVertex*>& vers = m_relVertexMCVertex.keys();
  for (std::vector<RecVertex*>::const_iterator it = vers.begin();
       it != vers.end(); ++it) {
    const RecVertex& ver = **it;
    double x = ver.position().x();
    double y = ver.position().y();
    double z = ver.position().z();

    const MCVertex& mcver = m_relVertexMCVertex.best_relation(ver,w);    
    double mcx = mcver.position().x();
    double mcy = mcver.position().y();
    double mcz = mcver.position().z();
    
    fillHisto(m_histoX,x,1.);
    fillHisto(m_histoY,y,1.);
    fillHisto(m_histoZ,z,1.);

    fillHisto(m_histoDX,x-mcx,1.);
    fillHisto(m_histoDY,y-mcy,1.);
    fillHisto(m_histoDZ,z-mcz,1.);

    debug() << " dx vertex " << x-mcx << endreq;
    debug() << " dy vertex " << y-mcy << endreq;
    debug() << " dz vertex " << z-mcz << endreq;
    
    
  }
}


StatusCode HltRecCheckVertices::finalize() {
  return HltAlgorithm::finalize();
}
