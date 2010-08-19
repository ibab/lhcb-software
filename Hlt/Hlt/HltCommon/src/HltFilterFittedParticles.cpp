// $Id: HltFilterFittedParticles.cpp,v 1.1 2010-08-19 09:34:40 gligorov Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "boost/foreach.hpp"

//#include "HltBase/HltUtils.h"
#include "HltFilterFittedParticles.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : HltFilterFittedParticles 
//
// 2010-05-14 : V. Gligorov
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltFilterFittedParticles );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltFilterFittedParticles::HltFilterFittedParticles( const std::string& name, ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
  , m_selections(*this)
{
  declareProperty("MinIPCHI2",         m_minIPChi2         = 25.);
  m_selections.declareProperties();
}

//=============================================================================
// Destructor
//=============================================================================
HltFilterFittedParticles::~HltFilterFittedParticles() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltFilterFittedParticles::initialize() {
  
  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  m_selections.retrieveSelections();
  m_selections.registerSelection();

  m_dist         = tool<IDistanceCalculator>("LoKi::DistanceCalculator",this);

  //Initialize the histograms
  histochi2 = initializeHisto("IP Chi2",0.,100.,100); 
  histoip = initializeHisto("IP (mm)",0.,4., 100);  
  histochi2best = initializeHisto("Best IP Chi2",0.,100.,100);
  histoipbest = initializeHisto("Best IP (mm)",0.,4., 100); 

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltFilterFittedParticles::execute() {

  StatusCode sc = StatusCode::SUCCESS;
  
  m_selections.output()->clean();

  //Lets see what we just did, for debug
  if (msgLevel(MSG::DEBUG)) {
        verbose() << "Printing out the vertices" << endmsg;
        BOOST_FOREACH(const LHCb::Particle* iT, *m_selections.input<1>()) {
                verbose() << iT << endmsg;
        }
  }

  double topchi2 = 0.;
  double topip   = 0.;

  //Do the filtering
  BOOST_FOREACH(Hlt::TSelection<LHCb::Particle>::candidate_type* iT, *m_selections.input<1>()) {
    double bestchi2 = 100000.;
    double bestip   = 100000.;
    BOOST_FOREACH(const LHCb::RecVertex* iV, *m_selections.input<2>()) {
      double ip = 0; double chi2 = 0;
      sc = m_dist->distance( iT, iV, ip, chi2 ); 
      if (chi2 < bestchi2) {
        bestchi2 = chi2;
        bestip   = ip  ;
      } 
    }
    //Save or don't
    if (bestchi2 > m_minIPChi2) {
        m_selections.output()->push_back(iT); 
    }
    //Monitor
    if (bestchi2 > topchi2) {
      topchi2 = bestchi2;
    } 
    if (bestip > topip) {
      topip = bestip;
    }
    //Fill the histograms
    fill(histochi2,bestchi2,1.);
    fill(histoip,  bestip  ,1.);
  }

  fill(histochi2best,topchi2,1.);
  fill(histoipbest,  topip  ,1.);

  if (msgLevel(MSG::DEBUG)) {
        debug() << "About to print out a mountain of crap" << endmsg;
        debug() << "Printing out the output tracks" << endmsg;
        BOOST_FOREACH(const LHCb::Particle *iT, *m_selections.output() ) {
                debug() << iT << endmsg;
        }
  }
  
  int ncan = m_selections.output()->size();
  
  debug() << " candidates found " << ncan << endmsg;
  
  return sc;
  
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltFilterFittedParticles::finalize() {

  StatusCode sc = HltAlgorithm::finalize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
  debug() << "==> Finalize" << endmsg;

  return  StatusCode::SUCCESS;
}

