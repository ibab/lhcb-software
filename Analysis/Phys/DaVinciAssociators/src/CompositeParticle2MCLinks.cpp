// $Id: CompositeParticle2MCLinks.cpp,v 1.5 2003-05-26 11:38:38 phicharp Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

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


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CompositeParticle2MCLinks::CompositeParticle2MCLinks( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : AsctAlgorithm ( name , pSvcLocator )
  , m_gamma( 22 )
{
  m_outputTable = "Phys/Relations/CompPart2MCfromLinks" ;
  m_inputData.push_back( ParticleLocation::Production );
  declareProperty( "AllowExtraMCPhotons", m_allowExtraMCPhotons = false );
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

  StatusCode sc = retrievePrivateAsct("Particle2MCLinksAsct", true, m_pAsct);
  if(sc.isFailure()){
    msg << MSG::FATAL << " Unable to retrieve Associator tool" << endreq;
    return sc;
  }

  return sc;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode CompositeParticle2MCLinks::execute() {

  MsgStream  msg( msgSvc(), name() );
  msg << MSG::DEBUG << "==> Execute" << endmsg;

  // create an association table and register it in the TES
  Particle2MCAsct::Table* table = new Particle2MCAsct::Table();
  
  //////////////////////////////////
  // Loop on Particles containers //
  //////////////////////////////////

  SmartDataPtr<MCParticles> mcParts( eventSvc(), MCParticleLocation::Default );
  for( std::vector<std::string>::iterator inp = m_inputData.begin(); 
       m_inputData.end()!= inp; inp++) {
    // Get Particles
    SmartDataPtr<Particles> parts (eventSvc(), *inp);
    if( 0 != parts ) {
      msg << MSG::DEBUG << "    Particles retrieved from " << *inp 
          << endmsg;
    }
    else {
      msg << MSG::FATAL << "    *** Could not retrieve Particles from " 
          << *inp << endmsg;
      continue;
    }
    
    // loop on Parts and MCParts to match them
    if( m_pAsct->tableExists() ) {
      for( Particles::const_iterator pIt=parts->begin() ;
           parts->end() != pIt; pIt++) {
        if ( 0 != table->relations(*pIt).size() ) continue; // already in table
        MCParticle *mcp =  myAssociatedFrom( *pIt );
        if ( 0 != mcp) { // copy from underlying associator
          msg << MSG::DEBUG << "Direct association found between " <<  (*pIt)->key()
            << " (" << (*pIt)->particleID().pid() << ") "
            << " and MC Particle " << mcp->key() << " (" << mcp->particleID().pid() << ") "
            << endreq;
          table->relate(*pIt,mcp);
        } else {
          SmartDataPtr<MCParticles> mcParts( eventSvc(),
                                             MCParticleLocation::Default );
          for (MCParticles::iterator i = mcParts->begin();
               i!=mcParts->end(); ++i) {
            associate1(*pIt,*i,table);
          }
        }
      }
    } else {
      delete table;
      table = 0;
    }
  }
  if (table!=0) {
    // Register the table on the TES
    StatusCode sc = eventSvc()->registerObject( outputTable(), table);
    if( sc.isFailure() ) {
      msg << MSG::FATAL << "     *** Could not register table " 
          << outputTable()
          << endmsg;
      delete table;
      return sc;
    } else {
      msg << MSG::DEBUG << "     Registered table " 
          << outputTable() << endmsg;
    }
  } else {
    msg << MSG::FATAL << "     *** Could not create table " 
        << outputTable()
        << endmsg;

  }
    
  return StatusCode::SUCCESS ;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode CompositeParticle2MCLinks::finalize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "==> Finalize" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
//  associate1 auxiliary method
//=============================================================================
bool 
CompositeParticle2MCLinks::associate1(const Particle *p, 
                                      const MCParticle *m, 
                                      Particle2MCAsct::Table* table) const {
  MsgStream  msg( msgSvc(), name() );
  bool s = false;
  const ParticleID& idP = p->particleID();
  const ParticleID& idM = m->particleID();
  msg << MSG::VERBOSE << " Checking particle " <<  p->key() 
      << " (" << idP.pid() << ") "
      << " and MC Particle " << m->key() << " (" <<idM.pid() << ") ";
  if ( idP.pid() == idM.pid() || 
       ( p->charge()==0 && idP.abspid() == idM.abspid()) ) {
    msg << " potential match " ;
    const Vertex *v = p->endVertex();
    if (v==0) {
      msg <<  " no daughter, check underlying particle associator" ;
      s = isAssociatedFrom(p, m); 
    } else {
      const SmartRefVector<Particle>& dau = v->products();
      const SmartRefVector<MCVertex>& vMC = m->endVertices(); 
      if (!vMC.empty()) {
        std::vector<const MCParticle*> mcdau;
        // Get a vector of products of the first vertex
        // check the sum of all endVertices???
        if (vMC[0]!=0) {
          const SmartRefVector<MCParticle>& d = vMC[0]->products();
          for (SmartRefVector<MCParticle>::const_iterator k=d.begin();
               k!=d.end();++k)
            mcdau.push_back(*k);
        }
        SmartRefVector<Particle>::const_iterator i;
        // Loop on the Particle's daughters
        for (i = dau.begin();i != dau.end();++i) {
          // Loop on MCParticles daughters and check if they are associated
          std::vector<const MCParticle*>::iterator j = mcdau.begin();
          while (j != mcdau.end() && !associate1(*i,*j,table)) ++j;
          if (mcdau.end() == j) break; // doesn't match any MCParticle -- give up
          // Match found, remove it from the MC daughters
          mcdau.erase(j);
        }

        if (m_allowExtraMCPhotons) { // maybe add an energy threshold
                                     // for extraneous photons?
          while (!mcdau.empty() &&
                 mcdau.back()->particleID().pid() == m_gamma ) 
            mcdau.pop_back();
        }

        s = ( i == dau.end() && mcdau.empty() ) ; // all matched up, 
                                                  // nothing left
      }
    }
  }
  if (s) {
    table->relate(p,m);
    msg << endreq;
    msg << MSG::DEBUG << "Compos association found between " <<  p->key()
      << " (" << idP.pid() << ") "
      << " and MC Particle " << m->key() << " (" <<idM.pid() << ") "
      << endreq;
  } else {
    msg  << " NO match " << endmsg;

  }
  return s;
}

MCParticle*
CompositeParticle2MCLinks::myAssociatedFrom(const Particle* p) const
{
  MsgStream  msg( msgSvc(), name() );
  MCsFromParticleLinks mcRange = m_pAsct->rangeFrom(p);
  if( !mcRange.empty() ) {
    if( msg.level() <= MSG::DEBUG ) {
      if( mcRange.begin()->to() != mcRange.rbegin()->to() ) {
        msg << MSG::DEBUG << "Particle " << p->key() << " has more than one associated MCParticle" << endreq;
      }
    }
    return mcRange.rbegin()->to();
  }
  return NULL;
}

bool CompositeParticle2MCLinks::isAssociatedFrom( const Particle* p, const MCParticle* m) const
{
  MsgStream  msg( msgSvc(), name() );
  MCsFromParticleLinks mcRange = m_pAsct->rangeFrom(p);
  if( !mcRange.empty() ) {
    for( MCsFromParticleLinksIterator mcIt = mcRange.begin(); mcRange.end() != mcIt; mcIt++) {
      const MCParticle* mp = mcIt->to();
      if( m == mp ) return true;
    }
  }
  return false;
}
