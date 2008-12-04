#ifndef SIMCOMPONENTS_MCPARTICLESELECTOR_H
#define SIMCOMPONENTS_MCPARTICLESELECTOR_H

// base class
#include "GaudiAlg/GaudiTool.h"

// interface
#include "MCInterfaces/IMCParticleSelector.h"

/** @class MCParticleSelector MCParticleSelector.h
 *
 *  Class for selection of MCParticles given a criteria
 *
 *  @author M.Needham
 *  @date   4/12/2008 [revised] 
 *  @date   13/3/2002
 */

class MCParticleSelector : public GaudiTool,
                           virtual public IMCParticleSelector
{

public:

  /// constructor
  MCParticleSelector( const std::string& type,
                      const std::string& name,
                      const IInterface* parent );

  /// destructor
  virtual ~MCParticleSelector();

   // Tool initialization
  virtual StatusCode initialize(); 

public:

  // select
  virtual bool accept(const LHCb::MCParticle* aParticle) const;

private: // methods

  /// Determine if the given MCParticle is from a B decay or not
  bool fromBdecay( const LHCb::MCParticle * aParticle ) const;

private: // data

  double m_zOrigin;       ///< Maximum z position of origin vertex
  double m_pMin;          ///< Minimum momentum
  double m_pMax;          ///< Maximum momentum
  double m_betaGammaMin;  ///< Minimum cut on beta * gamma

  double m_etaMax;        ///< Max eta cut
  double m_etaMin;        ///< Min eta cut

  bool m_selCharged;      ///< Select charged particles
  bool m_selNeutral;      ///< Select neutral particles

  bool m_rejectElectrons;    ///< Reject electrons

  bool m_rejectInteractions; ///< Reject interactions
  double m_zInteraction;     ///< z of the interaction

  bool m_selBprods;         ///< Select only b decay products

};

#endif // SIMCOMPONENTS_MCPARTICLESELECTOR_H
