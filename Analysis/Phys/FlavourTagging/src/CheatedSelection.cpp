// Include files 
#include "CheatedSelection.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CheatedSelection
//
// Author: Marco Musy
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<CheatedSelection>          s_factory ;
const        IAlgFactory& CheatedSelectionFactory = s_factory ; 

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CheatedSelection::CheatedSelection( const std::string& name,
				    ISvcLocator* pSvcLocator)
  : DVAlgorithm ( name , pSvcLocator ) {
  declareProperty( "SignalB",     m_SignalB     = "none" );
  declareProperty( "BMassWindow", m_BMassWindow = 0.3 );
}

//=============================================================================
CheatedSelection::~CheatedSelection() {}; 

//=============================================================================
StatusCode CheatedSelection::initialize() {

  MsgStream req(msgSvc(), name());
  
  // Load all necessary tools via the base class
  StatusCode sc = loadTools();
  if( !sc ) {
    req << MSG::ERROR << "Unable to load tools" << endreq;
    return sc;
  }
  sc = toolSvc()->retrieveTool( "Particle2MCLinksAsct", m_pAsctLinks, this);
  if( sc.isFailure() || 0 == m_pAsctLinks) {
    req << MSG::FATAL << "Unable to retrieve Link Associator tool" << endreq;
    return sc;
  }
  sc = service("ParticlePropertySvc", ppSvc);
  if( !sc ) {
    req << MSG::FATAL << "Unable to locate Particle Property Service" <<endreq;
    return sc;
  }
  sc = toolSvc()->retrieveTool( "DebugTool", m_debug, this );
  if( sc.isFailure() ) {
    req << MSG::FATAL << "Unable to retrieve Debug tool" << endreq;
    return sc;
  }

  m_BID   = ppSvc->find(m_SignalB)->jetsetID();
  m_BHEPm = ppSvc->find(m_SignalB)->mass()/GeV;

  if(abs(m_BID)<500 || abs(m_BID)>550) {
     req << MSG::FATAL << "Please set SignalB property correctly." << endreq;
     return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode CheatedSelection::execute() {
  
  MsgStream  req( msgSvc(), name() );
  StatusCode sc = StatusCode::SUCCESS;  
  
  // Retrieve informations about event
  SmartDataPtr<EventHeader> evt( eventSvc(), EventHeaderLocation::Default );
  if ( !evt ) {   
    req << MSG::ERROR << "    Unable to retrieve event" << endreq;
    return StatusCode::FAILURE;
  }
  req << MSG::DEBUG << ">>>>>  Processing Event Nr " << evt->evtNum()
      << " Run " << evt->runNum() << "  <<<<<" << endreq;

  //----------------------------------------------------------------------
  // Retrieve MCParticles
  SmartDataPtr<MCParticles> mcpart (eventSvc(), MCParticleLocation::Default );
  if ( ! mcpart ) { 
    req << MSG::ERROR << "No MCParticles retrieved" << endreq;
    return StatusCode::FAILURE;
  }
  req << MSG::DEBUG << "Nr of MCParticles retrieved="<< mcpart->size()<< endreq;

 //----------------------------------------------------------------------
  // look for Signal B 
  MCParticle* B0 = 0 ;
  MCParticles::const_iterator imc;
  for ( imc = mcpart->begin(); imc != mcpart->end(); imc++ ) 
            if( (*imc)->particleID().pid() == m_BID )  B0 = (*imc);
  
  ////////////////////////////////////////////////////
  if ( !B0 ) {
    req << MSG::ERROR << "Missing Signal B in MC."<< endreq;
    return StatusCode::FAILURE;                      
  }
  ////////////////////////////////////////////////////

  req << MSG::DEBUG << "-------- Found B meson in MC: "<< m_SignalB << endreq;
  m_debug -> printTree(B0);

  // CheatedSelection ----------------------------
  HepLorentzVector ptot=0;
  HepLorentzVector ptotmc=0;
  ParticleVector axdaughter;
  axdaughter.clear();
  MCParticleVector mcdaughter ;
  SignalTree( mcdaughter, axdaughter ); //fills axdaughter

  ParticleVector::const_iterator ipart;
  MCParticleVector::const_iterator imcpart;
  for ( ipart = axdaughter.begin(); ipart != axdaughter.end(); ipart++){
    ptot += (*ipart)->momentum();
  }
  for ( imcpart = mcdaughter.begin(); imcpart != mcdaughter.end(); imcpart++){
    ptotmc += (*imcpart)->momentum();
  }
  req << MSG::DEBUG << "CheatedSelection: MCmass=" << ptotmc.m()/GeV
      << "  RECmass=" << ptot.m()/GeV << endreq;

  ///////////////////////////////////////////////////////////////////////////////
  if( fabs(ptotmc.m()/GeV-m_BHEPm) > m_BMassWindow ) return StatusCode::SUCCESS;
  //////////////////////////////////////////////////////////////////////////// 

  // Create Particle candidate B 
  Vertex VertB;
  VertB.clearProducts( );
  for( ParticleVector::const_iterator ip = axdaughter.begin();
       ip != axdaughter.end(); ++ip ) {
    VertB.addToProducts( *ip );
  }
  //and set vertex position  
  VertB.setPosition( (*((B0->endVertices()).begin()))->position() );

  Particle candB;
  ParticleID BPID(m_BID);
  StatusCode scStuff = particleStuffer()->fillParticle(VertB,candB,BPID); 
  if(scStuff.isFailure()) {
    req << MSG::WARNING << "Failure in particleStuffer." << endreq;
    return StatusCode::SUCCESS;
  }
  // create selected B particle in desktop
  Particle* pDesktop = desktop()->createParticle(&candB);
  if(!pDesktop) {
    req << MSG::ERROR 
	<< "Unable to save selected B particle in desktop" << endreq;
    return StatusCode::FAILURE;
  }

  req << MSG::INFO << "Reconstructed " << m_SignalB
      << " with m=" << candB.momentum().m()/GeV 
      << " p="      << candB.p()/GeV 
      << " pt="     << candB.pt()/GeV
      << " phi="    << candB.momentum().vect().phi() <<endreq;

  // save desktop to TES in location specified by jobOptions
  sc = desktop()->saveDesktop();
  if (sc.isFailure()) {
    req << MSG::ERROR << "Unable to save Desktop to TES" << endreq;
    return StatusCode::FAILURE;
  }
  req << MSG::DEBUG << "Event Saved to TES." <<endreq;

  return StatusCode::SUCCESS;
}
//=============================================================================
StatusCode CheatedSelection::finalize() { return StatusCode::SUCCESS; }

//============================================================================
void CheatedSelection::SignalTree(MCParticleVector& mcsons, ParticleVector& sons){

  MsgStream req( msgSvc(), name() );
  sons.clear();
  mcsons.clear();

  // Get MCParticle container
  SmartDataPtr<MCParticles> mcpart( eventSvc(), 
				    MCParticleLocation::Default);
  MCParticles::const_iterator imc;
  for ( imc = mcpart->begin(); imc != mcpart->end(); imc++ ) {
    const MCParticle* mcmother = originof(*imc);

    if( mcmother ) {
      if( mcmother->particleID().pid() == m_BID ) {
	if( m_pAsctLinks->tableExists() ) {
	  Particle* axp = m_pAsctLinks->associatedTo( *imc );
	  if( axp ) {
	    m_debug -> printAncestor(*imc);
	    req << MSG::DEBUG<< " mcp=" << (*imc)->momentum().vect().mag()/GeV
		<< " axp=" << axp->p()/GeV <<endreq;
	    if((*imc)->particleID().pid() != axp->particleID().pid()) {
	      req << MSG::INFO << "Mis-ID particle in signal:  " 
		  << axp->particleID().pid() << endreq;
	      double mcmass = (*imc)->momentum().m();
	      HepLorentzVector pax = axp->momentum();
	      HepLorentzVector newpax( pax.x(),
				       pax.y(),
				       pax.z(),
				       sqrt(pax.vect().mag2()+mcmass*mcmass) );
	      axp->setMomentum(newpax); //OVERWRITE mass and PID
	      axp->setParticleID((*imc)->particleID());
	    }
	    mcsons.push_back(*imc);
	    sons.push_back(axp);
	  }
	}
      }
    }
  }
};
//============================================================================
const MCParticle* CheatedSelection::originof( const MCParticle* product ) {
  const MCParticle* mother = product->mother();
  if ( (!mother) || product->particleID().hasBottom() ) return product; 
  else return originof( mother );
}
//============================================================================
