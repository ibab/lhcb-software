// $Id: HltRecCheckGhosts.h,v 1.3 2009-06-02 19:48:17 gligorov Exp $
#ifndef HLTRECCHECKGHOSTS_H 
#define HLTRECCHECKGHOSTS_H 1

// Include files
//#include "HltBase/HltAlgorithm.h"
//#include "HltBase/HltSelectionContainer.h"

#include "MCInterfaces/ITrackGhostClassification.h"
#include "MCInterfaces/ILHCbIDsToMCParticles.h"
#include "Event/GhostTrackInfo.h"
#include "TrackInterfaces/IVeloExpectation.h"

#include "Kernel/LHCbID.h"
#include "Linker/LinkedTo.h"
#include "Event/MCParticle.h"

#include "Event/Particle.h"
#include "Event/Track.h"
#include "Event/TrackParameters.h"
#include "Event/State.h"

#include "GaudiAlg/GaudiTupleAlg.h"
#include "HltBase/HltBase.h"

#include "Event/ODIN.h"

/** @class HltRecCheckGhosts HltRecCheckGhosts.h
 *
 *  A algorithm to save ghost information about tracks
 *  used in the trigger. Please note the following:
 *
 *  -- For the moment it is assumed the the trigger is firing on long ("forward") tracks.
 *     There may eventually be the functionality to deal with other kinds but this use case
 *     is the most pressing right now. The tool does analyse the Velo and T parts of each
 *     long track, but it cannot be used to analyse Velo tracks on their own right now.
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

  typedef std::vector<LHCb::LHCbID> LHCbIDs;

  IHltDataSvc* m_dataSvc;
  ILHCbIDsToMCParticles* m_linkTool;
 
  double m_purityCut;
 
  double zBeamLine(const LHCb::Track& aTrack) const;
  StatusCode classifyParts(const LHCb::Track*);
  bool checkTrackPart(LHCb::Track*);  
  StatusCode writeNtuple(Tuple&);

  //Hlt::SelectionContainer2<LHCb::Track,LHCb::Track> m_selections;

  LHCb::GhostTrackInfo whatKindOfGhostIsHauntingMyDecay(const LHCb::Particle*);
  ITrackGhostClassification* m_ghostToolLong;
  ITrackGhostClassification* m_ghostToolVelo;
  ITrackGhostClassification* m_ghostToolVeloR;
  ITrackGhostClassification* m_ghostToolTTrack;
  ITrackGhostClassification* m_ghostToolUpstream;
  ITrackGhostClassification* m_ghostToolDownstream;

  IVeloExpectation* m_veloExpectation;

  std::string m_inputTrackSelection; 

  std::vector<int> m_nEvent;
  std::vector<int> m_nRun;

  std::vector<int> m_ghostResultsLong;
  std::vector<int> m_isFromBorDforLong;
  std::vector<int> m_ghostClassesLong;
  std::vector<unsigned int> m_nHitsLong;
  std::vector<unsigned int> m_nLinkedLong;
  std::vector<double> m_slopeLong_X;
  std::vector<double> m_slopeLong_Y;
  std::vector<double> m_pLong;
  std::vector<double> m_ptLong;
  std::vector<double> m_etaLong;
  std::vector<double> m_phiLong;
  std::vector<double> m_firstStateLong_X;
  std::vector<double> m_firstStateLong_Y;
  std::vector<double> m_firstStateLong_Z;
  std::vector<double> m_Chi2Long;
  std::vector<int> m_nDoFLong;
  std::vector<unsigned int> m_nStatesLong;
  

  std::vector<int> m_ghostResultsVelo;
  std::vector<int> m_ghostClassesVelo;
  std::vector<unsigned int> m_nHitsVelo;
  std::vector<unsigned int> m_nLinkedVelo;
  std::vector<int> m_nMissedBeforeBeamline;
  std::vector<int> m_nMissedInVelo;

  std::vector<int> m_ghostResultsVeloR;
  std::vector<int> m_ghostClassesVeloR;
  std::vector<unsigned int> m_nHitsVeloR;
  std::vector<unsigned int> m_nLinkedVeloR;

  std::vector<int> m_ghostResultsTTrack;
  std::vector<int> m_ghostClassesTTrack;
  std::vector<unsigned int> m_nHitsTTrack;
  std::vector<unsigned int> m_nLinkedTTrack;

};
#endif // HLTMCMONITOR_H
