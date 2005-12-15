#ifndef _MCPARTICLESELECTOR_H
#define _MCPARTICLESELECTOR_H

#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IMCParticleSelector.h"

/** @class MCParticleSelector MCParticleSelector.h MCTools/MCParticleSelector.h
 *
 *  Interface Class for selection of particles given a criteria
 *
 *  @author M.Needham
 *  @date   13/3/2002
 */

class MCParticleSelector : public GaudiTool, virtual public IMCParticleSelector {

public: 

  /// constructor
  MCParticleSelector(const std::string& type,
                     const std::string& name,
                     const IInterface* parent);
 
  virtual ~MCParticleSelector();

  /// select
  virtual bool accept(const LHCb::MCParticle* aParticle) const;

 private:

  double m_zOrigin;
  double m_pMin;
  double m_betaGammaMin;

  double m_etaMax;
  double m_etaMin; 

  bool m_rejectElectrons;

};

#endif // _MCPARTICLESELECTOR_H

