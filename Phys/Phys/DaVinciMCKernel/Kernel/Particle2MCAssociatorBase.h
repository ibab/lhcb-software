// $Id: Particle2MCAssociatorBase.h,v 1.17 2009-03-26 17:57:56 jpalac Exp $
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
 *  Common implementation for descendants of IParticle2MCWeightedAssociator.
 *  Mainly inline helper methods for common implementation of host of 
 *  similar methods in the interface.
 *  Set of methods is self-consistent. Derived classes only need to implement
 *  methods
 *  @code 
 *  virtual bool isAssociated(const LHCb::Particle*, 
 *                            const LHCb::MCParticle) const
 *  @endcode
 *  and
 *  @code
 *  virtual double associationWeight(const LHCb::Particle*,
                                     const LHCb::MCParticle* ) const
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


private:

  /**
   *
   * Return the association weight between an LHCb::Particle and an
   * LHCb::MCParticle
   *
   * @param particle pointer to LHCb::Particle to be associated
   * @param mcParticle pointer to LHCb::MCParticle to be associated
   * @return weight of the association. It should be a probability, therefore
   *         it is bounded by 0 and 1.
   *
   * @author Juan Palacios juan.palacios@nikhef.nl
   * @date   2009-26-03
   **/
  virtual double associationWeight(const LHCb::Particle*,
                                   const LHCb::MCParticle* ) const;

  virtual bool isAssociated(const LHCb::Particle*,
                            const LHCb::MCParticle* ) const;

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

  inline const LHCb::MCParticle* 
  i_bestMCPWithCheck(const Particle2MCParticle::ToVector& assoc) const
  {
    return (!assoc.empty() ) ? assoc.back().to() : 0;
  }
  

  template <typename Iter> 
  Particle2MCParticle::ToVector 
  i_relatedMCPs(const LHCb::Particle* particle,
                Iter begin,
                Iter end     ) const
  {
    Particle2MCParticle::ToVector associations;
    for ( Iter iMCP = begin ; iMCP != end ; ++iMCP){
      if (isAssociated(particle, *iMCP) ) {
        const double wt = associationWeight(particle, *iMCP);
        associations.push_back( MCAssociation(*iMCP, wt ) );
      }
    }
    i_normalise(associations);
    i_sort(associations);
    return associations;
  }


  inline void
  i_normalise(Particle2MCParticle::ToVector& associations) const
  {
    double weight_sum = std::accumulate( associations.begin(),
                                         associations.end(),
                                         0.,
                                         Particle2MCParticle::SumWeights() );
    for (Particle2MCParticle::ToVector::iterator iAssoc = associations.begin();
         iAssoc!=associations.end();
         ++iAssoc) {
      iAssoc->weight() /= weight_sum;
    }
    
  }
  

  inline void
  i_sort(Particle2MCParticle::ToVector& associations) const
  {
    std::stable_sort( associations.begin() , 
                      associations.end() , 
                      Particle2MCParticle::SortByWeight() ) ;
  }

private:

  std::string m_defMCLoc;

};
#endif // PARTICLE2MCASSOCIATORBASE_H
