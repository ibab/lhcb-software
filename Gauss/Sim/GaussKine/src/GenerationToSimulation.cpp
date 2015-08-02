// Local.
#include "GenerationToSimulation.h"
#include "MCInterfaces/IFlagSignalChain.h"

// Gaudi.
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/Transform4DTypes.h"
#include "GaudiKernel/PhysicalConstants.h"

// GiGa.
#include "GiGa/IGiGaSvc.h" 
#include "GiGaCnv/GiGaPrimaryParticleInformation.h"

// Event.
#include "Event/HepMCEvent.h"
#include "Event/MCHeader.h"

// Kernel.
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"

// HepMC.
#include "GenEvent/HepMCUtils.h"

// Geant4.
#include "G4PrimaryVertex.hh"
#include "G4PrimaryParticle.hh"
#include "G4ParticleTable.hh"
#include "G4ParticlePropertyTable.hh"

//-----------------------------------------------------------------------------
// Implementation file for class : GenerationToSimulation
//
// 2008-09-24 : Gloria CORTI, Patrick ROBBE
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory.
DECLARE_ALGORITHM_FACTORY(GenerationToSimulation)

//=============================================================================
// Standard constructor, declares properties.
//=============================================================================
GenerationToSimulation::GenerationToSimulation(const std::string& name,
					       ISvcLocator* pSvcLocator) : 
  GaudiAlgorithm(name, pSvcLocator), m_gigaSvc(0), m_particleContainer(0), 
  m_vertexContainer(0), m_keepCuts(LoKi::Constant<const HepMC::GenParticle*, 
				   bool>(true)) {
  declareProperty("GiGaService", m_gigaSvcName = "GiGa",
		  "Name of the GiGa service.");
  declareProperty("HepMCEventLocation",
		  m_generationLocation = LHCb::HepMCEventLocation::Default,
		  "Location to read the HepMC event.");
  declareProperty("Particles",
		  m_particlesLocation = LHCb::MCParticleLocation::Default,
		  "Location to place the MCParticles.");
  declareProperty("Vertices",
		  m_verticesLocation = LHCb::MCVertexLocation::Default,
		  "Location to place the MCVertices.");  
  declareProperty("TravelLimit", m_travelLimit = 1e-10*m,
		  "Pass particles to Geant4 with travel length above this.");
  declareProperty("LookForUnknownParticles", 
		  m_lookForUnknownParticles = false,
		  "Check if Geant4 knows the particle type.");
  declareProperty("SkipGeant", m_skipGeant4 = false,
		  "Skip passing everything to Geant4.");
  declareProperty("UpdateG4ParticleProperties", 
		  m_updateG4ParticleProperties = true,
		  "Update the Geant4 particle properties.");
  declareProperty("MCHeader", m_mcHeader = LHCb::MCHeaderLocation::Default,
		  "Location to retrieve the MCHeader.");
  declareProperty("KeepCode", m_keepCode = "", 
		  "The code to flag additional particles for storage.");
}

//=============================================================================
// Destructor, does nothing.
//=============================================================================
GenerationToSimulation::~GenerationToSimulation() {}

//=============================================================================
// Initialization.
//=============================================================================
StatusCode GenerationToSimulation::initialize() {

  // Initialize the GaudiAlgorithm, return if failure.
  StatusCode sc = GaudiAlgorithm::initialize(); 
  if (sc.isFailure()) return sc;

  // Intialize the tool.
  debug() << "==> Initialize" << endmsg;
  if (!m_skipGeant4) {
    
    // Create the GiGa service.
    m_gigaSvc = svc<IGiGaSvc>(m_gigaSvcName, true);
  
    // Update Geant4 particle properties from ParticlePropertySvc.
    if (m_updateG4ParticleProperties) { 
      LHCb::IParticlePropertySvc* ppSvc = 
        svc<LHCb::IParticlePropertySvc>("LHCb::ParticlePropertySvc", true);
      G4ParticlePropertyTable* PPT = G4ParticlePropertyTable::
	GetParticlePropertyTable();
      G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();

      // Suppress printing unknown PDGs from Gauss.
      if (!msgLevel(MSG::DEBUG)) particleTable->SetVerboseLevel(0);
      for (int i = 0; i < particleTable->size(); ++i) {
        G4ParticleDefinition* PDef = particleTable->GetParticle(i);
        const LHCb::ParticleProperty* pp = ppSvc->
	  find(LHCb::ParticleID(PDef->GetPDGEncoding()));
        if (pp) {
          G4ParticlePropertyData* PPData = PPT->GetParticleProperty(PDef);
          PPData->SetPDGMass(pp->mass());
          PPData->SetPDGLifeTime(pp->lifetime());
          PPT->SetParticleProperty(*PPData);
        }
      }
      release(ppSvc);
    }
  }
  
  // Cuts to keep additional particles.
  if (m_keepCode != "") {
    svc<IService>("LoKiSvc");
    LoKi::IGenHybridFactory* factory = tool<LoKi::IGenHybridFactory>
      ("LoKi::Hybrid::GenTool/GenFactory:PUBLIC", this);
    sc = factory->get(m_keepCode, m_keepCuts);
    if (sc.isFailure()) 
      always() << "Error from KeepCode = '" + m_keepCode + "'" << endmsg;
  }

  // get tool to set signal flag
  m_setSignalFlagTool = tool< IFlagSignalChain >( "FlagSignalChain" );

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution.
//=============================================================================
StatusCode GenerationToSimulation::execute() {
  
  // Retrieve the HepMCEvents.
  debug() << "==> Execute" << endmsg;
  LHCb::HepMCEvents* generationEvents = 
    get<LHCb::HepMCEvents>(m_generationLocation);

  // Create containers in TES for MCParticles and MCVertices.
  m_particleContainer = new LHCb::MCParticles();
  put(m_particleContainer, m_particlesLocation);
  m_vertexContainer = new LHCb::MCVertices();
  put(m_vertexContainer, m_verticesLocation);
  if (!generationEvents || generationEvents->size() == 0)
    {*gigaSvc() << NULL; return StatusCode::SUCCESS;}

  // Retrieve the MCHeader.
  LHCb::MCHeader* mcHeader = get<LHCb::MCHeader>(m_mcHeader);

  // Loop over the events (one for each pile-up interaction).
  for(LHCb::HepMCEvents::const_iterator genEvent = generationEvents->begin(); 
       generationEvents->end() != genEvent; ++genEvent) {

    // Retrieve the event.
    HepMC::GenEvent* ev = (*genEvent)->pGenEvt();

    // Empty the maps of converted particles.
    m_g4ParticleMap.clear();
    m_mcParticleMap.clear();
    m_particlesToDelete.clear();
    
    // Determine the position of the primary vertex.
    Gaudi::LorentzVector thePV = primaryVertex(ev);
    
    /*
      Set G4PrimaryVertex to this position. The assumption is that all
      particles transferred to Geant4 for this pile up interaction
      come from this primary vertex. This means that all particles
      with non zero lifetimes must be passed to Geant4 and then must
      be declared to Geant4 so that they can be transported correctly.
    */
    G4PrimaryVertex* origVertex = 
      new G4PrimaryVertex(thePV.X(), thePV.Y(), thePV.Z(), thePV.T());

    // Create and add the primary MCVertex.
    LHCb::MCVertex* primaryVertex = new LHCb::MCVertex();
    m_vertexContainer->insert(primaryVertex);
    primaryVertex->setPosition(Gaudi::XYZPoint( thePV.Vect()));
    primaryVertex->setTime(thePV.T());
    primaryVertex->setType(LHCb::MCVertex::ppCollision);
    mcHeader->addToPrimaryVertices(primaryVertex);
  
    // Set ID of all vertices to 0.
    for (HepMC::GenEvent::vertex_iterator itV = ev->vertices_begin();
	 itV != ev->vertices_end(); ++itV) (*itV)->set_id(0);

    // Extract the particles to store in MCParticles.
    std::vector<HepMC::GenParticle*> mctruthList;
    for (HepMC::GenEvent::particle_const_iterator itP = ev->particles_begin();
	 itP != ev->particles_end(); ++itP) {
      if (keep(*itP) || (m_keepCode != "" && m_keepCuts(*itP))) { 
        mctruthList.push_back(*itP);
	
	/*
         Set the ID of of the end vertex to one, then the particle
         with production vertex of ID 0 and end vertex of ID 1 (or
         without end vertex) will be the start point of a decay tree
         to convert to MCTruth.
	*/
        HepMC::GenVertex* endVertex = (*itP)->end_vertex();
        if (endVertex) endVertex->set_id(1);
      }
    }
    
    // Sort the particles to be stored by barcode and then convert them.
    std::sort(mctruthList.begin(), mctruthList.end(), 
	      HepMCUtils::compareHepMCParticles);
    for (std::vector<HepMC::GenParticle*>::iterator it = mctruthList.begin();
	 mctruthList.end() != it; ++it) {
      HepMC::GenVertex* prodVertex = (*it)->production_vertex();
      if (!prodVertex) 
        warning() << "The particle has no production vertex !!" << endreq;
      else if (0 == prodVertex->id())
        convert(*it, origVertex, primaryVertex, 0, 0);
    }
    
    // Remove from the primary vertex and delete the unneeded particles.
    if (!m_particlesToDelete.empty()) {
      std::vector< G4PrimaryParticle*>::const_iterator itDel;
      for (itDel = m_particlesToDelete.begin(); 
	   itDel != m_particlesToDelete.end(); ++itDel) { 
        removeFromPrimaryVertex(origVertex, *itDel);
        delete *itDel;
      }
    }
    if (!m_skipGeant4) *gigaSvc() << origVertex;
  }

  // This needs to be done only if Geant4 is not called since in that case
  // the setting of the flags for the daughters is taken care of by 
  // SimulationToMCTruth
  if ( m_skipGeant4 ) {
    LHCb::MCParticles::const_iterator ip;
    for ( ip = m_particleContainer->begin(); ip != m_particleContainer->end(); ip++ ) {
      if ( (*ip)->fromSignal() ) {
        m_setSignalFlagTool->setFromSignalFlag( *ip );
      }
    }
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Decides if a particle should be kept in MCParticles.
//=============================================================================
bool GenerationToSimulation::keep(const HepMC::GenParticle* particle) const{
  LHCb::ParticleID pid(particle->pdg_id());
  switch (particle -> status()) {
  case LHCb::HepMCEvent::StableInProdGen: return true;
  case LHCb::HepMCEvent::DecayedByDecayGen: return true;
  case LHCb::HepMCEvent::DecayedByDecayGenAndProducedByProdGen: return true;
  case LHCb::HepMCEvent::SignalInLabFrame: return true;
  case LHCb::HepMCEvent::StableInDecayGen: return true;

  // For some processes the resonance has status 3.
  case LHCb::HepMCEvent::DocumentationParticle:
    if (24 == particle->parent_event()->signal_process_id()) {
      if (23 == pid.abspid()) return true;
      else if (25 == pid.abspid()) return true;
    } else if (26 == particle->parent_event()-> signal_process_id()) {
      if (24 == pid.abspid()) return true;
      else if (25 == pid.abspid()) return true;
    } else if (102 == particle->parent_event()->signal_process_id()) {
      if (25 == pid.abspid()) return true;
    } else if (6 == pid.abspid()) return true;
    return false;
  case LHCb::HepMCEvent::Unknown: return false;
  case LHCb::HepMCEvent::DecayedByProdGen: 
    if ( pid.isHadron() ) return true;
    if ( pid.isLepton() ) return true;
    if ( pid.isNucleus() ) return true;
    if ( pid.isDiQuark() ) return false;

    // Store particles of interest.
    switch (pid.abspid()) {
    case LHCb::ParticleID::down   : return false;
    case LHCb::ParticleID::up     : return false;
    case LHCb::ParticleID::strange: return false;
    case LHCb::ParticleID::charm  : return true;
    case LHCb::ParticleID::bottom : return true;
    case LHCb::ParticleID::top    : return false;
    case 21: return false;  // Gluon.
    case 22: return true;   // Photon.
    case 23: // Z0.
      if (24 == particle->parent_event()->signal_process_id()) return false;
      else return true;
    case 24: // W. 
      if (26 == particle->parent_event()->signal_process_id()) return false;
      else return true;
    case 25: // SM Higgs.
      if (24 == particle-> parent_event()-> signal_process_id() || 
	  26 == particle-> parent_event()-> signal_process_id() || 
	  102 == particle-> parent_event()-> signal_process_id()) return false;
      else return true;
    case 32: return true;  // Z'.
    case 33: return true;  // Z''.
    case 34: return true;  // W'.
    case 35: return true;  // CP-even heavy Higgs (H0/H2).
    case 36: return true;  // CP-odd Higgs (A0/H3).
    case 37: return true;  // Charged Higgs (H+).
    // See Table 6 of the Pythia 6 manual (arxiv.org/abs/hep-ph/0603175).
    case 90: return false; // System particle.
    case 91: return false; // Parton system from cluster fragmentation.
    case 92: return false; // Parton system from string fragmentation.
    case 93: return false; // Parton system from independent fragmentation.
    case 94: return false; // Time-like showering system.
    default: return true;
    }
    return true;
  default: return false;
  }
  return false;
}

//=============================================================================
// Convert a decay tree into MCParticle or to G4PrimaryParticle.
//=============================================================================
void GenerationToSimulation::convert(HepMC::GenParticle*& particle, 
				     G4PrimaryVertex* pvertexg4,
				     LHCb::MCVertex* originVertex,
				     G4PrimaryParticle* motherg4,
				     LHCb::MCParticle* mothermcp) {

  // Decision to convert the particle.
  unsigned char conversionCode = transferToGeant4(particle);
  switch (conversionCode) {
  case 1: { // Convert particle to G4.

    // Check if particle has been converted.
    const int pBarcode = particle->barcode();
    std::map<int, std::pair<bool, G4PrimaryParticle*> >::const_iterator 
      result = m_g4ParticleMap.find(pBarcode);
    if (result != m_g4ParticleMap.end()) {

      // Return if converted.
      if (result->second.first) return;

      // Flag for deletion.
      else {
	m_g4ParticleMap.erase(pBarcode);
	m_particlesToDelete.push_back(result->second.second);
      }
    }
    G4PrimaryParticle* g4P = makeG4Particle(particle, mothermcp);

    // Check mother.
    if (!motherg4) {

      // Flag for recreation with correct mother link.
      if (!mothermcp) m_g4ParticleMap.insert
			(std::make_pair(pBarcode, std::make_pair(false, g4P)));
      else m_g4ParticleMap.insert
	     (std::make_pair(pBarcode, std::make_pair(true, g4P)));

      // Attach root particle to G4 primary vertex.
      if (pvertexg4) pvertexg4->SetPrimary(g4P);
      else error() << "Primary vertex points to NULL !" << endreq ;

    // Set mother link.
    } else {
      m_g4ParticleMap.insert
	(std::make_pair(pBarcode, std::make_pair(true, g4P)));
      motherg4->SetDaughter(g4P);
    }
    pvertexg4 = 0;
    originVertex = 0;
    motherg4 = g4P;
    mothermcp = 0; 
    break;
  }
  case 2: { // Convert to MCParticle.

    // Check if already converted.
    const int pBarcode = particle->barcode() ;
    std::map<int, bool>::const_iterator result = m_mcParticleMap.find(pBarcode);
    if (result != m_mcParticleMap.end()) return;
    
    // Convert the particle.
    LHCb::MCVertex* endVertex = 0 ;
    LHCb::MCParticle* mcP = makeMCParticle(particle, endVertex);
    if (originVertex) {
      mcP->setOriginVertex(originVertex);
      originVertex->addToProducts(mcP);
    }
    m_mcParticleMap.insert(std::make_pair(pBarcode, true));
    mothermcp = mcP;
    originVertex = endVertex; 
    break;
  }
  case 3: // Skip the particle.
  default:
    break;
  }

  // Convert all daughters of the HepMC particle (recurse).
  HepMC::GenVertex* ev = particle->end_vertex();
  if (ev) {

    // Create the list.
    std::vector<HepMC::GenParticle*> dList;
    for (HepMC::GenVertex::particle_iterator itD = ev->particles_begin
	   (HepMC::children); itD != ev->particles_end(HepMC::children); 
	 ++itD) {
      HepMC::GenParticle* P = (*itD);
      dList.push_back(P);
    }
    
    // Sort by barcode and convert.
    std::sort(dList.begin(), dList.end(), HepMCUtils::compareHepMCParticles);
    for (std::vector<HepMC::GenParticle*>::iterator itDD = dList.begin(); 
	 dList.end() != itDD; ++itDD) {
      HepMC::GenParticle* P = (*itDD);
      convert(P, pvertexg4, originVertex, motherg4, mothermcp);
    }
  }
}

//=============================================================================
// Decide how to treat the particle.
// 1: convert to Geant4 for tracking in detector.
// 2: convert to MCParticle directly, the particle is not sent to Geant4.
// 3: skip the particle completely.
//=============================================================================
unsigned char GenerationToSimulation::transferToGeant4
(const HepMC::GenParticle* p) const {
  if (!(keep(p) || (m_keepCode != "" && m_keepCuts(p)))) return 3;
  if (m_skipGeant4) return 2;

  // Return for Geant4 tracking if stable.
  HepMC::GenVertex* ev = p->end_vertex();
  if (!ev) { 
    if (m_lookForUnknownParticles) 
      if (!G4ParticleTable::GetParticleTable()->FindParticle(p->pdg_id())) 
        warning() << "The particle " << p -> pdg_id() 
		  << " is not known to Geant4 but travels a finite distance" 
		  << endreq;
    return 1;
  }

  // Determine the travel distance.
  HepMC::GenVertex* pv = p->production_vertex();
  Gaudi::XYZVector E(ev->point3d());
  Gaudi::XYZVector P(pv->point3d());
  double dist = (E - P).R();

  // Skip passing to Geant4 if under travel limit.
  if (dist < m_travelLimit) return 2;

  // Return for Geant4 tracking if stable.
  if (m_lookForUnknownParticles) 
    if (!G4ParticleTable::GetParticleTable()->FindParticle(p->pdg_id())) 
      warning() << "The particle " << p -> pdg_id() 
		<< " is not known to Geant4 but travels a finite distance" 
		<< endreq;
  return 1;
}

//=============================================================================
// Create a G4PrimaryParticle from a HepMC Particle.
//=============================================================================
G4PrimaryParticle* GenerationToSimulation::makeG4Particle
(HepMC::GenParticle*& particle, LHCb::MCParticle* mcp) const {
  HepMC::FourVector mom = particle -> momentum();
  G4PrimaryParticle * g4P = new G4PrimaryParticle(particle->pdg_id(), mom.x(), 
						  mom.y(), mom.z());
  g4P->SetMass(particle->generated_mass());
  
  // Create information containing the HepMC link and signal information.
  bool isSignalParticle(LHCb::HepMCEvent::SignalInLabFrame == 
			(particle->status()));
  GiGaPrimaryParticleInformation* gInfo = new GiGaPrimaryParticleInformation
    (isSignalParticle, particle->barcode(), 0);

  HepMC::GenVertex* ev = particle->end_vertex();
  if (ev) {
    
    // Set propertime of the particle.
    double life = lifetime(mom, particle->production_vertex(), ev);
    g4P->SetProperTime(life);
    
    // Store if particle has oscillated.
    const HepMC::GenParticle* B = hasOscillated(particle);
    if (B) { 
      gInfo->setHasOscillated(true);
      particle = const_cast<HepMC::GenParticle*>(B);
    }
  }
  
  // Link to MCParticle to be able to rebuild history later.
  if (mcp) gInfo->setMotherMCParticle(mcp);
  g4P->SetUserInformation(gInfo);
  return g4P ;
}

//=============================================================================
// Create an MCParticle from a HepMC GenParticle.
//=============================================================================
LHCb::MCParticle* GenerationToSimulation::makeMCParticle
(HepMC::GenParticle*& particle, LHCb::MCVertex*& endVertex) const {

  // Create and insert into TES.
  LHCb::MCParticle * mcp = new LHCb::MCParticle();
  m_particleContainer->insert(mcp);

  // Set properties.
  Gaudi::LorentzVector mom(particle->momentum());
  LHCb::ParticleID pid(particle->pdg_id());
  mcp -> setMomentum(mom);
  mcp -> setParticleID(pid);
  
  // Set the vertex.
  HepMC::GenVertex* V = particle->end_vertex() ;
  if (V) {
    endVertex = new LHCb::MCVertex();
    m_vertexContainer->insert(endVertex);
    endVertex->setPosition(Gaudi::XYZPoint(V->point3d()));
    endVertex->setTime(V->position().t());
    endVertex->setMother(mcp);

    // Check if the particle has oscillated.
    const HepMC::GenParticle* B = hasOscillated(particle);
    if (B) {
      endVertex->setType( LHCb::MCVertex::OscillatedAndDecay);
      particle = const_cast<HepMC::GenParticle*>(B);
    } else if (( 4 == pid.abspid()) || (5 == pid.abspid()))
      endVertex->setType( LHCb::MCVertex::StringFragmentation);
    else
      endVertex -> setType( LHCb::MCVertex::DecayVertex);
    mcp -> addToEndVertices(endVertex);
  }

  //  Set the fromSignal flag
  if ( LHCb::HepMCEvent::SignalInLabFrame == (particle->status()) ) {
    mcp -> setFromSignal(true);
  }

  return mcp;
}

//=============================================================================
// Determine the primary vertex for the event.
//=============================================================================
Gaudi::LorentzVector GenerationToSimulation::primaryVertex
(const HepMC::GenEvent* genEvent) const {
  Gaudi::LorentzVector result(0, 0, 0, 0);

  // First method, get the beam particle and use the decay vertex if it exists.
  if (genEvent->valid_beam_particles()) {
    HepMC::GenParticle* P = genEvent->beam_particles().first;
    HepMC::GenVertex*   V = P->end_vertex();
    if (V) result = V->position(); 
    else error() << "The beam particles have no end vertex!" << endreq;
  // Second method, use the singal vertex stored in HepMC.
  } else if ( 0 != genEvent -> signal_process_vertex() ) {
    HepMC::GenVertex* V = genEvent->signal_process_vertex();
    result = V->position();
  // Third method, take production/end vertex of the particle with barcode 1.
  } else {
    HepMC::GenParticle* P = genEvent->barcode_to_particle(1);
    HepMC::GenVertex*   V = 0;
    if (P) {
      V = P->production_vertex();
      if (V) result = V->position();
      else {
        V = P->end_vertex();
        if (V) result = V->position();
        else error() << "The first particle has no production vertex and "
		     << "no end vertex !" << endreq ;
      }
    } else error() << "No particle with barcode equal to 1!" << endreq;
  }
  return result;
}

//=============================================================================
// Compute the lifetime of a particle.
//=============================================================================
double GenerationToSimulation::lifetime(const HepMC::FourVector mom, 
					const HepMC::GenVertex* P,
					const HepMC::GenVertex* E ) const {
  if (!E) return 0;
  Gaudi::LorentzVector A(P->position()), B(E->position());
  Gaudi::LorentzVector AB = B - A;

  // Switch to mm for time.
  AB.SetE(AB.T() * Gaudi::Units::c_light);

  // Boost displacement 4-vector to rest frame of particle.
  Gaudi::LorentzVector M(mom);
  ROOT::Math::Boost theBoost(M.BoostToCM());
  Gaudi::LorentzVector ABStar = theBoost(AB);
  
  // Switch back to time.
  return ABStar.T() / Gaudi::Units::c_light;
}

//=============================================================================
// Check if a particle has oscillated.
//=============================================================================
const HepMC::GenParticle* GenerationToSimulation::hasOscillated
(const HepMC::GenParticle* P) const {
  const HepMC::GenVertex* ev = P->end_vertex();
  if (!ev) return 0 ; 
  if (1 != ev->particles_out_size()) return 0;
  const HepMC::GenParticle* D = *(ev->particles_out_const_begin());
  if (!D) return 0 ;
  if (-P->pdg_id() != D->pdg_id()) return 0;
  return D;
}

//=============================================================================
// Remove a particle from a primary vertex.
//=============================================================================
void GenerationToSimulation::removeFromPrimaryVertex
(G4PrimaryVertex*& pvertexg4, const G4PrimaryParticle* particleToDelete) const {

  // This should be rare, so warn.
  warning() << "A G4PrimaryParticle will be removed from the G4PrimaryVertex" 
	    << endmsg ;
  particleToDelete->Print();

  // Make a new vertex.
  G4PrimaryVertex* newVertex = new G4PrimaryVertex
    (pvertexg4->GetX0(), pvertexg4->GetY0(),
     pvertexg4->GetZ0(), pvertexg4->GetT0());

  // Copy particles to new vertex, except the one to remove.
  G4PrimaryParticle* particle = pvertexg4->GetPrimary();
  while (particle) {
    if (particle != particleToDelete) newVertex->SetPrimary(particle);
    particle = particle->GetNext();
  }
  delete pvertexg4;
  pvertexg4 = newVertex;
}
