// $Id: RichRecSegmentTool.h,v 1.2 2003-04-11 16:11:57 jonrob Exp $
#ifndef RICHRECTOOLS_RICHRECSEGMENTTOOL_H
#define RICHRECTOOLS_RICHRECSEGMENTTOOL_H 1

// from Gaudi
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
//#include "GaudiKernel/IChronoStatSvc.h"
#include "GaudiKernel/RndmGenerators.h"

// RichUtils
#include "RichUtils/IRich1DProperty.h"
#include "RichUtils/Rich1DTabProperty.h"

// interfaces
#include "RichRecBase/IRichRecSegmentTool.h"

// Detector
#include "RichDetTools/IRichDetInterface.h"

// Forward declarations
class IDataProviderSvc;
class IParticlePropertySvc;

/** @class RichRecSegmentTool RichRecSegmentTool.h
 *
 *  Tool which performs the association between RichRecTracks and
 *  RichRecPixels to form RichRecSegments
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

class RichRecSegmentTool : public AlgTool,
                           virtual public IRichRecSegmentTool,
                           virtual public IIncidentListener {

public:

  /// Standard constructor
  RichRecSegmentTool( const std::string& type,
                      const std::string& name,
                      const IInterface* parent );

  /// Destructor
  virtual ~RichRecSegmentTool() {};

  /// Initialize method
  StatusCode initialize();

  /// Finalize method
  StatusCode finalize();

  /// Implement the handle method for the Incident service.
  /// This is used to inform the tool of software incidents.
  void handle( const Incident& incident );

  /// Save a new RichRecSegment in the container
  void saveSegment( RichRecSegment * segment );

  /// Return Pointer to RichRecSegments
  RichRecSegments * richSegments();

  /// Obtain geometrical efficiency for this track and hypothesis
  double geomEfficiency ( RichRecSegment * segment,
                          const Rich::ParticleIDType & id );

  /// Obtain scattered geometrical efficiency for this track and hypothesis
  double geomEfficiencyScat ( RichRecSegment * segment,
                              const Rich::ParticleIDType & id );

  /// Expected number of signal photons for given segment and hypothesis
  double nSignalPhotons ( RichRecSegment * segment,
                          const Rich::ParticleIDType & id );

  /// Expected number of observable signal photons for given segment and hypothesis
  double nObservableSignalPhotons ( RichRecSegment * segment,
                                    const Rich::ParticleIDType & id );

  /// Expected number of scattered photons for given segment and hypothesis
  double nScatteredPhotons ( RichRecSegment * segment,
                             const Rich::ParticleIDType & id );

  /// Expected number of observable scattered photons for given segment and hypothesis
  double nObservableScatteredPhotons ( RichRecSegment * segment,
                                       const Rich::ParticleIDType & id );

  /// Expected number of observable signal+scattered photons for given segment and hypothesis
  double nTotalObservablePhotons ( RichRecSegment * segment,
                                   const Rich::ParticleIDType & id );

  /// Expected number of emitted photons for given segment and hypothesis
  double nEmittedPhotons ( RichRecSegment * segment,
                           const Rich::ParticleIDType& id );

  /// Expected number of emitted photons for given segment and hypothesis,
  /// scaled by the HPD quantum efficiency
  double nDetectablePhotons ( RichRecSegment * segment,
                              const Rich::ParticleIDType& id );

  /// Returns average Cherenkov angle for given particle hypothesis
  double avgCherenkovTheta( RichRecSegment * segment,
                            const Rich::ParticleIDType& id );

  /// Returns 'beta' for given particle hypothesis
  double beta( RichRecSegment * segment, const Rich::ParticleIDType& id );

  /// Is it geometrically possible for this segment to give Rich information
  bool hasRichInfo( RichRecSegment * segment );

  /// Is this segment above threshold for a given particle hypothesis
  bool aboveThreshold( RichRecSegment * segment, const Rich::ParticleIDType& type );

  /// Average refractive index for this segments radiator
  double refractiveIndex( RichRecSegment * segment );

  /// Photon resolution
  double ckThetaResolution( RichRecSegment * segment,
                            const Rich::ParticleIDType& id );

  // Private methods
private:

  /// Photon Rayleigh scattering probability in aerogel
  double photonUnscatteredProb( const double& energy,
                                const double& path );

  /// Refractive index for a given energy
  double refractiveIndex( const Rich::RadiatorType& radiator,
                          double energy );

  /// Average refractive index for given radiator
  double refractiveIndex( const Rich::RadiatorType& radiator );

  // Private data
private:

  /// Pointer to RichRecTracks
  RichRecSegments * m_segments;

  /// Pointer to event data service
  IDataProviderSvc* m_evtDataSvc;

  /// Pointer to detector data service
  IDataProviderSvc* m_detDataSvc;

  /// Pointer to RichDetInterface tool
  IRichDetInterface * m_richDetInterface;

  /// Pointer to Particle property service
  IParticlePropertySvc * m_ppSvc;

  /// Array containing particle masses
  std::vector<double> m_particleMass;

  /// Array containing particle masses
  std::vector<double> m_particleMassSq;

  /// Location of RichRecSegments in TES
  std::string m_richRecSegmentLocation;

  /// Number of photons to use in geometrical efficiency calculation
  int m_nGeomEff;

  /// Increment parameter for HPD efficiencies
  double m_hpdInc;

  /// Cherenkov phi incrementation
  double m_incPhi;

  /// Cache Sin(angle) for geometrical efficiency calculation
  std::vector<double> m_sinCkPhi;
  /// Cache Cos(angle) for geometrical efficiency calculation
  std::vector<double> m_cosCkPhi;

  /// Number of photons to quit after in geometrical efficiency calculation
  /// if all so far have failed
  int m_nGeomEffBailout;

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

  /// Flat Randomn distribution between 0 and 1
  Rndm::Numbers m_uniDist;

  std::vector<double> m_theerr[Rich::NRadiatorTypes][Rich::Track::NTrTypes];
  std::vector<double> m_thebin[Rich::NRadiatorTypes];

  /// temporary parameters to take into acount degraded performance for robustness tests
  double m_photonEffScale;
  double m_emisPntErrScale;

};

#endif // RICHRECTOOLS_RICHRECSEGMENTTOOL_H
