// $Id: Particle2MCAssociatorBase.h,v 1.11 2009-03-26 12:52:47 jpalac Exp $
#ifndef PARTICLE2MCASSOCIATORBASE_H 
#define PARTICLE2MCASSOCIATORBASE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
// LHCb
#include "Event/MCParticle.h"
#include "Kernel/IParticle2MCAssociator.h"            // Interface
#include "Kernel/IParticle2MCWeightedAssociator.h"            // Interface


/** @class Particle2MCAssociatorBase Particle2MCAssociatorBase.h
 *  
 *  Common implementation for descendants of IParticle2MCAssociator.
 *  Mainly inline helper methods for common implementation of host of 
 *  similar methods in the interface.
 *  Set of methods is self-consistent. Derived classes only need to implement
 *  methods
 *  @code 
 *  bool isMatched(const LHCb::Particle*, const LHCb::MCParticle)
 *  @endcode
 *  and
 *  @code
 *  LHCb::MCParticle::ConstVector sort(const LHCb::MCParticle::Container* mcParticles) const
 *  @code
 *
 *  @author Juan PALACIOS
 *  @date   2009-01-30
 */
class Particle2MCAssociatorBase : public GaudiTool, 
                                  virtual public IParticle2MCWeightedAssociator
{
public: 
  /// Standard constructor
  Particle2MCAssociatorBase( const std::string& type, 
                             const std::string& name,
                             const IInterface* parent);

  virtual StatusCode initialize() ;

  virtual StatusCode finalize() ;
  
  virtual ~Particle2MCAssociatorBase( );

  virtual const LHCb::MCParticle* 
  relatedMCP(const LHCb::Particle*) const ;


  virtual const LHCb::MCParticle*
  relatedMCP(const LHCb::Particle*,
             const std::string& mcParticleLocation) const ;

  virtual const LHCb::MCParticle*
  relatedMCP(const LHCb::Particle* particles,
             const LHCb::MCParticle::ConstVector& mcParticles) const ;

  virtual const LHCb::MCParticle*
  relatedMCP(const LHCb::Particle* particles,
             const LHCb::MCParticle::Container& mcParticles) const ;


  virtual Particle2MCParticle::ToVector 
  relatedMCPs(const LHCb::Particle* particle) const ;
  
  virtual Particle2MCParticle::ToVector 
  relatedMCPs(const LHCb::Particle* particle,
              const std::string& mcParticleLocation) const ;

  virtual Particle2MCParticle::ToVector 
  relatedMCPs(const LHCb::Particle* particle,
              const LHCb::MCParticle::Container& mcParticles) const ;

  virtual Particle2MCParticle::ToVector 
  relatedMCPs(const LHCb::Particle* particle,
              const LHCb::MCParticle::ConstVector& mcParticles) const ;

  virtual double associationWeight(const LHCb::Particle*,
                                   const LHCb::MCParticle* ) const;

private:

  inline LHCb::MCParticle::Container* 
  i_MCParticles(const std::string& location) const
  {
    return (exist<LHCb::MCParticle::Container>( location )) ? 
      get<LHCb::MCParticle::Container>( location ) : 0 ;
  }
  
  inline Particle2MCParticle::ToVector 
  i_relatedMCPs(const LHCb::Particle* particle,
                const std::string& mcParticleLocation) const
  {
    LHCb::MCParticle::Container* mcps = i_MCParticles(mcParticleLocation);
    return i_relatedMCPs( particle, mcps->begin(), mcps->end() );
  }

  template <typename Iter> 
  Particle2MCParticle::ToVector 
  i_relatedMCPs(const LHCb::Particle* particle,
                Iter begin,
                Iter end     ) const
  {
    Particle2MCParticle::ToVector associations;
    for ( Iter iMCP = begin ; iMCP != end ; ++iMCP){
      const double wt = associationWeight(particle, *iMCP);
      associations.push_back( MCAssociation(*iMCP, wt ) ); 
    }
    return i_sort(associations);
  }

private :

  Particle2MCParticle::ToVector 
  i_sort(const Particle2MCParticle::ToVector& associations) const
  {
    // should sort container by weight of MCAssociation
    return Particle2MCParticle::ToVector( associations.begin(), 
                                          associations.end()   );
  }

private:

  std::string m_defMCLoc;

};
#endif // PARTICLE2MCASSOCIATORBASE_H
