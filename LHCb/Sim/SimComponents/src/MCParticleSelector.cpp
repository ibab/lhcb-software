#include "Event/MCParticle.h"

// Gaudi
#include "MCParticleSelector.h"
#include "Event/MCFun.h"

#include "boost/limits.hpp"
#include "boost/numeric/conversion/bounds.hpp"

DECLARE_TOOL_FACTORY ( MCParticleSelector )

MCParticleSelector::MCParticleSelector( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent ) :
  GaudiTool( type, name, parent )
{
  // JOs
  declareProperty("zOrigin",      m_zOrigin      = boost::numeric::bounds<double>::highest() );
  declareProperty("pMin",         m_pMin         = 0.0*Gaudi::Units::GeV );
  declareProperty("pMax",         m_pMax         = boost::numeric::bounds<double>::highest() );
  declareProperty("betaGammaMin", m_betaGammaMin = 0.0 );
  declareProperty("etaMin",       m_etaMin       = -boost::numeric::bounds<double>::highest() );
  declareProperty("etaMax",       m_etaMax       =  boost::numeric::bounds<double>::highest() );
  declareProperty("rejectElectrons", m_rejectElectrons  = false );
  declareProperty("SelectChargedParticles", m_selCharged = true );
  declareProperty("SelectNeutralParticles", m_selNeutral = true );
  declareProperty("rejectInteractions", m_rejectInteractions = false );
  declareProperty("zInteraction", m_zInteraction = -boost::numeric::bounds<double>::highest() );
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
  info() << "MCParticle Momentum cut     : " << m_pMin/Gaudi::Units::GeV << " GeV/c < P < " << m_pMax << " GeV/c" << endmsg;
  info() << "           Beta * gamma cut : " << m_betaGammaMin << " < beta*gamma" << endmsg;
  info() << "           Eta cut          : " << m_etaMin << " < P < " << m_etaMax << endmsg;
  if ( m_rejectElectrons ) info() << "           Will reject electrons" << endmsg;
  if ( m_selBprods       ) info() << "           Will only select B decay products" << endmsg;
  if ( m_rejectInteractions ) info() << "           Will reject particles from interations before z=" << m_zInteraction << endmsg;
  if ( !m_selCharged ) info() << "           Will reject charged particles" << endmsg;
  if ( !m_selNeutral ) info() << "           Will reject neutral particles" << endmsg;

  return sc;
}

bool MCParticleSelector::accept(const LHCb::MCParticle* aParticle) const 
{
  // select particles of some quality ...

  // Check for NULL pointers
  if ( !aParticle ) return false;

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

  // beta * gamma - skip it for photons
  if( aParticle->virtualMass() > 1.*Gaudi::Units::keV ) {
    const double betaGamma = aParticle->betaGamma();
    if ( betaGamma < m_betaGammaMin ) return false;
  }

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
