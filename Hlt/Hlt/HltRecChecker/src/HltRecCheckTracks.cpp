// $Id: HltRecCheckTracks.cpp,v 1.1.1.1 2006-09-25 09:45:14 hernando Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

// from Event
#include "Linker/LinkedTo.h"
// local
#include "HltRecCheckTracks.h"

using namespace LHCb;

//---------------------------------------------------------------------------
// Implementation file for class : HltRecCheckTracks
//
// 2006-05-24 : Hugo Ruiz Perez
//---------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltRecCheckTracks );

HltRecCheckTracks::HltRecCheckTracks( const std::string& name,
                                  ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
{
  
  declareProperty( "LinkName" ,    m_linkName     = "" );
  
}

HltRecCheckTracks::~HltRecCheckTracks() {} 

StatusCode HltRecCheckTracks::initialize() {

  StatusCode sc = HltAlgorithm::initialize(); // must be executed first

  initializeHisto(m_histoGhost,"Ghost",0.,1.,100);

  initializeHisto(m_histoDx,"DeltaX",-0.3,0.3,100);
  initializeHisto(m_histoDy,"DeltaY",-0.3,0.3,100);
  initializeHisto(m_histoDz,"DeltaZ",-30.,30.,100);
  initializeHisto(m_histoDTx,"DeltaTX",-0.02,0.02,100);
  initializeHisto(m_histoDTy,"DeltaTY",-0.02,0.02,100);
  initializeHisto(m_histoPOP,"DeltaPOP",-1.,1.,100);

  return sc;
}

StatusCode HltRecCheckTracks::execute() {

  bool ok = HltAlgorithm::beginExecute();
  if (!ok) return stop(" No input tracks ");
  
  if (m_inputTracks) checkTracks();

  HltAlgorithm::endExecute();

  return StatusCode::SUCCESS;  
}

void HltRecCheckTracks::checkTracks() {

  LinkedTo<MCParticle> link(evtSvc(), msgSvc(), m_linkName);
  
  int nghost = 0;
  for (Hlt::TrackContainer::iterator it = m_inputTracks->begin();
       it != m_inputTracks->end(); ++it) {
    const Track& track = *(*it);
    const State& state = track.firstState();
    MCParticle* mcpar = link.first( track.key() );
    if (!mcpar) nghost += 1;
    if (!mcpar) continue;

    double mcpz = mcpar->momentum().z();
    double mctx = mcpar->momentum().x()/mcpz;
    double mcty = mcpar->momentum().y()/mcpz;
    double mcx = mcpar->originVertex()->position().x();
    double mcy = mcpar->originVertex()->position().y();
    double mcz = mcpar->originVertex()->position().z();
    double mcp = mcpar->p();

    double dx = state.x()-mcx;
    double dy = state.y()-mcy;
    double dz = state.z()-mcz;
    double dtx = state.tx()-mctx;
    double dty = state.ty()-mcty;
    double dpop = (track.p()-mcp)/mcp;


    fillHisto( m_histoDx, dx, 1.);
    fillHisto( m_histoDy, dy, 1.);
    fillHisto( m_histoDz, dz, 1.);

    fillHisto( m_histoDTx, dtx, 1.);
    fillHisto( m_histoDTy, dty, 1.);
    fillHisto( m_histoPOP, dpop, 1.);
    

  }

  int ntracks = m_inputTracks->size();
  double rat = (1.*nghost)/(1.*ntracks);
  fillHisto( m_histoGhost, rat, 1.);

  debug() << " checkReco ghost " << nghost << " " 
          << ntracks << " " << rat << endreq;
  
}


StatusCode HltRecCheckTracks::finalize() {
  return HltAlgorithm::finalize();
}
