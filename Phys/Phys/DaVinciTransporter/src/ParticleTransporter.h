// $Id: ParticleTransporter.h,v 1.4 2006-05-16 17:53:37 pkoppenb Exp $
#ifndef PARTICLETRANSPORTER_H 
#define PARTICLETRANSPORTER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IParticleTransporter.h"            // Interface
#include "Event/Particle.h"

class ITrackExtrapolator;
class IParticleStuffer;
class IParticlePropertySvc ;
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

  ITrackExtrapolator* m_tracks;        ///< Track extrapolator for particles from tracks
  ITrackExtrapolator* m_neutrals;      ///< Track extrapolator for neutrals
  ITrackExtrapolator* m_chargedComp;   ///< Track extrapolator for charged composite particles
  /// Track extrapolator for neutral composite particles, by default equal to the neutral extrapolator
  ITrackExtrapolator* m_neutralComp;   

  std::string m_tracksName;            ///< Type of Track extrapolator for particles from tracks
  std::string m_neutralsName ;         ///< Type of Track extrapolator for neutrals
  std::string m_chargedCompName;       ///< Type of Track extrapolator for charged composite particles
   /// Type of Track extrapolator for neutral composite particles, by default equal to the neutral extrapolator   
  std::string m_neutralCompName;
  
  /// Accessor for ParticlePropertySvc
  IParticlePropertySvc* m_ppSvc ;
  
  /// Particle to state convertion tool
  IParticle2State* m_p2s ;
  

};
#endif // PARTICLETRANSPORTER_H
