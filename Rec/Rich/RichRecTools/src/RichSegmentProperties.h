// $Id: RichSegmentProperties.h,v 1.1 2003-06-30 15:47:06 jonrob Exp $
#ifndef RICHRECTOOLS_RICHSEGMENTPROPERTIES_H
#define RICHRECTOOLS_RICHSEGMENTPROPERTIES_H 1

// from Gaudi
#include "GaudiKernel/IParticlePropertySvc.h"

// base class
#include "RichRecBase/RichRecToolBase.h"

// RichUtils
#include "RichUtils/IRich1DProperty.h"
#include "RichUtils/Rich1DTabProperty.h"

// Event model
#include "Event/RichRecSegment.h"
#include "Event/RichPID.h"

// interfaces
#include "RichRecBase/IRichSegmentProperties.h"
#include "RichRecBase/IRichGeomEff.h"
#include "RichDetTools/IRichDetInterface.h"

/** @class RichSegmentProperties RichSegmentProperties.h
 *
 *  Tool performing useful calculations and manipulations evolving
 *  RichRecSegments
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 *  @todo Review this class and move methods to more logical and sef-contained tools
 */

class RichSegmentProperties : public RichRecToolBase,
                              virtual public IRichSegmentProperties {

public:

  /// Standard constructor
  RichSegmentProperties( const std::string& type,
                         const std::string& name,
                         const IInterface* parent );

  /// Destructor
  virtual ~RichSegmentProperties() {};

  /// Initialize method
  StatusCode initialize();

  /// Finalize method
  StatusCode finalize();

  /// Expected number of signal photons for given segment and hypothesis
  double nSignalPhotons ( RichRecSegment * segment,
                          const Rich::ParticleIDType id );

  /// Expected number of observable signal photons for given segment and hypothesis
  double nObservableSignalPhotons ( RichRecSegment * segment,
                                    const Rich::ParticleIDType id );

  /// Expected number of scattered photons for given segment and hypothesis
  double nScatteredPhotons ( RichRecSegment * segment,
                             const Rich::ParticleIDType id );

  /// Expected number of observable scattered photons for given segment and hypothesis
  double nObservableScatteredPhotons ( RichRecSegment * segment,
                                       const Rich::ParticleIDType id );

  /// Expected number of observable signal+scattered photons for given segment and hypothesis
  double nTotalObservablePhotons ( RichRecSegment * segment,
                                   const Rich::ParticleIDType id );

  /// Expected number of emitted photons for given segment and hypothesis
  double nEmittedPhotons ( RichRecSegment * segment,
                           const Rich::ParticleIDType id );

  /// Expected number of emitted photons for given segment and hypothesis,
  /// scaled by the HPD quantum efficiency
  double nDetectablePhotons ( RichRecSegment * segment,
                              const Rich::ParticleIDType id );

  /// Returns average Cherenkov angle for given particle hypothesis
  double avgCherenkovTheta( RichRecSegment * segment,
                            const Rich::ParticleIDType id );

  /// Returns 'beta' for given particle hypothesis
  double beta( RichRecSegment * segment, const Rich::ParticleIDType id );

  /// Is it geometrically possible for this segment to give Rich information
  bool hasRichInfo( RichRecSegment * segment );

  /// Is this segment above threshold for a given particle hypothesis
  bool aboveThreshold( RichRecSegment * segment,
                       const Rich::ParticleIDType type );

  /// Average refractive index for this segments radiator
  double refractiveIndex( RichRecSegment * segment );

  /// Photon resolution
  double ckThetaResolution( RichRecSegment * segment,
                            const Rich::ParticleIDType id );

  /// Set the threshold information in a RichPID object for given segment
  void setThresholdInfo( RichRecSegment * segment, RichPID * pid );

  // Private methods
private:

  /// Photon Rayleigh scattering probability in aerogel
  double photonUnscatteredProb( const double energy,
                                const double path );

  /// Refractive index for a given energy
  double refractiveIndex( const Rich::RadiatorType radiator,
                          double energy );

  /// Average refractive index for given radiator
  double refractiveIndex( const Rich::RadiatorType radiator );

private:  // Private data

  /// Pointers to tool instances
  IRichDetInterface * m_richDetInt;
  IRichGeomEff * m_geomEff;

  /// Pointer to Particle property service
  IParticlePropertySvc * m_ppSvc;

  /// Array containing particle masses
  std::vector<double> m_particleMass;

  /// Array containing particle masses
  std::vector<double> m_particleMassSq;

  /// Increment parameter for HPD efficiencies
  double m_hpdInc;

  /// Cherenkov phi incrementation
  double m_incPhi;

  /// Cache Sin(angle) for geometrical efficiency calculation
  std::vector<double> m_sinCkPhi;
  /// Cache Cos(angle) for geometrical efficiency calculation
  std::vector<double> m_cosCkPhi;

  /// Average refractive index for each radiator
  std::vector<double> m_AvRefIndex;

  // Temporary variables
  double m_detReflectorEff;

  /// Momentum thresholds
  double m_momThres[Rich::NRadiatorTypes][Rich::NParticleTypes];

  /// Number of energy bins used in expect number of photons calculation
  int m_EnergyBins;

  /// The size of the photon energy bins for each radiator
  double m_deltaEn[Rich::NRadiatorTypes];

  /// The central enrgy value for each photon energy bin
  std::vector<double> m_eBinValue[Rich::NRadiatorTypes];

  /// Cached efficiency for each energy bin
  std::vector<double> m_binEfficiency[Rich::NRadiatorTypes];

  /// Cached refractive index for each energy bin, per radiator
  std::vector<double> m_binRefIndex[Rich::NRadiatorTypes];

  // Sellmeir parameters
  double m_selF1[Rich::NRadiatorTypes];
  double m_selF2[Rich::NRadiatorTypes];
  double m_selE1[Rich::NRadiatorTypes];
  double m_selE2[Rich::NRadiatorTypes];
  double m_molW[Rich::NRadiatorTypes];
  double m_rho[Rich::NRadiatorTypes];
  double m_maxPhotEn[Rich::NRadiatorTypes];
  double m_minPhotEn[Rich::NRadiatorTypes];
  // W function parameters
  std::vector<double> m_WParaT[Rich::NRadiatorTypes];
  std::vector<double> m_WParaB[Rich::NRadiatorTypes];

  /// Rayleigh sattering parameters
  double m_rayleighPara[2];

  /// quartz window losses efficiency
  double m_quartzWinEff;

  /// Digitisation pedestal loss
  double m_pedLoss;

  /// Quantum Efficiency function. For time being assume only one reference
  /// curve for all HPDs
  Rich1DTabProperty * m_referenceQE;

  std::vector<double> m_theerr[Rich::NRadiatorTypes][Rich::Track::NTrTypes];
  std::vector<double> m_thebin[Rich::NRadiatorTypes];

  /// temporary parameters to take into acount degraded performance for robustness tests
  double m_photonEffScale;
  std::vector<double> m_emisPntErrScale;

};

#endif // RICHRECTOOLS_RICHSEGMENTPROPERTIES_H
