// $Id: ParticleTransporter.h,v 1.8 2008-04-15 15:45:33 jpalac Exp $
#ifndef PARTICLETRANSPORTER_H
#define PARTICLETRANSPORTER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// Event model
#include "Event/Particle.h"
#include "Event/TrackTypes.h" /// @todo temporary

// from LHCb
#include "Kernel/ParticleProperty.h"
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/IParticleTransporter.h"
#include "Kernel/IParticle2State.h"
#include "LHCbMath/MatrixManip.h"
#include "Kernel/TransporterFunctions.h"

#include "TrackInterfaces/ITrackExtrapolator.h"  // TrackExtrapolator

/** @class ParticleTransporter ParticleTransporter.h
 *
 *  Tool to transport an LHCb::Particle to a new Z position.
 *  For composite or neutral particles, there is a choice of
 *  simple transport or a transport plus a projeciton into
 *  track-like 5x5 space.
 *
 *  For all charged, basic particles, an ITrackExtrapolator is used. The
 *  implementation is controlled by the "TrackExtrapolator" property, it's
 *  default value being "TrackParabolicExtrapolator". The basic principle
 *  here is to obtain a 5x5 track-space state from the Particle, transport
 *  it with the track extrapolator, and project the transported state into
 *  Particle 7x7 space. For electrons and particles without an
 *  associated LHCb::ProtoParticle (assumed to be from MCParticles) the 5x5
 *  state is obtained from the Particle. For all other charged, basic particles
 *  it is obtained from the track state closest to the Z position to which
 *  the particle is to be transported.
 *  The projections between 5x5 track and 7x7 particle space are performed
 *  using the Particle2State implementation of IParticle2State.
 *
 *  @todo state method does a lot of useful stuff, yet is private. See if the
 *  funcitonality can be put in a tool or incorporated into an IParticleToState
 *  implementation.
 *
 *  @author Patrick KOPPENBURG
 *  @date   2006-01-20
 *
 *  @author Juan Palacios juan.palacios@nikhef.nl
 *  @date   2008-04-15
 */
class ParticleTransporter : public GaudiTool,
                            virtual public IParticleTransporter
{

public:

  /// Standard constructor
  ParticleTransporter( const std::string& type,
                       const std::string& name,
                       const IInterface* parent);

  virtual ~ParticleTransporter( ); ///< Destructor

  StatusCode initialize(); ///< Initialize

  /// Transport a Particle to specified z position.
  StatusCode transport(const LHCb::Particle*,
                       const double zNew,
                       LHCb::Particle& transParticle);

  StatusCode transportAndProject(const LHCb::Particle*,
                                 const double zNew,
                                 LHCb::Particle& transParticle);

private:

  /**
   * Get or make a state from a Particle
   *
   * @author Patrick Koppenburg
   *
   */
  StatusCode state(const LHCb::Particle* P,
                   const double zNew,
                   LHCb::State&) const ;


  /**
   * Transport a charged, basic LHCb::Particle to a new Z position.
   * It simply obtains an LHCb::State using the state method, propagates
   * it using the ITrackExtrapolator, and transforms back to 7x7 space
   * using the IParticle2State tool.
   *
   * @param particle Input LHCb::Particle.
   * @param zNew     Z-position to which particle is to be transported to.
   * @param transParticle transported equivalent of input particle.
   * @return StatusCode FAILURE if the track state creation, track propagation
   * or particle stare creation fail. SUCCESS otherwise.
   *
   *
   * @author Juan Palacios juan.palacios@nikhef.nl
   *
   */

  StatusCode transportChargedBasic(const LHCb::Particle* particle,
                                   const double zNew,
                                   LHCb::Particle& transParticle);

  /**
   *  Check a transported Particle for infinite momentum and print
   *  an error warning if necessary.
   *
   *  @param transParticle The transported LHCb::Particle.
   *  @return StautsCode::FAILURE if the particle's momentum is infinity.
   *
   *  @author Patrick Koppenburg
   *  @author Juan Palacios
   *
   */
  StatusCode checkParticle(const LHCb::Particle& transParticle);


private:

  ITrackExtrapolator* m_trackExtrapolator;        ///< Track extrapolator for particles from tracks

  std::string m_trackExtrapolatorName;            ///< Type of Track extrapolator for particles from tracks

  /// Accessor for ParticlePropertySvc
  LHCb::IParticlePropertySvc* m_ppSvc ;

  /// Particle to state convertion tool
  IParticle2State* m_particle2State ;

  /// Electron ID
  unsigned int m_eID ;

};
#endif // PARTICLETRANSPORTER_H
