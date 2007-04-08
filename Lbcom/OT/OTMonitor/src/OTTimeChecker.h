// $Id: OTTimeChecker.h,v 1.6 2007-04-08 17:00:48 janos Exp $
#ifndef OTMONITOR_OTTIMECHECKER_H
#define OTMONITOR_OTTIMECHECKER_H 1

/// STL
#include <vector>

/// Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"

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

// forward declarations
class IOTrtRelation;

namespace AIDA 
{
  class IHistogram1D;
}

namespace LHCb
{
  class MCHit;
  class MCParticle;
  class OTTime;
}

class OTTimeChecker : public GaudiHistoAlg {

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

private:
 
  /// book the histograms
  void initHistograms();

  /// fill the resolution histograms
  void fillResolutionHistos( LHCb::OTTime* time,
                             const LHCb::MCHit* aHit );

  /// Struct for storing the time multiplicity of a MCHit
  struct HitMultiplicity{int mult ; const LHCb::MCHit* mcHit; };

  /// typedef for a vector of HitMultiplicities
  typedef std::vector<HitMultiplicity> HitMultVec;

  /// Struct for storing the time multiplicity of a MCParticle
  struct PartMultiplicity{int mult ; const LHCb::MCParticle* mcParticle; };

  /// typedef for a vector of PartMultiplicities
  typedef std::vector<PartMultiplicity> PartMultVec;

  /// Return my particle code
  const unsigned int myParticleCode(const LHCb::MCParticle* aMCPart) const;

  DeOTDetector* m_tracker;               ///< pointer to geometry
  /// flag to cut on momentum used for resolution plot 
  bool m_doMomentumCut;
  double m_momentumCut;                  ///< value of momentum cut

  /// histograms
  AIDA::IHistogram1D* m_effHisto;       ///< Overall efficiency of the times
  AIDA::IHistogram1D* m_eff1Histo;      ///< Efficiency for only 1 time per MCHit 
  AIDA::IHistogram1D* m_eff2Histo;      ///< Efficiency for 2 times per MCHit
  AIDA::IHistogram1D* m_eff3Histo;      ///< Efficiency for >3 times per MCHit
  AIDA::IHistogram1D* m_ghostRateHisto; ///< Ghost rate 
  AIDA::IHistogram1D* m_hitMultHisto;   ///< number of times per mchit
  AIDA::IHistogram1D* m_resHisto;       ///< distance resolution (mm)
  AIDA::IHistogram1D* m_pdgCodeHisto;   ///< Particle type distribution
  AIDA::IHistogram1D* m_momentumHisto;  ///< Momentum distribution (GeV)
  AIDA::IHistogram1D* m_nParticlesHisto;///< Number of MCParticles making times
  AIDA::IHistogram1D* m_partMultHisto;  ///< Number of times per MCParticle

};

inline const unsigned int OTTimeChecker::myParticleCode(const LHCb::MCParticle* aMCPart) const {
  /// electron
  if ((aMCPart->particleID()).abspid() == 11 ) return 1u;
  /// muon
  if ((aMCPart->particleID()).abspid() == 13 ) return 2u;
  /// pion
  if ((aMCPart->particleID()).abspid() == 211 ) return 3u;
  /// kaon
  if ((aMCPart->particleID()).abspid() == 321 ) return 4u;
  /// proton
  if ((aMCPart->particleID()).abspid() == 2212 ) return 5u;
  /// other
  return 6u;
}

#endif // OTMONITOR_OTTIMECHECKER_H
