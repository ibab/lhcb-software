// Include files 
#include "CheatedSelection.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CheatedSelection
//
// Author: Marco Musy
//-----------------------------------------------------------------------------

using namespace LHCb ;
using namespace Gaudi::Units;

// Declaration of Factory
DECLARE_ALGORITHM_FACTORY( CheatedSelection )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CheatedSelection::CheatedSelection( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : DaVinciAlgorithm ( name , pSvcLocator )
  , m_debug(0), m_linker(0), m_forcedBtool(0)
{
  declareProperty( "GeneratedBMassWindow", 
                   m_GenBMassWindow =  100.0 * Gaudi::Units::MeV ); 
  declareProperty( "ReconstructedBMassWindow", 
                   m_RecBMassWindow = 1000.0 * Gaudi::Units::MeV ); 
  declareProperty( "MomentumTolerance", m_tolerance = 0.10 ); // 10% of p

  declareProperty( "AssociatorInputData", m_setInputData );
  m_setInputData.clear();
  declareProperty( "InputLocation",
                   m_inputLocation = "/Event/Phys/TaggingPions/Particles");
}

//=============================================================================
CheatedSelection::~CheatedSelection() {}

//=============================================================================
StatusCode CheatedSelection::initialize() {

  debug() << "CheatedSelection initialize"<< endmsg;

  if(m_setInputData.size()==0) {
    fatal() << "AssociatorInputData property is not set!"<<endmsg;
    return StatusCode::FAILURE;
  }
  m_linker = new Particle2MCLinker( this, Particle2MCMethod::Links,
                                    m_setInputData );
  if( !m_linker ) {
    fatal()<< "Unable to retrieve Link Associator tool"<<endmsg;
    return StatusCode::FAILURE;
  }
  m_debug = tool<IPrintMCDecayTreeTool> ( "PrintMCDecayTreeTool", this );
  if( ! m_debug ) {
    fatal() << "Unable to retrieve Debug tool "<< endmsg;
    return StatusCode::FAILURE;
  }
  //m_forcedBtool = tool<IForcedBDecayTool> ( "ForcedBDecayTool", this );
  m_forcedBtool = tool<IBDecayTool> ( "BDecayTool", this );
  if( ! m_forcedBtool ) {
    fatal() << "Unable to retrieve BDecayTool tool "<< endmsg;
    return StatusCode::FAILURE;
  }

  return DaVinciAlgorithm::initialize() ;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode CheatedSelection::execute() {

  setFilterPassed( false );

  debug() << "Getting tagging particles saved in "<<m_inputLocation<< endmsg ;
  Particle::ConstVector parts ;
  const Particle::Range ptmp = get<Particle::Range>( m_inputLocation );
  for( Particle::Range::const_iterator ip=ptmp.begin(); 
       ip!=ptmp.end(); ++ip){
    const MCParticle* mcp= m_linker->first(*ip);
    if (mcp) debug()<<"part: "<< (*ip)->pt()<< "  mcpart: "<< mcp->pt()<<endmsg;
    parts.push_back(*ip);
  }

  ////////////////////////////////////////////////////
  //check what is the B forced to decay
  Particle::ConstVector axdaughter(0);
  MCParticle::Vector    mcdaughter(0) ;
  const MCParticle*     mcSignal = m_forcedBtool->forcedB();

  if(mcSignal){
    debug()<<"Found B="<<mcSignal->particleID().pid()<<endmsg;
  } else {
    warning()<<"No B forced to decay in this event. Skip."<<endmsg;
    return StatusCode::SUCCESS;
  }
  
  SignalTree( mcSignal, mcdaughter, axdaughter ); //fills daughter vectors

  Gaudi::LorentzVector ptotmc(0,0,0,0), ptot(0,0,0,0);
  for ( MCParticle::Vector::iterator imcpart = mcdaughter.begin();
        imcpart != mcdaughter.end(); ++imcpart) ptotmc += (*imcpart)->momentum();
  for( Particle::ConstVector::iterator ip = axdaughter.begin();
       ip != axdaughter.end(); ++ip ) ptot += (*ip)->momentum();

  if( fabs(ptotmc.M() - mcSignal->momentum().M())   < m_GenBMassWindow
      &&  fabs(ptot.M() - mcSignal->momentum().M()) < m_RecBMassWindow ) {
    //m_debug -> printTree(mcSignal);
  } else return StatusCode::SUCCESS;

  debug() << "Calculated signal MCmass= " <<ptotmc.M()
          << "   RECmass= " <<ptot.M()<<endmsg;

  //----------------------------------------------------------------------
  // Create candidate B

  Particle* candB = new Particle;
  Vertex* VertB   = new Vertex;
  candB->clearDaughters();
  VertB->clearOutgoingParticles( );
  for( Particle::ConstVector::iterator ip = axdaughter.begin();
       ip != axdaughter.end(); ++ip ) {
    VertB->addToOutgoingParticles( *ip );
    candB->addToDaughters( *ip );
  }

  //and set vertex position
  VertB->setPosition( mcSignal->endVertices().at(0)->position() );

  candB->setParticleID(mcSignal->particleID());
  candB->setEndVertex(VertB);
  candB->setMomentum(ptot);
  candB->setReferencePoint(mcSignal->endVertices().at(0)->position());

  debug() << "Reconstructed "<< candB->particleID().pid()
          << " with m="      << candB->momentum().M()
          << " p="           << candB->p()/Gaudi::Units::GeV
          << " pt="          << candB->pt()/Gaudi::Units::GeV <<endmsg;


  axdaughter.push_back(candB);
  debug()<<"Going to save this B hypo to TES with "<<axdaughter.size()-1
         <<" daughters."<<endmsg;
  // marke particles for savint to TES in location specified by jobOptions
  this->markNewTrees(axdaughter);

  setFilterPassed( true );
  return StatusCode::SUCCESS;
}
//=============================================================================
StatusCode CheatedSelection::finalize() {
  if( NULL != m_linker ) delete m_linker;
  return DaVinciAlgorithm::finalize();
}

//============================================================================
void CheatedSelection::SignalTree(const MCParticle* B0,
                                  MCParticle::Vector& mcdaughters,
                                  Particle::ConstVector& daughters) {
  daughters.clear();
  mcdaughters.clear();

  // Get MCParticle container
  MCParticles* mcpart = get<MCParticles>( MCParticleLocation::Default );
  MCParticles::const_iterator imc;
  for ( imc = mcpart->begin(); imc != mcpart->end(); ++imc ) {
    if( originof(*imc) == B0 ) {
      Particle* axp = m_linker->firstP( *imc );
      if( axp ) {

        if( ((*imc)->p() - axp->p()) /(*imc)->p() > m_tolerance ) continue;

        debug() << " mcp=" << (*imc)->p()/Gaudi::Units::GeV
                << " axp=" << axp->p()/Gaudi::Units::GeV    <<endmsg;
        if((*imc)->particleID().abspid() != axp->particleID().abspid()) {
          debug() << "Mis-ID true " << (*imc)->particleID().pid()
                  << " reconst as " << axp->particleID().pid() << endmsg;
          double mcmass = (*imc)->momentum().M();
          Gaudi::LorentzVector pax = axp->momentum();
          Gaudi::LorentzVector newpax( pax.x(), pax.y(), pax.z(),
                                       sqrt(pax.Vect().Mag2()+mcmass*mcmass) );
          axp->setMomentum(newpax); //OVERWRITE mass and PID
          axp->setParticleID((*imc)->particleID());
        }
        mcdaughters.push_back(*imc);
        daughters.push_back(axp);
      }      
    }
  }
}
//============================================================================
const MCParticle* CheatedSelection::originof( const MCParticle* product ) {
  const MCParticle* mother = product->mother();
  if ( (!mother) || product->particleID().hasBottom() ) return product;
  else return originof( mother );
}

//============================================================================
MCParticle* CheatedSelection::associatedofHEP(HepMC::GenParticle* hepmcp) {

  MCParticles* mcpart = get<MCParticles> ( MCParticleLocation::Default );

  int mid = hepmcp->pdg_id();
  double mothmom = hepmcp->momentum().rho();
  double moththeta = hepmcp->momentum().theta();
  MCParticles::const_iterator imc;
  for ( imc = mcpart->begin(); imc != mcpart->end(); ++imc ) {
    if( mid == (*imc)->particleID().pid() ) {
      if( fabs(mothmom - (*imc)->momentum().P())< 1.0){
        if( fabs(moththeta -(*imc)->momentum().Theta())< 0.0001){
          return (*imc);
        }
      }
    }
  }
  return 0;
}
//============================================================================
