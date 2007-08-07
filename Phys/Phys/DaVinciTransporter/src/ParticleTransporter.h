// $Id: ParticleTransporter.h,v 1.7 2007-08-07 15:55:56 pkoppenb Exp $
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

  StatusCode state(const LHCb::Particle* P,  const double znew, LHCb::State&) const ;   ///< Get or make a state from a Particle

  ITrackExtrapolator* m_tracks;        ///< Track extrapolator for particles from tracks

  std::string m_tracksName;            ///< Type of Track extrapolator for particles from tracks
  
  /// Accessor for ParticlePropertySvc
  IParticlePropertySvc* m_ppSvc ;
  
  /// Particle to state convertion tool
  IParticle2State* m_p2s ;

  /// Electron ID
  unsigned int m_eID ;

};
#endif // PARTICLETRANSPORTER_H
