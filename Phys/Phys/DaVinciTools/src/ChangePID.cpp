// $Id: ChangePID.cpp,v 1.2 2004-07-09 13:50:10 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"

#include "Event/ProtoParticle.h"
#include "DaVinciTools/IParticleStuffer.h"

// local
#include "ChangePID.h"
#include <assert.h>

//-----------------------------------------------------------------------------
// Implementation file for class : ChangePID
//
// 2004-07-08 : Patrick KOPPENBURG
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<ChangePID>          s_factory ;
const        IAlgFactory& ChangePIDFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ChangePID::ChangePID( const std::string& name,
                      ISvcLocator* pSvcLocator)
  : DVAlgorithm ( name , pSvcLocator ),
    m_oldPP(),
    m_newPP()
{
  declareProperty( "Particles",    m_oldNames   );
  declareProperty( "Replacements", m_newNames   );

}
//=============================================================================
// Destructor
//=============================================================================
ChangePID::~ChangePID() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode ChangePID::initialize() {

  MsgStream msg(msgSvc(), name());

  IParticlePropertySvc* m_ppSvc;        ///< Reference to ParticlePropertySvc  
  StatusCode sc = service("ParticlePropertySvc", m_ppSvc);
  if( sc.isFailure() ) {
    msg << MSG::FATAL << "    Unable to locate Particle Property Service" 
        << endmsg;
    return sc;
  }

  std::vector<std::string>::const_iterator i;
  assert(m_oldNames.size()==m_newNames.size());

  for ( i = m_oldNames.begin(); i != m_oldNames.end(); ++i) {
    ParticleProperty *ppi = m_ppSvc->find(*i);
    if (ppi==0) {
      msg << MSG::FATAL << " Unable to retrieve particle property for " 
          << *i << endmsg;
      return StatusCode::FAILURE;
    }
    std::vector<std::string>::iterator j = 
      m_newNames.begin()+(i-m_oldNames.begin());

    ParticleProperty *ppj = m_ppSvc->find(*j);
    if (ppj==0) {
      msg << MSG::FATAL << " Unable to retrieve particle property for " 
          << *j << endmsg;
      return StatusCode::FAILURE;
    }
    msg << MSG::INFO << "Going to replace all " << *i << " by " << *j 
        << endreq ;
    m_oldPP.push_back(ppi);
    m_newPP.push_back(ppj);    
  }
  
  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode ChangePID::execute() {

  MsgStream  msg( msgSvc(), name() );
  setFilterPassed( false );
  
  StatusCode sc = desktop()->getInput() ;
  if ( sc != StatusCode::SUCCESS ) msg << MSG::ERROR << "No Desktop" << endreq ;
  typedef ParticleVector PV;
  PV pv = desktop()->particles();

  msg << MSG::DEBUG << "Retrieved " << pv.size() << 
    " particles from desktop" << endmsg;
  
  PV ps;
  PV::iterator i ;
  std::vector<ParticleProperty*>::const_iterator prop ;

  for ( i=pv.begin(); i!=pv.end(); i++ ) {
    if (*i==0) {
      msg << MSG::ERROR 
          << "Retrieved null particle ptr from desktop" << endmsg;
      return StatusCode::FAILURE ;
    }
    Particle* old = (*i);
    for ( prop = m_oldPP.begin(); prop != m_oldPP.end(); ++prop ) {
      int oldid = (*prop)->pythiaID() ;
      if ( oldid == old->particleID().pid() ) {
        std::vector<ParticleProperty*>::iterator newprop = 
          m_newPP.begin()+(prop-m_oldPP.begin());
        int newid = (*newprop)->pythiaID() ;
        msg << MSG::DEBUG << "Found a particle of ID " << oldid 
            << ", going to make it ID " << newid << endreq ;
        msg << MSG::VERBOSE << "Particle " << old->particleID().pid() 
            << " with momentum " << old->momentum() << " m=" 
            << old->mass() << endreq ;

        // Make the clone
        Particle* clone = old->clone();

        Particle* porigin = 
          dynamic_cast<Particle*>(old->origin());  // origin particle
        Particle* psaved = old ;                   // saved origin
        // wend while the origin is a particle
        while ( porigin ) {
          msg << MSG::VERBOSE << "Origin is a particle, getting up one level" 
              << endreq;
          psaved = porigin ;
          porigin = dynamic_cast<Particle*>(porigin->origin());
        }
        // psaved is now the last origin particle
        // now check for a protoparticle
        const ProtoParticle* pp = 
          dynamic_cast<const ProtoParticle*>(psaved->origin());
        if ( pp ){                        // There is a protoparticle
          HepLorentzVector quadri = psaved->momentum();
          double newmass = (*newprop)->mass() ;
          double oldP =  quadri.rho() ;
          quadri.setE( sqrt ( oldP*oldP + newmass*newmass ));
          clone->setParticleID( (*newprop)->pythiaID()  );
          clone->setMass( newmass );
          clone->setMomentum( quadri );
          msg << MSG::VERBOSE 
              << "Cloned the particle by hand from a protoparticle " 
              << endreq ;
          // particleStuffer()->fillParticle( *pp, *clone, newid ); 
        } else {                         // There is no protoparticle
          const Vertex* vx = psaved->endVertex() ;
          if ( vx ) {                    // There is a decay vertex
            particleStuffer()->fillParticle( *vx , *clone, newid );   
            msg << MSG::VERBOSE << "Using the particle Stuffer"  << endreq ;
          } else {
            msg << MSG::ERROR << "Particle with ID " << oldid 
                << " has no proptoarticle, nor endvertex" << endreq ;
            return StatusCode::FAILURE ;
          }
        }
        clone->setOrigin(old);

        msg << MSG::VERBOSE << "   Clone " << clone->particleID().pid() 
            << " with momentum " << clone->momentum() << " m=" 
            << clone->mass() << endreq ;
        msg << MSG::VERBOSE << "Part now " << old->particleID().pid() 
            << " with momentum " << old->momentum() << " m=" 
            << old->mass() << endreq ;  
        ps.push_back(clone);
        break ;
      }
    }
  }
  
  bool m_found = (ps.size()>0) ;
  
  if ( m_found ) {
    msg << MSG::INFO << "Saved " << ps.size() << " from " << pv.size() 
        << " candidates" << endmsg;   
  } else msg << MSG::DEBUG << "Writing nothing to output" << endmsg;

  sc = desktop()->saveTrees(ps);
  if ( sc ) setFilterPassed( m_found );

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode ChangePID::finalize() {

return StatusCode::SUCCESS;
}

//=============================================================================
