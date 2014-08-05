// $Id$
#ifndef NEUTRALCCCHANGEPIDTOOL_H 
#define NEUTRALCCCHANGEPIDTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
//from LHCb
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"
#include "Kernel/IChangePIDTool.h"            // Interface


/** @class NeutralCCChangePIDTool NeutralCCChangePIDTool.h
 *   
 *  Very simple implementation of IChangePIDTool.
 *
 *  Changes the PID of a neutral non-self-conjugate particle, or of each
 *  member of a vector of such particles, to the conjugate PID.  The original
 *  Particles are not modified.  Renamed Particles are returned by value.
 *
 *  This tool is ONLY usable for Particles with a neutral, non-self-conjugate
 *  PID.  It rather ungracefully throws an exception for other particle types.
 *
 *  @author Patrick Spradlin
 *  @date   2006-12-15
 */
class NeutralCCChangePIDTool : public GaudiTool, virtual public IChangePIDTool {
public: 
  /// Standard constructor
  NeutralCCChangePIDTool( const std::string& type, 
                          const std::string& name,
                          const IInterface* parent);

  virtual ~NeutralCCChangePIDTool( ); ///< Destructor

  StatusCode initialize();      ///< Tool initialization method
  // StatusCode finalize();     ///< Use standard Tool finalization method

  /// Change the PID of a single particle
  LHCb::Particle changePID( const LHCb::Particle & );

  /// Change the PID of each member of a vector of particles
  std::vector<LHCb::Particle> changePID( const LHCb::Particle::ConstVector & );

protected:

  /// Test for charge neutrality of a ParticleProperty
  inline bool isNeutralPP( const LHCb::ParticleProperty *aPP ) const
  { return aPP && (floor(3.0*(aPP->charge()) + 0.5) == 0); }

private:

  mutable LHCb::IParticlePropertySvc *m_ppSvc;    ///< Ref to ParticlePropertySvc

};
#endif // NEUTRALCCCHANGEPIDTOOL_H
