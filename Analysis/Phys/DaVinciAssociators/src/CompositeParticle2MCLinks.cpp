// $Id: CompositeParticle2MCLinks.cpp,v 1.4 2003-04-24 13:55:07 gcorti Exp $
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

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Initialise" << endmsg;

  StatusCode sc = toolSvc()->retrieveTool("Particle2MCLinksAsct", 
                                          "P2MCAsct",m_pAsct, this);
  if(sc.isFailure()){
    log << MSG::FATAL << " Unable to retrieve Associator tool" << endreq;
    return sc;
  }

  return sc;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode CompositeParticle2MCLinks::execute() {

  MsgStream  log( msgSvc(), name() );
  log << MSG::DEBUG << "==> Execute" << endmsg;

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
      log << MSG::DEBUG << "    Particles retrieved from " << *inp 
          << endmsg;
    }
    else {
      log << MSG::FATAL << "    *** Could not retrieve Particles from " 
          << *inp << endmsg;
      continue;
    }
    
    // loop on Parts and MCParts to match them
    if( m_pAsct->tableExists() ) {
      for( Particles::const_iterator pIt=parts->begin() ;
           parts->end() != pIt; pIt++) {
        if ( table->relations(*pIt).size()!=0 ) continue; // already in table
        MCParticle *mcp =  m_pAsct->associatedFrom( *pIt );
        if ( mcp!=0) { // copy from underlying associator
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
      log << MSG::FATAL << "     *** Could not register table " 
          << outputTable()
          << endmsg;
      delete table;
      return sc;
    } else {
      log << MSG::DEBUG << "     Registered table " 
          << outputTable() << endmsg;
    }
  } else {
    log << MSG::FATAL << "     *** Could not create table " 
        << outputTable()
        << endmsg;

  }
    
  return StatusCode::SUCCESS ;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode CompositeParticle2MCLinks::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Finalize" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
//  associate1 auxiliary method
//=============================================================================
bool 
CompositeParticle2MCLinks::associate1(const Particle *p, 
                                      const MCParticle *m, 
                                      Particle2MCAsct::Table* table) const {
  MsgStream  log( msgSvc(), name() );
  bool s = false;
  const ParticleID& idP = p->particleID();
  const ParticleID& idM = m->particleID();
  log << MSG::DEBUG << " checking particle " <<  p->key() 
      << " (" <<idP.pid() << ") "
      << " and MC Particle " << m->key()<< " (" <<idM.pid() << ") ";
  if ( idP.pid() == idM.pid() || 
       ( p->charge()==0 && idP.abspid() == idM.abspid()) ) {
    log << MSG::DEBUG << " potential match " ;
    const Vertex *v = p->endVertex();
    if (v==0) {
      log <<  " no daughter, check underlying particle associator" ;
      s = ( m ==  m_pAsct->associatedFrom(p) ); 
    } else {
      const SmartRefVector<Particle>& dau = v->products();
      const SmartRefVector<MCVertex>& vMC = m->endVertices(); 
      if (!vMC.empty()) {
        std::vector<const MCParticle*> mcdau;
        // check the sum of all endVertices???
        if (vMC[0]!=0) {
          const SmartRefVector<MCParticle>& d = vMC[0]->products();
          for (SmartRefVector<MCParticle>::const_iterator k=d.begin();
               k!=d.end();++k)
            mcdau.push_back(*k);
        }
        SmartRefVector<Particle>::const_iterator i;
        for (i = dau.begin();i != dau.end();++i) {
          std::vector<const MCParticle*>::iterator j = mcdau.begin();
          while (j != mcdau.end() && !associate1(*i,*j,table)) ++j;
          if (j==mcdau.end()) break; // doesn't match -- give up
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
    log  << " registered match " << endmsg;
  } else {
    log  << " NO match " << endmsg;

  }
  return s;
}
