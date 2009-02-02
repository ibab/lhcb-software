// $Id: Particle2MCAssociatorBase.h,v 1.4 2009-02-02 14:33:13 jpalac Exp $
#ifndef PARTICLE2MCASSOCIATORBASE_H 
#define PARTICLE2MCASSOCIATORBASE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
// LHCb
#include "Event/MCParticle.h"
#include "Kernel/IParticle2MCAssociator.h"            // Interface


/** @class Particle2MCAssociatorBase Particle2MCAssociatorBase.h
 *  
 *  Common implementation for descendants of IParticle2MCAssociator.
 *  Mainly inline helper methods for common implementation of host of 
 *  similar methods in the interface.
 *  Set of methods is self-consistent. Derived classes only need to implement
 *  method
 *  @code 
 *  double weight(const LHCb::Particle*, const LHCb::MCParticle)
 *  @endcode
 *
 *  @author Juan PALACIOS
 *  @date   2009-01-30
 */
class Particle2MCAssociatorBase : public GaudiTool, 
                                  virtual public IParticle2MCAssociator {
public: 
  /// Standard constructor
  Particle2MCAssociatorBase( const std::string& type, 
                             const std::string& name,
                             const IInterface* parent);

  virtual StatusCode initialize() ;

  virtual StatusCode finalize() ;
  
  virtual ~Particle2MCAssociatorBase( );

  virtual Particle2MCParticle::ToVector 
  associate(const LHCb::Particle* particle) const ;

  virtual Particle2MCParticle::ToVector 
  associate(const LHCb::Particle* particle,
            const std::string& mcParticleLocation) const ;

  virtual Particle2MCParticle::LightTable 
  relatedMCPs(const LHCb::Particle*) const ;

  virtual Particle2MCParticle::LightTable 
  relatedMCPs(const LHCb::Particle*,
              const std::string& mcParticleLocation) const ;
  
  virtual Particle2MCParticle::LightTable 
  relatedMCPs(const LHCb::Particle*,
              const LHCb::MCParticle::ConstVector& mcParticles) const ;

  virtual Particle2MCParticle::LightTable 
  associations(const LHCb::Particle::ConstVector& particles) const ;
  
  virtual Particle2MCParticle::LightTable 
  associations(const LHCb::Particle::ConstVector& particles,
               const std::string& mcParticleLocation) const ;

  virtual Particle2MCParticle::LightTable 
  associations(const LHCb::Particle::ConstVector& particles,
               const LHCb::MCParticle::ConstVector& mcParticles) const ;

  virtual Particle2MCParticle::LightTable 
  associations(const LHCb::Particle::Container& particles) const ;

  virtual Particle2MCParticle::LightTable 
  associations(const LHCb::Particle::Container& particles,
               const std::string& mcParticleLocation) const ;

  virtual Particle2MCParticle::LightTable 
  associations(const LHCb::Particle::Container& particles,
               const LHCb::MCParticle::ConstVector& mcParticles) const ;

  virtual double 
  weight(const LHCb::Particle* particle, 
         const LHCb::MCParticle* mcParticle) const ;
  
private:

  inline LHCb::MCParticle::Container* 
  i_MCParticles(const std::string& location) const
  {
    return (exist<LHCb::MCParticle::Container>( location )) ? 
      get<LHCb::MCParticle::Container>( location ) : 0 ;
  }
  

  template <typename Iter> 
  Particle2MCParticle::LightTable 
  i_relatedMCPs(const LHCb::Particle* particle,
                Iter begin,
                Iter end     ) const
  {
    Particle2MCParticle::LightTable table;
    if (0!=particle) {
      for ( Iter iMCP = begin ; iMCP != end ; ++iMCP){
        const double wt = weight(particle, *iMCP);
        if (wt > 0. ) table.i_push(particle,*iMCP, wt );
      }
      table.i_sort();
    } else {
      Warning("No particle!").ignore();
    }
    return table;
  }

  template <typename pIter, typename mcPIter>
  Particle2MCParticle::LightTable 
  i_associations(const pIter pBegin, 
                     const pIter pEnd, 
                     const mcPIter mcBegin, 
                     const mcPIter mcEnd) const
  {

    Particle2MCParticle::LightTable table;

    for (pIter part = pBegin; part != pEnd; ++part) {
      for ( mcPIter iMCP = mcBegin ; iMCP != mcEnd ; ++iMCP){
        const double wt = weight(*part, *iMCP);
        if (wt > 0. ) {
          table.i_push( *part, *iMCP,  wt );
        }
      }
    }
    table.i_sort();
    return table;
  }
  
  template <typename Iter> 
  inline Particle2MCParticle::LightTable 
  i_associations(const Iter pBegin,
                     const Iter pEnd,
                     const std::string& mcParticleLocation) const
  {
    LHCb::MCParticle::Container* mcps = i_MCParticles(mcParticleLocation);
    if (0!=mcps) {
      return i_associations(pBegin, pEnd, mcps->begin(), mcps->end());
    } else {
      return Particle2MCParticle::LightTable();
    }
  }

private:

  std::string m_defMCLoc;

};
#endif // PARTICLE2MCASSOCIATORBASE_H
