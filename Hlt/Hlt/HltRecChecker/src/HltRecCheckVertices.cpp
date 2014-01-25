// $Id: HltRecCheckVertices.cpp,v 1.12 2009-10-08 19:39:00 graven Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// from Event
#include "Linker/LinkedTo.h"
// local
#include "HltRecCheckVertices.h"
#include "HltRecCheckUtils.h"

using namespace LHCb;

//---------------------------------------------------------------------------
// Implementation file for class : HltRecCheckVertices
//
// 2006-05-24 : Hugo Ruiz Perez
//---------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltRecCheckVertices )

HltRecCheckVertices::HltRecCheckVertices( const std::string& name,
                                  ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
  , m_selections(*this)
{
  declareProperty( "LinkName" ,    m_linkName     = "" );
  declareProperty( "TESInputVerticesName", m_TESInputVerticesName = "");
  m_selections.declareProperties();
}

HltRecCheckVertices::~HltRecCheckVertices() {
} 

StatusCode HltRecCheckVertices::initialize() {

  StatusCode sc = HltAlgorithm::initialize(); // must be executed first

  m_selections.retrieveSelections();

  m_histoNRCV = initializeHisto("NRCV",0.,10.,10);
  m_histoNMCV = initializeHisto("NMCV",0.,10.,10);
  m_histoNDV = initializeHisto("NDV",-10.,10.,20);

  m_histoX = initializeHisto("VX",-1.,1.,100);
  m_histoY = initializeHisto("VY",-1.,1.,100);
  m_histoZ = initializeHisto("VZ",-200.,200.,100);

  m_histoDX = initializeHisto("DeltaVX",-0.2,0.2,100);
  m_histoDY = initializeHisto("DeltaVY",-0.2,0.2,100);
  m_histoDZ = initializeHisto("DeltaVZ",-1.,1.,100);

  return sc;
}

StatusCode HltRecCheckVertices::execute() {
  
  if (m_selections.input<1>()) {
    relateVertices();
    checkVertices();
  }

  return StatusCode::SUCCESS;  
}

void HltRecCheckVertices::relateVertices() {
  LinkedTo<MCParticle> link(evtSvc(), msgSvc(), m_linkName);

  m_TESInputVertices = get<RecVertices>(m_TESInputVerticesName);
  m_relTrackMCVertex.clear();
  m_relVertexMCVertex.clear();
  for (Hlt::TrackSelection::const_iterator it = m_selections.input<1>()->begin();
       it != m_selections.input<1>()->end(); ++it) {
    const Track& track = *(*it);
    MCParticle* mcpar = link.first( track.key() );

    size_t index = 0;
    RecVertex& vertex = **(m_TESInputVertices->begin()+index);
    
    if (!mcpar) continue;    

    const MCParticle& mother = MCHlt::ancestor(*mcpar);
    const MCVertex& mcvertex = *(mother.originVertex());

    m_relVertexMCVertex.relate((RecVertex*) &vertex, (MCVertex*) &mcvertex);
    m_relTrackMCVertex.relate((Track*) &track, (MCVertex*) &mcvertex);
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
  fill(m_histoNRCV,nrcver,1.);
  fill(m_histoNMCV,nmcver,1.);
  fill(m_histoNDV,nrcver-nmcver,1.);
  
  if ( msgLevel(MSG::DEBUG) ) debug() << " n rec vertices " << nrcver << endmsg;
  if ( msgLevel(MSG::DEBUG) ) debug() << " n mc vertices " << nmcver << endmsg;
  

  double w = 0.;
  const std::vector<RecVertex*>& vers = m_relVertexMCVertex.keys();
  for (std::vector<RecVertex*>::const_iterator it = vers.begin();
       it != vers.end(); ++it) {
    double x = (*it)->position().x();
    double y = (*it)->position().y();
    double z = (*it)->position().z();

    const MCVertex* mcver = m_relVertexMCVertex.best_relation(*it,w);    
    double mcx = mcver->position().x();
    double mcy = mcver->position().y();
    double mcz = mcver->position().z();
    
    fill(m_histoX,x,1.);
    fill(m_histoY,y,1.);
    fill(m_histoZ,z,1.);

    fill(m_histoDX,x-mcx,1.);
    fill(m_histoDY,y-mcy,1.);
    fill(m_histoDZ,z-mcz,1.);

    if ( msgLevel(MSG::DEBUG) ) debug() << " dx vertex " << x-mcx << endmsg;
    if ( msgLevel(MSG::DEBUG) ) debug() << " dy vertex " << y-mcy << endmsg;
    if ( msgLevel(MSG::DEBUG) ) debug() << " dz vertex " << z-mcz << endmsg;
  }
}


StatusCode HltRecCheckVertices::finalize() {
  return HltAlgorithm::finalize();
}
