// $Id: CombinedParticleMaker.h,v 1.1 2003-05-30 11:55:13 gcorti Exp $
#ifndef COMBINEDPARTICLEMAKER_H 
#define COMBINEDPARTICLEMAKER_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/ParticleProperty.h"

// from EventSys
#include "Event/Particle.h"
#include "Event/ProtoParticle.h"

// from DaVinci
#include "DaVinciTools/IParticleMaker.h"

// locals
#include "PMakerSelection.h"

/** @class CombinedParticleMaker CombinedParticleMaker.h
 *  Particle maker to be used as DEFAULT unique one for Charged
 *  Particles.
 *  Uses all detectro information available for a given hypothesis.
 *
 *  @author Gloria Corti
 *  @date   2003-05-19
 */
class CombinedParticleMaker : public AlgTool, 
                              virtual public IParticleMaker {
public:

  typedef std::vector<std::string>       SelectionDescs;
  typedef std::vector<PMakerSelection*>  SelectionSpecs;
  typedef std::pair< const ParticleProperty*, SelectionSpecs > SelectionType;
  typedef std::vector< SelectionType >   TypeSelections;

  /// Standard constructor
  CombinedParticleMaker( const std::string& type, 
                         const std::string& name,
                         const IInterface* parent);

  virtual ~CombinedParticleMaker( ); ///< Destructor

  /// Make the particles
  virtual StatusCode makeParticles( ParticleVector& parts );
  
  /// Standard initialization of tool
  virtual StatusCode initialize();
  
  /// Standard finalization of tool
  virtual StatusCode finalize();

protected:
  
  /** Fill Particle Parameters from ProtoParticle given a ParticleProperty
   *  @param proto     Pointer to ProtoParticle
   *  @param pprop     Pointer to ParticleProperty 
   *  @param particle  Pointer to Particle
   *  @return StatusCode
   */
  StatusCode fillParticle( const ProtoParticle* proto,
                           const ParticleProperty* pprop,
                           Particle* particle );

  /** Check if a selection is satisfied by a given ProtoParticle
   *  @param proto      Pointer to ProtoParticle
   *  @param selection  Pointer to PMakerSelection
   *  @return bool
   */ 
  bool selectionIsSatisfied( const ProtoParticle* proto,
                             const PMakerSelection* selection );

  void setSelections( const SelectionDescs& selDescriptions,
                      SelectionSpecs& selections );

  void defineSelection(const std::string description, 
                       PMakerSelection* selection );

  void setDetectorFlag( const std::string detector,
                        PMakerSelection* selection );

  double bremMomentum( const ProtoParticle* proto );

  double dllValue( const ProtoParticle* proto,
                   const ProtoParticle::detectorPID& det );

  /// The standard event data service. Method not in IParticleMaker.h.
  IDataProviderSvc* eventSvc() const;

private:

  /// Job option wtih TES input for ProtoParticles
  std::string m_input;

  /// Job Option List of particles type to make. Possible values are muons, 
  /// electrons, kaons, protons, pions
  std::vector<std::string>  m_particleList; 

  /// Job option for exclusive selection of particle types
  bool m_exclusive;

  /// Job option descriptions of selections for muons
  SelectionDescs m_muonSelection; 
 
  /// Job option descriptions of selections for electrons
  SelectionDescs m_electronSelection; 
 
  /// Job option descriptions of selections for kaons
  SelectionDescs m_kaonSelection; 
 
  /// Job option descriptions of selections for protons
  SelectionDescs m_protonSelection; 
 
  /// Job option descriptions of selections for pions
  SelectionDescs m_pionSelection; 

  /// Job option to add or not momentum from Brem photon
  bool m_addBremPhoton;
  
  /// List of particles to make with list of selections with related cuts
  TypeSelections m_typeSelections;

  IDataProviderSvc* m_EDS;        ///<  Reference to Event Data Service
  
};
#endif // COMBINEDPARTICLEMAKER_H
