// $Id: OTTimeChecker.h,v 1.1 2004-09-10 13:13:49 cattanem Exp $
#ifndef OTMONITOR_OTTIMECHECKER_H
#define OTMONITOR_OTTIMECHECKER_H 1

// Gaudi
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IHistogramSvc.h"

// CLHEP
#include "CLHEP/Units/SystemOfUnits.h"

// base class
#include "OTMonitorAlgorithm.h"

// from Associators
#include "OTAssociators/OTTime2MCHitAsct.h"

// geometry
#include "OTDet/DeOTDetector.h"

// histogramming
#include "AIDA/IHistogram1D.h"

// forward declarations
class IOTrtRelation;
class MCHit;

/** @class OTTimeChecker OTTimeChecker.h "OTMonitor/OTTimeChecker.h"
 *
 *  Top level algorithm for checking OTTimes. Checker algorithms 
 *  use the associators for efficiency and resolution studies.
 *  The efficiency is the number of OTTimes with 
 *  an association to an MCHit divided by the number of MCHits. Only MCHits 
 *  in the right spill are considered. OTTimes coming from spillover are 
 *  ghosts. The ghost rate is the number of times not associated to an 
 *  MCHit divided by the total number of times.
 *  The resolution is calculated using the cheated ambiguity and the MCHit's
 *  entry- and exit points.
 *
 *  @author J. van Tilburg and Jacopo Nardulli
 *  @date   22-06-2002
 */

class OTTimeChecker : public OTMonitorAlgorithm {

public:

  /// constructor
  OTTimeChecker(const std::string& name, 
                   ISvcLocator *svcloc );

  /// destructor
  virtual ~OTTimeChecker();

  /// initialize
  StatusCode initialize();

  /// execute
  StatusCode execute();

  /// finalize
  StatusCode finalize();

private:
 
  /// book the histograms
  StatusCode initHistograms();

  /// fill the resolution histograms
  StatusCode fillResolutionHistos(OTTime* time,
                                  MCHit* aHit);

  /// Struct for storing the time multiplicity of a MCHit
  struct HitMultiplicity{int mult ;
                         MCHit* mcHit; };

  /// typedef for a vector of HitMultiplicities
  typedef std::vector<HitMultiplicity> HitMultVector;

  /// Struct for storing the time multiplicity of a MCParticle
  struct PartMultiplicity{int mult ;
                          MCParticle* mcParticle; };

  /// typedef for a vector of PartMultiplicities
  typedef std::vector<PartMultiplicity> PartMultVector;
  
  std::string m_nameHitAsct;             ///< name of the associator
  OTTime2MCHitAsct::IAsct* m_hitAsct; ///< pointer to associator
  DeOTDetector* m_tracker;               ///< pointer to geometry
  /// flag to cut on momentum used for resolution plot 
  bool m_doMomentumCut;
  double m_momentumCut;                  ///< value of momentum cut

  /// histograms
  IHistogram1D* m_effHisto;       ///< Overall efficiency of the times
  IHistogram1D* m_eff1Histo;      ///< Efficiency for only 1 time per MCHit 
  IHistogram1D* m_eff2Histo;      ///< Efficiency for 2 times per MCHit
  IHistogram1D* m_eff3Histo;      ///< Efficiency for >3 times per MCHit
  IHistogram1D* m_ghostRateHisto; ///< Ghost rate 
  IHistogram1D* m_hitMultHisto;   ///< number of times per mchit
  IHistogram1D* m_resHisto;       ///< distance resolution (mm)
  IHistogram1D* m_pdgCodeHisto;   ///< Particle type distribution
  IHistogram1D* m_momentumHisto;  ///< Momentum distribution (GeV)
  IHistogram1D* m_nParticlesHisto;///< Number of MCParticles making times
  IHistogram1D* m_partMultHisto;  ///< Number of times per MCParticle

};

#endif // OTMONITOR_OTTIMECHECKER_H
