// $Id: HltFilterFittedVertices.cpp,v 1.3 2010-08-17 08:47:19 graven Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "boost/foreach.hpp"

// from Event
#include "HltFilterFittedVertices.h"
#include "HltFunctions.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : HltFilterFittedVertices 
//
// 2010-05-14 : V. Gligorov
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltFilterFittedVertices );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltFilterFittedVertices::HltFilterFittedVertices( const std::string& name, ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
  , m_selections(*this)
{
  declareProperty("MinVDCHI2",         m_minVDChi2         = 50.);
  m_selections.declareProperties();
}

//=============================================================================
// Destructor
//=============================================================================
HltFilterFittedVertices::~HltFilterFittedVertices() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltFilterFittedVertices::initialize() {
  
  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  m_selections.retrieveSelections();
  m_selections.registerSelection();

  m_vertexFitter = tool<IVertexFit>("LoKi::FastVertexFitter",this);
  m_dist         = tool<IDistanceCalculator>("LoKi::DistanceCalculator",this);
  m_p2s          = tool<IParticle2State>("Particle2State",this);

  //Initialize the histograms
  histochi2 = initializeHisto("Flight distance Chi2",0.,400.,100); 
  histodist = initializeHisto("Flight distance (mm)",0.,40., 100);  
  histochi2best = initializeHisto("Best flight distance Chi2",0.,400.,100);
  histodistbest = initializeHisto("Best flight distance (mm)",0.,40., 100); 

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltFilterFittedVertices::execute() {

  StatusCode sc = StatusCode::SUCCESS;
  
  m_selections.output()->clean();

  //Lets see what we just did, for debug
  if (msgLevel(MSG::DEBUG)) {
        verbose() << "Printing out the vertices" << endmsg;
        BOOST_FOREACH( const LHCb::RecVertex* iV, *m_selections.input<1>()) {
                verbose() << iV << endmsg;
        }
  }

  double topchi2 = 0.;
  double topfd   = 0.;

  //Do the filtering
  BOOST_FOREACH( const LHCb::RecVertex* iV, *m_selections.input<1>()) {
    //First get the tracks from the vertex
    const LHCb::Track* t1 = iV->tracks()[0];
    const LHCb::Track* t2 = iV->tracks()[1];
    //Make the protos
    LHCb::ProtoParticle protoP1; protoP1.setTrack(t1);
    LHCb::ProtoParticle protoP2; protoP2.setTrack(t2);
    //Make what we need for the vertex fit
    LHCb::ParticleID pidPi(211);
    LHCb::Particle daughter1(pidPi);
    LHCb::Particle daughter2(pidPi);
    daughter1.setMeasuredMass(139.57);
    daughter2.setMeasuredMass(139.57);
    daughter1.setMeasuredMassErr(0);
    daughter2.setMeasuredMassErr(0);
    daughter1.setProto(&protoP1);
    daughter2.setProto(&protoP2);
    const LHCb::State* state1 = t1->stateAt( LHCb::State::ClosestToBeam ); 
    const LHCb::State* state2 = t2->stateAt( LHCb::State::ClosestToBeam );
    sc = m_p2s->state2Particle( *state1, daughter1 );
    sc = m_p2s->state2Particle( *state2, daughter2 );
    //Make the daughters vector
    LHCb::Particle::ConstVector daughters;
    daughters.clear();
    daughters.push_back(&daughter1);
    daughters.push_back(&daughter2);
    //Fit the vertex
    LHCb::Vertex     vertex;
    m_vertexFitter->fit(vertex,daughters);
    //Cut on the vertex
    //If more than one PV, we will require that the minimum value is greater than the cut
    double bestchi2 = 100000.;
    double bestfd   = 100000.;
    BOOST_FOREACH(const  LHCb::RecVertex* iV2, *m_selections.input<2>()) {
      double fd = 0; double chi2 = 0;
      sc = m_dist->distance( iV2, &vertex, fd, chi2 ); 
      // always() << " chi2 = " << chi2 << " : " << Hlt::DS()(*iV,*iV2) << endmsg;
      if (chi2 < bestchi2) {
        bestchi2 = chi2;
        bestfd   = fd  ;
      } 
    }
    //Save or don't
    if (bestchi2 > m_minVDChi2) {
        m_selections.output()->push_back(iV); 
    }
    //Monitor
    if (bestchi2 > topchi2) {
      topchi2 = bestchi2;
    } 
    if (bestfd > topfd) {
      topfd = bestfd;
    }
    //Fill the histograms
    fill(histochi2,bestchi2,1.);
    fill(histodist,bestfd  ,1.);
  }

  fill(histochi2best,topchi2,1.);
  fill(histodistbest,topfd  ,1.);

  if (msgLevel(MSG::DEBUG)) {
        debug() << "About to print out a mountain of crap" << endmsg;
        debug() << "Printing out the output tracks" << endmsg;
        BOOST_FOREACH( const LHCb::RecVertex *iV, *m_selections.output() ) {
                debug() << iV << endmsg;
        }
  }
  
  int ncan = m_selections.output()->size();
  
  debug() << " candidates found " << ncan << endmsg;
  
  return sc;
  
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltFilterFittedVertices::finalize() {

  StatusCode sc = HltAlgorithm::finalize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
  debug() << "==> Finalize" << endmsg;

  return  StatusCode::SUCCESS;
}

