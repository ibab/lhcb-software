// $Id: ParticleTransporter.h,v 1.2 2006-03-15 13:41:00 pkoppenb Exp $
#ifndef PARTICLETRANSPORTER_H 
#define PARTICLETRANSPORTER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IParticleTransporter.h"            // Interface
#include "Event/Particle.h"

class ITrackExtrapolator;
class IParticleStuffer;

/** @class ParticleTransporter ParticleTransporter.h
 *  
 *  Particle transporter calling 
 *  the master track extrapolator for Particles made from Tracks,
 *  the parabolic track extrapolator for charged composites
 *  and the linear track extrapolator for neutrals.
 *
 *  @author Patrick KOPPENBURG
 *  @date   2006-01-20
 */
class ParticleTransporter : public GaudiTool, virtual public IParticleTransporter {
public: 
  /// Standard constructor
  ParticleTransporter( const std::string& type, 
                       const std::string& name,
                       const IInterface* parent);

  virtual ~ParticleTransporter( ); ///< Destructor

  StatusCode initialize(); ///< Initialize

  /// Transport a Particle to specified z position.
  StatusCode transport(const LHCb::Particle*, 
                       const double znew,
                       LHCb::Particle& transParticle);
protected:

private:

  ITrackExtrapolator* extrapolator(const LHCb::Particle* P) const ;  ///< Get type of extrapolator

  StatusCode state(const LHCb::Particle* P,  const double znew, LHCb::State&) const ;   ///< Get or make a state from a Particle
  StatusCode makeState(const LHCb::Particle* P, LHCb::State&) const ;   ///< Make a state from a Particle

  ITrackExtrapolator* m_tracks;        ///< Track extrapolator for particles from tracks
  ITrackExtrapolator* m_neutrals;      ///< Track extrapolator for neutrals
  ITrackExtrapolator* m_chargedComp;   ///< Track extrapolator for charged composite particles

  IParticleStuffer* m_stuffer;    ///< Particle stuffer tool
  

  std::string m_tracksName;            ///< Type of Track extrapolator for particles from tracks
  std::string m_neutralsName ;         ///< Type of Track extrapolator for neutrals
  std::string m_chargedCompName;       ///< Type of Track extrapolator for charged composite particles
  

};
#endif // PARTICLETRANSPORTER_H
