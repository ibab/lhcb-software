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

  m_pAsctLinks = tool<Particle2MCLinksAsct::IAsct>
                   ( "Particle2MCLinksAsct", "Cheating", this );
  if(0 == m_pAsctLinks) {
    fatal() << "Unable to retrieve Link Associator tool"<<endreq;
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
  GenMCLinks* sigLinks;
  if(exist<GenMCLinks> (GenMCLinkLocation::Default)) {
    sigLinks = get<GenMCLinks> (GenMCLinkLocation::Default);
  } else {
    err() << "Unable to Retrieve GenMCLinks" << endreq;
    return StatusCode::FAILURE;
  }
  MCParticle* mcSignal = (*(sigLinks->begin()))->signal();    
  if ( !mcSignal ) {
    err() << "No B was forced to decay in MC. "<< endreq;
    return StatusCode::FAILURE;
  }

  ////////////////////////////////////////////////////
  const ParticleVector& Parts = desktop()->particles();
  const VertexVector&   Verts = desktop()->primaryVertices();
  debug()<< "Parts=" << Parts.size() << "  NVerts=" << Verts.size()<< endreq;

  //----------------------------------------------------------------------
  // CheatedSelection 
  ParticleVector axdaughter(0);
  MCParticleVector mcdaughter ;
  
  m_BHEPm = mcSignal->momentum().m()/GeV;
  m_debug -> printTree(mcSignal);

  SignalTree( mcSignal, mcdaughter, axdaughter ); //fills daughter vectors

  HepLorentzVector ptotmc=0;
  MCParticleVector::const_iterator imcpart;
  for ( imcpart = mcdaughter.begin(); imcpart != mcdaughter.end(); imcpart++){
    ptotmc += (*imcpart)->momentum();
  }
  double MCmass = ptotmc.m()/GeV;
  debug() << "MCmass= " << MCmass << endreq;

  if(MCmass == 0) return StatusCode::SUCCESS;
  if( fabs(MCmass-m_BHEPm) > m_BMassWindow ) return StatusCode::SUCCESS;

  //----------------------------------------------------------------------
  // Create candidate B 

  Vertex VertB;
  VertB.clearProducts( );
  for( ParticleVector::const_iterator ip = axdaughter.begin();
       ip != axdaughter.end(); ++ip ) {
    VertB.addToProducts( *ip );
  }

  //and set vertex position  
  VertB.setPosition( (*((mcSignal->endVertices()).begin()))->position() );

  Particle candB;
  ParticleID BPID(mcSignal->particleID());
  StatusCode scStuff = particleStuffer()->fillParticle(VertB,candB,BPID); 
  if(scStuff.isFailure()) {
    warning() << "Failure in particleStuffer." << endreq;
    return StatusCode::SUCCESS;
  }
  // create selected B particle in desktop
  Particle* pDesktop = desktop()->createParticle(&candB);
  if(!pDesktop) {
    err() << "Unable to save selected B particle in desktop" << endreq;
    return StatusCode::SUCCESS;
  }
  debug() << "Reconstructed "<< candB.particleID().pid()
	  << " with m="      << candB.momentum().m()/GeV
	  << " p="           << candB.p()/GeV 
	  << " pt="          << candB.pt()/GeV <<endreq;

  // save desktop to TES in location specified by jobOptions
  axdaughter.push_back(pDesktop);
  StatusCode sc = desktop()->saveTrees(axdaughter);
  if (sc.isFailure()) {
    warning() << "Unable to save Tree to TES" << endreq;
    return StatusCode::SUCCESS;
  }

  setFilterPassed( true );
  return StatusCode::SUCCESS;
}
//=============================================================================
StatusCode CheatedSelection::finalize() { return StatusCode::SUCCESS; }

//============================================================================
void CheatedSelection::SignalTree(const MCParticle* B0, 
				  MCParticleVector& mcsons, 
				  ParticleVector& sons) {
  sons.clear();
  mcsons.clear();

  // Get MCParticle container
  SmartDataPtr<MCParticles> mcpart( eventSvc(), MCParticleLocation::Default);
  MCParticles::const_iterator imc;
  for ( imc = mcpart->begin(); imc != mcpart->end(); imc++ ) {

    if( originof(*imc) == B0 ) {
      if( m_pAsctLinks->tableExists() ) {
	Particle* axp = m_pAsctLinks->associatedTo( *imc );
	if( axp ) {
	  m_debug -> printAncestor(*imc);
	  debug() << " mcp=" << (*imc)->momentum().vect().mag()/GeV
		  << " axp=" << axp->p()/GeV <<endreq;
	  if((*imc)->particleID().abspid() != axp->particleID().abspid()) {
	    debug() << "Mis-ID true " << (*imc)->particleID().pid() 
		    << " as " << axp->particleID().pid() << endreq;
	    double mcmass = (*imc)->momentum().m();
	    HepLorentzVector pax = axp->momentum();
	    HepLorentzVector newpax( pax.x(), pax.y(), pax.z(),
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
//============================================================================
const MCParticle* CheatedSelection::originof( const MCParticle* product ) {
  const MCParticle* mother = product->mother();
  if ( (!mother) || product->particleID().hasBottom() ) return product; 
  else return originof( mother );
}
//============================================================================
