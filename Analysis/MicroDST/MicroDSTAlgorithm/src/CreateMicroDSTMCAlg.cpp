// $Id: CreateMicroDSTMCAlg.cpp,v 1.7 2007-04-13 15:13:34 ukerzel Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

// local
#include "CreateMicroDSTMCAlg.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CreateMicroDSTMCAlg
//
// 2007-02-09 : Ulrich Kerzel
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CreateMicroDSTMCAlg );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CreateMicroDSTMCAlg::CreateMicroDSTMCAlg( const std::string& name,
                                      ISvcLocator* pSvcLocator)
  : DVAlgorithm                    ( name , pSvcLocator  ),
    m_OutputPrefix                 ( "microDST"          ),
    m_CloneEndVertices             ( false               ),
    m_CloneVertexProducts          ( false               ),
    m_part2MCLinkerComposite       ( 0                   ),
    m_part2MCLinkerLinks           ( 0                   ),
    m_nBasicParticles              ( 0                   ),
    m_nCompositeParticles          ( 0                   ),
    m_nBasicParticlesAssociated    ( 0                   ),
    m_nCompositeParticlesAssociated( 0                   )
{
 
  declareProperty( "OutputPrefix"        , m_OutputPrefix        = "microDST" );
  declareProperty( "CloneEndVertices"    , m_CloneEndVertices    = false      );
  declareProperty( "CloneVertexProducts" , m_CloneVertexProducts = false      );
 
} //constructor
//=============================================================================
// Destructor
//=============================================================================
CreateMicroDSTMCAlg::~CreateMicroDSTMCAlg() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode CreateMicroDSTMCAlg::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  info() << "output prefix for microDST           " << m_OutputPrefix        << endmsg;
  info() << "clone end vertices of particles      " << m_CloneEndVertices    << endmsg;
  info() << "clone product particles of vertices  " << m_CloneVertexProducts << endmsg;

  // initialise associator to MC
  m_part2MCLinkerComposite = new Particle2MCLinker(this,Particle2MCMethod::Composite,"")  ;
  m_part2MCLinkerLinks     = new Particle2MCLinker(this,Particle2MCMethod::Links,"")  ;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode CreateMicroDSTMCAlg::execute() {

  debug() << "==> Execute" << endmsg;

  StatusCode sc;

  //
  // get particles in input-location(s) from PhysDesktop
  //

  LHCb::Particle::ConstVector particles = desktop()->particles();
  debug() << "got #particles  " << particles.size() << endmsg;
  if ( particles.size()<= 0) {
    verbose() << "no particle found, skip" << endmsg;
    return StatusCode::SUCCESS;
  }//if nBsCand

  LHCb::Particle::ConstVector::const_iterator iParticle;
  LHCb::Particle::ConstVector::const_iterator iParticleBegin = particles.begin();
  LHCb::Particle::ConstVector::const_iterator iParticleEnd   = particles.end();


  for (iParticle = iParticleBegin; iParticle != iParticleEnd; iParticle++){
    
    std::string locTES = CreateMicroDSTMCAlg::objectLocation((*iParticle)->parent());

    verbose() << "now call StoreMCParticle for particle fom location " << locTES << endmsg;
    sc = CreateMicroDSTMCAlg::StoreMCParticle(*iParticle); 
    if (sc != StatusCode::SUCCESS) {
      Warning("something went wrong when storing particle", StatusCode::SUCCESS);
    }// if sc
    
  }// for iParticle


  setFilterPassed(true);   // accept event
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode CreateMicroDSTMCAlg::finalize() {

  debug() << "==> Finalize" << endmsg;

  info() << "# associations attempted for basic particles     " << m_nBasicParticles               << endmsg;
  info() << "  #successful                                    " << m_nBasicParticlesAssociated     << endmsg;
  info() << "# associations attempted for composite particles " << m_nCompositeParticles           << endmsg;
  info() << "  #successful                                    " << m_nCompositeParticlesAssociated << endmsg;

  //
  // clean up
  //
  if (m_part2MCLinkerComposite)
    delete m_part2MCLinkerComposite;
  if (m_part2MCLinkerLinks)
    delete m_part2MCLinkerLinks;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}



//=============================================================================
StatusCode CreateMicroDSTMCAlg::StoreMCParticle(const LHCb::Particle * particle) {

   
  verbose() << "StoreMCParticle for particle with key " << particle->key()
            << " PID " << particle->particleID().pid()
            << "   m " << particle->measuredMass()/Gaudi::Units::GeV
            << endmsg;

  StatusCode sc;
  
  std::string locTES; // location on TES

  
  // get MC particles for this particle 
  std::vector<const LHCb::MCParticle*> associatedParticles;

  if (particle->isBasicParticle()) {
    verbose() << "get associated MC particles using linker: Links" << endmsg;
    associatedParticles = CreateMicroDSTMCAlg::GetMCParticle(particle,m_part2MCLinkerLinks);

  } else {
    verbose() << "get associated MC particles using linker: composite" << endmsg;
    associatedParticles = CreateMicroDSTMCAlg::GetMCParticle(particle,m_part2MCLinkerComposite);

  } // if isBasicParticle

  verbose() << "got #associated particles " << associatedParticles.size() << endmsg;
  if (associatedParticles.size() > 0) {

    std::vector<const LHCb::MCParticle*>::const_iterator iMCPart;
    std::vector<const LHCb::MCParticle*>::const_iterator iMCPartBegin = associatedParticles.begin();
    std::vector<const LHCb::MCParticle*>::const_iterator iMCPartEnd   = associatedParticles.end();

    locTES = CreateMicroDSTMCAlg::objectLocation((*iMCPartBegin)->parent());
    verbose() << "associated MC particles are on TES in " << locTES << endmsg;

    
    for (iMCPart = iMCPartBegin; iMCPart != iMCPartEnd; iMCPart++){
      verbose() << "now store associated true particle with PID " << (*iMCPart)->particleID().pid() << endmsg;
      
      LHCb::MCParticle* mcParticleClone = CreateMicroDSTMCAlg::StoreMCParticle(*iMCPart, true);
      verbose() << "got clone of stored particle with PID " << mcParticleClone->particleID().pid() << endmsg;
      
      if (mcParticleClone) {

        if (!particle->isBasicParticle()) {
          verbose() << "store direct link for composite particle" << endmsg;
          sc = StoreLink<LHCb::Particle,LHCb::MCParticle>(particle,mcParticleClone);
          if (sc != StatusCode::SUCCESS) {
            Warning("something went wrong when storing link from part->MCPart", StatusCode::SUCCESS);
          }// if sc
        }// if !basicPart

        // check if particle has a proto-particle
        if (particle->proto()) {

          // check if (proto-) particle has a track -> link track -> MCParticle
          if (particle->proto()->track()) {
            verbose() << "now store link from track -> MCParticle" << endmsg;
            sc = StoreLink<LHCb::Track,LHCb::MCParticle>(particle->proto()->track(),mcParticleClone);
            if (sc != StatusCode::SUCCESS) {
              Warning("something went wrong when storing link from track-clone->MCPart", StatusCode::SUCCESS);
            }// if sc
          } else {
            verbose() << "no track attached to this (proto) particle" << endmsg;
          } // if track
        } else {
          verbose() << "no proto-particle for this particle" << endmsg;
        } // if proto

//        // check if particle has daughters: if yes, link composite particle to MC particle
//        // ?? done this way??
//        if (!particle->isBasicParticle ()) {
//          verbose() << "this particle is not a basic particle, link composite particle" << endmsg;
//          sc = StoreLink<LHCb::Particle,LHCb::MCParticle>(particle,mcParticleClone);
//          if (sc != StatusCode::SUCCESS) {
//            Warning("something went wrong when storing link from particle->MCPart", StatusCode::SUCCESS);
//          }// if sc          
//        } // isBasicParticle

      } else {
        verbose() << "did not get clone of MC particle " << endmsg;
      } // if particleClone

    }//for iMCPart
  } else {
    verbose() << "no MC particle associated to this particle" << endmsg;
  }// if size
  

  //
  // store MC information for daughter particles
  //
  const SmartRefVector< LHCb::Particle > &daughters = particle->daughters();
  verbose() << "number of daughters " << daughters.size() << endmsg;
  SmartRefVector< LHCb::Particle >::const_iterator iDaughter;
  SmartRefVector< LHCb::Particle >::const_iterator iDaughterBegin = daughters.begin();
  SmartRefVector< LHCb::Particle >::const_iterator iDaughterEnd   = daughters.end();
  for (iDaughter = iDaughterBegin; iDaughter != iDaughterEnd; iDaughter++) {
    locTES = CreateMicroDSTMCAlg::objectLocation((*iDaughter)->parent());
    verbose() << " consider next daughter particle with PID " << (*iDaughter)->particleID().pid() 
              << " and key " << (*iDaughter)->key() 
              << " and TES location " << locTES
              << endmsg;
 
    StatusCode sc = CreateMicroDSTMCAlg::StoreMCParticle(*iDaughter);
    if (sc != StatusCode::SUCCESS) {
      Warning("something went wrong while storing the daughter particle");
    } // if sc

  } //for iDaughter


  return StatusCode::SUCCESS;
}//sc StoreParticle

//=============================================================================
template<class T> T *CreateMicroDSTMCAlg::getContainer(std::string locTES){
  
  verbose() << "now get container for original TES location " << locTES << endmsg;

  // most orignial locations starts with /Event/<alg,Phys,MC, ..>/...
  //  - but not all, "/Event" is optional, some have it, some don't
  // insert the output-prefix to distinguish the microDST location
  // -> remove the "/Event" bit if found at beginning of routine
  std::string            tmpString = "/Event/";
  std::string::size_type loc       = locTES.find(tmpString);
  if ( loc != std::string::npos) {
    locTES.replace(loc, tmpString.length(),"");
    verbose() << "TES location ID is now " << locTES << endmsg;
  } //if loc
  
  std::string containerLocation =  locTES;
  // now insert identifier for microDST after "/Event/"
  containerLocation.insert(0,"/Event/"+ m_OutputPrefix + "/");
  verbose() << "location of container in TES " << containerLocation << endmsg;

  if (exist<T>(containerLocation) ){
    verbose() << "returning existing container" << endmsg;
    T* container = get<T>(containerLocation);
    if (container){
      return container;
    } else {
      return NULL;
    }// if container
  } else {
    // create new container and store into TES
    T* container = new T();
    put (container, containerLocation);     
    return container;
  }// if exist

  return NULL; // something went wrong
}// getContainer


//=============================================================================
//=============================================================================
std::vector<const LHCb::MCParticle*> CreateMicroDSTMCAlg::GetMCParticle (const LHCb::Particle* particle,
                                                                         Particle2MCLinker* particleLinker) {


  verbose() << "get MC particles associated to particle with PID " 
            << particle -> particleID().pid()
            << " and mass " << particle->measuredMass() << " +- " 
            << particle->measuredMassErr()
            << endmsg;

  bool basicParticle = particle->isBasicParticle();

  //
  // statistics
  //

  if (basicParticle) {
     m_nBasicParticles++;
  } else {
    m_nCompositeParticles++;
  } // if basic

  //
  // init
  //
  std::vector<const LHCb::MCParticle*> associatedParticles;

  //
  // check if associated to MC truth
  //
  bool isAssociatedLinks     = particleLinker-> isAssociated(particle);
  verbose() << "Associated to MC truth ? "   << isAssociatedLinks     << endmsg;
  if (!isAssociatedLinks) {
    debug() << " true particle " << particle->particleID().pid () 
            << " not found" << endmsg;
    associatedParticles.clear();
    return associatedParticles;
  } // if 
  
  
  const LHCb::MCParticle* mcParticle = particleLinker->firstMCP( particle );    
  if (mcParticle) {      
    verbose() << "Found MC particle " << endmsg;
    // loop over all possible assignments
    while (mcParticle) {
      verbose() << "particle is associated  to MC particle with key "  
                << mcParticle->key() << endmsg;
      verbose() << " particle ID " << particle   -> particleID().pid () 
                << " true pid    " << mcParticle -> particleID().pid ()
                << endmsg;
      associatedParticles.push_back(mcParticle);
      mcParticle = particleLinker->next();
    } // while
  } else {
    verbose() << "MC particle not found" << endmsg;
  } // if mcParticle

  const int nAssociations = associatedParticles.size();

  //
  // statistics
  //
  if (basicParticle && nAssociations > 0) {
    m_nBasicParticlesAssociated++;
  } 
  
  if (!basicParticle && nAssociations > 0){
    m_nCompositeParticlesAssociated++;
  }


  verbose() << "return #associated particles " << nAssociations 
            << " basic particle "              << basicParticle 
            << endmsg;  
  return associatedParticles;

}// void GetMCParticleLinks


//=============================================================================
//=============================================================================
LHCb::MCParticle* CreateMicroDSTMCAlg::StoreMCParticle(const LHCb::MCParticle *mcParticle,
                                                       bool                    storeOriginVertex){

  debug() << "Now store MC particle with PID " << mcParticle->particleID().pid() 
          << " and pt " << mcParticle->pt() 
          << endmsg;
  

  //
  // get MC particle container
  //
  std::string locTES = CreateMicroDSTMCAlg::objectLocation(mcParticle->parent());
  verbose() << "MC particle on TES " << locTES << endmsg;
  LHCb::MCParticles* mcParticleContainer =  CreateMicroDSTMCAlg::getContainer<LHCb::MCParticles>(locTES);
  if (!mcParticleContainer) {
    Warning("Could not obtain container for MC Particles", StatusCode::SUCCESS);
    return NULL;
  }// if
  
 

  // clone and store MC particle
  LHCb::MCParticle *mcParticleClone = mcParticleContainer->object(mcParticle->key());
  if (mcParticleClone) {
    verbose() << "returnning MC particle which is already in microDST container" << endmsg;
    return mcParticleClone;
  }// if mcParticleClone


  //
  // clone and store MC particle
  //

  verbose() <<" MC particle not yet in microDST container" << endmsg;
  mcParticleClone = mcParticle->clone();
  //  mcParticleClone = new LHCb::MCParticle(mcParticle);
  //  mcParticleClone->setMomentum  (mcParticle->momentum());
  //  mcParticleClone->setParticleID(mcParticle->particleID());
  
  mcParticleContainer->insert (mcParticleClone, mcParticle->key());
 
 
  //
  // store mother particle
  // 

  verbose() << "store mother particle for particle with PID " 
            << mcParticle->particleID().pid() << " ? "
            << endmsg;
  const LHCb::MCParticle *mother      = mcParticle->mother();
  LHCb::MCParticle       *motherClone = NULL;
  if (storeOriginVertex && mother) {
    verbose() << "mother has PID " << mother->particleID().pid() << endmsg;
     motherClone = CreateMicroDSTMCAlg::StoreMCParticle(mother, true); 
    if (motherClone) {
      verbose() << "got clone of mother" << endmsg;
    } else {
      verbose() << "something went wrong while getting mother clone" << endmsg;
    } // if motherClone
  } else {
    verbose() << "do not store mother particle" << endmsg;
  }// if storeOriginVertex

  //
  // associated MC origin vertex
  //
  
  const LHCb::MCVertex* originVertex = mcParticle->originVertex();
  if (originVertex) {
    verbose() << "store origin vertex at " << originVertex->position() << endmsg;
    if (storeOriginVertex) {
      LHCb::MCVertex* originVertexClone = CreateMicroDSTMCAlg::StoreMCVertex(originVertex);
      if (originVertexClone) {
        verbose() << "now set origin Vertex of TES loc " 
                  << CreateMicroDSTMCAlg::objectLocation(originVertexClone->parent())
                  << endmsg;
        mcParticleClone->setOriginVertex(originVertexClone); 
        originVertexClone->setMother(motherClone);
      } else {
        Warning("something went wrong storing the origin vertex" , StatusCode::SUCCESS);
      }// if clone
    } else {
      verbose() << "do not store origin Vertex, set SmartRef" << endmsg;
      mcParticleClone->setOriginVertex(mcParticle->originVertex());            
    } // if storeOriginVertex
  }// if originVertex

 

    
  //
  // store end-vertices
  //
  mcParticleClone -> clearEndVertices ();
  const SmartRefVector< LHCb::MCVertex > & endVertices = mcParticle->endVertices();
  SmartRefVector< LHCb::MCVertex >::const_iterator iEndVtx;
  SmartRefVector< LHCb::MCVertex >::const_iterator iEndVtxBegin = endVertices.begin();
  SmartRefVector< LHCb::MCVertex >::const_iterator iEndVtxEnd   = endVertices.end();
  verbose() << "#endVertices for the MC particle " << endVertices.size() << endmsg;

  for (iEndVtx = iEndVtxBegin; iEndVtx != iEndVtxEnd; iEndVtx++) {
    verbose() << " consider new end-vertex at " << (*iEndVtx)->position() << endmsg;

    if (m_CloneEndVertices) {
      verbose() << "now store end-vertex into microDST" << endmsg;
      LHCb::MCVertex* mcVertexClone = CreateMicroDSTMCAlg::StoreMCVertex(*iEndVtx);
      if (mcParticleClone) {
        mcParticleClone ->addToEndVertices(mcVertexClone);
      } else {
        Warning("something went wrong storing the end vertex" , StatusCode::SUCCESS);
      }// if clone

    } else {
      verbose() << "set SmartRef to end-vertex" << endmsg;
      mcParticleClone ->addToEndVertices(*iEndVtx); // set SmartRef
    }// if m_CloneEndVertices

  }// for iEndVtx
  
  verbose() << "--> end of StoreMCParticle, return mcParticleClone <--" << endmsg;
  return mcParticleClone;


}// storeMCParticle


//=============================================================================
//=============================================================================
LHCb::MCVertex* CreateMicroDSTMCAlg::StoreMCVertex(const LHCb::MCVertex *mcVertex){

  std::string locTES = CreateMicroDSTMCAlg::objectLocation(mcVertex->parent());
  verbose() << "now store MC vertex at " << mcVertex->position() 
            << " from TES " << locTES << endmsg;

  //
  // get container
  //
  LHCb::MCVertices *mcVertexContainer = CreateMicroDSTMCAlg::getContainer<LHCb::MCVertices>(locTES);
  if (!mcVertexContainer) {
    Warning("Could not get container for MCVertex", StatusCode::SUCCESS);
    return NULL;
  } // if mcVertexContainer

  //
  // check if already stored
  //
  LHCb::MCVertex* mcVertexClone = mcVertexContainer->object(mcVertex->key());
  if (mcVertexClone) {
    verbose() << "returning MC vertex clone already stored " << endmsg;
    return mcVertexClone;
  } // mcVertexClone

  //
  // clone and store vertex
  //
  verbose() << "Now clone and store MC Vertex " << endmsg;
  mcVertexClone = mcVertex->clone();
  //mcVertexClone =  new LHCb::MCVertex(mcVertex);
  //mcVertexClone->setPosition(mcVertex->position());
  //mcVertexClone->setTime    (mcVertex->time());
  //mcVertexClone->setType    (mcVertex->type());
  mcVertexContainer->insert(mcVertexClone, mcVertex->key());

  //
  // clone and store products of the vertex 
  //
  verbose() << "store products of the MC vertex"<< endmsg;
  mcVertexClone->clearProducts();
  const SmartRefVector< LHCb::MCParticle > &products = mcVertex->products();
  SmartRefVector< LHCb::MCParticle >::const_iterator iProduct;
  SmartRefVector< LHCb::MCParticle >::const_iterator iProductBegin = products.begin();
  SmartRefVector< LHCb::MCParticle >::const_iterator iProductEnd   = products.end();
  for (iProduct = iProductBegin; iProduct != iProductEnd; iProduct++) {
    verbose() << "consider next product of this vertex with PID " << (*iProduct)->particleID().pid() << endmsg;
    if (m_CloneVertexProducts) {
      verbose() << "   -> store clone of vertex product particle into microDST" << endmsg;
      LHCb::MCParticle *mcProductClone = CreateMicroDSTMCAlg::StoreMCParticle(*iProduct, false);
      if (mcProductClone) {
        mcVertexClone->addToProducts(mcProductClone);
      } else {
        verbose() << "something went wrong storing the MC particle with PID "<< (*iProduct)->particleID().pid()  << endmsg;
      }// if productClone    
    } else {
      verbose() << "   -> store SmartRef to vertex product" << endmsg;
      mcVertexClone->addToProducts(*iProduct);
    }// if cloneVertexProducts
  }//for iProduct


  verbose() << "--> end of StoreMCVertex, return mcVertexClone <--" << endmsg;
  return mcVertexClone;
}// sc StoreMCVertex
//=============================================================================
//=============================================================================
template<class S, class T> StatusCode CreateMicroDSTMCAlg::StoreLink(const S* s, const T* t){
  
  verbose() << "now store link from reconstructed source object s -> MC object t" << endmsg;

  if ( (!s) || (!t)) {
    Warning("either source s or target t do not exist, cannot establish link", StatusCode::SUCCESS);
    return StatusCode::SUCCESS;
  }// if

  std::string tmpString = "/Event/";

  //
  // get location for linker
  //
  std::string linkerLocation = CreateMicroDSTMCAlg::objectLocation(s->parent());
  verbose() << "original source location on TES for link " << linkerLocation << endmsg;

  std::string::size_type loc       = linkerLocation.find(tmpString);
//  if ( loc == std::string::npos) {
//    verbose() << "insert /Event " << endmsg;
//    linkerLocation.insert(0,"/Event/");
//  } //if loc
  if (loc != std::string::npos) {
    linkerLocation.replace(loc, tmpString.length(),"");
  } //if
  linkerLocation.insert(0,"/Event/"+ m_OutputPrefix + "/");
  verbose() << "position for linker " << linkerLocation << endmsg;


  //
  // get location for cloned source object (e.g. reconstructed track)
  //
  
  std::string containerLocation =  CreateMicroDSTMCAlg::objectLocation(s->parent());
  loc = linkerLocation.find(tmpString);
  if (loc != std::string::npos) {
    containerLocation.replace(loc, tmpString.length(),"");
  }// if loc
  // now insert identifier for microDST after "/Event/"
  containerLocation.insert(0,"/Event/"+ m_OutputPrefix + "/");
  verbose() << "location of container for cloned source in TES " << containerLocation << endmsg;

  //
  // get source container
  //
  KeyedContainer<S,Containers::HashMap> *sourceContainer = NULL;
  if (exist< KeyedContainer<S,Containers::HashMap> >(containerLocation)) {
    sourceContainer = get< KeyedContainer<S,Containers::HashMap> >(containerLocation);
  } else {
    Warning("could not get container of cloned source particle, cannot establish link", StatusCode::SUCCESS);
    return StatusCode::SUCCESS;
  }//if exist

  //
  // now get clone of source object: sClone
  // (N.B. clone has same key as orignal object)
  //
  S *sClone = sourceContainer->object(s->key());
  if (!sClone) {
    Warning("could not get source particle", StatusCode::SUCCESS);
    return StatusCode::SUCCESS;
  }// if sClone

  verbose() << "TES locatio of source s            " << CreateMicroDSTMCAlg::objectLocation(s->parent())      << endmsg;
  verbose() << "                      clone sClone " << CreateMicroDSTMCAlg::objectLocation(sClone->parent()) << endmsg;
  verbose() << "                      target       " << CreateMicroDSTMCAlg::objectLocation(t->parent())      << endmsg;

  //
  // now setup linker to store the connection between
  // cloned source object sClone -> (MC) target object t
  //
  verbose() << "now setup linker" << endmsg;
  LinkerWithKey<T, S> linker(evtSvc(), msgSvc(), linkerLocation);

  verbose() << "now link sClone with key " << sClone->key() <<" to target t wth key " << t->key() << endmsg;
  linker.link(sClone, t);

  return StatusCode::SUCCESS;
}// sc StoreLink  
//=============================================================================
//=============================================================================
