// $Id: LoKi_Test1.cpp,v 1.2 2007/04/16 16:16:42 pkoppenb Exp $
// ============================================================================
// CVS tag $Name: v1r1p1 $, version $Revision: 1.2 $ 
// ============================================================================
// $Log: LoKi_Test1.cpp,v $
// Revision 1.2  2007/04/16 16:16:42  pkoppenb
// removed polemic comment
//
// Revision 1.1.1.1  2006/08/16 17:01:31  ibelyaev
// New package: the first import
// 
// ============================================================================
// Include files 
// ============================================================================
// LoKiAlgo
// ============================================================================
#include "LoKi/LoKiAlgoMC.h"
// ============================================================================
// LoKiPhys 
// ============================================================================
#include "LoKi/LoKiPhys.h"
#include "LoKi/LoKiPhysMC.h"
#include "LoKi/LoKiMC.h"
#include "LoKi/LoKiGenMC.h"
#include "LoKi/Record.h"
#include "Kernel/IParticle2State.h"
#include "bRefit.h"
#include "TrackInterfaces/ITrackFitter.h"
#include "TrackInterfaces/ITrackStateInit.h"
#include "GaudiKernel/AlgFactory.h"
#include "LoKi/Particles.h"
#include "LoKi/GenParticles.h"
#include "LoKi/ParticleProperties.h"
#include "LHCbMath/MatrixUtils.h"
#include "LHCbMath/Kinematics.h"
#include "LoKi/GenParticleCuts.h"
#include "Kernel/ParticleID.h"
#include "TrackInterfaces/ITrackExtrapolator.h"
#include "TrackInterfaces/ITrackVertexer.h"

// Boost
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/assign/std/vector.hpp>
#include <boost/assign/list_of.hpp>

#include "LoKi/select.h"
#include <fstream>
#include <iostream>

//#include "Math/MatrixRepresentationsStatic.h"

#include "TrackKernel/TrackTraj.h"
#include "TrackInterfaces/ITrackManipulator.h"

#include "LHCbMath/MatrixManip.h"

//#include "GaudiAlg/TupleObj.h"

using namespace boost::lambda;

DECLARE_ALGORITHM_FACTORY( bRefit );
// ============================================================================
/** @file 
 *  Implementation file for test algorithm class LoKi_Test1
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @2006-08-16 
 */
// ===========================================================================

bRefit::bRefit(const std::string& name, ISvcLocator* pSvc)
  :DVAlgorithm(name,pSvc)
{

  declareProperty("txScale", m_txScale = 1.2);
  declareProperty("tyScale", m_tyScale = 1.2);
  declareProperty("qpScale", m_qpScale = 1.45);
  declareProperty("scaleErrors", m_scaleErrors = false);
}


bRefit::~bRefit() {}

StatusCode bRefit::initialize() {

  StatusCode sc = DVAlgorithm::initialize();
  if (sc.isFailure()){
    return Warning("Failed to init base class", StatusCode::FAILURE);
  } 

  m_pToState = tool<IParticle2State>("Particle2State");
 
  m_trackFit = tool<ITrackFitter>("TrackMasterFitter", "fit", this);
  // m_trackFit = tool<ITrackFitter>("TrackInitFit", "fit", this);

   m_trackExtrapolator = tool<ITrackExtrapolator>("TrackMasterExtrapolator");

   using namespace Gaudi;
   m_scaleMatrix = TrackMatrix(ROOT::Math::SMatrixIdentity());  
   m_scaleMatrix(2,2)*= m_txScale;
   m_scaleMatrix(3,3)*= m_tyScale;
   m_scaleMatrix(4,4)*= m_qpScale;


  //  m_trackStateInit = tool<ITrackStateInit>( "TrackStateInitTool" );

  return StatusCode::SUCCESS;
}

StatusCode bRefit::finalize() {


  return DVAlgorithm::finalize();
}


StatusCode bRefit::execute(){


 const LHCb::Particle::ConstVector& mothers = desktop()->particles();
 for (LHCb::Particle::ConstVector::const_iterator iterP = mothers.begin(); iterP !=
        mothers.end(); ++iterP){  

   LHCb::Particle* part = const_cast<LHCb::Particle*>(*iterP);

   double zVert = 0.0; 
   const LHCb::Vertex* vert = part->endVertex();
   if (vert){ 
      zVert = vert->position().z();
   } 
   std::vector<LHCb::Particle*> daughters; // get the daughters
   LoKi::Extract::getParticles(part, std::back_inserter(daughters));
   for (std::vector<LHCb::Particle*>::const_iterator iter = daughters.begin(); iter != daughters.end(); ++iter){

      std::vector<LHCb::Particle*> gdaughters; // get the daughters
      if ((*iter)->isBasicParticle() == false){
        LoKi::Extract::getParticles(*iter, std::back_inserter(gdaughters));
      } 
      else {
        gdaughters.push_back(part);   
      }
      refit(daughters, (*iter)->isBasicParticle(), zVert); // refit
      if ((*iter)->isBasicParticle() == false) particleReFitter()->reFit(**iter);

   } // iter
 
   particleReFitter()->reFit(*part);
     
 } // iterP

  

 setFilterPassed(true);
  
 return StatusCode::SUCCESS;
 
}


LHCb::Track* bRefit::track(const LHCb::Particle* part) const{
  std::vector<LHCb::Track*> tracks;
  LoKi::Extract::getTracks(part,std::back_inserter(tracks));
  return (tracks.empty() == false ?  tracks.front(): 0 ); 
}




//double bRefit::refit(std::vector<LHCb::Particle*>& daughters) const{  
void bRefit::refit(std::vector<LHCb::Particle*>& daughters, bool isBasic, double z)const{  
  

 for (std::vector<LHCb::Particle*>::iterator iter = daughters.begin(); 
      iter != daughters.end(); ++iter){
      // then refit

   if ((*iter)->charge() == 0) continue;  
   LHCb::Track* aTrack = track(*iter);
   if (!aTrack) continue;
   //   m_trackStateInit->fit(*aTrack, true) ;
   m_trackFit->fit(*aTrack);
  
   // put it back to an unfitted state
   aTrack->setFitResult(0);
   aTrack->setPatRecStatus(LHCb::Track::PatRecIDs);

   LHCb::State* state;

   
   if (aTrack->type() == LHCb::Track::Long) {
     if (isBasic == true){
       state = aTrack->stateAt(LHCb::State::ClosestToBeam);
     }
     else {
       //std::cout << "propagate " << std::endl;
       state = aTrack->stateAt(LHCb::State::FirstMeasurement);
       m_trackExtrapolator->propagate(*state, z);
       //       LHCb::State* beamState = aTrack->stateAt(LHCb::State::ClosestToBeam);
       //    aTrack->removeFromStates(beamState);
       //    state->setLocation(LHCb::State::ClosestToBeam);
       //aTrack->addToStates(*state);
       
     }
   }
   else {
     state = aTrack->stateAt(LHCb::State::FirstMeasurement);
     if (isBasic == false)  m_trackExtrapolator->propagate(*state, z);
   }
  

   /*
  if (aTrack->type() == LHCb::Track::Long) {
       state = aTrack->stateAt(LHCb::State::ClosestToBeam);
   }
   else {
     state = aTrack->stateAt(LHCb::State::FirstMeasurement);
   }
   */

   


   if (m_scaleErrors == true){
     const std::vector<LHCb::State*>& states = aTrack->states();
     for (std::vector<LHCb::State*>::const_iterator iter = states.begin(); iter != states.end(); ++iter){
       LHCb::State* theState = const_cast<LHCb::State*>(*iter);
       scaleErrors(theState);
       //       std::cout << "Scaling " << theState->location() << std::endl;
     }
   }

   //   std::cout << (*iter)->charge() << (*iter)->particleID() << std::endl;
   m_pToState->state2Particle(*state, **iter);   
 }
}

void bRefit::scaleErrors(LHCb::State* state) const {
 
 using namespace Gaudi::Math;
 Gaudi::TrackSymMatrix& stateCov = state->covariance(); 
 stateCov = Similarity(m_scaleMatrix, stateCov); 
 //Gaudi::TrackSymMatrix newcov = m_scaleMatrix * 
 //state->setCovariance(newCov);
}


  
