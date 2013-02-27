// $Id: HltRecCheckTracks.cpp,v 1.7 2009-10-08 15:44:42 graven Exp $
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
  , m_selections(*this)
  , m_histoGhost(0)
  , m_histoDx(0)
  , m_histoDy(0)
  , m_histoDz(0)
  , m_histoDTx(0)
  , m_histoDTy(0)
  , m_histoPOP(0)
{
  declareProperty( "LinkName" ,    m_linkName );
  m_selections.declareProperties();
}

HltRecCheckTracks::~HltRecCheckTracks() {} 

StatusCode HltRecCheckTracks::initialize() {

  StatusCode sc = HltAlgorithm::initialize(); // must be executed first

  m_selections.retrieveSelections();

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
  
  if (m_selections.input<1>()==0) return StatusCode::SUCCESS;  

  LinkedTo<MCParticle> link(evtSvc(), msgSvc(), m_linkName);
  
  int nghost = 0;
  for (Hlt::TrackSelection::const_iterator it = m_selections.input<1>()->begin();
       it != m_selections.input<1>()->end(); ++it) {
    const Track& track = **it;
    const State& state = track.firstState();
    MCParticle* mcpar = link.first( track.key() );
    if (mcpar==0) { 
        ++nghost;
        continue;
    }

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


    fill( m_histoDx, dx, 1.);
    fill( m_histoDy, dy, 1.);
    fill( m_histoDz, dz, 1.);

    fill( m_histoDTx, dtx, 1.);
    fill( m_histoDTy, dty, 1.);
    fill( m_histoPOP, dpop, 1.);
    

  }

  int ntracks = m_selections.input<1>()->size();
  if (ntracks!=0) {
      double rat = double(nghost)/double(ntracks);
      fill( m_histoGhost, rat, 1.);
      debug() << " checkReco ghost " << nghost << " " 
              << ntracks << " " << rat << endmsg;
  }
  return StatusCode::SUCCESS;
}
