// $Id: HltRecChecker.cpp,v 1.9 2010-08-17 11:02:41 graven Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// from Event
#include "Linker/LinkedTo.h"
// local
#include "HltRecChecker.h"
#include "HltRecCheckUtils.h"

using namespace LHCb;

//---------------------------------------------------------------------------
// Implementation file for class : HltRecChecker
//
// 2006-05-24 : Hugo Ruiz Perez
//---------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltRecChecker )

HltRecChecker::HltRecChecker( const std::string& name,
                                  ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
  , m_selections(*this)
{
  declareProperty( "LinkName" ,    m_linkName     = "" );
  m_selections.declareProperties();
}

HltRecChecker::~HltRecChecker() 
{
} 

StatusCode HltRecChecker::initialize() {

  StatusCode sc = HltAlgorithm::initialize(); // must be executed first

  m_selections.retrieveSelections();
  m_selections.registerSelection();


  book1D("Quark",0.,6.,6);
  book1D("Track Bs",0.,10.,10);
  book1D("Track TISTOS",0.,4.,8);

  book1D("Vertex Bs",0.,15.,15);
  book1D("Vertex TISTOS",0.,4.,8);

  // toolSvc()->retrieveTool("MCDecayFinder",m_finder);

  return sc;
}

StatusCode HltRecChecker::execute() {

  //  bool ok = true;
  
  checkQuark();
  
  if (m_selections.input<1>()!=0) checkTracks(*m_selections.input<1>());

  // checkVertices();

  return StatusCode::SUCCESS;  
}

void HltRecChecker::checkQuark() {
  MCParticles* mcpars = get<MCParticles>(MCParticleLocation::Default);
  int q = MCHlt::iquark(*mcpars);
  fill( histo1D(std::string("Quark")), q, 1.);
  if ( msgLevel(MSG::DEBUG) ) debug() << " check quark " << q << endmsg;
}

void HltRecChecker::checkTracks(const Hlt::TrackSelection& tracks) {


  LinkedTo<MCParticle> link(evtSvc(), msgSvc(), m_linkName);
  
  unsigned nbs = 0;
  for (Hlt::TrackSelection::const_iterator it = tracks.begin();
       it != tracks.end(); ++it) {
    printTrack(*it).ignore();
    MCParticle* par = link.first( (*it)->key() );
    if (par==0) continue;
    const MCParticle& mother = MCHlt::ancestor( (*par) );
    int q = MCHlt::iquark(mother);
    if (q != 1) continue;
    ++nbs;
    if (m_selections.output()) m_selections.output()->push_back( *it );
  }
  
  fill( histo1D(std::string("Track Bs")), nbs, 1.);
  
  //bool tos = false;
  //bool tis = false;
  
  //if (nbs >0) tos = true;
  //if (tracks.size() > nbs) tis = true;

  // int tostis = MCHlt::tostis(tos,tis);
  // fill( histo1D(Track TISTOS),tostis,1.);

  if ( msgLevel(MSG::DEBUG) ) debug() << " check tracks nbs " << nbs << endmsg;
  // debug() << " check tracks tostis " << tostis << endmsg;

}

void HltRecChecker::checkVertices(const Hlt::VertexSelection&) {
  
}

//=========================================================================
//  Print a track
//=========================================================================
StatusCode HltRecChecker::printTrack(const LHCb::Track* T) {
  if (!msgLevel(MSG::DEBUG)) return StatusCode::SUCCESS ;
  if ( T==0 ){
    Warning("NULL Track pointer");
    return StatusCode::SUCCESS;
  }
  
  if ( T->firstState().qOverP()!=0.){
    Gaudi::SymMatrix6x6 cov6D ; 
    T->posMomCovariance(cov6D);
    if ( msgLevel(MSG::DEBUG) ) debug() << "    Track " << T->key() << " " << T->type() << " " 
            << T->position() << " " << T->momentum() << "\n"
            << cov6D << endmsg ;
  } else { // velo tracks
    Gaudi::XYZPoint p ;
    Gaudi::SymMatrix3x3 cov3D ; 
    T->position(p,cov3D);
    if ( msgLevel(MSG::DEBUG) ) debug() << "    Track " << T->key() << " " << T->type() << " " 
            << p << "\n" << cov3D << endmsg ; 
  }
  
  if (!T->ancestors().empty()) 
  {
    if ( msgLevel(MSG::DEBUG) ) debug() << "Looking at " << T->ancestors().size() << " ancestors" << endmsg ;
  }
  
  for ( SmartRefVector< LHCb::Track >::const_iterator t = 
          T->ancestors().begin(); t!= T->ancestors().end(); ++t ) {
    const LHCb::Track* at = *t ;
    StatusCode sc = printTrack(at);
    if (!sc) return sc;
  }

  return StatusCode::SUCCESS;
}

StatusCode HltRecChecker::finalize() {
  return HltAlgorithm::finalize();
}
