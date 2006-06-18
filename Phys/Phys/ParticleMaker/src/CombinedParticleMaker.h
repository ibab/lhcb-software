
//-----------------------------------------------------------------------------
/** @file CombinedParticleMaker.h
 *
 * Header file for Particle maker CombinedParticleMaker
 *
 * CVS Log :-
 * $Id: CombinedParticleMaker.h,v 1.6 2006-06-18 14:45:41 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 2006-05-03
 */
//-----------------------------------------------------------------------------

#ifndef PARTICLEMAKER_COMBINEDPARTICLEMAKER_H
#define PARTICLEMAKER_COMBINEDPARTICLEMAKER_H 1

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/HashMap.h"

// from EventSys
#include "Event/Particle.h"
#include "Event/ProtoParticle.h"

// Interfaces
#include "Kernel/IParticleMaker.h"
#include "Kernel/IProtoParticleFilter.h"
#include "Kernel/IParticle2State.h"

/** @class CombinedParticleMaker CombinedParticleMaker.h
 *
 *  Particle maker to be used as DEFAULT unique one for Charged
 *  Particles.
 *  Uses all detector information available for a given hypothesis.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date 2006-05-03
 *
 *  @todo Add Bremstraluung correction to momentum for electrons
 *  @todo Add error on measured momentum (from PDG ?)
 *  @todo make sure the various correlation matrices are set correctly
 *  @todo Re-assess how the confidence level is calculated
 */

class CombinedParticleMaker : public GaudiTool,
                              virtual public IParticleMaker
{

public:

  /// Standard constructor
  CombinedParticleMaker( const std::string& type,
                         const std::string& name,
                         const IInterface* parent);

  virtual ~CombinedParticleMaker( ); ///< Destructor

  /// Make the particles
  virtual StatusCode makeParticles( LHCb::Particle::ConstVector & parts );

  /// Standard initialization of tool
  virtual StatusCode initialize();

  /// Standard finalization of tool
  virtual StatusCode finalize();

private:

  /** Fill Particle Parameters from ProtoParticle given a ParticleProperty
   *  @param proto     Pointer to ProtoParticle
   *  @param pprop     Pointer to ParticleProperty
   *  @param particle  Pointer to Particle
   *  @return StatusCode
   */
  StatusCode fillParticle( const LHCb::ProtoParticle * proto,
                           const ParticleProperty    * pprop,
                           LHCb::Particle            * particle ) const;

  /// Converts a string to a constant format
  std::string convertName( const std::string & in ) const;

  /// Set the Particle confidence level
  void setConfLevel( const LHCb::ProtoParticle * proto,
                     const ParticleProperty    * pprop,
                     LHCb::Particle            * particle ) const;

private:

  /// Job option wtih TES input for ProtoParticles
  std::string m_input;

  /** @brief Job Option List of particles type to make.
   * Possible values are muons, electrons, kaons, protons, pions
   */
  std::vector<std::string> m_particleList;

  std::string m_elProtoFilter; ///< The tool type to use for electron selection
  std::string m_muProtoFilter; ///< The tool type to use for muon selection
  std::string m_piProtoFilter; ///< The tool type to use for pion selection
  std::string m_kaProtoFilter; ///< The tool type to use for kaon selection
  std::string m_prProtoFilter; ///< The tool type to use for proton selection

 /// Job option for exclusive selection of particle types
  bool m_exclusive;

  /// Map type that takes a particle type to a ProtoParticle filter
  typedef std::pair< const ParticleProperty *, const IProtoParticleFilter* > ProtoPair;
  typedef std::vector < ProtoPair > ProtoMap;

  /// Map that takes a particle type to a ProtoParticle filter
  ProtoMap m_protoMap;

  /// Tool to fill State information into a Particle
  IParticle2State* m_p2s ;

  // tallies

  /// Simple utility tally classes
  class TrackTally
  {
  public:
    TrackTally() : protos(0), el(0), mu(0), pi(0), ka(0), pr(0) { }
    unsigned long protos;
    unsigned long el;
    unsigned long mu;
    unsigned long pi;
    unsigned long ka;
    unsigned long pr;
    inline void addToType( const std::string & type )
    {
      if      ( "e+"  == type ) { ++el; }
      else if ( "mu+" == type ) { ++mu; }
      else if ( "pi+" == type ) { ++pi; }
      else if ( "K+"  == type ) { ++ka; }
      else if ( "p+"  == type ) { ++pr; }
    }
  };

  /// Map type containing tally for various track types
  typedef GaudiUtils::HashMap < const LHCb::Track::Types, TrackTally > TrackMap;

  /// Total number of tracks considered and selected
  TrackMap m_nTracks;

};

#endif // PARTICLEMAKER_COMBINEDPARTICLEMAKER_H
