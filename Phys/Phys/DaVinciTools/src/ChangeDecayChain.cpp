// from Gaudi
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"

#include "Event/ProtoParticle.h"
#include "DaVinciTools/IParticleStuffer.h"
#include "DaVinciTools/IDecayFinder.h"

// local
#include "ChangeDecayChain.h"
#include <assert.h>

//-----------------------------------------------------------------------------
// Implementation file for class : ChangeDecayChain
//
// 2004-07-26 : Luis Fernandez
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<ChangeDecayChain>          s_factory ;
const        IAlgFactory& ChangeDecayChainFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ChangeDecayChain::ChangeDecayChain( const std::string& name,
                      ISvcLocator* pSvcLocator)
  : DVAlgorithm ( name , pSvcLocator ),
    m_pDKFinder(0),
    m_oldPP(),
    m_newPP()
{
  declareProperty("Decay", m_Decay = "");
  declareProperty("Particles", m_oldNames);
  declareProperty("Replacements", m_newNames);

}
//=============================================================================
// Destructor
//=============================================================================
ChangeDecayChain::~ChangeDecayChain() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode ChangeDecayChain::initialize() {

  MsgStream msg(msgSvc(), name());

  IParticlePropertySvc* m_ppSvc;        ///< Reference to ParticlePropertySvc  
  StatusCode sc = service("ParticlePropertySvc", m_ppSvc);

  if( sc.isFailure() ) {
    msg << MSG::FATAL << "   Unable to locate Particle Property Service" 
        << endmsg;
    return sc;
  }

  sc = toolSvc()->retrieveTool("DecayFinder", m_pDKFinder, this);
  if(sc.isFailure()){
    msg << MSG::FATAL
        << "   Unable to retrieve DecayFinder" << endmsg;
    return sc;
  }

  m_pDKFinder->setDecay(m_Decay);

  msg << MSG::INFO << "   Will look for the decay: "
      << m_pDKFinder->decay() << endmsg;

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
    msg << MSG::INFO << "Going to replace " << *i << " by " << *j
        << endreq ;
    m_oldPP.push_back(ppi);
    m_newPP.push_back(ppj);
  }

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode ChangeDecayChain::execute() {

  MsgStream  msg(msgSvc(), name());

  setFilterPassed(false);

  msg << MSG::DEBUG << "HOLA" << endmsg;
  
  StatusCode sc = desktop()->getInput() ;
  if ( sc != StatusCode::SUCCESS ) msg << MSG::ERROR << "No Desktop" << endmsg;

  std::vector<Particle*> vecp = desktop()->particles();

  msg << MSG::DEBUG << "Retrieved " << vecp.size() << 
    " particles from desktop" << endmsg;

  // container that will hold the new head particles
  std::vector<Particle*> pshead;
  // container that will hold the new flagged particles
  std::vector<Particle*> psfinal;

  // container holding the head of the decay
  std::vector<Particle*> headvec;

  // start from beginning
  const Particle* iheadvec = NULL;

  while(m_pDKFinder->findDecay(vecp, iheadvec)){
    Particle* jheadvec = const_cast<Particle*>(iheadvec);
    headvec.push_back(jheadvec);
  }
  msg << MSG::DEBUG << "Found " << headvec.size() 
      << " " << m_pDKFinder->decay() << endmsg;

  // Number of flagged dk products 
  unsigned int nDKProducts = 0;
  
  // Mother (or head) of the decay
  unsigned int n = 0;
  std::vector<Particle*>::iterator ihead;

  // iterator for the old properties
  std::vector<ParticleProperty*>::const_iterator iprop ;

  //--------------------------------------------------------------
  // Loop over all the mothers
  for(ihead = headvec.begin();
      ihead != headvec.end();
      ihead++){

    n++;
    msg << MSG::DEBUG << "*** Looping over mother number " << n << endmsg;
    msg << MSG::DEBUG << "Mother ID = " 
        << (*ihead)->particleID().pid() << endmsg;

    //--------------------------------------------------------------
    // get final states
    std::vector<Particle*> DKProducts;
    std::vector<Particle*>::iterator iProducts;
    
    m_pDKFinder->decayMembers(*ihead, DKProducts);
    
    // Number of final states (must be flagged)
    msg << MSG::DEBUG << " Number of final states " 
        << DKProducts.size() << endmsg;

    nDKProducts = DKProducts.size();

    std::vector<bool> usedDKProducts(DKProducts.size(), false);
    std::vector<bool>::iterator iusedDKProducts;

    // We must have the number of flagged states
    // equal to the number of replacements minus 1 (the head)
    if ((m_newNames.size() - 1) != nDKProducts){
      msg << MSG::ERROR << "Number of stable particles "
        "should be equal to number of replacements minus one" 
          << endreq;
      return StatusCode::FAILURE;      
    }
    //--------------------------------------------------------------

    // flag no to check twice the same old property
    // for special case of identical flagged particles
    std::vector<bool> usedoldPP(m_oldPP.size(), false);
    std::vector<bool>::iterator iusedoldPP;

    //--------------------------------------------------------------
    // Loop over old properties
    for(iprop = m_oldPP.begin(), iusedoldPP = usedoldPP.begin(); 
        iprop != m_oldPP.end(), iusedoldPP != usedoldPP.end(); 
        iprop++, iusedoldPP++){
      
      msg << MSG::DEBUG << ">>> Considering old PID = " 
          << (*iprop)->pythiaID() << endmsg;

      int oldid = (*iprop)->pythiaID();

      // First change the head of the decay (i.e. the mother)
      if ((oldid == (*ihead)->particleID().pid())
          && (*iusedoldPP!=true)){

        // iterator for the new properties
        std::vector<ParticleProperty*>::iterator inewprop =
          m_newPP.begin()+(iprop-m_oldPP.begin());

        int newid = (*inewprop)->pythiaID() ;

        msg << MSG::DEBUG << "Found a particle of ID " << oldid
            << ", going to make it ID " << newid << endreq ;
        msg << MSG::VERBOSE << "Particle " << (*ihead)->particleID().pid()
            << " with momentum " << (*ihead)->momentum() << " m = "
            << (*ihead)->mass() << endreq ;
        
        // Make the clone for the head which in principle should be a composite
        Particle* headclone = (*ihead)->clone();
        // save head particle
        Particle* head = (*ihead);
        // the origin particle, last level
        Particle* phead = head; 
        // find the origin of this head particle
        Particle* poriginhead = dynamic_cast<Particle*>(head->origin());
        // check if this particle originate from another particle
        while(poriginhead){
          msg << MSG::VERBOSE << "This particle comes from another one" 
              << endmsg;
          phead = poriginhead;
          poriginhead = dynamic_cast<Particle*>(poriginhead->origin());
        }
        // Find proto-particle from which this last level particle has been made
        ContainedObject* orighead = phead->origin();
        const ProtoParticle* 
          pporighead = dynamic_cast<const ProtoParticle*>(orighead);

        // there is a protoparticle for the head
        if(pporighead){
          msg << MSG::VERBOSE << "Found protoparticle for the head" << endmsg;

          // clone by hand here
          HepLorentzVector quadri = phead->momentum();
          double newmass = (*inewprop)->mass();
          double oldP = quadri.rho() ;
          quadri.setE(sqrt(oldP*oldP + newmass*newmass));
          headclone->setParticleID((*inewprop)->pythiaID());
          headclone->setMass(newmass);
          headclone->setMomentum(quadri);
          msg << MSG::DEBUG
              << "Cloned the particle by hand from a protoparticle "
              << endreq ;

        } // if pporighead
        // there is no protoparticle: composite particle
        else{
          const Vertex* vx = phead->endVertex() ;

          // there is a decay vertex
          if(vx){                    
            particleStuffer()->fillParticle(*vx , *headclone, newid);
            msg << MSG::VERBOSE << "Using the particleStuffer"  << endreq ;
          } 
          else{
            msg << MSG::ERROR << "Particle with ID " << oldid
                << " has no protoparticle, nor endvertex" << endreq ;
            return StatusCode::FAILURE;
          }// else no dk vtx
        }// else no protoparticles

        // clone the head of the decay
        headclone->setOrigin(head);

        msg << MSG::DEBUG << "Clone ID " << headclone->particleID().pid()
            << " with momentum " << headclone->momentum() << " m = "
            << headclone->mass() << endreq ;
        msg << MSG::DEBUG << "from PID " << head->particleID().pid()
            << " with momentum " << head->momentum() << " m = "
            << head->mass() << endreq ;

        // fill container with the modified head
        pshead.push_back(headclone);
        // Do not break here because we need the daughters

        // flag no to check twice the same old property
        // for special case of identical flagged particles
        *iusedoldPP = true;

      }// end if same id for head and old part

      // Now look at the decay products of the head
      else{

        // Loop over the stable final states        
        for(iProducts = DKProducts.begin(), 
              iusedDKProducts = usedDKProducts.begin(); 
            iProducts != DKProducts.end(), 
              iusedDKProducts != usedDKProducts.end();
            iProducts++, 
              iusedDKProducts++){

          // check if the DKProducts has already been considered
          // for special case of identical flagged particles
          if ((oldid == (*iProducts)->particleID().pid())
              && (*iusedDKProducts!=true)
              && (*iusedoldPP!=true)){

            // iterator for the new properties
            std::vector<ParticleProperty*>::iterator inewprop =
              m_newPP.begin()+(iprop-m_oldPP.begin());
            
            int newid = (*inewprop)->pythiaID() ;
            
            msg << MSG::DEBUG << "Found a particle of ID " << oldid
                << ", going to make it ID " << newid << endreq ;
            msg << MSG::VERBOSE << "Particle " 
                << (*iProducts)->particleID().pid()
                << " with momentum " << (*iProducts)->momentum() << " m = "
                << (*iProducts)->mass() << endreq ;

            // Make the clone for the dk products
            Particle* clone = (*iProducts)->clone();
            // save final particle
            Particle* final = (*iProducts);
            // the origin particle, last level
            Particle* pfinal = final; 
            // find the origin of this final particle
            Particle* poriginfinal = dynamic_cast<Particle*>(final->origin());
            // check if this particle originate from another particle
            while(poriginfinal){
              msg << MSG::VERBOSE << "This particle comes from another one" 
                  << endmsg;
              pfinal = poriginfinal;
              poriginfinal = dynamic_cast<Particle*>(poriginfinal->origin());
            }
            // Find proto-part from which this last level particle has been made
            ContainedObject* origfinal = pfinal->origin();
            const ProtoParticle* 
              pporigfinal = dynamic_cast<const ProtoParticle*>(origfinal);

            // there is a protoparticle for the final
            if(pporigfinal){
              msg << MSG::VERBOSE << "Found protoparticle for the final state" 
                  << endmsg;

              // clone by hand here
              HepLorentzVector quadri = pfinal->momentum();
              double newmass = (*inewprop)->mass();
              double oldP = quadri.rho() ;
              quadri.setE(sqrt(oldP*oldP + newmass*newmass));
              clone->setParticleID((*inewprop)->pythiaID());
              clone->setMass(newmass);
              clone->setMomentum(quadri);
              msg << MSG::DEBUG
                  << "Cloned the particle by hand from a protoparticle "
                  << endreq ;
              
            } // if there is pporigfinal
            // there is no protoparticle: composite particle
            else{
              const Vertex* vx = pfinal->endVertex() ;
              // there is a decay vertex
              if(vx){                    
                particleStuffer()->fillParticle(*vx , *clone, newid);
                msg << MSG::VERBOSE << "Using the particleStuffer"  << endreq ;
              } 
              else{
                msg << MSG::ERROR << "Particle with ID " << oldid
                    << " has no protoparticle, nor endvertex" << endreq ;
                return StatusCode::FAILURE;
              }// else no dk vtx
            }// else no protoparticles

            // clone the flagged final states
            clone->setOrigin(final);
            
            msg << MSG::DEBUG << "Clone ID " << clone->particleID().pid()
                << " with momentum " << clone->momentum() << " m = "
                << clone->mass() << endreq ;
            msg << MSG::DEBUG << "from PID " << final->particleID().pid()
                << " with momentum " << final->momentum() << " m = "
                << final->mass() << endreq ;
            
            // fill container with the modified final state
            psfinal.push_back(clone);

            // special flag no to use twice the same flagged state
            // for special case of identical flagged particles
            *iusedDKProducts = true;

            // flag no to check twice the same old property
            // for special case of identical flagged particles
            *iusedoldPP = true;

          } // if id matches for final states
        } // loop over iProducts
      } // else not the head
    } // for iprop = m_oldPP.begin(); ...
    //--------------------------------------------------------------    
  } // loop over mother
  //--------------------------------------------------------------  

  // Now we have:
  // - container holding all the new head particles
  // -> std::vector<Particle*> pshead;
  // - container holding all the new flagged particles
  // -> std::vector<Particle*> psfinal;

  // Prepare the new flagged particles for the new decay vertex of the new head
  std::vector<SmartRefVector<Particle> > NewProducts(pshead.size());
  unsigned int m =0; // to insert nDKProducts in each of the NewProducts[j]
  unsigned int j =0; // to iterate over NewProducts
  std::vector<Particle*>::iterator ipsfinal; // to iterate over psfinal
    
  // loop over new final particles and fill NewProducts with the set of 
  // final particles for each decay of interest
  for(ipsfinal = psfinal.begin(); 
      ipsfinal != psfinal.end(); 
      ipsfinal++){
    if(nDKProducts>0){
      j = int(m/nDKProducts);
      if(j<headvec.size()){
        NewProducts[j].push_back(*ipsfinal);
      }
      m +=1;
    }
  }
  
  msg << MSG::DEBUG
      << "NewProducts.size() = "
      << NewProducts.size() << endmsg;

  for(j=0; j<headvec.size(); j++){
    msg << MSG::DEBUG
        << "NewProducts["<< j << "].size() = "
        << NewProducts[j].size() << endmsg;  
  }

  std::vector<Particle*>::iterator ipshead; // to iterate over pshead
  // loop over new head particles
  for(ipshead = pshead.begin(), j =0; 
      ipshead != pshead.end(), j < pshead.size(); 
      ipshead++, j++){
    msg << MSG::VERBOSE << "New head ID = " 
        << (*ipshead)->particleID().pid() << endmsg;
    // Decay vertex of the new head
    Vertex* psheadVtx = (*ipshead)->endVertex();
    // Clone the new head decay vertex
    Vertex* cloneVtx = psheadVtx->clone();
    // If head is a composite
    if (cloneVtx!=NULL) {
      // and stuff in the new final states with new IDs
      cloneVtx->setProducts(NewProducts[j]);
      msg <<  MSG::VERBOSE << "New head clone vtx z = " 
          << cloneVtx->position().z() 
          << " and chi2 = " << cloneVtx->chi2() << endmsg;
      int id = (*ipshead)->particleID().pid();
      particleStuffer()->fillParticle(*cloneVtx ,**ipshead, id);
    }// psheadVtx!=NULL
  }// for ipshead
  
//   // Cross check
//   // loop over new head particles
//   for(ipshead = pshead.begin(); 
//       ipshead != pshead.end(); 
//       ipshead++){
//     msg << MSG::INFO << "Final New head ID = " 
//         << (*ipshead)->particleID().pid() << endmsg;
//     // Decay vertex of the new head
//     Vertex* psheadVtx = (*ipshead)->endVertex();
//     // If head is a composite
//     if (psheadVtx!=NULL) {
//       msg <<  MSG::INFO << "Final New head vtx z = " 
//           << psheadVtx->position().z() 
//           << " and chi2 = " << psheadVtx->chi2() << endmsg;

//       SmartRefVector<Particle> Products = psheadVtx->products();
//       SmartRefVector<Particle>::iterator iproducts;
//       msg << MSG::INFO
//           << "Final Number of decay products of the new head = "
//           << Products.size() << endmsg;

//       for(iproducts = Products.begin(); 
//           iproducts != Products.end(); 
//           iproducts++){
//         msg << MSG::INFO << "Final New head daughter IDs = " 
//             << (*iproducts)->particleID().pid() << endmsg;   

//       } // for iproducts
//     }// psheadVtx!=NULL
//   }
  
  bool foundhead = (pshead.size()>0) ;
  if (foundhead) {
    msg << MSG::INFO << "Will save " << pshead.size() 
        << " new head particles" << endmsg;

  } else msg << MSG::DEBUG << "Writing nothing to output" << endmsg;

  sc = desktop()->saveTrees(pshead);
  if (sc) setFilterPassed(foundhead);
  
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode ChangeDecayChain::finalize() {

return StatusCode::SUCCESS;
}

//=============================================================================
