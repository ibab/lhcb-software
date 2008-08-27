// $Id: ParticleFilter.h,v 1.6 2008-08-27 14:15:26 pkoppenb Exp $
#ifndef PARTICLEFILTER_H 
#define PARTICLEFILTER_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from DaVinciKernel
#include "Kernel/IParticleFilter.h"
#include "Kernel/IFilterCriterion.h"

class IParticlePropertySvc;

/** @class ParticleFilter ParticleFilter.h
 *  Given a vector of Particles, provides a sub-vector of Particles 
 *  satisfying a set of FilterCriteria
 *  @author Paul colrain
 *  @date   14/03/2002
 *  Sandra Amato: Modified to include counters 
 *
 *  Added the byPID methods
 *  @author P. Koppenburg
 *  @date   16/12/2004  
 */

class ParticleFilter : public GaudiTool, 
                       virtual public IParticleFilter {

public:

  /// Standard constructor
  ParticleFilter( const std::string& type, 
                  const std::string& name,
                  const IInterface* parent);
  
  /// Desctructor
  virtual ~ParticleFilter( ){ }; 
  
  /// Initialize: Retrieve all the Filter Criterion Tools. 
  StatusCode initialize( );

  /// Finalize: Release all the Filter Criterion Tools. 
  StatusCode finalize( );

  /// Apply the filter to the input vector of Particles and outputs the 
  /// result in another vector.
  StatusCode filter( const LHCb::Particle::ConstVector&, LHCb::Particle::ConstVector& ); 
  
  /// Apply the filter and selects positive particles.
  StatusCode filterPositive( const LHCb::Particle::ConstVector&, LHCb::Particle::ConstVector& ); 
  
  /// Apply the filter and selects negative particles.
  StatusCode filterNegative( const LHCb::Particle::ConstVector&, LHCb::Particle::ConstVector& );

  /// get particles by PID
  /// These two methods allow to get a subset of particles
  /// with the same PID without having to use the PIDFilterCriterion
  /// -> The same instance of ParticleFilter can be used for all
  /// Particles. 
  /// ParticleVector myKaons, myPiPlus, myPiMinus;
  /// sc = particleFilter()->filterByPID(parts, myKaons,   "K+");
  /// sc = particleFilter()->filterByPID(parts, myPiPlus,  "pi+", false);
  /// sc = particleFilter()->filterByPID(parts, myPiMinus, "pi-", false);
  /// would return a vector of kaons (both sign) and pions.
  ///
  StatusCode filterByPID( const LHCb::Particle::ConstVector&,
                          LHCb::Particle::ConstVector&,  
                          const std::string&, 
                          const bool alsoCC = true );

  /// get particles by PID. This needs no string comparison
  /// and no call to particlePropertySvc.
  StatusCode filterByPID( const LHCb::Particle::ConstVector&,
                          LHCb::Particle::ConstVector&,  
                          const int& pid, 
                          const bool alsoCC = true );
  
private:

  /// The FilterCriterion names (properties)
  std::vector< std::string >     m_criteriaNames; 

  /// The Filter Criterions
  std::vector< IFilterCriterion* > m_criteria;

  /// Particle Property Service
  IParticlePropertySvc* m_ppSvc;                                            
  

};

#endif // PARTICLEFILTER_H
