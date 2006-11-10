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
, m_linkLinks(0)
{
  declareProperty( "BMassWindow", m_BMassWindow = 100.0 ); //MeV
  declareProperty( "AssociatorInputData", m_setInputData );
  m_setInputData.clear();
}

//=============================================================================
CheatedSelection::~CheatedSelection() {}; 

//=============================================================================
StatusCode CheatedSelection::initialize() {

  debug() << "CheatedSelection initialize"<< endreq;

  m_linkLinks = new Particle2MCLinker( this, 
				       Particle2MCMethod::Chi2,
				       //Particle2MCMethod::Links , 
				       m_setInputData);
  if( !m_linkLinks ) {
    fatal()<< "Unable to retrieve Link Associator tool"<<endreq;
    return StatusCode::FAILURE;
  }

  m_debug = tool<IDebugTool> ( "DebugTool", this );
  if( ! m_debug ) {
    fatal() << "Unable to retrieve Debug tool "<< endreq;
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
  
  ////////////////////////////////////////////////////
  //check what is the B forced to decay
  const MCParticle*     mcSignal = 0;
  Particle::ConstVector axdaughter(0);
  MCParticle::Vector    mcdaughter(0) ;

  SmartDataPtr<HepMCEvents> hepVect(eventSvc(), HepMCEventLocation::Default);
  if ( ! hepVect ) {
    fatal() << "This event has no HepMCEvent" << endreq;
    return StatusCode::FAILURE;
  }

  int iq=0;
  for( std::vector<LHCb::HepMCEvent*>::iterator q=hepVect->begin();
       q!=hepVect->end(); ++q ) {
    for ( HepMC::GenEvent::particle_iterator 
	    p  = (*q)->pGenEvt()->particles_begin();
	    p != (*q)->pGenEvt()->particles_end();   ++p ) {
      if( (*p)->status() != 889 ) continue;
      debug()<<"Search for: "<<(*p)->pdg_id()
	     <<"  p="<<(*p)->momentum().vect().mag()
	     <<"  th="<<(*p)->momentum().vect().theta()
	     <<"  status "<<(*p)->status()
	     <<"  nr:"<<iq<<endreq;
      mcSignal = associatedofHEP(*p);
      ++iq;
      if(mcSignal) break; 
    }
  }
  if(mcSignal){
    debug()<<"Found B="<<mcSignal->particleID().pid()<<endreq;
    if(iq>1) debug()<<"More than one B found?"<<endreq;
  } else return StatusCode::SUCCESS;

  SignalTree( mcSignal, mcdaughter, axdaughter ); //fills daughter vectors

  Gaudi::LorentzVector ptotmc(0,0,0,0);
  for ( MCParticle::Vector::iterator imcpart = mcdaughter.begin(); 
	imcpart != mcdaughter.end(); imcpart++) 
        ptotmc += (*imcpart)->momentum();
  debug() << "Calculated signal MCmass= " <<ptotmc.M()<<endreq;
  if( fabs(ptotmc.M() - mcSignal->momentum().M()) < m_BMassWindow) {
    m_debug -> printTree(mcSignal);
  } else return StatusCode::SUCCESS;
	  
  //----------------------------------------------------------------------
  // Create candidate B 

  Vertex* VertB = new Vertex;
  VertB->clearOutgoingParticles( );
  Gaudi::LorentzVector ptot(0,0,0,0);
  for( Particle::ConstVector::iterator ip = axdaughter.begin();
       ip != axdaughter.end(); ++ip ) {
    VertB->addToOutgoingParticles( *ip );
    ptot += (*ip)->momentum();
  }

  //and set vertex position  
  VertB->setPosition( mcSignal->endVertices().at(0)->position() );

  Particle* candB = new Particle;
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

  info() << "BTAGGING MCB   " 
	 << originof(mcdaughter.at(0))->particleID().pid()<<endreq;

  setFilterPassed( true );
  return StatusCode::SUCCESS;
}
//=============================================================================
StatusCode CheatedSelection::finalize() { 
  if( NULL != m_linkLinks ) delete m_linkLinks;
  return StatusCode::SUCCESS; 
}

//============================================================================
void CheatedSelection::SignalTree(const MCParticle* B0, 
				  MCParticle::Vector& mcdaughters, 
				  Particle::ConstVector& daughters) {
  daughters.clear();
  mcdaughters.clear();

  // Get MCParticle container
  SmartDataPtr<MCParticles> mcpart( eventSvc(), MCParticleLocation::Default);
  MCParticles::const_iterator imc;
  for ( imc = mcpart->begin(); imc != mcpart->end(); imc++ ) {

    if( originof(*imc) == B0 ) {

      Particle* axp = m_linkLinks->firstP( *imc ); 
      if( axp ) {
	m_debug -> printAncestor(*imc);
	debug() << "ID=" << axp->particleID().pid()
		<< " mcp=" << (*imc)->p()/Gaudi::Units::GeV
		<< " axp=" << axp->p()/Gaudi::Units::GeV <<endreq;
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

  SmartDataPtr<MCParticles> mcpart (eventSvc(), MCParticleLocation::Default );

  int mid = hepmcp->pdg_id();
  double mothmom = hepmcp->momentum().vect().mag();
  double moththeta = hepmcp->momentum().vect().theta();
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
