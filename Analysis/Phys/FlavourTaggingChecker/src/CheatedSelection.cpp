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
DECLARE_ALGORITHM_FACTORY( CheatedSelection );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CheatedSelection::CheatedSelection( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : DVAlgorithm ( name , pSvcLocator )
  , m_debug(0), m_linker(0), m_forcedBtool(0)
{
  declareProperty( "BMassWindow", m_BMassWindow = 100.0 * Gaudi::Units::MeV ); 
  declareProperty( "AssociatorInputData", m_setInputData );
  m_setInputData.clear();
  m_setInputData.push_back("Phys/CheatedSelection");
}

//=============================================================================
CheatedSelection::~CheatedSelection() {};

//=============================================================================
StatusCode CheatedSelection::initialize() {

  debug() << "CheatedSelection initialize"<< endreq;

  m_linker = new Particle2MCLinker( this,
                                    //Particle2MCMethod::Chi2,
                                    Particle2MCMethod::Links,
                                    m_setInputData );
  if( !m_linker ) {
    fatal()<< "Unable to retrieve Link Associator tool"<<endreq;
    return StatusCode::FAILURE;
  }
  m_debug = tool<IPrintMCDecayTreeTool> ( "PrintMCDecayTreeTool", this );
  if( ! m_debug ) {
    fatal() << "Unable to retrieve Debug tool "<< endreq;
    return StatusCode::FAILURE;
  }
  m_forcedBtool = tool<IForcedBDecayTool> ( "ForcedBDecayTool", this );
  if( ! m_forcedBtool ) {
    fatal() << "Unable to retrieve ForcedBDecayTool tool "<< endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode CheatedSelection::execute() {

  setFilterPassed( false );

  debug() << "-> Processing CheatedSelection " <<endreq;

  //look in location where Selection has put the B candidates
  const LHCb::Particle::ConstVector& parts = desktop()->particles();
  if( parts.empty() ) return StatusCode::SUCCESS;
  desktop()->saveDesktop();

  ////////////////////////////////////////////////////
  //check what is the B forced to decay
  Particle::ConstVector axdaughter(0);
  MCParticle::Vector    mcdaughter(0) ;
  const MCParticle*     mcSignal = m_forcedBtool->forcedB();

  if(mcSignal){
    debug()<<"Found B="<<mcSignal->particleID().pid()<<endreq;
  } else return StatusCode::SUCCESS;

  SignalTree( mcSignal, mcdaughter, axdaughter ); //fills daughter vectors

  Gaudi::LorentzVector ptotmc(0,0,0,0);
  for ( MCParticle::Vector::iterator imcpart = mcdaughter.begin();
        imcpart != mcdaughter.end(); imcpart++)
    ptotmc += (*imcpart)->momentum();
  debug() << "Calculated signal MCmass= " <<ptotmc.M()<<endreq;
  if( fabs(ptotmc.M() - mcSignal->momentum().M()) < m_BMassWindow) {
    //m_debug -> printTree(mcSignal);
  } else return StatusCode::SUCCESS;

  //----------------------------------------------------------------------
  // Create candidate B

  Particle* candB = new Particle;
  Vertex* VertB   = new Vertex;
  candB->clearDaughters();
  VertB->clearOutgoingParticles( );
  Gaudi::LorentzVector ptot(0,0,0,0);
  for( Particle::ConstVector::iterator ip = axdaughter.begin();
       ip != axdaughter.end(); ++ip ) {
    VertB->addToOutgoingParticles( *ip );
    candB->addToDaughters( *ip );
    ptot += (*ip)->momentum();
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
          << " pt="          << candB->pt()/Gaudi::Units::GeV <<endreq;

  // save desktop to TES in location specified by jobOptions
  axdaughter.push_back(candB);
  debug()<<"Going to save this B hypo to TES with "<<axdaughter.size()-1
         <<" daughters."<<endreq;
  StatusCode sc = desktop()->saveTrees(axdaughter);
  if (sc.isFailure()) {
    warning() << "Unable to save Tree to TES" << endreq;
    return StatusCode::SUCCESS;
  }

  info() << "BTAGGING MCB   " << mcSignal->particleID().pid()<<endreq;

  setFilterPassed( true );
  return StatusCode::SUCCESS;
}
//=============================================================================
StatusCode CheatedSelection::finalize() {
  if( NULL != m_linker ) delete m_linker;
  return StatusCode::SUCCESS;
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
  for ( imc = mcpart->begin(); imc != mcpart->end(); imc++ ) {
    if( originof(*imc) == B0 ) {
      Particle* axp = m_linker->firstP( *imc );
      if( axp ) {
        //m_debug -> printAncestor(*imc);
        debug() << "ID=" << axp->particleID().pid()
                << " mcp=" << (*imc)->p()/Gaudi::Units::GeV
                << " axp=" << axp->p()/Gaudi::Units::GeV<<endreq;
        if((*imc)->particleID().abspid() != axp->particleID().abspid()) {
          debug() << "Mis-ID true " << (*imc)->particleID().pid()
                  << " reconst as " << axp->particleID().pid() << endreq;
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
  double mothmom = hepmcp->momentum().mag();
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
