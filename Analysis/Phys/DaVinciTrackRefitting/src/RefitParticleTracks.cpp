// $Id: $
#include "GaudiKernel/AlgFactory.h"

#include "RefitParticleTracks.h"

#include "TrackInterfaces/ITrackFitter.h"
#include "TrackInterfaces/ITrackManipulator.h"
#include "Event/Track.h"
#include "LoKi/PhysExtract.h"
#include <boost/foreach.hpp>


//-----------------------------------------------------------------------------
// Implementation file for class : RefitParticleTracks
//
// 2012-11-23 : Paul Seyfert <pseyfert@cern.ch>
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( RefitParticleTracks )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RefitParticleTracks::RefitParticleTracks(const std::string& name, ISvcLocator* pSvcLocator)
  : DaVinciAlgorithm(name, pSvcLocator)
{
  declareProperty("DoFit"              ,m_fit = true);
  declareProperty("DoProbability"      ,m_manipulate = false);
  declareProperty("WriteToTES"         ,m_writeOutput = false);
  declareProperty("UpdateProbability"  ,m_update = false);
  declareProperty("ReplaceTracks"      ,m_overwrite = true);
  declareProperty("Fitter"             ,m_fittername = "TrackInitFit");
  declareProperty("Manipulator"        ,m_manipulatorname = "TrackNNGhostId");
}

//=============================================================================
// Destructor
//=============================================================================
RefitParticleTracks::~RefitParticleTracks() {;}

//=============================================================================
// Initialization
//=============================================================================
StatusCode RefitParticleTracks::initialize()
{
  StatusCode sc = DaVinciAlgorithm::initialize(); // must be executed first
  if (sc.isFailure()) return sc;  // error printed already by DaVinciAlgorithm

  if (!m_overwrite && !m_update && !m_writeOutput) {
    Warning("Neither output nor overwriting/updating specified. Don't know what to do.").ignore();
  }
  if (m_overwrite || m_update) {
    Warning("Don't run this option in the Stripping! (UpdateProbability or ReplaceTracks are evil there.) Please copy your tracks somewhere.").ignore();
  }
  if (!m_manipulate && m_update) {
    return Warning("Not implemented combination (UpdateProbability w/o DoProbability) [1]",StatusCode::FAILURE);
  }
  if (m_overwrite && m_manipulate && !m_update) {
    return Warning("Sorry, I thought about this combination but didn't want to implement it (ReplaceTracks and DoProbability w/o UpdateProbability[2]",StatusCode::FAILURE);
  }
  if (m_overwrite && m_manipulate && m_update) {
    m_update = false; // sorry for this hack. I here attempt to keep the code more readable.[3]
  }

  if ( UNLIKELY(msgLevel(MSG::DEBUG)) ) debug() << "==> Initialize" << endmsg;

  m_trackFitter = tool<ITrackFitter>(m_fittername,this);
  m_trackManipulator = tool<ITrackManipulator>(m_manipulatorname,this);


  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode RefitParticleTracks::execute()
{
  if ( UNLIKELY(msgLevel(MSG::DEBUG)) ) debug() << "==> Execute" << endmsg;

  const LHCb::Particle::Range inputParticles = particles();

  if (0==inputParticles.size()) return StatusCode::SUCCESS;
  if ( UNLIKELY(msgLevel(MSG::DEBUG)) ) verbose() << inputParticles.size() << "input particles" << endmsg;

  std::vector<const LHCb::Track*> alltracks;
  std::vector<LHCb::Track*>::const_iterator iterT;
  LHCb::Tracks::const_iterator iterTs;
  std::vector<const LHCb::Track*>::const_iterator itercT;
  LoKi::Extract::getTracks(inputParticles.begin(), inputParticles.end(), std::back_inserter(alltracks));

  ///std::vector<const LHCb::Particle*> seekers;
  ///BOOST_FOREACH(const LHCb::Particle* part, inputParticles) {
  ///  seekers.clear();
  ///  seekers.push_back(part);
  ///  while(seekers.size()) {
  ///    const LHCb::Particle* s = seekers.back();
  ///    seekers.pop_back() ;
  ///    if (s->proto() && s->proto()->track()) {
  ///      alltracks.push_back(s->proto()->track());
  ///    } else {
  ///      BOOST_FOREACH(const LHCb::Particle* ss, s->daughtersVector()) { 
  ///        seekers.push_back(ss);
  ///      }
  ///    }
  ///  }
  ///}


  LHCb::Tracks* uniqueTracks = new LHCb::Tracks();
  if ( UNLIKELY(msgLevel(MSG::DEBUG)) && inputParticles.size()>0 && 0==alltracks.size())
    verbose() << "no tracks from " << inputParticles.size() << " particles " << endmsg;

  itercT = alltracks.begin();
  uniqueTracks->reserve(alltracks.size());
  for ( ; alltracks.end() != itercT ; ++itercT ) {
    if (!uniqueTracks->object((*itercT)->key())) {
      uniqueTracks->add(const_cast<LHCb::Track*>(*itercT));  ///@todo: this is evil
    }
  }

  std::vector<LHCb::Track*> incasts;
  std::vector<LHCb::Track*> copies;

  std::vector<LHCb::Track*>* toworkwith;


  incasts.reserve(uniqueTracks->size());
  //if (m_overwrite || m_update) 
  iterTs = uniqueTracks->begin();
  for ( ; uniqueTracks->end() != iterTs ; ++iterTs ) {
    incasts.push_back( const_cast<LHCb::Track*>(*iterTs) );
  }


  /// @todo: check if necessary
  iterTs = uniqueTracks->begin();
  for ( ; uniqueTracks->end() != iterTs ; ++iterTs ) {
    copies.push_back( (*iterTs)->cloneWithKey() );
  }

  if (m_overwrite) {
    toworkwith = &incasts;
  } else {
    toworkwith = &copies;
  }

  if (m_fit) fit(toworkwith);
  if (m_manipulate) manipulate(toworkwith);

  if (m_update) { /// if we anyhow overwrite, the property has been switched in initialize
    std::vector<LHCb::Track*>::const_iterator iterFrom = copies.begin();
    std::vector<LHCb::Track*>::const_iterator iterTo   = toworkwith->begin();
    for ( ; copies.end() != iterFrom ; ++iterFrom) {
      ++iterTo;
      /* do the copying */
    }
  }

  if (m_writeOutput) {
    LHCb::Tracks* outtracks = new LHCb::Tracks();
    iterT = toworkwith->begin();
      for ( ; toworkwith->end() != iterT ; ++iterT) {
      outtracks->add(*iterT);
    }
    put(outtracks,outputLocation());
  }

  setFilterPassed(true);
  return StatusCode::SUCCESS;
}

StatusCode RefitParticleTracks::fit(std::vector<LHCb::Track*> *vec) {
  if ( UNLIKELY(msgLevel(MSG::DEBUG)) ) debug() << "==> FITTING " << vec->size() << " tracks" << endmsg;

  std::vector<LHCb::Track*>::iterator iterT = vec->begin();
  for ( ; vec->end() != iterT ; ++iterT ) {
    m_trackFitter->fit(*(*iterT)).ignore();
  }
  return StatusCode::SUCCESS;
}

StatusCode RefitParticleTracks::manipulate(std::vector<LHCb::Track*> *vec) {
  if ( UNLIKELY(msgLevel(MSG::DEBUG)) ) debug() << "==> MANIPULATING" << endmsg;
  std::vector<LHCb::Track*>::iterator iterT = vec->begin();
  for ( ; vec->end() != iterT ; ++iterT ) {
    m_trackManipulator->execute(*(*iterT)).ignore();
  }
  return StatusCode::SUCCESS;
}

////=============================================================================
//// Finalize
////=============================================================================
//StatusCode RefitParticleTracks::finalize()
//{
//  if ( UNLIKELY(msgLevel(MSG::DEBUG)) ) debug() << "==> Finalize" << endmsg;
//
//
//  return DaVinciAlgorithm::finalize();  // must be called after all other actions
//}


