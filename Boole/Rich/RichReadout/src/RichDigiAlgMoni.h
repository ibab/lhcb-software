// $Id: RichDigiAlgMoni.h,v 1.3 2003-10-31 16:46:30 jonrob Exp $
#ifndef RICHMONITOR_RICHDIGIALGMONI_H
#define RICHMONITOR_RICHDIGIALGMONI_H 1

// from Gaudi
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"

// Event model
#include "Event/RichDigit.h"
#include "Event/MCRichDigit.h"
#include "Event/MCRichDeposit.h"
#include "Event/MCRichSummedDeposit.h"
#include "Event/MCRichHit.h"
#include "Event/MCTruth.h"

// RichKernel
#include "RichKernel/RichDetectorType.h"
#include "RichKernel/RichRadiatorType.h"
#include "RichKernel/RichParticleIDType.h"
#include "RichKernel/MessageSvcStl.h"

// Histogramming
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"

// CLHEP
#include "CLHEP/Units/PhysicalConstants.h"

// Detector tools
#include "RiSicbGeom/PixelFinder.h"
#include "RichDetTools/IMaPMTDetTool.h"
#include "RichDetTools/IRichDetInterface.h"

// temporary histogramming numbers
#include "RichDetParams.h"

/** @class RichDigiAlgMoni RichDigiAlgMoni.h RichDigiQC/RichDigiAlgMoni.h
 *
 *  Monitor for Rich Digitisation algorithm performance
 *
 *  @author Chris Jones   (Christopher.Rob.Jones@cern.ch)
 *  @date   2003-09-08
 */

class RichDigiAlgMoni : public Algorithm {

public:

  /// Standard constructor
  RichDigiAlgMoni( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~RichDigiAlgMoni( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private: // methods

  // Map to count cherenkov photons for each radiator
  typedef std::pair<const MCParticle*,Rich::RadiatorType> PhotPair;
  typedef std::map< PhotPair, int > PhotMap;

  // PD occupancies
  typedef std::map<RichSmartID,int> PDMulti;

  /// Book histograms
  StatusCode bookHistograms();

  /// Computes the position for a given SmartID
  bool getPosition( const RichSmartID & id, HepPoint3D & position );

  /// Returns the mass hypothesis for a given MCParticle
  Rich::ParticleIDType massHypothesis( const MCParticle * mcPart );

  /// Particle mass
  double mass( const MCParticle * mcPart );

  /// Returns beta for a given MCParticle
  double mcBeta( const MCParticle * mcPart );

  /// Returns the momentum for a given MCParticle
  double momentum( const MCParticle * mcPart );

  void countNPE( PhotMap & photMap,
                 const MCRichHit * hit );

private: // data

  IMaPMTDetTool *     m_mapmtDet;
  IPixelFinder *      m_sicbDet;
  IRichDetInterface * m_detInt;

  // job options
  std::string m_histPth;        ///< Output histogram path
  std::string m_digitTES;       ///< Location of RichDigits in TES
  std::string m_mcdigitTES;     ///< Location of MCRichDigits in TES
  std::string m_mcdepTES;       ///< Location of MCRichDeposits in TES
  std::string m_mchitTES;       ///< Location of MCRichHits in TES

  // temporary whilst we have different detector tools
  std::string m_detMode;

  // Particle masses
  std::map<Rich::ParticleIDType,double> m_particleMass;

  // Mapping between PDG ID and Rich::ParticleIDType
  std::map<int,Rich::ParticleIDType> m_localID;  

  // Histograms
  IHistogram1D* m_digitMult[Rich::NRiches]; ///< MCRichDigit event multiplicity
  IHistogram1D* m_hitMult[Rich::NRiches]; ///< MCRichHit event multiplicity

  IHistogram1D* m_tofDep[Rich::NRiches];   ///< TOF Information for MCRichDeposit
  IHistogram1D* m_depEnDep[Rich::NRiches]; ///< Deposit Energy for MCRichDeposit

  IHistogram1D* m_tofHit[Rich::NRiches];   ///< TOF Information for MCRichDeposit
  IHistogram1D* m_depEnHit[Rich::NRiches]; ///< Deposit Energy for MCRichDeposit

  IHistogram1D* m_pdDigsXGlobal[Rich::NRiches];     ///< Observed PD digits x global
  IHistogram1D* m_pdDigsYGlobal[Rich::NRiches];     ///< Observed PD digits y global
  IHistogram1D* m_pdDigsZGlobal[Rich::NRiches];     ///< Observed PD digits z global
  IHistogram2D* m_pdDigsXYGlobal[Rich::NRiches];    ///< Observed PD digits xVy global
  IHistogram2D* m_pdDigsXZGlobal[Rich::NRiches];    ///< Observed PD digits xVz global
  IHistogram2D* m_pdDigsYZGlobal[Rich::NRiches];    ///< Observed PD digits yVz global
  IHistogram2D* m_pdCloseUpXZ[Rich::NRiches];
  IHistogram2D* m_pdCloseUpYZ[Rich::NRiches]; 

  IHistogram1D* m_mcHitsXGlobal[Rich::NRiches];     ///< Observed hits x global
  IHistogram1D* m_mcHitsYGlobal[Rich::NRiches];     ///< Observed hits y global
  IHistogram1D* m_mcHitsZGlobal[Rich::NRiches];     ///< Observed hits z global
  IHistogram2D* m_mcHitsXYGlobal[Rich::NRiches];    ///< Observed hits xVy global
  IHistogram2D* m_mcHitsXZGlobal[Rich::NRiches];    ///< Observed hits xVz global
  IHistogram2D* m_mcHitsYZGlobal[Rich::NRiches];    ///< Observed hits yVz global
  IHistogram2D* m_mcCloseUpXZ[Rich::NRiches];
  IHistogram2D* m_mcCloseUpYZ[Rich::NRiches];

  IHistogram1D* m_digiErrX[Rich::NRiches]; ///< Distance in x between hit and digit positions
  IHistogram1D* m_digiErrY[Rich::NRiches]; ///< Distance in y between hit and digit positions
  IHistogram1D* m_digiErrZ[Rich::NRiches]; ///< Distance in z between hit and digit positions
  IHistogram1D* m_digiErrR[Rich::NRiches]; ///< Absolute distance between hit and digit positions

  IHistogram1D* m_hitsPerDigi[Rich::NRiches];  ///< The number of MCRichHits per Digit
  IHistogram1D* m_pdOcc[Rich::NRiches];  ///< The number of digits in each PD

  IHistogram1D* m_mchitNpes[Rich::NRadiatorTypes];   ///< Number of MCRichHit p.e.s. per radiator 
  IHistogram1D* m_mcdigitNpes[Rich::NRadiatorTypes]; ///< Number of MCRichDigit p.e.s. per radiator 
  IHistogram1D* m_mcdepNpes[Rich::NRadiatorTypes];   ///< Number of MCRichDeposit p.e.s. per radiator 

  IHistogram1D* m_npesRetained[Rich::NRadiatorTypes]; ///< The fraction of signal p.e.s retained by the digitisation

  IHistogram1D* m_chargedTkDeps[Rich::NRiches];
  IHistogram1D* m_chargedTkDigs[Rich::NRiches];

};

inline double RichDigiAlgMoni::mass( const MCParticle * mcPart )
{
  return m_particleMass[ massHypothesis(mcPart) ];
}

inline Rich::ParticleIDType RichDigiAlgMoni::massHypothesis( const MCParticle * mcPart ) 
{
  return ( mcPart ? m_localID[abs(mcPart->particleID().pid())] : Rich::Unknown );
}

inline double RichDigiAlgMoni::momentum( const MCParticle * mcPart )
{
  return mcPart->momentum().vect().mag();
}

#endif // RICHMONITOR_RICHDIGIALGMONI_H
