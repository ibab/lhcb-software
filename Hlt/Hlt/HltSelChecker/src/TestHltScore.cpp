// $Id: TestHltScore.cpp,v 1.1 2005-05-10 16:37:00 pkoppenb Exp $
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
  : GaudiAlgorithm ( name , pSvcLocator )
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
{

}
//=============================================================================
// Destructor
//=============================================================================
TestHltScore::~TestHltScore() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode TestHltScore::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  m_algorithm2ID = tool<IAlgorithm2ID>("Algorithm2ID");
  if (!m_algorithm2ID){
    err() << "Could not get tool Algorithm2ID " << endmsg ;
    return StatusCode::FAILURE;
  }

  debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode TestHltScore::execute() {

  debug() << "==> Execute" << endmsg;

  // code goes here  
  const HltScore* score = getScore() ;
  if ( 0 == score ) return  StatusCode::SUCCESS;

  StatusCode sc = printScore(score);
  if (!sc) return sc;
  sc = checkScore(score) ;
  if (!sc) return sc;
  sc = fillStats(score) ;
  if (!sc) return sc;

  setFilterPassed(score->decision());   // Mandatory. Set to true if event is accepted.

  return sc;
}

//=============================================================================
// Print Statistics
//=============================================================================
StatusCode TestHltScore::printScore(const HltScore* score) {

  debug() << "==> Execute" << endmsg;
  boost::format line("%1$-40S");
  info()   << "\n=================== HLT summary ====================\n" ;
  info()   << line % "   HLT Decision" << " : " << score->decision() << "\n" ;
  info() << "----------------------------------------------------\n" ;
  if ( score->decision() ){
    info() << line % "   Inclusive B Decision" << " : " << score->decisionInclusiveB() << "\n" ;
    info() << line % "   Dimuon Decision" << " : " << score->decisionDimuon() << "\n" ;
    info() << line % "   D* Decision" << " : " << score->decisionDstar() << "\n" ;
    info() << line % "   Exclusive Decision" << " : " << score->decisionExclusive() << "\n" ;
  }
  info() << line % "   Generic Decision" << " : " << score->decisionGen() << "\n" ;
  const std::vector<HltAlgorithmScore> algoScores = score->candidates() ;
  if ( !algoScores.empty() ){
    info() << "==================== Candidates ====================\n" ;
    info() << " Algorithm                 (ID)     Mass      Chi2 \n" ;
    info() << "----------------------------------------------------\n" ;
    for ( std::vector<HltAlgorithmScore>::const_iterator ias = algoScores.begin() ;
          ias != algoScores.end() ; ++ias ){
      StatusCode sc = printAlgorithm(&(*ias));
      if (!sc) return sc;
    }
  }
  if (score->decisionGen()){
    info()   << "==================== Statistics ====================\n" ;
    if (score->decisionDstar()) 
      info()   << line % "   Number of D* candidates" << " : " << score->nbDstar() << "\n" ;
    if (score->decisionExclusive()) 
      info()   << line % "   Number of B candidates" << " : " << score->nbB() << "\n" ;
    info()   << line % "   Number of reconstructed PV" << " : " << score->nbPV() << "\n" ;
    info()   << line % "   Number of tracks" << " : " << score->nbTrack() << "\n" ;
    info()   << line % "   Number of photons" << " : " << score->nbPhoton() << "\n" ;
    info()   << line % "   Number of pi0s" << " : " << score->nbPi0() << "\n" ;
    info()   << line % "   Number of muons" << " : " << score->nbMuon() << "\n" ;
    info()   << line % "   Number of dimuons" << " : " << score->nbDimu() << "\n" ;
  }
  info()   << "====================================================" ;
  info()   << endmsg ;

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
  info() << " " << algo % name << " (" << integer % ias->algorithmID() << ")"
         << real1 % ias->mass()
         << real4 % ias->chi2() << "\n";

  return StatusCode::SUCCESS;
};

//=============================================================================
// check score
//=============================================================================
StatusCode TestHltScore::checkScore(const HltScore* score) {

  // Nothing yet!

  return StatusCode::SUCCESS;
}
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

  return GaudiAlgorithm::finalize();  // must be called after all other actions
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
