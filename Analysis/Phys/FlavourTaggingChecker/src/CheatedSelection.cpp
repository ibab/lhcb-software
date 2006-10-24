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
//   , m_setDecay(false)
//   , m_evtTypeSvc(NULL)
//   , m_mcFinder(NULL)
, m_linkLinks(0)
{
  declareProperty( "BMassWindow", m_BMassWindow = 0.3 );
  declareProperty( "AssociatorInputData", m_setInputData );
//   declareProperty( "AssociatorInputData", m_setInputData );
//   declareProperty( "EvtCodeFromData",  m_fromData = true );
//   declareProperty( "EvtCode",          m_evtCode  = 0 );
  m_setInputData.clear();
}

//=============================================================================
CheatedSelection::~CheatedSelection() {}; 

//=============================================================================
StatusCode CheatedSelection::initialize() {

  debug() << "CheatedSelection initialize"<< endreq;

  m_linkLinks = new Particle2MCLinker( this, 
				       Particle2MCMethod::Links , 
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
  
  const Particle::ConstVector& parts = desktop()->particles();
  debug()<< "Parts=" << parts.size() << endreq;

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
       q!=hepVect->end(); ++q, ++iq) {
    for ( HepMC::GenEvent::particle_iterator 
	    p  = (*q)->pGenEvt()->particles_begin();
	    p != (*q)->pGenEvt()->particles_end();   ++p ) {
      if( (*p)->status() != 889 ) continue;
      debug()<<iq<<" search for: "<<(*p)->pdg_id()
	     <<"  p="<<(*p)->momentum().vect().mag()
	     <<"  th="<<(*p)->momentum().vect().theta()
	     <<"  status "<<(*p)->status()<<endreq;
      mcSignal = associatedofHEP(*p);
      if(mcSignal) break; 
    }
  }
  if(mcSignal){
    debug()<<"Found B="<<mcSignal->particleID().pid()<<endreq;
  } else return StatusCode::SUCCESS;

  SignalTree( parts, mcSignal, mcdaughter, axdaughter ); //fills daughter vectors

  Gaudi::LorentzVector ptotmc(0,0,0,0);
  for ( MCParticle::Vector::iterator imcpart = mcdaughter.begin(); 
	imcpart != mcdaughter.end(); imcpart++) 
        ptotmc += (*imcpart)->momentum();
  if( fabs(ptotmc.M() - mcSignal->momentum().M()) < m_BMassWindow) {
    debug() << "Using signal MCmass= " <<ptotmc.M()<<endreq;
    m_debug -> printTree(mcSignal);
  } else return StatusCode::SUCCESS;


//   if( m_fromData && !m_setDecay ) {
//     LHCb::GenHeader* header =
//       get<LHCb::GenHeader>( evtSvc(), LHCb::GenHeaderLocation::Default );
//     if( setDecayToFind( header->evType()) ) {
//       fatal() << " 'setDecayToFind' failed in 'execute' "<< endmsg;
//       return StatusCode::FAILURE;
//     }
//   }
//   if( m_mcFinder ) if( m_mcFinder->hasDecay() ){
//     debug()<<" Analysing decay: "<<m_mcFinder->decay()<< endmsg;

  // Get MCParticle container
//   SmartDataPtr<MCParticles> mcpart( eventSvc(), MCParticleLocation::Default);
//   MCParticles::const_iterator imc;
//   for ( imc = mcpart->begin(); imc != mcpart->end(); imc++ ) {
//     if( (*imc)->particleID().hasBottom() ) {
//       SignalTree( parts, *imc, mcdaughter, axdaughter ); //fills daughter vectors
//       Gaudi::LorentzVector ptotmc(0,0,0,0);
//       for ( MCParticle::Vector::iterator imcpart = mcdaughter.begin(); 
// 	    imcpart != mcdaughter.end(); imcpart++) 
// 	     ptotmc += (*imcpart)->momentum();
//       double MCmass = ptotmc.M();
//       if( fabs(MCmass - (*imc)->momentum().M()) < m_BMassWindow) {
// 	debug() << "Using signal MCmass= " <<MCmass/Gaudi::Units::GeV<<endreq;
// 	m_debug -> printTree(*imc);
// 	mcSignal = (*imc);
// 	break;
//       }
//     }
//   }
//   if(!mcSignal) return StatusCode::SUCCESS;
	  
  //----------------------------------------------------------------------
  // Create candidate B 

  Vertex* VertB = new Vertex;
  VertB->clearOutgoingParticles( );
  for( Particle::ConstVector::iterator ip = axdaughter.begin();
       ip != axdaughter.end(); ++ip ) {
    VertB->addToOutgoingParticles( *ip );
  }

  //and set vertex position  
  VertB->setPosition( mcSignal->endVertices().at(0)->position() );

  Particle* candB = new Particle;
  candB->setParticleID(mcSignal->particleID());
  candB->setEndVertex(VertB);
  candB->setMomentum(mcSignal->momentum());
  candB->setReferencePoint(mcSignal->endVertices().at(0)->position());

  debug() << "Reconstructed "<< candB->particleID().pid()
	  << " with m="      << candB->momentum().M()/Gaudi::Units::GeV
	  << " p="           << candB->p()/Gaudi::Units::GeV 
	  << " pt="          << candB->pt()/Gaudi::Units::GeV <<endreq;

  // save desktop to TES in location specified by jobOptions
  axdaughter.push_back(candB);
  debug()<<"Going to save this B hypo to TES with "<<axdaughter.size()
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
void CheatedSelection::SignalTree(const Particle::ConstVector& parts,
				  const MCParticle* B0, 
				  MCParticle::Vector& mcdaughters, 
				  Particle::ConstVector& daughters) {
  daughters.clear();
  mcdaughters.clear();

  // Get MCParticle container
  SmartDataPtr<MCParticles> mcpart( eventSvc(), MCParticleLocation::Default);
  MCParticles::const_iterator imc;
  for ( imc = mcpart->begin(); imc != mcpart->end(); imc++ ) {

    if( originof(*imc) == B0 ) {

      Particle* axp = 0;
      Particle::ConstVector::const_iterator ipart;
      for ( ipart = parts.begin(); ipart != parts.end(); ipart++){
	const MCParticle* mcPart = m_linkLinks->firstMCP( *ipart );
	if(mcPart==(*imc)) {
	  axp = const_cast<Particle*>(*ipart);
	  debug()<< "Part=" << (*ipart)->particleID().pid()
		 << " MCPart=" << (mcPart)->particleID().pid()
		 << " nr="<< m_linkLinks->associatedMCP( *ipart ) << endreq;
	}
      }
      if( axp ) {
	m_debug -> printAncestor(*imc);
	debug() << "mcp=" << (*imc)->p()/Gaudi::Units::GeV
		<< " axp=" << axp->p()/Gaudi::Units::GeV <<endreq;
	if((*imc)->particleID().abspid() != axp->particleID().abspid()) {
	  debug() << "Mis-ID true " << (*imc)->particleID().pid() 
		  << " as " << axp->particleID().pid() << endreq;
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
// StatusCode CheatedSelection::setDecayToFind( const int evtCode ) {

//   // Check if code exist
//   if( !(m_evtTypeSvc->typeExists( evtCode )) ) {
//     fatal() << "EvtCode " << evtCode << "is not known by the EvtTypeSvc"
//             << endmsg;
//     return StatusCode::FAILURE;
//   }

//   // Retrieve tool and set decay descriptor
//   m_mcFinder = tool<IMCDecayFinder>( "MCDecayFinder", this );
//   std::string sdecay = m_evtTypeSvc->decayDescriptor( evtCode );
//   if( (m_mcFinder->setDecay( sdecay )).isFailure() ) {
//     fatal() << "Unable to set decay for EvtCode " << evtCode << endmsg;
//     return StatusCode::FAILURE;
//   }

//   m_setDecay = true;
//   m_evtCode  = evtCode;   // in case called when reading data

//   return StatusCode::SUCCESS;
// }
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
