// $Id: MCParticleSelector.cpp,v 1.7 2007-07-02 07:30:29 mneedham Exp $

#include "Event/MCParticle.h"

// Gaudi
#include "GaudiKernel/ToolFactory.h"

#include "MCParticleSelector.h"
#include "Event/MCFun.h"

DECLARE_TOOL_FACTORY ( MCParticleSelector );

MCParticleSelector::MCParticleSelector( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent ) :
  GaudiTool( type, name, parent )
{
  // constructor
  declareProperty("zOrigin",      m_zOrigin      = 9999.0*Gaudi::Units::cm );
  declareProperty("pMin",         m_pMin         = 1.0*Gaudi::Units::GeV );
  declareProperty("pMax",         m_pMax         = 999999*Gaudi::Units::GeV );
  declareProperty("betaGammaMin", m_betaGammaMin = 1.0 );
  declareProperty("etaMin",       m_etaMin       = -9999.);
  declareProperty("etaMax",       m_etaMax       = 9999. );
  declareProperty("rejectElectrons", m_rejectElectrons  = false );
  declareProperty("SelectChargedParticles", m_selCharged = true );
  declareProperty("SelectNeutralParticles", m_selNeutral = true );
  declareProperty("rejectInteractions", m_rejectInteractions = false);
  declareProperty("zInteraction", m_zInteraction = -9999.0*Gaudi::Units::m);

  // need a line here to get the interface correct !!!!
  declareInterface<IMCParticleSelector>(this);
}

MCParticleSelector::~MCParticleSelector() { }

bool MCParticleSelector::accept(const LHCb::MCParticle* aParticle) const 
{
  // select particles of some quality ...

  // charge selection
  const int charge = aParticle->particleID().threeCharge();
  if ( (0 == charge && !m_selNeutral) ||
       (0 != charge && !m_selCharged) ) return false;
                                                                           
  // origin vertex
  const LHCb::MCVertex* origin = aParticle->originVertex();
  if ( (NULL == origin) || 
       (origin->position().z() > m_zOrigin) ) return false;

  // momentum cuts
  const double tMomentum = aParticle->p();
  if ( tMomentum < m_pMin || tMomentum > m_pMax ) return false;
  
  // beta * gamma
  const double betaGamma = aParticle->beta() * aParticle->gamma();
  if ( betaGamma < m_betaGammaMin ) return false;

  // eta
  const double tEta = aParticle->pseudoRapidity();
  if ( tEta < m_etaMin || tEta > m_etaMax ) return false;

  // reject electrons ?
  if ( m_rejectElectrons &&
       (aParticle->particleID().abspid() == 11) ) return false;

  if (m_rejectInteractions == true && 
      LHCb::MC::zInteraction(aParticle) < m_zInteraction) return false;
  
  // all OK
  return true;
}
