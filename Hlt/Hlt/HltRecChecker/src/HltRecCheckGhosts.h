// $Id: HltRecCheckGhosts.h,v 1.1 2009-01-30 18:55:59 gligorov Exp $
#ifndef HLTRECCHECKGHOSTS_H 
#define HLTRECCHECKGHOSTS_H 1

// Include files
//#include "HltBase/HltAlgorithm.h"
//#include "HltBase/HltSelectionContainer.h"

#include "MCInterfaces/ITrackGhostClassification.h"
#include "MCInterfaces/ILHCbIDsToMCParticles.h"
#include "Event/GhostTrackInfo.h"

#include "Event/Particle.h"
#include "Event/Track.h"

#include "GaudiAlg/GaudiTupleAlg.h"
#include "HltBase/HltBase.h"

/** @class HltRecCheckGhosts HltRecCheckGhosts.h
 *
 *  A algorithm to save ghost information about tracks
 *  which caused the trigger to fire. Please note the following:
 *
 *  -- For the moment it is assumed tha the trigger is firing on long ("forward") tracks.
 *     There will eventually be the functionality to deal with other kinds but this use case
 *     is the most pressing right now.  
 *
 *  -- The algorithm takes one input, m_inputTrackSelection, which is the name of the container
 *     where the tracks are which you wish to run over. This will just
 *     be the name of the selection in question.
 *
 *  -- If you want to run over particles output by an Hlt2 Selection, you will need to use an
 *     instance of the HltTrackFromParticle algorithm to convert from the particle to the tracks.
 *
 *  @author V. Gligorov
 *  @date   2009-01-26
 */
//typedef std::vector<const LHCb::Particle*> ParticleVector;

class HltRecCheckGhosts : public GaudiTupleAlg {
public: 
  /// Standard constructor
  HltRecCheckGhosts( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltRecCheckGhosts( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:

  IHltDataSvc* m_dataSvc;

  StatusCode classifyParts(const LHCb::Track*);
  StatusCode writeNtuple(Tuple&);

  //Hlt::SelectionContainer2<LHCb::Track,LHCb::Track> m_selections;

  LHCb::GhostTrackInfo whatKindOfGhostIsHauntingMyDecay(const LHCb::Particle*);
  ITrackGhostClassification* m_ghostToolLong;
  ITrackGhostClassification* m_ghostToolVelo;
  ITrackGhostClassification* m_ghostToolVeloR;
  ITrackGhostClassification* m_ghostToolTTrack;
  ITrackGhostClassification* m_ghostToolUpstream;
  ITrackGhostClassification* m_ghostToolDownstream;

  std::string m_inputTrackSelection; 

  std::vector<int> m_ghostResultsLong;
  std::vector<int> m_ghostClassesLong;
  std::vector<int> m_ghostResultsVelo;
  std::vector<int> m_ghostClassesVelo;
  std::vector<int> m_ghostResultsVeloR;
  std::vector<int> m_ghostClassesVeloR;
  std::vector<int> m_ghostResultsTTrack;
  std::vector<int> m_ghostClassesTTrack;

};
#endif // HLTMCMONITOR_H
