// $Id: CompositeParticle2MCLinks.cpp,v 1.9 2004-06-17 11:20:58 phicharp Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/ParticleProperty.h"

// from Event
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"
#include "Event/Particle.h"
#include "Event/Vertex.h"

// local
#include "CompositeParticle2MCLinks.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CompositeParticle2MCLinks
//
// 03/09/2002 : Gerhard Raven
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<CompositeParticle2MCLinks>          s_factory ;
const        IAlgFactory& CompositeParticle2MCLinksFactory = s_factory ; 

#define ifMsg(sev) msg << sev; if( msg.level() <= (sev) ) msg

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CompositeParticle2MCLinks::CompositeParticle2MCLinks( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : AsctAlgorithm ( name , pSvcLocator )
  , m_p2MCLink(NULL)
  , m_ppSvc( 0 )
  , m_gamma( 22 )
  , m_table(NULL)
{
  m_outputTable = "Phys/Relations/CompPart2MCfromLinks" ;
  declareProperty( "AllowExtraMCPhotons", m_allowExtraMCPhotons = false );
  declareProperty( "inclusiveMode", m_inclusiveMode = false );
  declareProperty( "skipResonances", m_skipResonances = false );
  declareProperty( "maxResonanceLifeTime", 
                   m_maxResonanceLifeTime = 1.e-18*s );
  declareProperty( "AssociationMethod", m_asctMethod = "Links");

}

//=============================================================================
// Destructor
//=============================================================================
CompositeParticle2MCLinks::~CompositeParticle2MCLinks() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode CompositeParticle2MCLinks::initialize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "==> Initialise" << endmsg;

  StatusCode sc = GaudiAlgorithm::initialize();
  if( !sc.isSuccess() ) return sc;

  m_asctMethod = "/" + m_asctMethod;
  int method = Particle2MCMethod::Links;
  for( int i=Particle2MCMethod::No; Particle2MCMethod::Max != i; i++) {
    if( Particle2MCMethod::extension[i] == m_asctMethod ) {
      method = i;
      break;
    }
  }
  msg << MSG::INFO << "Composite association uses "
      << Particle2MCMethod::extension[method] << " as input" << endreq;
  
  m_p2MCLink = new Object2MCLink( this, method, m_inputData ) ;
  m_ppSvc  = svc<IParticlePropertySvc>("ParticlePropertySvc");
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode CompositeParticle2MCLinks::finalize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "==> Finalize" << endmsg;
  if( NULL != m_p2MCLink ) delete m_p2MCLink;
  return GaudiAlgorithm::finalize();
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode CompositeParticle2MCLinks::execute() {

  MsgStream  msg( msgSvc(), name() );
  msg << MSG::DEBUG << "==> Execute" << endmsg;

  // create an association table and register it in the TES
  Particle2MCAsct::Table* m_table = 
    "" == outputTable() ? NULL : new Particle2MCAsct::Table();
  // get MCParticles
  MCParticles* mcParts = get<MCParticles>( MCParticleLocation::Default );

  //////////////////////////////////
  // Loop on Particles containers //
  //////////////////////////////////

  for( std::vector<std::string>::iterator inp = m_inputData.begin(); 
       m_inputData.end()!= inp; inp++) {
    // Get Particles
    // create a Linker table for that Particle container
    const std::string linkerContainer = 
      *inp + Particle2MCMethod::extension[Particle2MCMethod::Composite];
    // The actual constructor will be called in the linkerTable() method
    Object2MCLink::To test( eventSvc(), NULL, linkerContainer);
    m_linkerTable = m_p2MCLink->linkerTable( linkerContainer, test);

    // If there is no need to fill in any table for that container
    //    just go to the next
    if( NULL == m_table && NULL == m_linkerTable ) continue;

    SmartDataPtr<Particles> parts (eventSvc(), *inp);
    if( 0 == parts ) continue;
    int npp = parts->size();
    m_nrel = 0;

    ifMsg(MSG::DEBUG) << npp << " Particles retrieved from " << *inp << endmsg;
    
    // loop on Parts and MCParts to match them
    for( Particles::const_iterator pIt=parts->begin() ;
         parts->end() != pIt; pIt++) {
      // If the particle is apready in the table, don't look for it
      if( NULL != test.first( *pIt )) continue;
      
      // Check for direct association
      MCParticle *mcp =  m_p2MCLink->first( *pIt );
      if ( 0 != mcp) { // copy from underlying associator
        ifMsg(MSG::VERBOSE) << "Direct association found between " 
                            <<  (*pIt)->key()
                            << " (" << (*pIt)->particleID().pid() << ") "
                            << " and MC Particle " << mcp->key() << " (" 
                            << mcp->particleID().pid() << ") " << endreq;
        if( NULL != m_table ) m_table->relate( *pIt, mcp);
        if( NULL != m_linkerTable ) m_linkerTable->link( *pIt, mcp);
        m_nrel++;
      } else {                                             
        for (MCParticles::iterator i = mcParts->begin();
             i!=mcParts->end(); ++i) {
          associate1(*pIt,*i);
        }
      }
    }
  }
  
  // Register the table on the TES
  if( NULL != m_table ) {
    put( m_table, outputTable() );
    msg << MSG::DEBUG << "     Registered table " 
        << outputTable() << endmsg;
  }
    
  return StatusCode::SUCCESS ;
};

//=============================================================================
//  associate1 auxiliary method
//=============================================================================
bool 
CompositeParticle2MCLinks::associate1(const Particle *p, 
                                      const MCParticle *m) {
  MsgStream  msg( msgSvc(), name() );
  bool s = false;
  const ParticleID& idP = p->particleID();
  const ParticleID& idM = m->particleID();
#ifdef DEEPDEBUG
  ifMsg(MSG::VERBOSE) << " Checking particle " <<  p->key() 
      << " (" << idP.pid() << ") "
      << " and MC Particle " << m->key() << " (" <<idM.pid() << ") ";
#endif
  if ( idP.pid() == idM.pid() || 
       ( p->charge()==0 && idP.abspid() == idM.abspid()) ) {
#ifdef DEEPDEBUG
    ifMsg(MSG::VERBOSE) << " potential match " ;
#endif
    const Vertex *v = p->endVertex();
    if (v==0) {
#ifdef DEEPDEBUG
      ifMsg(MSG::VERBOSE) <<  
        " no daughter, check underlying particle associator" ;
#endif
      s = m_p2MCLink->checkAssociation(p, m); 
    } else {
      std::vector<const Particle*> dau;
      addDaughters ( p , dau );

      // Get a vector of products of the MCParticle
      // check the sum of all endVertices???
      std::vector<const MCParticle*> mcdau;
      if ( addMCDaughters ( m, mcdau ) ) {

        std::vector<const Particle*>::const_iterator i;
        // Loop on the Particle's daughters
        for (i = dau.begin();i != dau.end();++i) {
          // Loop on MCParticles daughters and check if they are associated
          std::vector<const MCParticle*>::iterator j = mcdau.begin();
          while (j != mcdau.end() && !associate1(*i,*j)) ++j;
          //doesn't match any MCParticle -- give up
          if (mcdau.end() == j) break; 
          // Match found, remove it from the MC daughters
          mcdau.erase(j);
        }

        if (m_allowExtraMCPhotons) { // maybe add an energy threshold
                                     // for extraneous photons?
          while (!mcdau.empty() &&
                  mcdau.back()->particleID().pid() == m_gamma ) 
            mcdau.pop_back();
        }

        // all matched up, nothing left
        s = ( i == dau.end() && ( m_inclusiveMode || mcdau.empty() ) ) ;        
      }
    }
  }
  if (s) {
    if( NULL != m_table ) m_table->relate( p, m);
    if( NULL != m_linkerTable ) m_linkerTable->link( p, m);
    m_nrel++;
    ifMsg(MSG::VERBOSE) << endreq 
                        << "Comp. association found between " <<  p->key()
      << " (" << idP.pid() << ") "
      << " and MC Particle " << m->key() << " (" <<idM.pid() << ") "
      << endreq;
#ifdef DEEPDEBUG
  } else {
    ifMsg(MSG::VERBOSE)  << " NO match " << endmsg;
#endif
  }
  return s;
}


bool CompositeParticle2MCLinks::addDaughters( const Particle* p,
                                std::vector<const Particle*>& daughters) const
{
  const Vertex* v = p->endVertex();
  if ( v != 0 ) {
    const SmartRefVector<Particle>& dau = v->products();
    for (SmartRefVector<Particle>::const_iterator k = dau.begin();
         k != dau.end(); ++k ) {

      // if resonances should be skipped add daughters instead
      if ( (*k)->isResonance() && 
           m_skipResonances && addDaughters ( *k, daughters ) );
      else daughters.push_back( *k );
    }
    return true;
  }
  return false;
}

bool CompositeParticle2MCLinks::addMCDaughters( const MCParticle* m,
                                std::vector<const MCParticle*>& daughters) const
{
  const SmartRefVector<MCVertex>& vMC = m->endVertices();
  if ( !vMC.empty() && vMC[0] != 0 ) {
    const SmartRefVector<MCParticle>& d = vMC[0]->products();
    for (SmartRefVector<MCParticle>::const_iterator k = d.begin();
         k != d.end(); ++k ) {

      // Use lifetime to determine if MCParticle is a resonance
      ParticleProperty* partProp = m_ppSvc ->
        findByStdHepID( (*k)->particleID().abspid() );
      bool resonance = false;
      if ( partProp != 0 && partProp->lifetime() < m_maxResonanceLifeTime ) {
        resonance = true;
      }      

      // if resonances should be skipped add daughters instead
      if ( resonance && m_skipResonances && addMCDaughters ( *k, daughters ) );
      else daughters.push_back( *k );
    }
    return true;
  }
  return false;
}
