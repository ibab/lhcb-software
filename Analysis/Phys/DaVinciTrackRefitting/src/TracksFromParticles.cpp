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
  :DaVinciAlgorithm(name,pSvc)
{
  declareProperty("inputLocation",m_outputLocation = "/Event/Rec/MyTracks");
///@fixme: "inputLocation" as property for m_outputLocation ???
}


TracksFromParticles::~TracksFromParticles() {}

StatusCode TracksFromParticles::execute(){

  // get the input particles
 const LHCb::Particle::Range inputParticles = particles();
 std::vector<LHCb::Track*> tracks;
 LoKi::Extract::getTracks(inputParticles.begin(), inputParticles.end(), std::back_inserter(tracks));
 

 if ( UNLIKELY( msgLevel(MSG::VERBOSE) ) ) {
   verbose() << " Found  " << tracks.size() << endmsg;
 }

 // now clone the tracks and output
 LHCb::Tracks* outputCont = new LHCb::Tracks();
 std::vector<LHCb::Track*>::iterator iterT = tracks.begin();
 for (; iterT != tracks.end(); ++iterT ){
   if (!outputCont->object((*iterT)->key())) {
     // clone track only if it's not a duplicate from another candidate
     LHCb::Track* newTrack = (*iterT)->cloneWithKey();
     outputCont->insert(newTrack);
   }
 } // iterT

 put(outputCont,m_outputLocation); 


 setFilterPassed(true);
  
 return StatusCode::SUCCESS;
 
}



  
