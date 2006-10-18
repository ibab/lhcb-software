// $Id: CompositeParticle2MCLinks.cpp,v 1.15 2006-10-18 14:57:49 jpalac Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/ParticleProperty.h"

// from Event
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"
#include "Event/Particle.h"
#include "Event/Vertex.h"

// local
#include "CompositeParticle2MCLinks.h"

using namespace LHCb;


//#define DEEPDEBUG

//-----------------------------------------------------------------------------
// Implementation file for class : CompositeParticle2MCLinks
//
// 03/09/2002 : Gerhard Raven
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<CompositeParticle2MCLinks>          s_factory ;
const        IAlgFactory& CompositeParticle2MCLinksFactory = s_factory ; 

#define _verbose if( msgLevel(MSG::VERBOSE) ) verbose()
#define _debug if( msgLevel(MSG::DEBUG) ) debug()
#define _info if( msgLevel(MSG::INFO) ) info()

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CompositeParticle2MCLinks::CompositeParticle2MCLinks( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : AsctAlgorithm ( name , pSvcLocator )
  , m_ppSvc( NULL )
  , m_p2MCLink(NULL)
  , m_p2MCComp(NULL)
  , m_gamma( 22 )
  , m_linkerTable(NULL)
  , m_nrel(0)
  , m_nass(0)
{
  m_outputTable = "Phys/Relations/CompPart2MCfromLinks" ;
  declareProperty( "AllowExtraMCPhotons",  m_allowExtraMCPhotons = false );
  declareProperty( "inclusiveMode",        m_inclusiveMode = false );
  declareProperty( "skipResonances",       m_skipResonances = false );
  declareProperty( "maxResonanceLifeTime", m_maxResonanceLifeTime = 1.e-18*Gaudi::Units::s );
  declareProperty( "AssociationMethod",    m_asctMethod = "Links");
  declareProperty( "IgnorePID",            m_ignorePID = false );
}

//=============================================================================
// Destructor
//=============================================================================
CompositeParticle2MCLinks::~CompositeParticle2MCLinks() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode CompositeParticle2MCLinks::initialize() {

  _debug << "==> Initialise" << endreq;

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
  _info << "Composite association uses "
        << Particle2MCMethod::extension[method] << " as input" << endreq;
  // This is a link helper for the underlying associator (default : Links)
  m_p2MCLink = new Object2MCLinker<>( this, method, m_inputData ) ;
  // This is a linker to itself, to check if Particles have already been associated
  m_p2MCComp = new Object2MCLinker<>( this, "",
                   Particle2MCMethod::extension[Particle2MCMethod::Composite], 
                                  m_inputData);
  
  m_ppSvc  = svc<IParticlePropertySvc>("ParticlePropertySvc");
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode CompositeParticle2MCLinks::finalize() {

  _debug << "==> Finalize" << endreq;
  if( NULL != m_p2MCLink ) delete m_p2MCLink;
  if( NULL != m_p2MCComp ) delete m_p2MCComp;
  m_p2MCLink = NULL;
  m_p2MCComp = NULL;
  return GaudiAlgorithm::finalize();
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode CompositeParticle2MCLinks::execute() {

  _debug << "==> Execute" << endreq;

  if( "" != outputTable() ) {
    return Error( "Usage of Relations in Particle Associators is obsolete, use Linkers instead");
  }

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
    m_linkerTable = m_p2MCLink->linkerTable( linkerContainer);

    // If there is no need to fill in any table for that container
    //    just go to the next
    if( NULL == m_linkerTable ) continue;

    SmartDataPtr<Particles> parts (eventSvc(), *inp);
    if( 0 == parts ) continue;
    int npp = parts->size();
    m_nrel = 0;
    m_nass = 0;

    _debug << npp << " Particles retrieved from " << *inp << endreq;
    
    // loop on Parts and MCParts to match them
    for( Particles::const_iterator pIt=parts->begin() ;
         parts->end() != pIt; pIt++) {
      // If the particle is apready in the table, don't look for it
      if( NULL != m_p2MCComp->first( *pIt )) continue;
      
      // Check for direct association
      const MCParticle *mcp =  m_p2MCLink->first( *pIt );
      if ( NULL != mcp) { // copy from underlying associator
        const ParticleID& idP = (*pIt)->particleID();
        bool counted = false;
        _verbose << "Direct association for " 
                 << objectName(*pIt) 
                 << " (" << idP.pid() << ") "
                 << " -> MCParts";
        while( NULL != mcp ) {          
          const ParticleID& idM = mcp->particleID();
          if ( m_ignorePID || idP.pid() == idM.pid() || 
               ( (*pIt)->charge()==0 && idP.abspid() == idM.abspid()) ) {
            if( !counted ) {
              ++m_nass;
              counted = true;
            }
            _verbose << " - " << mcp->key()
                     << " (" << idM.pid() << ")" ;
            if( NULL != m_linkerTable ) m_linkerTable->link( *pIt, mcp);
            ++m_nrel;
          } else {
            _verbose << " - " << mcp->key()
                     << " Bad PID (" << idM.pid() << ")" ;
          }
          mcp = m_p2MCLink->next();
        }
        _verbose << endreq;
      } else if( NULL != (*pIt)->endVertex()) {
        // If the particle has no daughters, useless to check matching daughters
        for (MCParticles::iterator m = mcParts->begin(); mcParts->end() != m; ++m) {
          if( associateTree(*pIt,*m)) break;
        }
      } else {
        _verbose << "Particle " << objectName(*pIt)
                 << " not considered: no direct asct and no endVertex"
                 << endreq;
      }
    }
    _debug << "Out of " << npp << " Particles in " << *inp << ", "
           << m_nass << " are associated, "
           << m_nrel << " relations found" << endreq;
  }    
  return StatusCode::SUCCESS ;
};

//=============================================================================
//  associateTree auxiliary method
//=============================================================================
bool 
CompositeParticle2MCLinks::associateTree(const Particle *p, 
                                      const MCParticle *m) {
  bool s = false;
  std::string asctType;
  const ParticleID& idP = p->particleID();
  const ParticleID& idM = m->particleID();
  if ( m_ignorePID || idP.pid() == idM.pid() || 
       ( p->charge()==0 && idP.abspid() == idM.abspid()) ) {
#ifdef DEEPDEBUG
    _verbose << " Checking particle " <<  objectName(p) 
             << " (" << idP.pid() << ") "
             << " and MCPart " 
             << m->key() << " (" <<idM.pid()
             << ") potential match " << endreq;
#endif
    // Check if these particles have already been associated
    if( m_p2MCComp->checkAssociation( p, m) ) {
      _verbose << "     Association already registered between Particle "
               << objectName(p) << " and MCPart "
               << m->key() << endreq;
      return true;
    }
    const Vertex *v = p->endVertex();
    if (v==0) {
#ifdef DEEPDEBUG
      _verbose << "     No daughter, check underlying particle associator" 
               << endreq;
#endif
      s = m_p2MCLink->checkAssociation(p, m); 
      asctType = "Direct";
    } else {
      asctType = "Composite";
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
          while (j != mcdau.end() && !associateTree(*i,*j)) ++j;
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
    _verbose <<  asctType << " association found: " 
             << objectName(p) 
             << " (" << idP.pid() << ") "
             << " -> MCPart - " 
             << m->key()
             << " (" <<idM.pid() << ")"
             << endreq;
    if( NULL != m_linkerTable ) m_linkerTable->link( p, m);
    ++m_nrel;
    ++m_nass;  
  }
  return s;
}


bool CompositeParticle2MCLinks::addDaughters( const Particle* p,
                                std::vector<const Particle*>& daughters) const
{
  const Vertex* v = p->endVertex();
  if ( v != 0 ) {
    const SmartRefVector<Particle>& dau = v->outgoingParticles();
    for (SmartRefVector<Particle>::const_iterator k = dau.begin();
         k != dau.end(); ++k ) {

      // if resonances should be skipped add daughters instead
      /*      if ( (*k)->isResonance() && 
           m_skipResonances && addDaughters ( *k, daughters ) );
	   else */ daughters.push_back( *k );
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
