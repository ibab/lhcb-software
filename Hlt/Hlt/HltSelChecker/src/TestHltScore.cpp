// $Id: TestHltScore.cpp,v 1.3 2005-12-09 07:00:20 pkoppenb Exp $
// Include files 

#include <boost/format.hpp>
// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// Event model
#include "Event/HltScore.h"
#include "Kernel/IAlgorithm2ID.h"

// local
#include "TestHltScore.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TestHltScore
//
// 2005-05-10 : Patrick KOPPENBURG
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<TestHltScore>          s_factory ;
const        IAlgFactory& TestHltScoreFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TestHltScore::TestHltScore( const std::string& name,
                            ISvcLocator* pSvcLocator)
  : GaudiHistoAlg ( name , pSvcLocator )
  , m_algorithm2ID()
  , m_selEvents(0)  // number of selected events
  , m_rejEvents(0)  // number of rejected events
  , m_notEvents(0)  // number of not processed events
  , m_selPV(0)  // number of PVs in selected events
  , m_rejPV(0)  // number of PVs in rejected events
  , m_selTracks(0)  // number of tracks in selected events
  , m_rejTracks(0)  // number of tracks in rejected events
  , m_selPhotons(0)  // number of photons in selected events
  , m_rejPhotons(0)  // number of photons in rejected events
  , m_selPi0s(0)  // number of Pi0s in selected events
  , m_rejPi0s(0)  // number of Pi0s in rejected events
  , m_selB(0)  // number of B in selected events
  , m_selDstar(0)  // number of D* in selected events
  , m_hSelPV(0)  // histo of number of PVs in selected events
  , m_hRejPV(0)  // histo of number of PVs in rejected events
  , m_hSelTracks(0)  // histo of number of tracks in selected events
  , m_hRejTracks(0)  // histo of number of tracks in rejected events
  , m_hSelPhotons(0)  // histo of number of photons in selected events
  , m_hRejPhotons(0)  // histo of number of photons in rejected events
  , m_hSelPi0s(0)  // histo of number of Pi0s in selected events
  , m_hRejPi0s(0)  // histo of number of Pi0s in rejected events
  , m_hSelB(0)  // histo of number of B in selected events
  , m_hSelDstar(0)  // histo of number of D* in selected events
  , m_hExcSel(0) // Exclusive selections: One bin for each selection
  , m_hMassBd(0) // Bd mass
  , m_hMassDstar(0) // Dstar mass
  , m_hMassDimu(0) // Dimu mass
  , m_dstarSel(-1) //  ID of D* selection
  , m_psiSel(-1) // ID of Dimu selection
{
  declareProperty ( "DstarSelection"   , m_dstarSelName = "HLTFilterDstar" ) ;
  declareProperty ( "PsiSelection"   , m_psiSelName = "HLTFilterB2JpsiX" ) ;
}
//=============================================================================
// Destructor
//=============================================================================
TestHltScore::~TestHltScore() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode TestHltScore::initialize() {
  StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  m_algorithm2ID = tool<IAlgorithm2ID>("Algorithm2ID");
  if (!m_algorithm2ID){
    err() << "Could not get tool Algorithm2ID " << endmsg ;
    return StatusCode::FAILURE;
  }
  m_histosvc = svc<IHistogramSvc>("HistogramDataSvc", true);
  if( 0==m_histosvc )   {
    fatal() << "Unable to locate HistogramSvc" << endreq;
    return StatusCode::FAILURE;
  }

  if ( produceHistos() ){
    // Booking the histograms
    m_hSelPV = m_histosvc->book("selPV", "Number of PVs in selected events",10,-0.5,9.5);
    m_hRejPV = m_histosvc->book("rejPV","Number of PVs in rejected events",10,-0.5,9.5);	 
    m_hSelTracks = m_histosvc->book("selTracks","Number of tracks in selected events",150,-0.5,149.5);
    m_hRejTracks = m_histosvc->book("rejTracks","Number of tracks in rejected events",150,-0.5,149.5);
    m_hSelPhotons = m_histosvc->book("selPhotons","Number of photons in selected events",150,-0.5,149.5);
    m_hRejPhotons = m_histosvc->book("rejPhotons","Number of photons in rejected events",150,-0.5,149.5);
    m_hSelPi0s = m_histosvc->book( "selPi0s", "Number of Pi0s in selected events",150,-0.5,149.5);
    m_hRejPi0s = m_histosvc->book( "rejPi0s", "Number of Pi0s in rejected events",150,-0.5,149.5);
    m_hSelB = m_histosvc->book(	"selB", "Number of B in selected events",10,-0.5,9.5);
    m_hSelDstar = m_histosvc->book("selDstar","Number of D* in selected events",10,-0.5,9.5);  
    
    m_hExcSel = m_histosvc->book("excSel", "Exclusive selections", 11,-0.5,10.5);
    m_hMassBd = m_histosvc->book("MassBd", "Bd mass", 40,4000.,6000.);
    m_hMassDstar = m_histosvc->book("MassDstar", "Dstar mass", 20, 1500.,2500.);
    m_hMassDimu = m_histosvc->book("MassDimu", "Dimuon mass", 40, 2500.,4500.);
    for ( int i=0 ; i<m_algorithm2ID->size() ; ++i){     
      std::string name = m_algorithm2ID->nameFromId(i);
      debug() << "Making histogram with name " << name << endmsg ;
      IHistogram1D* h =  m_histosvc->book(name, "Candidate mass from "+name, 80, 2000.,6000.);
      m_hMassCand.push_back(h);
      if ( name == m_dstarSelName ) m_dstarSel = i ;
      if ( name == m_psiSelName ) m_psiSel = i ;
    }
  }

  // Declare information for monitoring. Ignored if MonitoringSvc not started
  m_status = "intializing";
#ifdef DC06
  debug() << "Declaring Infos to be published" << endreq;
  declareInfo("status",    m_status,"Online status of HltGaucho algorithm");
  declareInfo("selEvents", m_selEvents, "Number of selected events");
  declareInfo("rejEvents", m_rejEvents, "Number of rejected events");
  declareInfo("notEvents", m_notEvents, "Number of not processed events");
  declareInfo("selPV",     m_hSelPV,     "Number of PVs in selected events");
  declareInfo("rejPV",     m_hRejPV,     "Number of PVs in rejected events");
  declareInfo("selTracks", m_hSelTracks, "Number of tracks in selected events");
  declareInfo("rejTracks", m_hRejTracks, "Number of tracks in rejected events");
  declareInfo("selPhotons",m_hSelPhotons,"Number of photons in selected events");
  declareInfo("rejPhotons",m_hRejPhotons,"Number of photons in rejected events");
  declareInfo("selPi0s",   m_hSelPi0s,   "Number of Pi0s in selected events");
  declareInfo("rejPi0s",   m_hRejPi0s,   "Number of Pi0s in rejected events");
  declareInfo("selB",      m_hSelB,      "Number of B in selected events");
  declareInfo("selDstar",  m_hSelDstar,  "Number of D* in rejected events");
  
  declareInfo("excSel",    m_hExcSel,    "Exclusive selections");
  declareInfo("massBd",    m_hMassBd,    "Bd mass");
  declareInfo("massDstar", m_hMassDstar, "Dstar mass");
  declareInfo("massDimu",  m_hMassDimu,  "Dimuon mass");
  for ( int i=0 ; i<m_algorithm2ID->size() ; ++i){     
    IHistogram1D* h = m_hMassCand[i];
    std::string name = m_algorithm2ID->nameFromId(i);
    if ( 0!=h) declareInfo(name, h, "Candidate mass from "+name);
  }
#endif
  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode TestHltScore::execute() {

  debug() << "==> Execute" << endmsg;
  
  m_status="executing";
  
  // code goes here  
  const HltScore* score = getScore() ;
  if ( 0 == score ) return  StatusCode::SUCCESS;

  StatusCode sc = StatusCode::SUCCESS;
  // check printout level here and not in method
  if (( msgLevel( MSG::DEBUG )) || 
      ((score->decisionGen() && (msgLevel( MSG::INFO ))))){
    sc = printScore(score);
    if (!sc) return sc;
  }

  if ( produceHistos() ) {
    sc = fillHistos(score);
    if (!sc) return sc;
  }
  
  sc = fillStats(score) ;
  if (!sc) return sc;

  return sc;
}

//=============================================================================
// Print Statistics
//=============================================================================
StatusCode TestHltScore::printScore(const HltScore* score) {

  debug() << "printScore" << endmsg;
  // Boost clashes with info() for instance -> check printout level in execute
  // and set print level to always() here 
  boost::format line("%1$-40S");
  always()   << "\n=================== HLT summary ====================\n" ;
  always()   << line % "   HLT Decision" << " : " << score->decision() << "\n" ;
  always() << "----------------------------------------------------\n" ;
  if ( score->decision() ){
    always() << line % "   Inclusive B Decision" << " : " << score->decisionInclusiveB() << "\n" ;
    always() << line % "   Dimuon Decision" << " : " << score->decisionDimuon() << "\n" ;
    always() << line % "   D* Decision" << " : " << score->decisionDstar() << "\n" ;
    always() << line % "   Exclusive Decision" << " : " << score->decisionExclusive() << "\n" ;
  }
  always() << line % "   Generic Decision" << " : " << score->decisionGen() << "\n" ;
  const std::vector<HltAlgorithmScore> algoScores = score->candidates() ;
  if ( !algoScores.empty() ){
    always() << "==================== Candidates ====================\n" ;
    always() << " Algorithm                 (ID)     Mass      Chi2 \n" ;
    always() << "----------------------------------------------------\n" ;
    for ( std::vector<HltAlgorithmScore>::const_iterator ias = algoScores.begin() ;
          ias != algoScores.end() ; ++ias ){
      StatusCode sc = printAlgorithm(&(*ias));
      if (!sc) return sc;
    }
  }
  if (score->decisionGen()){
    always()   << "==================== Statistics ====================\n" ;
    if (score->decisionDstar()) 
      always()   << line % "   Number of D* candidates" << " : " << score->nbDstar() << "\n" ;
    if (score->decisionExclusive()) 
      always()   << line % "   Number of B candidates" << " : " << score->nbB() << "\n" ;
    always()   << line % "   Number of reconstructed PV" << " : " << score->nbPV() << "\n" ;
    always()   << line % "   Number of tracks" << " : " << score->nbTrack() << "\n" ;
    always()   << line % "   Number of photons" << " : " << score->nbPhoton() << "\n" ;
    always()   << line % "   Number of pi0s" << " : " << score->nbPi0() << "\n" ;
    always()   << line % "   Number of muons" << " : " << score->nbMuon() << "\n" ;
    always()   << line % "   Number of dimuons" << " : " << score->nbDimu() << "\n" ;
  }
  always()   << "====================================================" ;
  always()   << endmsg ;

  return StatusCode::SUCCESS;
};


//=============================================================================
// Fill histograms
//=============================================================================
StatusCode TestHltScore::fillHistos(const HltScore* score) {

  debug() << "==> Execute: fillHistos" << endmsg;
  if ( score->decisionGen() ){
    if ( score->decision() ){
      m_hSelPV     ->fill( score->nbPV() ); // number of PVs in selected events
      m_hSelTracks ->fill( score->nbTrack() ); // number of tracks in selected events
      m_hSelPhotons->fill( score->nbPhoton() ); // number of photons in selected events
      m_hSelPi0s   ->fill( score->nbPi0() ); // number of Pi0s in selected events
      m_hSelB      ->fill( score->nbB() ); // number of B in selected events
      m_hSelDstar  ->fill( score->nbDstar() ); // number of Dstar in selected events
    } else {
      m_hRejPV     ->fill( score->nbPV() ); // number of PVs in rejected events
      m_hRejTracks ->fill( score->nbTrack() ); // number of tracks in rejected events
      m_hRejPhotons->fill( score->nbPhoton() ); // number of photons in rejected events
      m_hRejPi0s   ->fill( score->nbPi0() ); // number of Pi0s in rejected events
    }
  } 
  
  const std::vector<HltAlgorithmScore>& exclusiveSels = score->candidates();
  for (std::vector<HltAlgorithmScore>::const_iterator iExc = exclusiveSels.begin();
       iExc != exclusiveSels.end(); ++iExc ){
    debug() << "Exclusive: " << (*iExc).algorithmID() << " " 
            << m_algorithm2ID->nameFromId((*iExc).algorithmID()) << endreq;
    m_hExcSel->fill( (*iExc).algorithmID() );
    //  How to make this independent of the  specific  algorithmID?
    int sel = (*iExc).algorithmID() ;
    m_hMassCand[sel]->fill( (*iExc).mass() ); 
    // psi and D* selection
    if ( sel == m_dstarSel )  m_hMassDstar->fill( (*iExc).mass() );
    else if ( sel == m_psiSel )  m_hMassDimu->fill( (*iExc).mass() );
    else  m_hMassBd->fill( (*iExc).mass() );
  }
  return StatusCode::SUCCESS;
};
//=============================================================================
// Print candidates
//=============================================================================
StatusCode TestHltScore::printAlgorithm(const HltAlgorithmScore* ias ) {

  boost::format algo("%1$-25S");
  boost::format real1("%1$10.1f");
  boost::format real4("%1$10.3f");
  boost::format integer("%1$2i");
  std::string name = m_algorithm2ID->nameFromId(ias->algorithmID());
  always() << " " << algo % name << " (" << integer % ias->algorithmID() << ")"
           << real1 % ias->mass()
           << real4 % ias->chi2() << "\n";

  return StatusCode::SUCCESS;
};

//=============================================================================
// check score
//=============================================================================
StatusCode TestHltScore::fillStats(const HltScore* score) {

  if ( score->decisionGen() ){
    if ( score->decision() ){
      ++m_selEvents ; // number of selected events
      m_selPV      += score->nbPV() ; // number of PVs in selected events
      m_selTracks  += score->nbTrack() ; // number of tracks in selected events
      m_selPhotons += score->nbPhoton() ; // number of photons in selected events
      m_selPi0s    += score->nbPi0() ; // number of Pi0s in selected events
      m_selB       += score->nbB() ; // number of B in selected events
      m_selDstar   += score->nbDstar() ; // number of Dstar in selected events
    } else {
      ++m_rejEvents ; // number of rejected events
      m_rejPV      += score->nbPV() ; // number of PVs in rejected events
      m_rejTracks  += score->nbTrack() ; // number of tracks in rejected events
      m_rejPhotons += score->nbPhoton() ; // number of photons in rejected events
      m_rejPi0s    += score->nbPi0() ; // number of Pi0s in rejected events
    }
  } else {
    ++m_notEvents ; // number of not processed events 
  }
  
  return StatusCode::SUCCESS;
}
//=============================================================================
// Get score
//=============================================================================
const HltScore* TestHltScore::getScore(void) {

  // code goes here  
  const HltScore* score = NULL ;
  if ( exist<HltScore>(HltScoreLocation::Default) ){
    score = get<HltScore>(HltScoreLocation::Default);
    if ( 0 == score ){
      warning() << "No HltScore saved at " << HltScoreLocation::Default << endmsg ;
      return NULL;
    }
  } else {
    err() << "No HltScore found at " << HltScoreLocation::Default << endmsg ; 
    return NULL ;
  }
  return score;
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode TestHltScore::finalize() {

  debug() << "==> Finalize" << endmsg;
  
  StatusCode sc = printStatistics();
  if (!sc) return sc;

  return GaudiHistoAlg::finalize();  // must be called after all other actions
}
//=============================================================================
// Print Statistics
//=============================================================================
StatusCode TestHltScore::printStatistics() {

  boost::format line("%1$-20S");
  boost::format real("%1$10.2f");
 
  int ntot = m_selEvents+m_rejEvents+m_notEvents ;
  if ( ntot == 0 ) ntot = 1 ;
  int nsel = m_selEvents ;
  if ( nsel == 0 ) nsel = 1;
  int nrej = m_rejEvents ;
  if ( nrej == 0 ) nrej = 1;

  always() << "\n===================== HLT statistics ====================\n" ;
  always() << line % " " << "     HLT yes     HLT no  Generic no\n" ;
  always() << line % "  HLT Decision" << ":" 
           << real % (100.*m_selEvents/ntot) << "%"  // boost needs parentheses
           << real % (100.*m_rejEvents/ntot) << "%" 
           << real % (100.*m_notEvents/ntot) << "%\n" ;
  always() << line % "  Number of PVs" << ":" 
           << real % (double(m_selPV)/nsel) << " " 
           << real % (double(m_rejPV)/nrej) << "\n" ;
  always() << line % "  Number of Tracks" << ":" 
           << real % (double(m_selTracks)/nsel) << " " 
           << real % (double(m_rejTracks)/nrej) << "\n" ;
  always() << line % "  Number of Photons" << ":" 
           << real % (double(m_selPhotons)/nsel) << " " 
           << real % (double(m_rejPhotons)/nrej) << "\n" ;
  always() << line % "  Number of Pi0" << ":" 
           << real % (double(m_selPi0s)/nsel) << " " 
           << real % (double(m_rejPi0s)/nrej) << "\n" ;  
  always() << line % "  Number of B" << ":" 
           << real % (double(m_selB)/nsel) << "\n" ;  
  always() << line % "  Number of D*" << ":" 
           << real % (double(m_selDstar)/nsel) << "\n" ;  
  always() << endmsg ;

  return StatusCode::SUCCESS;
};


//=============================================================================
