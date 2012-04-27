#include "TracksFromParticles.h"
#include "GaudiKernel/AlgFactory.h"
#include "Event/Track.h"
#include "Event/Particle.h"
#include "LoKi/PhysExtract.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TracksFromParticles
//
// 2012-10-27 : Matthew Needham   
//-----------------------------------------------------------------------------


DECLARE_ALGORITHM_FACTORY( TracksFromParticles );

TracksFromParticles::TracksFromParticles(const std::string& name, ISvcLocator* pSvc)
  :DVAlgorithm(name,pSvc)
{
  declareProperty("inputLocation",m_outputLocation = "/Event/Rec/MyTracks");
}


TracksFromParticles::~TracksFromParticles() {}

StatusCode TracksFromParticles::execute(){

  // get the input particles
 const LHCb::Particle::Range inputParticles = particles();
 std::vector<LHCb::Track*> tracks;
 LoKi::Extract::getTracks(inputParticles.begin(), inputParticles.end(), std::back_inserter(tracks));
 

 verbose() << " Found  " << tracks.size() << endreq;

 // now clone the tracks and output
 LHCb::Tracks* outputCont = new LHCb::Tracks();
 std::vector<LHCb::Track*>::iterator iterT = tracks.begin();
 for (; iterT != tracks.end(); ++iterT ){
   LHCb::Track* newTrack = (*iterT)->cloneWithKey();
   if (!outputCont->object(newTrack->key())) {
     outputCont->insert(newTrack);
   }
   else {
     // duplicate from another candidate
     delete newTrack;
   }
 } // iterT

 put(outputCont,m_outputLocation); 


 setFilterPassed(true);
  
 return StatusCode::SUCCESS;
 
}



  
