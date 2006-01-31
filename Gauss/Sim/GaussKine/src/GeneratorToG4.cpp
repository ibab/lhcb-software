// $Id: GeneratorToG4.cpp,v 1.1.1.1 2006-01-31 11:03:51 gcorti Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// from GiGa
#include "GiGa/IGiGaSvc.h" 
#include "GiGaCnv/GiGaPrimaryParticleInformation.h"

// from LHCb
#include "Kernel/ParticleID.h"
#include "Event/HepMCEvent.h"

// from Geant4
#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"

// local
#include "GeneratorToG4.h"

// Little auxiliary class
class compareBarCode {
public:
  bool operator() (HepMC::GenParticle* partA, HepMC::GenParticle* partB) {
    return partA->barcode() < partB->barcode();
  }
};

//-----------------------------------------------------------------------------
// Implementation file for class : GeneratorToG4
//
// 2005-10-06 : Gloria CORTI
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<GeneratorToG4>          s_factory ;
const        IAlgFactory& GeneratorToG4Factory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GeneratorToG4::GeneratorToG4( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_gigaSvc      ( 0 )
{

  declareProperty( "GiGaService",    m_gigaSvcName    = "GiGa" );
  declareProperty( "HepMCEventLocation", 
                   m_location = LHCb::HepMCEventLocation::Default );
  
}
//=============================================================================
// Destructor
//=============================================================================
GeneratorToG4::~GeneratorToG4() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode GeneratorToG4::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  m_gigaSvc = svc<IGiGaSvc>( m_gigaSvcName, true ); // Create if necessary

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode GeneratorToG4::execute() {

  debug() << "==> Execute" << endmsg;

  LHCb::HepMCEvents* hepEvts = get<LHCb::HepMCEvents> ( m_location );
  
  // Temporary container of particles to store
  std::vector<HepMC::GenParticle*> outParts;
  outParts.clear();
  
  // Loop over the events (one for each pile-up)
  for( LHCb::HepMCEvents::const_iterator iEvt = hepEvts->begin(); 
       hepEvts->end() != iEvt; ++iEvt ) {

    // Loop over the vertices in HepMC::GenEvent
    for( HepMC::GenEvent::vertex_const_iterator
           iVtx = (*iEvt)->pGenEvt()->vertices_begin(); 
         iVtx != (*iEvt)->pGenEvt()->vertices_end(); ++iVtx ) {
      
      // Loop over outgoing particles, check for those with status 1,2, 888 or 
      // 889 and store them in a temporary vector (see explanation of code in
      // Gauss User Manual
      for( HepMC::GenVertex::particles_out_const_iterator
             iPart = (*iVtx)->particles_out_const_begin();
           (*iVtx)->particles_out_const_end() != iPart; ++iPart ) {
        
        if( ( (*iPart)->status() == 1 ) ||
            ( (*iPart)->status() == 2 ) ||
            ( (*iPart)->status() == 888 ) ||
            ( (*iPart)->status() == 889 ) ) {
          // Now choose which one to convert to G4, criteria are:
          // -> the particle has no production vertex (then convert all the
          //             daughters)
          if( !((*iPart)->production_vertex()) ) {
            outParts.push_back( *iPart );
          } 
          // -> the particle is a hadron, a lepton, a nucleus or a photon
          //             and has only one mother which is not a hadron,
          //             not a lepton and not a nucleus and not a photon 
          //             i.e. in practice which is a string (the convert 
          //             all the daughters)
          // -> the particle is hadron, a lepton, a nucleus or a photon 
          //             and has several mother (in pratice a quark-gluon
          //             interaction) or no mother, but a production vertex
          //             (that is to say a particle gun or machine background)
          else {
            if( (*iPart)->production_vertex()->particles_in_size() == 1 ) {
              LHCb::ParticleID pidMother( (*(*iPart)->production_vertex()
                                     ->particles_in_const_begin())->pdg_id() );
              LHCb::ParticleID pid( (*iPart)->pdg_id() );
              if( ( ( ( !pidMother.isHadron() ) &&
                    ( !pidMother.isLepton() ) &&
                    ( !pidMother.isNucleus() ) &&
                    ( pidMother.abspid() != 22 ) ) ||
                    ( (*(*iPart)->production_vertex()->particles_in_const_begin())
                      ->status() == 3 ) ) && 
                  ( ( pid.isHadron() ) ||
                    ( pid.isLepton() ) ||
                    ( pid.isNucleus() ) ||
                    ( pid.abspid() == 22 ) ) ) {
                outParts.push_back( *iPart );
              }
            } else { 
              LHCb::ParticleID pid( (*iPart)->pdg_id() );
              if( ( pid.isHadron( ) ) ||
                  ( pid.isLepton( ) ) ||
                  ( pid.isNucleus( ) ) ||
                  ( pid.abspid() == 22 ) ) {
                outParts.push_back( *iPart );
              }
            }
          }
        }    
      } // end of for loop on particles
    }   // end of for loop on vertices
    
    // Sort the vector, so that particles are always given to G4 in the 
    // same order
    std::sort(outParts.begin(), outParts.end(), compareBarCode() );
    
    // The assumption is made here that all particles produced by Pythia
    // have the same PHYSICAL vertex
    G4PrimaryVertex* origVertex = new G4PrimaryVertex
      ( (*(outParts.begin()))->production_vertex()->position().x(),
        (*(outParts.begin()))->production_vertex()->position().y(),
        (*(outParts.begin()))->production_vertex()->position().z(),
        ((*(outParts.begin()))->production_vertex()->position().t())/c_light );
    
    for( std::vector<HepMC::GenParticle*>::iterator iOut = outParts.begin();
         outParts.end() != iOut; ++iOut ) {
      G4PrimaryParticle* aParticle = makeG4Particle( *iOut, (*iEvt) );
      origVertex->SetPrimary( aParticle );
    }
    
    *gigaSvc() << origVertex;
    outParts.clear();

  } // end loop on events

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode GeneratorToG4::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
G4PrimaryParticle* GeneratorToG4::makeG4Particle(HepMC::GenParticle* genPart,
                                                 LHCb::HepMCEvent* theEvent ) {

  //  static const std::string 
  //  ErrMsg1("GiGaCnv::GenPartG4Part: GenParticle* points to NULL!");

  //if( 0 == particle ) { throw GiGaException( ErrMsg1 ) ; }
  
  std::vector<HepMC::GenParticle*> outParticles;
  
  G4PrimaryParticle*  g4Particle = 
    new G4PrimaryParticle( genPart->pdg_id() , 
                           genPart->momentum().px()*GeV ,
                           genPart->momentum().py()*GeV ,
                           genPart->momentum().pz()*GeV );  
  
  // create a new User information to contain the link to the HepMC
  // particle and signal information for particles with 889 status
  bool isSignalParticle = ( 889 == (genPart->status()) );
  GiGaPrimaryParticleInformation* gInfo = 
    new GiGaPrimaryParticleInformation( isSignalParticle, genPart->barcode(),
                                        theEvent );
  g4Particle -> SetUserInformation( gInfo ) ;
  
  if( genPart->end_vertex() ) {
    // assign decay time
    HepLorentzVector theLorentzV = (genPart->end_vertex()->position()
                                    - genPart->production_vertex()->position());
    Hep3Vector theBoost = genPart->momentum().boostVector();
    
    g4Particle->SetProperTime((theLorentzV.boost(-theBoost)).t()/c_light);
    
    // Better to use print here instead of verbose because
//     verbose() << "assigning time " 
//               << (theLorentzV.boost(-theBoost)).t()/c_light
//               << " to " <<  genPart->pdg_id() << endmsg;
    
    if( ( genPart->end_vertex() )->particles_out_size() == 1 ) {
      HepMC::GenVertex::particles_out_const_iterator dPart =
        (genPart->end_vertex())->particles_out_const_begin();
      if( abs( (*dPart)->pdg_id() ) == abs( genPart->pdg_id() ) ) {
        if( (*dPart)->pdg_id() == -(genPart->pdg_id()) ) {
          // particle has oscillated
          // put this info in GiGaPrimaryGenPartInformation
          if ( 0 == g4Particle->GetUserInformation() ) {
            GiGaPrimaryParticleInformation* gInfo =
              new GiGaPrimaryParticleInformation( true ) ;
            g4Particle -> SetUserInformation( gInfo ) ;
          }
          else {
            // update the existing one
            GiGaPrimaryParticleInformation* theInfo =
              gigaPrimaryParticleInformation( g4Particle->GetUserInformation( ) ); 
            if( theInfo ) theInfo->setHasOscillated( true );
          }
        }
        // skip this daughter particle.
        // Since the particle has oscillated it has a end_vertex()
        // and end_vertex() for the daughter is not NULL !
        genPart = ( *dPart );
      }
    }              
    
    // if particle has daughters, carry on with the conversion
    for( HepMC::GenVertex::particles_out_const_iterator outPart = 
           (genPart->end_vertex())->particles_out_const_begin();
         outPart != (genPart->end_vertex())->particles_out_const_end(); ++outPart ) {
      outParticles.push_back(*outPart);
    }

    // sort the vector, so we always put them in the same order into G4
    std::sort(outParticles.begin(), outParticles.end(), compareBarCode());
    
    for( std::vector<HepMC::GenParticle*>::iterator iOutPart = outParticles.begin();
         outParticles.end() != iOutPart; ++iOutPart ) {
      g4Particle->SetDaughter( makeG4Particle(*iOutPart, theEvent) );
    }
  }
  
  return g4Particle;  
};

