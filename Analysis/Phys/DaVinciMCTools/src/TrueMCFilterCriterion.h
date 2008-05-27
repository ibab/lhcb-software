// $Id: TrueMCFilterCriterion.h,v 1.15 2008-05-27 13:32:56 pkoppenb Exp $
#ifndef TRUEMCFILTERCRITERION_H 
#define TRUEMCFILTERCRITERION_H 1


// base class
#include "Kernel/FilterCriterionBase.h"

class IMCDecayFinder;
#include "Relations/RelationWeighted1D.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"

/** @class TrueMCFilterCriterion TrueMCFilterCriterion.h
 *  
 *  Filters particles that come from the wanted true decay
 *
 *  @author Patrick KOPPENBURG
 *  @date   2004-09-13
 *
 */
class TrueMCFilterCriterion : public FilterCriterionBase,
                              virtual public IIncidentListener {

public: 

  /// Standard constructor
  TrueMCFilterCriterion( const std::string& type, 
                         const std::string& name,
                         const IInterface* parent);

  virtual ~TrueMCFilterCriterion( ); ///< Destructor
  /// Initialize
  StatusCode initialize( );
  
  /// Implement the handle method for the Incident service.
  void handle( const Incident& incident );

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
  LHCb::MCParticle::ConstVector m_decayMembers ; ///< list of decay members in this decay
  bool m_complain ;  ///< Complain about events without MC truth (i.e. for signal)
  int m_method ; ///< method to associate
  

};
#endif // TRUEMCFILTERCRITERION_H
