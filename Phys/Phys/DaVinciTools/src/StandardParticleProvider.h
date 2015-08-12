// $Id: $
#ifndef STANDARDPARTICLEPROVIDER_H 
#define STANDARDPARTICLEPROVIDER_H 1

#include <map>

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/HashMap.h"

// Interface
#include "Kernel/IStandardParticleProvider.h"

// Event model
#include "Event/ProtoParticle.h"
#include "Event/Particle.h"

// BOOST
#include <boost/assign/list_of.hpp>

/** @class StandardParticleProvider StandardParticleProvider.h
 *  
 *  Tool that provides pointer to 'standard' Particles
 *  for a given ProtoParticle and PID hypothesis
 *  
 *  @author Chris Jones
 *  @date   2012-02-26
 */
class StandardParticleProvider : public GaudiTool, 
                                 virtual public IStandardParticleProvider,
                                 virtual public IIncidentListener
{

public: 

  /// Standard constructor
  StandardParticleProvider( const std::string& type, 
                            const std::string& name,
                            const IInterface* parent);

  /// Destructor
  virtual ~StandardParticleProvider( );

  // Initialization of the tool after creation
  StatusCode initialize();

public:

  /** Implement the handle method for the Incident service.
   *  This is used to inform the tool of software incidents.
   *
   *  @param incident The incident identifier
   */
  void handle( const Incident& incident );
  
public:
  
  // Get a Particle for a given ProtoParticle and PID
  virtual const LHCb::Particle * particle( const LHCb::ProtoParticle * proto,
                                           const LHCb::ParticleID& pid ) const;
  
private:

  typedef std::map<unsigned int,std::string> PIDToTESMap;

  typedef std::map<const LHCb::ProtoParticle*,const LHCb::Particle*> ProtoToPartMap;

  typedef std::map<int,ProtoToPartMap> PIDToPartsMap;

private:

  bool fillParticleMap( const unsigned int pid ) const;

private:

  /// Mapping between PID types and TES locations for the standard Particles
  PIDToTESMap m_pidToTESMap;

  /// Local map to store association between ProtoParticles and Particles of a certain PID
  mutable PIDToPartsMap m_pidToParts;

};

#endif // STANDARDPARTICLEPROVIDER_H
