// $Id: TrackCheckerNT.h,v 1.4 2007-10-22 09:45:51 mschille Exp $
#ifndef TRACKCHECKERNT_H 
#define TRACKCHECKERNT_H 1

// Include files

// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"
#include "GaudiAlg/Tuples.h"

// from Kernel/MCInterfaces
//#include "MCInterfaces/ITrackCriteriaSelector.h"
#include "MCInterfaces/IIdealStateCreator.h"

// from Tr/TrackInterfaces
#include "TrackInterfaces/ITrackExtrapolator.h"
#include "TrackInterfaces/ITrackProjectorSelector.h"
#include "TrackInterfaces/ITrackProjector.h"

// from Event/LinkerEvent
#include "Linker/LinkedTo.h"
#include "Linker/LinkedFrom.h"

// from Event/TrackEvent
#include "Event/Track.h"

// from Event/MCEvent
#include "Event/MCParticle.h"

// Forward declarations
namespace LHCb
{
  class State;
  class Measurement;
  class STCluster;
  class OTTime;
}

class ITrajPoca;
class IMagneticFieldSvc;

/** @class TrackCheckerNT TrackCheckerNT.h
 *  
 *  This class writes out an Ntuple which can be used to check Tracks.
 *  This includes data which can be used to plot
 *  properties like resolutions, hit and track efficiencies,
 *  ghost rates (track averaged and event averaged) etc.
 *  This algorithm uses the TrackAssociator from Track to MCParticle.
 *  The algorithm is seperate from TrackChecker to facilitate the
 *  analysis of the results by saving the whole fit and extrapolation
 *  business when running on the same data but producing slightly
 *  different plots (e.g. different momentum range).
 *
 *  Based on TrackChecker (history see below), initial release of NT(uple) version
 *  @author Manuel Schiller
 *  @date 2006-11-16
 * 
 *  Adapted to the new Track Event Model, now using Linkers
 *  @author Edwin Bos
 *  @date   2005-11-14
 *
 *  Based on the Tr/TrCheck/TrChecker code by :
 *  @author Jeroen van Tilburg
 *  @date   2003-07-07
 *
 */

class TrackCheckerNT : public GaudiTupleAlg {

public:

  // Standard constructor
  TrackCheckerNT( const std::string& name, ISvcLocator* pSvcLocator );

  // Destructor
  virtual ~TrackCheckerNT();

  // Algorithm initialization
  virtual StatusCode initialize();

  // Algorithm execution
  virtual StatusCode execute();

  // Algorithm finalization
  virtual StatusCode finalize();

private:

  // the following is there for internal use
  // (I suspect we won't care for the value of the stereo angle
  // in our ntuple analysis macros...)
  enum MeasType {
	  X = 1,	// x measurements	  
	  U = 2,	// u measurements
	  V = 3,	// v measurements
	  R = 4,	// r measurements
	  Phi = 5,	// phi measurements
	  Unknown = -1	// unknown measurements
  };	  

  // Helper functions
  StatusCode resolutionHistos( LHCb::Track* track, LHCb::MCParticle* mcPart );
  std::string measType( unsigned int type );
  
  // NT version helper functions
  void fillTrackState(std::vector<float>& statevec, LHCb::State &st);
  MeasType getMeasType(const LHCb::Measurement* m);
  MeasType getMeasType(const LHCb::STCluster* stc);
  MeasType getMeasType(const LHCb::OTTime* ott);
  LHCb::MCParticle* getMCParticle(const LHCb::Measurement *mm);
  StatusCode fillGlobalTrackParameters(
      LHCb::Tracks *tracks,
      std::map<LHCb::MCParticle*, int>& mcPartIdx,
      LinkedTo<LHCb::MCParticle, LHCb::Track>& directLink,
      Tuples::Tuple& tuple);
  int convertMeasurementToID(const LHCb::Measurement *m);
  void fillDetTrackParametersAtMeasurements(
      int arrmax,
      unsigned int type,
      LHCb::Tracks* tracks,
      std::map<LHCb::MCParticle*, int>& mcPartIdx,
      LinkedTo<LHCb::MCParticle, LHCb::Track>& directLink,
      Tuples::Tuple& tuple);
  StatusCode fillHitPurEff(
      Tuples::Tuple& tuple,
      LHCb::Tracks* tracks,
      LinkedTo<LHCb::MCParticle, LHCb::Track>& directLink);
  StatusCode fillMCGeneralInfo(
      Tuples::Tuple& tuple,
      std::map<LHCb::Track*, int>& trackIdx,
      std::map<LHCb::MCParticle*, int>& mcPartIdx,
      LHCb::MCParticles* particles,
      LinkedFrom<LHCb::Track,LHCb::MCParticle>& revLink);
  StatusCode TrackCheckerNT::fillMCHitStatistics(
      Tuples::Tuple& tuple,
      LHCb::MCParticles* particles);
 
  // Interfaces
  IIdealStateCreator*      m_stateCreator; // Pointer to IdealStateCreator
  ITrackExtrapolator*      m_extrapolator; // Pointer to TrackMasterExtrapolator
  ITrackProjectorSelector* m_projectorSelector;    // Pointer to Projector selection tool

  // jobOptions
  std::string m_tracksInContainer; // Input Tracks container location
  std::string m_linkerInTable;     // Input Linker table location
  // Additional z-positions where to determine resolutions and pulls
  std::vector<double> m_zPositions;

  // Counters
  int m_nMCEvt;             // Total # of Events containing selected MCParticles
  int m_nEvt;               // Total # of Events containing selected Tracks

  long m_maxTracks;		// # of tracks per event to save to NTuple
  long m_maxMCParticles;	// # of MCParticles per event to save to NTuple

};
#endif // TRACKCHECKERNT_H
