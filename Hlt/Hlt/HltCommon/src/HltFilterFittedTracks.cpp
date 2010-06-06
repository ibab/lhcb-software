// $Id: HltFilterFittedTracks.cpp,v 1.4 2010-06-06 19:18:21 graven Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "boost/foreach.hpp"

//#include "HltBase/HltUtils.h"
#include "HltFilterFittedTracks.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : HltFilterFittedTracks 
//
// 2010-05-14 : V. Gligorov
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltFilterFittedTracks );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltFilterFittedTracks::HltFilterFittedTracks( const std::string& name, ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
  , m_selections(*this)
{
  declareProperty("MinIPCHI2",         m_minIPChi2         = 25.);
  m_selections.declareProperties();
}

//=============================================================================
// Destructor
//=============================================================================
HltFilterFittedTracks::~HltFilterFittedTracks() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltFilterFittedTracks::initialize() {
  
  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  m_selections.retrieveSelections();
  m_selections.registerSelection();

  m_dist         = tool<IDistanceCalculator>("LoKi::DistanceCalculator",this);
  m_p2s          = tool<IParticle2State>("Particle2State",this);

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
StatusCode HltFilterFittedTracks::execute() {

  StatusCode sc = StatusCode::SUCCESS;
  
  m_selections.output()->clean();

  //Lets see what we just did, for debug
  if (msgLevel(MSG::DEBUG)) {
        verbose() << "Printing out the vertices" << endmsg;
        BOOST_FOREACH( LHCb::Track* iT, *m_selections.input<1>()) {
                verbose() << iT << endmsg;
        }
  }

  double topchi2 = 0.;
  double topip   = 0.;

  //Do the filtering
  BOOST_FOREACH( LHCb::Track* iT, *m_selections.input<1>()) {
    //Make the protos
    LHCb::ProtoParticle protoP1; protoP1.setTrack(iT);
    //Make the particle
    LHCb::ParticleID pidPi(211);
    LHCb::Particle daughter1(pidPi);
    daughter1.setMeasuredMass(139.57);
    daughter1.setMeasuredMassErr(0);
    daughter1.setProto(&protoP1);
    const LHCb::State* state1 = iT->stateAt( LHCb::State::ClosestToBeam ); 
    sc = m_p2s->state2Particle( *state1, daughter1 );
    //Cut on the IP
    //If more than one PV, we will require that the minimum value is greater than the cut
    double bestchi2 = 100000.;
    double bestip   = 100000.;
    BOOST_FOREACH( LHCb::RecVertex* iV, *m_selections.input<2>()) {
      double ip = 0; double chi2 = 0;
      sc = m_dist->distance( &daughter1, iV, ip, chi2 ); 
      //double ips =  HltUtils::impactParameterSignificance(*iV,*iT) ;
      //always() << ip << " : " << chi2 << " : " << ips*ips << endmsg;
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
        BOOST_FOREACH( LHCb::Track *iT, *m_selections.output() ) {
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
StatusCode HltFilterFittedTracks::finalize() {

  StatusCode sc = HltAlgorithm::finalize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
  debug() << "==> Finalize" << endmsg;

  return  StatusCode::SUCCESS;
}

