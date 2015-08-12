// Include files 

// local
#include "MuMonitor.h"
#include "Event/HltDecReports.h"
#include "Event/ODIN.h"
#include "Event/Track.h"
#include "LoKi/HLTCuts.h"
#include "AIDA/IAxis.h"
#include<TMath.h>
#include "TProfile.h"
#include "GaudiUtils/Aida2ROOT.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MuMonitor
//
// 2011-04-01 : Patrick Koppenburg
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MuMonitor )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuMonitor::MuMonitor( const std::string& name,
                            ISvcLocator* pSvcLocator)
  : HltMonitorBase ( name , pSvcLocator )
  , m_filter  ( LoKi::Constant<const LHCb::HltDecReports*,bool> ( true ) )
  , m_nBX(2808)
{
  declareProperty("HltDecReportsLocation",m_location = LHCb::HltDecReportsLocation::Default );
  declareProperty("HltRegex", m_pattern = "Hlt1.*NoBias.*Decision", 
                  "Expression to select HLT decision")->
    declareUpdateHandler ( &MuMonitor::handler_1 , this );
}
//=============================================================================
// Destructor
//=============================================================================
MuMonitor::~MuMonitor() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode MuMonitor::initialize() {
  StatusCode sc = HltMonitorBase::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by HltMonitorBase

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_filter = LoKi::Cuts::HLT_PASS_RE ( m_pattern ) ;
  m_bBx = book1D("BCID",-0.5,m_nBX+0.5,m_nBX+1);
  m_hVelo = book1D("2 velo tracks",-0.5,1.5,2);
  m_hMuVelo = bookProfile1D(">2 velo tracks vs bunch", -0.5,m_nBX+0.5,m_nBX+1,"",0.,1.);

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode MuMonitor::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  bool ok = ( "" == m_pattern ) ;
  if (!ok) {
    const LHCb::HltDecReports *dec = get<const LHCb::HltDecReports> (  m_location );
    ok = (m_filter)(dec);
    if (msgLevel(MSG::DEBUG)) debug() << "Passed Hlt Filter " << ok << endmsg ;
  }
  if (ok || true){
    const LHCb::ODIN* odin = get<LHCb::ODIN> ( LHCb::ODINLocation::Default );
    fill(m_bBx,odin->bunchId(),1.);
    unsigned int nv = 0;
    const LHCb::Tracks* veloTracks = getIfExists<LHCb::Track::Container>(LHCb::TrackLocation::Velo);
    if ( NULL != veloTracks ){
      nv = veloTracks->size() ;
    }
    bool seen = (nv>=2) ; // 
    fill(m_hVelo, seen, 1.);
    fill(m_hMuVelo,odin->bunchId(),seen,1.);
    
    if (msgLevel(MSG::DEBUG)) debug() << "Velo Tracks " << nv << endmsg ;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode MuMonitor::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;
  TH1D* h = Gaudi::Utils::Aida2ROOT::aida2root ( book1D( "MuvsBX", "Mu from velo Tk>2 vs Bunch",
                                                         -0.5,m_nBX+0.5,m_nBX+1) ) ;
  h->SetEntries(m_hMuVelo->entries());
  for ( int i=0; i!=m_hMuVelo->axis().bins(); i++){
    if (m_hMuVelo->binEntries(i)>0){
      double z = 1.-m_hMuVelo->binHeight(i);  // empty events
      double e = m_hMuVelo->binError(i);
      double mu = 0;
      double mue = 0;
      if (z!=0.){
        mu  = -log(z);
        mue = e/z ;
      }
      h->SetBinContent(i,mu);
      h->SetBinError(i,mue);
      if (msgLevel(MSG::DEBUG)) debug() << "BX " << i << " : " 
                                        << m_hMuVelo->binHeight(i) << " +/- " << e 
                                        << " -> mu= " << mu << " +/- " << mue 
                                        << " :" << h->GetBinContent(i) << endmsg ;
    }
  }
  

  m_filter = LoKi::Constant<const LHCb::HltDecReports*,bool> ( true ) ;

  return HltMonitorBase::finalize();  // must be called after all other actions
}

//=============================================================================
void MuMonitor::handler_1 ( Property& /* p */ ){
  m_filter = LoKi::Cuts::HLT_PASS_RE ( m_pattern ) ;
}
