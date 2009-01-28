// $Id: GenericParticle2MCAssociator.h,v 1.1 2009-01-28 13:37:21 jpalac Exp $
#ifndef GENERICPARTICLE2MCASSOCIATOR_H 
#define GENERICPARTICLE2MCASSOCIATOR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IParticle2MCAssociator.h"            // Interface


/** @class GenericParticle2MCAssociator GenericParticle2MCAssociator.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2009-01-20
 */
template <typename AssocLogic>
class GenericParticle2MCAssociator : public GaudiTool, virtual public IParticle2MCAssociator {
public: 
  /// Standard constructor
  GenericParticle2MCAssociator( const std::string& type, 
                                const std::string& name,
                                const IInterface* parent)
    :
    GaudiTool(type, name, parent)
  {
    declareInterface<IParticle2MCAssociator>(this);
  }
  

  virtual ~GenericParticle2MCAssociator( ) {} ///< Destructor

  static const InterfaceID& interfaceID() { return IID_IParticle2MCAssociator; }

  virtual Particle2MCParticle::LightTable relatedMCPs(const LHCb::Particle* particle,
                                                      const std::string& mcParticleLocation) const
  {
    LHCb::MCParticle::Container* mcps = i_MCParticles(const std::string& mcParticleLocation);
    if (0!=mcps) {
      return i_relatedMCPs(particle,  mcps->begin(), mcps->end());
    } else {
      return Particle2MCParticle::LightTable();
    }   
  }

  virtual Particle2MCParticle::LightTable relatedMCPs(const LHCb::Particle* particle,
                                                      const LHCb::MCParticle::ConstVector& mcParticles) const
  {
    return i_relatedMCPs(particle,  mcParticles.begin(), mcParticles.end());
  }
  
  virtual Particle2MCParticle::LightTable associationTable(const LHCb::Particle::ConstVector& particles,
                                                           const std::string& mcParticleLocation) const
  {
    return i_associationTable(particles.begin(), particles.end(), mcParticleLocation);
  }
  
  virtual Particle2MCParticle::LightTable associationTable(const LHCb::Particle::Container& particles,
                                                           const std::string& mcParticleLocation) 
  {
    return i_associationTable(particles.begin(), particles.end(), mcParticleLocation);  
  }
  

  virtual Particle2MCParticle::LightTable associationTable(const LHCb::Particle::ConstVector& particles,
                                                           const LHCb::MCParticle::ConstVector& mcParticles) const
  {
    return i_associationTable(particles.begin(), particles.end(), mcParticles.begin(), mcParticles.end());      
  }
  

  virtual Particle2MCParticle::LightTable associationTable(const LHCb::Particle::Container& particles,
                                                           const LHCb::MCParticle::ConstVector& mcParticles) const
  {
    return i_associationTable(particles.begin(), particles.end(), mcParticles.begin(), mcParticles.end());          
  }
  

private:

  inline LHCb::MCParticle::Container* i_MCParticles(const std::string& location) const
  {
    return (exist<LHCb::MCParticle>( PVLocation )) ? get<LHCb::MCParticle>( PVLocation ) : 0 ;
  }
  

  template <typename Iter> 
  Particle2MCParticle::LightTable i_relatedMCPs(const LHCb::Particle* particle,
                                                Iter begin,
                                                Iter end     ) const
  {
    Particle2MCParticle::LightTable table;
    if (0!=particle) {
      for ( Iter iMCP = begin ; iMCP != end ; ++iMCP){
        const double wt = AssocLogic::weight(particle, *iMCP);
        if (wt > 0. ) {
          table.i_push(particle,*iMCP, wt );
        }
      }
      table.i_sort();
    } else {
      Warning("No particle!").ignore();
    }
    return table;
  }


  template <typename pIter, typename mcPIter>
  Particle2MCParticle::LightTable i_associationTable(const pIter pBegin, 
                                                     const pIter pEnd, 
                                                     const mcPIter mcBegin, 
                                                     const mcPIter mcEnd) const
  {

    Particle2MCParticle::LightTable table;

    for (pIter part = pBegin; part != pEnd; ++part) {
      for ( mcPIter iMCP = begin ; iMCP != end ; ++iMCP){
        const double wt = AssocLogic::weight(particle, *iMCP);
        if (wt > 0. ) {
          table.i_push( *part, *iMCP,  wt );
        }
      }
    }
    table.i_sort();
    return table;
  }
  
  template <typename Iter> 
  inline Particle2MCParticle::LightTable i_associationTable(const Iter pBegin,
                                                            const Iter pEnd,
                                                            const std::string& mcParticleLocation) const
  {
    LHCb::MCParticle::Container* mcps = i_MCParticles(const std::string& mcParticleLocation);
    if (0!=mcps) {
      return i_associationTable(pBegin, pEnd, mcps->begin(), mcps->end());
    } else {
      return Particle2MCParticle::LightTable();
    }
  }

};
#endif // GENERICPARTICLE2MCASSOCIATOR_H
