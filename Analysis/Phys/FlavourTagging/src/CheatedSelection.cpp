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
  declareProperty( "BMassWindow", m_BMassWindow = 0.3 );
}

//=============================================================================
CheatedSelection::~CheatedSelection() {}; 

//=============================================================================
StatusCode CheatedSelection::initialize() {

  MsgStream req(msgSvc(), name());
  StatusCode sc;

  // Load all necessary tools via the base class
  sc = toolSvc()->retrieveTool( "Particle2MCLinksAsct", m_pAsctLinks, this);
  if( sc.isFailure() || 0 == m_pAsctLinks) {
    req << MSG::FATAL << "Unable to retrieve Link Associator tool" << endreq;
    return sc;
  }
  sc = toolSvc()->retrieveTool( "DebugTool", m_debug, this );
  if( sc.isFailure() ) {
    req << MSG::FATAL << "Unable to retrieve Debug tool" << endreq;
    return sc;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode CheatedSelection::execute() {
  
  setFilterPassed( false );

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

  //check what is the B forced to decay
  MCParticle* mcSignal = 0;
  SmartDataPtr<GenMCLinks> sigLinks(evtSvc(), GenMCLinkLocation::Default);
  if( 0 == sigLinks ) {
    req << MSG::DEBUG << "GenMCLinks not found at"
        << GenMCLinkLocation::Default << endreq;
    return StatusCode::SUCCESS;
  } 
  if( sigLinks->size() != 1 ) {
    req << MSG::DEBUG << "More than one signal found:"
	<< sigLinks->size()  << endreq;
  }
  for( GenMCLinks::iterator aLink = sigLinks->begin();
       sigLinks->end() != aLink; ++aLink ) {
    mcSignal = (*aLink)->signal();
    req << MSG::DEBUG << "mcSignal:" << endreq;
    m_debug -> printTree(mcSignal);
  }
  m_BID   = mcSignal->particleID().pid();
  m_BHEPm = mcSignal->momentum().m()/GeV;

  ////////////////////////////////////////////////////
  if ( !mcSignal ) {
    req << MSG::ERROR << "Missing Signal B in MC."<< endreq;
    return StatusCode::FAILURE;                      
  }
  ////////////////////////////////////////////////////

  const ParticleVector& Parts = desktop()->particles();
  const VertexVector& Verts = desktop()->vertices();
  req << MSG::DEBUG << "NParts=" << Parts.size()
      << "  NVerts=" << Verts.size()<< endreq;

  //----------------------------------------------------------------------
  // Retrieve MCParticles
  SmartDataPtr<MCParticles> mcpart (eventSvc(), MCParticleLocation::Default );
  if ( ! mcpart ) { 
    req << MSG::ERROR << "No MCParticles retrieved" << endreq;
    return StatusCode::FAILURE;
  }
  req << MSG::DEBUG << "Nr of MCParticles retrieved="<< mcpart->size()<< endreq;

  //----------------------------------------------------------------------
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
  VertB.setPosition( (*((mcSignal->endVertices()).begin()))->position() );

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

  setFilterPassed( true );
  req << MSG::DEBUG << "Reconstructed " << m_BID
      << " with m=" << candB.momentum().m()/GeV 
      << " p="      << candB.p()/GeV 
      << " pt="     << candB.pt()/GeV <<endreq;

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
	    if((*imc)->particleID().abspid() != axp->particleID().abspid()) {
	      req << MSG::DEBUG << "Mis-ID particle in signal:  " 
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
