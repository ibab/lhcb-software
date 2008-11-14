// $Id: MCParticleSelector.cpp,v 1.11 2008-11-14 12:03:43 cattanem Exp $

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
  // JOs
  declareProperty("zOrigin",      m_zOrigin      = 9999.0*Gaudi::Units::cm );
  declareProperty("pMin",         m_pMin         = 0.0*Gaudi::Units::GeV );
  declareProperty("pMax",         m_pMax         = 999999*Gaudi::Units::GeV );
  declareProperty("betaGammaMin", m_betaGammaMin = 0.0 );
  declareProperty("etaMin",       m_etaMin       = -9999.);
  declareProperty("etaMax",       m_etaMax       = 9999. );
  declareProperty("rejectElectrons", m_rejectElectrons  = false );
  declareProperty("SelectChargedParticles", m_selCharged = true );
  declareProperty("SelectNeutralParticles", m_selNeutral = true );
  declareProperty("rejectInteractions", m_rejectInteractions = false );
  declareProperty("zInteraction", m_zInteraction = -9999.0*Gaudi::Units::m );
  declareProperty("SelectOnlyBDecayProducts", m_selBprods = false );

  // interface
  declareInterface<IMCParticleSelector>(this);
}

MCParticleSelector::~MCParticleSelector() { }

StatusCode MCParticleSelector::initialize()
{
  // Initialize base class
  const StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) { return sc; }

  // printout selection criteria
  info() << "MCParticle Momentum cut     : " << m_pMin/Gaudi::Units::GeV << " GeV/c < P < " << m_pMax << " GeV/c" << endreq;
  info() << "           Beta * gamma cut : " << m_betaGammaMin << " < beta*gamma" << endreq;
  info() << "           Eta cut          : " << m_etaMin << " < P < " << m_etaMax << endreq;
  if ( m_rejectElectrons ) info() << "           Will reject electrons" << endreq;
  if ( m_selBprods       ) info() << "           Will only select B decay products" << endreq;
  if ( m_rejectInteractions ) info() << "           Will reject particles from interations before z=" << m_zInteraction << endreq;
  if ( !m_selCharged ) info() << "           Will reject charged particles" << endreq;
  if ( !m_selNeutral ) info() << "           Will reject neutral particles" << endreq;

  return sc;
}

bool MCParticleSelector::accept(const LHCb::MCParticle* aParticle) const 
{
  // select particles of some quality ...

  // Check for NULL pointers
  if ( !aParticle ) return false;

  // Sanity check, protects FPEs later
  if( 0. > aParticle->gamma() ) return false;

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

  // reject interactions
  if ( m_rejectInteractions && 
       LHCb::MC::zInteraction(aParticle) < m_zInteraction) return false;
  
  // select only b decay products ?
  if ( m_selBprods && !fromBdecay(aParticle) ) return false;
  
  // all OK
  return true;
}

bool MCParticleSelector::fromBdecay( const LHCb::MCParticle * aParticle ) const
{
  // loop back and see if there is a B in the history
  bool fromB = false;
  const LHCb::MCParticle * motherP = aParticle->mother();
  while ( motherP != NULL && false == fromB ) 
  {
    fromB   = motherP->particleID().hasBottom();
    motherP = motherP->mother();
  }
  return fromB;
}
