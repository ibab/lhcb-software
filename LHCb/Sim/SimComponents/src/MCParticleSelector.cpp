// $Id: MCParticleSelector.cpp,v 1.2 2006-05-08 15:01:53 cattanem Exp $

#include "Event/MCParticle.h"

// Gaudi
#include "GaudiKernel/ToolFactory.h"

#include "MCParticleSelector.h"

static const ToolFactory<MCParticleSelector>  s_factory;
const IToolFactory& MCParticleSelectorFactory = s_factory;

MCParticleSelector::MCParticleSelector( const std::string& type,
                                        const std::string& name, 
                                        const IInterface* parent ) : 
  GaudiTool( type, name, parent )
{
  // constructor
  declareProperty("zOrigin",m_zOrigin = 50.0*Gaudi::Units::cm);
  declareProperty("pMin", m_pMin = 1.0*Gaudi::Units::GeV);
  declareProperty("betaGammaMin", m_betaGammaMin = 1.0);  
  declareProperty("etaMin", m_etaMin = 1.9);
  declareProperty("etaMax", m_etaMax = 4.9);
  declareProperty("rejectElectrons", m_rejectElectrons = true);

  // need a line here to get the interface correct !!!!
  declareInterface<IMCParticleSelector>(this); 
}


MCParticleSelector::~MCParticleSelector() {
  // destructor
}

bool MCParticleSelector::accept(const LHCb::MCParticle* aParticle) const {

  // select particles of some quality ...
 
  if (aParticle->particleID().threeCharge() == 0) return false ;

  const LHCb::MCVertex* origin = aParticle->originVertex();
  if ((0 == origin) || (origin->position().z() > m_zOrigin)) return false; 

  double tMomentum = aParticle->p();
  if (tMomentum < m_pMin) return false;

  double betaGamma = aParticle->beta() * aParticle->gamma();
  if (betaGamma < m_betaGammaMin) return false;

  double tEta = aParticle->pseudoRapidity();
  if (tEta < m_etaMin || tEta > m_etaMax) return false;

  if ((m_rejectElectrons == true)&&
      (aParticle->particleID().abspid() == 11)) return false;

  return true;
}
