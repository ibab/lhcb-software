// $Id: HltRecCheckTracks.cpp,v 1.4 2008-01-22 11:04:06 hernando Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

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

  m_histoGhost = initializeHisto("Ghost",0.,1.,100);
  m_histoDx = initializeHisto("DeltaX",-0.3,0.3,100);
  m_histoDy = initializeHisto("DeltaY",-0.3,0.3,100);
  m_histoDz = initializeHisto("DeltaZ",-30.,30.,100);
  m_histoDTx = initializeHisto("DeltaTX",-0.02,0.02,100);
  m_histoDTy = initializeHisto("DeltaTY",-0.02,0.02,100);
  m_histoPOP = initializeHisto("DeltaPOP",-1.,1.,100);

  return sc;
}

StatusCode HltRecCheckTracks::execute() {
  
  if (m_inputTracks) checkTracks();

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


    fillHisto( *m_histoDx, dx, 1.);
    fillHisto( *m_histoDy, dy, 1.);
    fillHisto( *m_histoDz, dz, 1.);

    fillHisto( *m_histoDTx, dtx, 1.);
    fillHisto( *m_histoDTy, dty, 1.);
    fillHisto( *m_histoPOP, dpop, 1.);
    

  }

  int ntracks = m_inputTracks->size();
  double rat = (1.*nghost)/(1.*ntracks);
  fillHisto( *m_histoGhost, rat, 1.);

  debug() << " checkReco ghost " << nghost << " " 
          << ntracks << " " << rat << endreq;
  
}


StatusCode HltRecCheckTracks::finalize() {
  return HltAlgorithm::finalize();
}
