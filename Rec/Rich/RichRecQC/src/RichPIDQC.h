// $Id: RichPIDQC.h,v 1.4 2003-10-13 16:12:42 jonrob Exp $
#ifndef RICHRECQC_RICHPIDQC_H
#define RICHRECQC_RICHPIDQC_H 1

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"

// Relations
#include "Relations/IAssociatorWeighted.h"
#include "Relations/IRelationWeighted.h"

// RichRecBase
#include "RichRecBase/RichTrackSelector.h"

// Event model
#include "Event/RichPID.h"
#include "Event/TrStoredTrack.h"
#include "Event/MCParticle.h"

// RICH kernel
#include "RichKernel/MessageSvcStl.h"

// temporary histogramming numbers
#include "RichRecBase/RichDetParams.h"

// Histogramming
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"

// CLHEP
#include "CLHEP/Units/PhysicalConstants.h"


// Forward Declarations
class IHistogram1D;
class IHistogram2D;
class IParticlePropertySvc;

/** @class RichPIDQC RichPIDQC.h
 *
 *  Quality control monitor for RichPIDs
 *
 *  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
 *  @date   2002-06-13
 */

// tracking MC truth
typedef IAssociatorWeighted<TrStoredTrack,MCParticle,double> TrackFitAsct ;

class RichPIDQC : public Algorithm {

public:

  /// Standard constructor
  RichPIDQC( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~RichPIDQC( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private: // methods

  /// Loads the PID data from configured location
  StatusCode loadPIDData();

  /// Loads the track data from configured location
  StatusCode loadTrackData();

  /// Book histograms
  StatusCode bookHistograms();

  /// Book MCTruth histograms
  StatusCode bookMCHistograms();

  /// perform c++ gymnastics to get momentum state
  TrStateP * getTrStateP( const TrStoredTrack * track,
                          const double zPos = -999999 );

private: // data

  // Pointer to base contianer
  std::vector<ContainedObject*> m_richPIDs;

  /// Pointer to container of TrStoredTracks
  TrStoredTracks * m_tracks;

  // job options
  std::string m_tkMCTruthName;   ///< Track MC truth relation name
  std::string m_tkMCTruthType;   ///< Track MC truth relation type
  std::string m_mcHstPth;        ///< Output MC truth histogram path
  std::string m_hstPth;          ///< Output histogram path
  std::string m_pidTDS;          ///< Location of target RichPIDs in TDS
  std::string m_trackTDS;        ///< location of tracks in TDS
  double m_pMinCut;              ///< minimum momentum cut
  double m_pMaxCut;              ///< maximum momentum cut
  int m_minMultCut;              ///< Minimum track multiplicity
  int m_maxMultCut;              ///< Maximum track multiplicity
  bool m_truth;                  ///< MCTruth available
  bool m_doHistos;               ///< Flag controlling the creation of histograms
  int m_bins;                    ///< Number of bins
  bool m_finalPrintOut;          ///< Perform final prinout of PID tables
  bool m_extraHistos;            ///< Fill full set of histograms
  RichTrackSelector m_trSelector;  ///< Track selector
  TrackFitAsct* m_trackToMCP;    ///< Track MCTruth
  std::map<int,Rich::ParticleIDType> m_localID;   ///< Local PID mapping

  // Summary information
  double m_sumTab[6][6];
  int m_nEvents[2];
  int m_nTracks[2];

  // Histograms

  IHistogram1D* m_ids;           ///< Particle identifications per mass hypothesis
  IHistogram1D* m_Nids;          ///< Number of particle identifications per event

  IHistogram1D* m_pRaw[Rich::NParticleTypes];     ///< Particle ID raw probability
  IHistogram1D* m_pNorm[Rich::NParticleTypes];    ///< Particle ID normalised probability
  IHistogram1D* m_deltaLL[Rich::NParticleTypes];  ///< Delta Log-Likelihood
  IHistogram1D* m_deltaLLTrue[Rich::NParticleTypes];  ///< Delta Log-Likelihood true particle hypothesis
  IHistogram1D* m_deltaLLFake[Rich::NParticleTypes];  ///< Delta Log-Likelihood fakeparticle hypothesis
  
  /// dll between types : True type
  IHistogram1D* m_dLLTrue[Rich::NParticleTypes][Rich::NParticleTypes];
  /// dll between types : Fake type
  IHistogram1D* m_dLLFalse[Rich::NParticleTypes][Rich::NParticleTypes];

  /// Momentum spectrum for IDed type versus true type
  IHistogram1D* m_ptotSpec[Rich::NParticleTypes+1][Rich::NParticleTypes+1];

  IHistogram2D* m_perfTable;        ///< Overall PID performance table

  IHistogram1D* m_pidRate;          ///< Fraction of selected tracks with PID results
  IHistogram1D* m_eventRate;        ///< Events with/without PID results

};
#endif // RICHRECQC_RICHPIDQC_H
