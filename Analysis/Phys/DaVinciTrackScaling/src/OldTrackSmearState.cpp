// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "Kernel/ILHCbMagnetSvc.h"

// track interfaces
#include "Event/Track.h"
#include "Event/State.h"

#include "OldTrackSmearState.h"
#include <TFile.h>
#include <TGraphErrors.h>

#include "Event/ODIN.h"


#include "GaudiKernel/RndmGenerators.h"
   


DECLARE_ALGORITHM_FACTORY( OldTrackSmearState )

OldTrackSmearState::OldTrackSmearState(const std::string& name,
                       ISvcLocator* pSvcLocator):
  GaudiAlgorithm(name, pSvcLocator),
  m_graph(0)
{
  // constructor

  this->declareProperty("fudgeFactor",m_fudgeFactor = 1.0); 
  this->declareProperty("inputRootFile",m_inputRootFile = "smear.root" );
  this->declareProperty("inputLocation", m_inputLocation = LHCb::TrackLocation::Default);
  this->declareProperty("year",m_year = "2011");
  this->declareProperty("smearAngle", m_smearAngle = 2e-5);
 
 
}

OldTrackSmearState::~OldTrackSmearState()
{
  //
}

StatusCode OldTrackSmearState::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  sc = randSvc()->generator(Rndm::Gauss(0.,1.),m_gaussDist.pRef());

  // get the histograms and make the object for scaling the momentum
  TFile* histos = new TFile(m_inputRootFile.c_str());
  if (!histos){
     return Warning("Failed to init base class: no histo file ", StatusCode::FAILURE);
  }

  m_graph = (TGraphErrors*) histos->Get("res;1");
  if (!m_graph) {
    return Warning("Failed to find graph", StatusCode::FAILURE);
  }


  return StatusCode::SUCCESS;
}

StatusCode OldTrackSmearState::execute(){

  // get the tracks to scale
  LHCb::Tracks* trackCont = get<LHCb::Tracks>(m_inputLocation);

  // loop and do the scaling
  for (LHCb::Tracks::iterator iterT = trackCont->begin(); iterT != trackCont->end(); ++iterT){
    const std::vector<LHCb::State*>& theStates = (*iterT)->states();
    double q =  (*iterT)->charge();
    double rndmVal = m_gaussDist->shoot();
    //double smearAngleX = m_gaussDist->shoot()*m_smearAngle;
    // double smearAngleY = m_gaussDist->shoot()*m_smearAngle;

    for (std::vector<LHCb::State*>::const_iterator iterState = theStates.begin(); iterState != theStates.end(); ++iterState ) {
      double p = (*iterState)->p();
      double smear = m_graph->Eval(p)*p *m_fudgeFactor; //graph is dp/p
      double smearedVal =p + smear*rndmVal; 
      (*iterState)->setQOverP(q/smearedVal);
      //double tx = (*iterState)->tx();
      //  double ty = (*iterState)->ty();
//      (*iterState)->setTx(tx + smearAngleX);
//     (*iterState)->setTy(ty + smearAngleY);
      //      std::cout << " smear value " << smear << " "  <<  " p " << p  <<  " " << smearedVal << std::endl;   
    }//loop over the states   
  } // iterT
   
  return StatusCode::SUCCESS;
}

