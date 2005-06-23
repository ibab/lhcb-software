
//-----------------------------------------------------------------------------
/** @file RichPhotonMonitor.h
 *
 *  Header file for algorithm class : RichPhotonMonitor
 *
 *  CVS Log :-
 *  $Id: RichPhotonMonitor.h,v 1.2 2005-06-23 15:14:55 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECMONITOR_RICHPHOTONMONITOR_H
#define RICHRECMONITOR_RICHPHOTONMONITOR_H 1

// base class
#include "RichRecBase/RichRecAlgBase.h"

// temporary histogramming numbers
#include "RichRecBase/RichDetParams.h"

// rec helpers
#include "RichRecBase/RichTrackSelector.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// Relations
#include "Relations/IAssociatorWeighted.h"
#include "Relations/IRelationWeighted.h"

// Event
#include "Event/RichRecStatus.h"
#include "Event/MCRichOpticalPhoton.h"
#include "Event/MCRichTrack.h"
#include "Event/MCRichSegment.h"

// Histogramming
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"

// Interfaces
#include "RichRecBase/IRichRecMCTruthTool.h"
#include "RichRecBase/IRichParticleProperties.h"
#include "RichRecBase/IRichPhotonSignal.h"
#include "RichRecBase/IRichExpectedTrackSignal.h"
#include "RichRecBase/IRichGeomEff.h"
#include "RichRecBase/IRichCherenkovAngle.h"
#include "RichRecBase/IRichRecGeomTool.h"
#include "RichKernel/IRichMCTrackInfoTool.h"
#include "RichKernel/IRichRefractiveIndex.h"

//---------------------------------------------------------------------------
/** @class RichPhotonMonitor RichPhotonMonitor.h
 *
 *  Monitor for the RICH photon reconstruction.
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2002-07-02
 */
//---------------------------------------------------------------------------

class RichPhotonMonitor : public RichRecAlgBase {

public:

  /// Standard constructor
  RichPhotonMonitor( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~RichPhotonMonitor( ); ///< Destructor

  virtual StatusCode initialize();    // Algorithm initialization
  virtual StatusCode execute   ();    // Algorithm execution
  virtual StatusCode finalize  ();    // Algorithm finalization

private: // methods

  /// Book histograms
  StatusCode bookHistograms();

  /// Book MCTruth histograms
  StatusCode bookMCHistograms();

  /// Book on demand reco photon energy distribution plots
  inline IHistogram1D * 
  bookRecPhotonHist ( const Rich::ParticleIDType hypo, ///< The mass hypothesis
                      const Rich::RadiatorType rad,    ///< The radiator
                      const RichPhotonSpectra<RichRecSegment::FloatType> & spec ///< Energy distribution
                      )
  {
    if ( !m_recPhotEnAtProd[rad][hypo] ) {
      RADIATOR_NAMES;
      HYPOTHESIS_NAMES;
      RAD_HISTO_OFFSET;
      HYPO_HISTO_OFFSET;
      const std::string title = "Rec Photon En at prod. (eV) : "+hypothesis[hypo]+" "+radiator[rad];
      m_recPhotEnAtProd[rad][hypo] =
        histoSvc()->book(m_histPth,radOffset*(rad+1) + 7 + hypoOffset*(hypo+1),title,
                         spec.energyBins(),spec.minEnergy(),spec.maxEnergy());
    }
    return m_recPhotEnAtProd[rad][hypo];
  }

private: // data

  const IRichPhotonSignal * m_photonSig; ///< Pointer to RichPhotonSignal tool
  const IRichExpectedTrackSignal * m_tkSignal; ///< Pointer to RichExpectedTrackSignal tool
  const IRichGeomEff * m_geomEfficRay; ///< Pointer to ray-tracing RichGeomEff tool
  const IRichGeomEff * m_geomEfficFix; ///< Pointer to fixed RichGeomEff tool
  const IRichRecMCTruthTool* m_richRecMCTruth; ///< Pointer to RichRecMCTruthTool interface
  const IRichCherenkovAngle * m_ckAngle; ///< Pointer to RichCherenkovAngle tool
  const IRichParticleProperties * m_richPartProp; ///< Rich Particle properties
  const IRichMCTrackInfoTool * m_mcTkInfo; ///< MC track information
  const IRichRefractiveIndex * m_refIndex; ///< Refractive index tool
  const IRichRecGeomTool * m_geomTool; ///< Simple geometrical questions
  const IRichPhotonCreator * m_forcedPhotCreator;

  /// Track selector
  RichTrackSelector m_trSelector;

  // job options
  std::string m_mcHistPth; ///< Output MC truth histogram path
  std::string m_histPth;   ///< Output histogram path
  int m_maxUsedTracks;     ///< Maximum number of tracks
  bool m_truth;            ///< Flag to turn on MC truth histograms
  bool m_buildPhotons;     ///< Flag to turn on/off the reconstruction of all photons if none are present

  // Tally max/min hit seperations
  //double m_maxTrueHitSepG[Rich::NRadiatorTypes][Rich::Track::NTrTypes];
  //double m_minTrueHitSepG[Rich::NRadiatorTypes][Rich::Track::NTrTypes];
  double m_maxTrueHitSepL[Rich::NRadiatorTypes][Rich::Track::NTrTypes];
  double m_minTrueHitSepL[Rich::NRadiatorTypes][Rich::Track::NTrTypes];

  // tally max/min photon energies
  double m_maxPhotEnAtProd[Rich::NRadiatorTypes];
  double m_minPhotEnAtProd[Rich::NRadiatorTypes];

  // Histograms
  IHistogram1D* m_ckTheta[Rich::NRadiatorTypes];            ///< Cherenkov Theta
  IHistogram1D* m_ckTrueRecTheta[Rich::NRadiatorTypes];        ///< True Rec Cherenkov Theta
  IHistogram1D* m_ckTrueTheta[Rich::NRadiatorTypes];        ///< True Rec Cherenkov Theta
  IHistogram2D* m_ckTruePhiVThetaR0[Rich::NRadiatorTypes];    ///< Cherenkov Theta versus Cherenkov Phi - Region 0
  IHistogram2D* m_ckTruePhiVThetaR1[Rich::NRadiatorTypes];    ///< Cherenkov Theta versus Cherenkov Phi - Region 1
  IHistogram2D* m_ckTruePhiVThetaR2[Rich::NRadiatorTypes];    ///< Cherenkov Theta versus Cherenkov Phi - Region 2
  IHistogram2D* m_ckTruePhiVThetaR3[Rich::NRadiatorTypes];    ///< Cherenkov Theta versus Cherenkov Phi - Region 3

  IHistogram1D* m_ckTrueDTheta[Rich::NRadiatorTypes];       ///< TrueRecon - TrueExpected Ck theta
  IHistogram1D* m_ckAllDTheta[Rich::NRadiatorTypes];        ///< AllRecon - TrueExpected Ck theta
  IHistogram1D* m_ckTrueDThetaH[Rich::NRadiatorTypes][Rich::NParticleTypes];   ///< TrueRecon - TrueExpected Ck theta vs Hypo
  IHistogram1D* m_ckTrueDThetaT[Rich::NRadiatorTypes][Rich::Track::NTrTypes];  ///< TrueRecon - TrueExpected Ck theta vs track type

  IHistogram1D* m_ckTrueDTheta2[Rich::NRadiatorTypes];       ///< TrueRecon - TrueExpected Ck theta
  IHistogram1D* m_ckTrueDThetaT2[Rich::NRadiatorTypes][Rich::Track::NTrTypes];  ///< TrueRecon - TrueExpected Ck theta vs track type

  IHistogram1D* m_ckTrueDTheta3[Rich::NRadiatorTypes];       ///< TrueRecon - TrueExpected Ck theta
  IHistogram1D* m_ckTrueDThetaT3[Rich::NRadiatorTypes][Rich::Track::NTrTypes];  ///< TrueRecon - TrueExpected Ck theta vs track type

  IHistogram2D* m_ckresVthetaT[Rich::NRadiatorTypes][Rich::Track::NTrTypes];   ///< CK theta resolution versus true Cherenkov theta
  IHistogram2D* m_ckresVPtot[Rich::NRadiatorTypes][Rich::Track::NTrTypes];     ///< CK theta resolution versus log10(momentum)

  IHistogram2D* m_ckresVthetaT2[Rich::NRadiatorTypes][Rich::Track::NTrTypes];   ///< CK theta resolution versus true Cherenkov theta
  IHistogram2D* m_ckresVPtot2[Rich::NRadiatorTypes][Rich::Track::NTrTypes];     ///< CK theta resolution versus log10(momentum)

  IHistogram2D* m_ckresVthetaT3[Rich::NRadiatorTypes][Rich::Track::NTrTypes];   ///< CK theta resolution versus true Cherenkov theta
  IHistogram2D* m_ckresVPtot3[Rich::NRadiatorTypes][Rich::Track::NTrTypes];     ///< CK theta resolution versus log10(momentum)

  IHistogram2D* m_ckVptot[Rich::NRadiatorTypes][Rich::Track::NTrTypes];        ///< CK theta versus log10(momentum)
  IHistogram2D* m_trueckVptot[Rich::NRadiatorTypes][Rich::Track::NTrTypes];        ///< true CK theta versus log10(momentum)

  IHistogram2D* m_ckresVthetaTHypo[Rich::NRadiatorTypes][Rich::NParticleTypes];   ///< CK theta resolution versus true Cherenkov theta
  IHistogram2D* m_ckresVPtotHypo[Rich::NRadiatorTypes][Rich::NParticleTypes];  ///< CK theta resolution versus log10(momentum)
  IHistogram1D* m_ckAllDThetaH[Rich::NRadiatorTypes][Rich::NParticleTypes];    ///< AllRecon - TrueExpected Ck theta
  IHistogram1D* m_expEmitPhots[Rich::NRadiatorTypes][Rich::NParticleTypes];    ///< Number of expected emitted photons
  IHistogram1D* m_expSignalPhots[Rich::NRadiatorTypes][Rich::NParticleTypes];  ///< Number of expected signal photons
  IHistogram1D* m_trueSignalPhotsRec[Rich::NRadiatorTypes][Rich::NParticleTypes]; ///< Number of truth signal photons used in reconstruction
  IHistogram1D* m_trueSignalPhotsRecB[Rich::NRadiatorTypes][Rich::NParticleTypes]; ///< Number of truth signal photons used in reconstruction
  IHistogram1D* m_trueSignalPhots[Rich::NRadiatorTypes][Rich::NParticleTypes]; ///< Number of truth signal photons
  IHistogram1D* m_trueSignalPhotsB[Rich::NRadiatorTypes][Rich::NParticleTypes]; ///< Number of truth signal photons
  IHistogram1D* m_diffSignalPhots[Rich::NRadiatorTypes][Rich::NParticleTypes]; ///< true-reco signal photons
  IHistogram1D* m_geomEffFix[Rich::NRadiatorTypes][Rich::NParticleTypes];         ///< Geometrical Efficiencies (fixed)
  IHistogram1D* m_geomEffRay[Rich::NRadiatorTypes][Rich::NParticleTypes];         ///< Geometrical Efficiencies (ray-trace)
  IHistogram2D* m_geomEffCorr[Rich::NRadiatorTypes][Rich::NParticleTypes]; ///< Correlation betwen fixed and ray-traced
  IHistogram1D* m_avCKTheta[Rich::NRadiatorTypes][Rich::NParticleTypes];       ///< Average Cherenkov Theta
  IHistogram1D* m_emissionX[Rich::NRadiatorTypes];          ///< x coordinate of assumed emission point true photons
  IHistogram1D* m_emissionY[Rich::NRadiatorTypes];          ///< y coordinate of assumed emission point true photons
  IHistogram1D* m_emissionZ[Rich::NRadiatorTypes];          ///< z coordinate of assumed emission point true photons
  IHistogram1D* m_predPixelSignal[Rich::NRadiatorTypes][Rich::NParticleTypes]; ///< The predicted pixel signal
  IHistogram1D* m_predPixelSignalTrue[Rich::NRadiatorTypes][Rich::NParticleTypes];///< The predicted pixel signal for true types
  IHistogram1D* m_signalProb[Rich::NRadiatorTypes][Rich::NParticleTypes]; ///< The signal probability
  IHistogram1D* m_signalProbTrue[Rich::NRadiatorTypes][Rich::NParticleTypes];///< The signal probability for true types
  IHistogram1D* m_activeSegFract[Rich::NRadiatorTypes]; ///< The active segment fraction
  IHistogram1D* m_sigPhotEn[Rich::NRadiatorTypes][Rich::NParticleTypes];   ///< Average energy of signal Cherenkov photons
  IHistogram1D* m_emitPhotEn[Rich::NRadiatorTypes][Rich::NParticleTypes];   ///< Average energy of emitted Cherenkov photons

  IHistogram1D* m_trackHitSepL[Rich::NRadiatorTypes][Rich::Track::NTrTypes]; ///< The track hit - pixel hit seperation
  IHistogram1D* m_trackHitSepLTrue[Rich::NRadiatorTypes][Rich::Track::NTrTypes]; ///< The track hit - pixel hit seperation for true associations
  IHistogram1D* m_trackHitSepLTrueSigOnly[Rich::NRadiatorTypes][Rich::Track::NTrTypes]; ///< The track hit - pixel hit seperation for true Cherenkov photons

  IHistogram1D* m_nPhotsPerRad[Rich::NRadiatorTypes]; ///< Number of photon candidates per event for each radiator
  IHistogram1D* m_nTruePhotsPerRad[Rich::NRadiatorTypes]; ///< Number of true photon candidates per event for each radiator

  IHistogram2D* m_ckResVerrX[Rich::NRadiatorTypes][Rich::Track::NTrTypes]; ///< Cherenkov theta resolution versus error on x position
  IHistogram2D* m_ckResVerrY[Rich::NRadiatorTypes][Rich::Track::NTrTypes]; ///< Cherenkov theta resolution versus error on y position
  IHistogram2D* m_ckResVerrTX[Rich::NRadiatorTypes][Rich::Track::NTrTypes]; ///< Cherenkov theta resolution versus error on x slope
  IHistogram2D* m_ckResVerrTY[Rich::NRadiatorTypes][Rich::Track::NTrTypes]; ///< Cherenkov theta resolution versus error on y slope

  IHistogram2D* m_nSigVCktheta[Rich::NRadiatorTypes][Rich::NParticleTypes];

  IHistogram1D* m_emissResX[Rich::NRadiatorTypes][Rich::Track::NTrTypes]; ///< Emission point resolution x global
  IHistogram1D* m_emissResY[Rich::NRadiatorTypes][Rich::Track::NTrTypes]; ///< Emission point resolution y global
  IHistogram1D* m_emissResZ[Rich::NRadiatorTypes][Rich::Track::NTrTypes]; ///< Emission point resolution z global

  IHistogram1D* m_mcPhotEnAtProd[Rich::NRadiatorTypes][Rich::NParticleTypes];  ///< Cherenkov photon energy at production in the MC 
  IHistogram1D* m_recPhotEnAtProd[Rich::NRadiatorTypes][Rich::NParticleTypes]; ///< assumed Cherenkov photon energy at production in the reconstruction 
  IHistogram1D* m_photRefIndexMC[Rich::NRadiatorTypes];  ///< The effective refrative index as seen by the MC photons
  IHistogram1D* m_photRefIndexRec[Rich::NRadiatorTypes]; ///< The effective refrative index using the average photon energies for each segment

  IHistogram2D* m_ckTvsep2L[Rich::NRadiatorTypes];
  IHistogram2D* m_ckTvsep2Ltrue[Rich::NRadiatorTypes];
  IHistogram2D* m_ckTvsep2LtrueMC[Rich::NRadiatorTypes];

  IHistogram2D* m_ckPvsep2L[Rich::NRadiatorTypes];
  IHistogram2D* m_ckPvsep2Ltrue[Rich::NRadiatorTypes];

  IHistogram1D* m_notFoundCKT[Rich::NRadiatorTypes];
  IHistogram1D* m_notFoundCKTMC[Rich::NRadiatorTypes];
  IHistogram1D* m_notFoundSepL[Rich::NRadiatorTypes];
  IHistogram2D* m_notFoundLocXvY[Rich::NRadiatorTypes];
  IHistogram2D* m_notFoundSepLVCKT[Rich::NRadiatorTypes];
  IHistogram2D* m_notFoundSepLVCKTMC[Rich::NRadiatorTypes];
  IHistogram1D* m_notFoundCKres[Rich::NRadiatorTypes];
  IHistogram1D* m_notFoundCKresMC[Rich::NRadiatorTypes];
  IHistogram1D* m_notFoundTkSep[Rich::NRadiatorTypes];
  IHistogram2D* m_notFoundSepLVCKP[Rich::NRadiatorTypes];

};

#endif // RICHRECMONITOR_RICHPHOTONMONITOR_H
