// $Id: TrueMCFilterCriterion.h,v 1.12 2007-07-24 19:59:00 pkoppenb Exp $
#ifndef TRUEMCFILTERCRITERION_H 
#define TRUEMCFILTERCRITERION_H 1


// base class
#include "Kernel/FilterCriterionBase.h"

class IMCDecayFinder;
#include "Relations/RelationWeighted1D.h"

/** @class TrueMCFilterCriterion TrueMCFilterCriterion.h
 *  
 *  Filters particles that come from the wanted true decay
 *
 *  @author Patrick KOPPENBURG
 *  @date   2004-09-13
 *
 */
class TrueMCFilterCriterion : public FilterCriterionBase {

public: 

  /// Standard constructor
  TrueMCFilterCriterion( const std::string& type, 
                         const std::string& name,
                         const IInterface* parent);

  virtual ~TrueMCFilterCriterion( ); ///< Destructor
  /// Initialize
  StatusCode initialize( );
  

  /// Finalize
  StatusCode finalize( );

protected: // methods

  /// Test if filter is satisfied.
  virtual bool testParticle( const LHCb::Particle* const & part );

private:
  bool findMCParticle(const LHCb::MCParticle* MC); ///< Find associated MC particle in list of decay members
  bool preloadMCParticles();                       ///< Preload list of decay members


  IMCDecayFinder* m_pMCDecFinder ;        ///<  MCDecayFinder
  Particle2MCLinker* m_pLinker;   ///<  Link associator
  bool m_filterOut ;                      ///<  If true signal will be vetoed
  std::vector<std::string> m_particlePaths;  ///< paths of particles
  std::string m_mcParticlePath ; ///< path of MC particles
  
  const LHCb::MCParticle::Container* m_mcParticles ; ///< pointer to MC particles
  LHCb::MCParticle::Vector m_decayMembers ; ///< list of decay members in this decay
  int m_eventCount ; ///< Count the number of new events
  int m_foundDecay ; ///< Count number of times the decay has been found per event
  bool m_complain ;  ///< Complain about events without MC truth (i.e. for signal)
  
};
#endif // TRUEMCFILTERCRITERION_H
