// $Id: OTTimeChecker.h,v 1.8 2010-01-12 14:13:11 akozlins Exp $
#ifndef OTMONITOR_OTTIMECHECKER_H
#define OTMONITOR_OTTIMECHECKER_H 1

// STL
#include <string>
#include <vector>

// Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"
#include "GaudiKernel/ToolHandle.h"

#include "OTDAQ/IOTRawBankDecoder.h"

// LHCb
#include "Event/MCParticle.h"

/** @class OTTimeChecker OTTimeChecker.h
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

// Forward declarations
namespace AIDA
{
  class IHistogram1D;
}

namespace LHCb
{
  class MCHit;
  class OTLiteTime;
}

class DeOTDetector;

class OTTimeChecker: public GaudiHistoAlg
{
public:
  /// constructor
  OTTimeChecker(const std::string& name, ISvcLocator *svcloc );

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
  void fillResolutionHistos( const LHCb::OTLiteTime* time, const LHCb::MCHit* hit );

  /// Struct for storing the time multiplicity of a MCHit
  struct HitMultiplicity
  {
    int mult;
    const LHCb::MCHit* mcHit;
  };

  /// typedef for a vector of HitMultiplicities
  typedef std::vector<HitMultiplicity> HitMultVec;

  /// Struct for storing the time multiplicity of a MCParticle
  struct PartMultiplicity
  {
    int mult;
    const LHCb::MCParticle* mcParticle;
  };

  /// typedef for a vector of PartMultiplicities
  typedef std::vector<PartMultiplicity> PartMultVec;

  /// Return my particle code
  unsigned int myParticleCode(const LHCb::MCParticle* mcParticle) const;

  ToolHandle<IOTRawBankDecoder> m_decoder;
  DeOTDetector* m_tracker; ///< pointer to geometry

  /// flag to cut on momentum used for resolution plot
  bool m_doMomentumCut;
  double m_momentumCut; ///< value of momentum cut

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

inline unsigned int OTTimeChecker::myParticleCode(const LHCb::MCParticle* mcParticle) const
{
  switch(mcParticle->particleID().abspid())
  {
  case 11: return 1;
  case 13: return 2;
  case 211: return 3;
  case 321: return 4;
  case 2212: return 5;
  default: return 6;
  }
}

#endif // OTMONITOR_OTTIMECHECKER_H
