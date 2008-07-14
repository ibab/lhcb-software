// $Id: GeneratorFullMonitor.cpp,v 1.7 2008-07-14 20:34:20 robbep Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/NTuple.h"

// from Event
#include "Event/HepMCEvent.h"
#include "Event/GenCollision.h"

// from Kernel
#include "Kernel/ParticleID.h"

// from HepMC
#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenParticle.h"

// local
#include "GeneratorFullMonitor.h"

//-----------------------------------------------------------------------------
// Implementation file for class : GeneratorFullMonitor
//
// 2004-03-02 : Patrick Robbe
// 2007-01-30 : Paul Szczypka, add information on hard interaction
// 2007-02-16 : Gloria Corti, initialize m_event
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( GeneratorFullMonitor );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GeneratorFullMonitor::GeneratorFullMonitor( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
    , m_nTuple ( 0 ) 
    , m_nPart ( ) 
    , m_e ( ) 
    , m_px ( ) 
    , m_py ( ) 
    , m_pz ( ) 
    , m_vxProd ( ) 
    , m_vyProd ( ) 
    , m_vzProd ( ) 
    , m_vxDecay( )
    , m_vyDecay( ) 
    , m_vzDecay( ) 
    , m_pdgId  ( ) 
    , m_nDau   ( )
    , m_pdgIdMother( ) 
    , m_pdgIdDau1( ) 
    , m_pdgIdDau2( ) 
    , m_pdgIdDau3( ) 
    , m_pdgIdDau4( ) 
    , m_pdgIdDau5( ) 
    , m_pdgIdDau6( ) 
    , m_indexMother( )
    , m_indexInter( )
    , m_nInter( ) 
    , m_isBB( ) 
    , m_event ( )
    , m_shat ( )
    , m_that ( )
    , m_uhat ( )
    , m_pthat( )
    , m_x1 ( )
    , m_x2 ( )
    , m_procId ( )
    , m_nPartMax( 2000 ) 
    , m_nInterMax( 20 ) 
    , m_event_max( 5 )
{
  declareProperty( "HepMCEvents", 
                   m_inputHepMC = LHCb::HepMCEventLocation::Default);
  declareProperty( "Collisions", 
                   m_inputColl = LHCb::GenCollisionLocation::Default);
}

//=============================================================================
// Destructor
//=============================================================================
GeneratorFullMonitor::~GeneratorFullMonitor() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode GeneratorFullMonitor::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;
  
  // Ntuple initialization

  if ( 0 == ntupleSvc() ) {
    fatal() << "NtupleSvc is not available !" << endmsg;
    return StatusCode::FAILURE ;
  }

  NTupleFilePtr file ( ntupleSvc() , "/NTUPLES/FILE1" ) ;
  
  if ( !file ) {
    fatal() << "Could not access /NTUPLES/FILE1" << endmsg;
    return StatusCode::FAILURE ;
  }
  
  NTuplePtr nt ( ntupleSvc() , "/NTUPLES/FILE1/MCTruth/1" ) ;
  if ( !nt ) {
    nt = ntupleSvc() -> book ( "/NTUPLES/FILE1/MCTruth/1" ,
                               CLID_ColumnWiseTuple ,
                               "MCTruth" ) ;
  }
  if ( ! nt ) {
    fatal() << "Could not create Ntuple (1)" << endmsg;
    return StatusCode::FAILURE ;
  }
  
  m_nTuple = nt ;
  
  // PART block

  sc = nt -> addItem ( "NPart" , m_nPart , 0 , m_nPartMax ) ;
  if ( sc.isSuccess() ) {
    sc = nt -> addIndexedItem( "e" , m_nPart , m_e ) ;
  }
  if ( sc.isSuccess() ) {
    sc = nt -> addIndexedItem( "px" , m_nPart , m_px ) ;
  }
  if ( sc.isSuccess() ) {
    sc = nt -> addIndexedItem( "py" , m_nPart , m_py ) ;
  }
  if ( sc.isSuccess() ) {
    sc = nt -> addIndexedItem( "pz" , m_nPart , m_pz ) ;
  }
  if ( sc.isSuccess() ) {
    sc = nt -> addIndexedItem( "vxProd" , m_nPart , m_vxProd ) ;
  }
  if ( sc.isSuccess() ) {
    sc = nt -> addIndexedItem( "vyProd" , m_nPart , m_vyProd ) ;
  }
  if ( sc.isSuccess() ) {
    sc = nt -> addIndexedItem( "vzProd" , m_nPart , m_vzProd ) ;
  }
  if ( sc.isSuccess() ) {
    sc = nt -> addIndexedItem( "vtProd" , m_nPart , m_vtProd ) ;
  }
  if ( sc.isSuccess() ) {
    sc = nt -> addIndexedItem( "vxDecay" , m_nPart , m_vxDecay ) ;
  }
  if ( sc.isSuccess() ) {
    sc = nt -> addIndexedItem( "vyDecay" , m_nPart , m_vyDecay ) ;
  }
  if ( sc.isSuccess() ) {
    sc = nt -> addIndexedItem( "vzDecay" , m_nPart , m_vzDecay ) ;
  }
  if ( sc.isSuccess() ) {
    sc = nt -> addIndexedItem( "vtDecay" , m_nPart , m_vtDecay ) ;
  }
  if ( sc.isSuccess() ) {
    sc = nt -> addIndexedItem( "pdgId" , m_nPart , m_pdgId ) ;
  }
  if ( sc.isSuccess() ) {
    sc = nt -> addIndexedItem( "nDau" , m_nPart , m_nDau ) ;
  }
  if ( sc.isSuccess() ) {
    sc = nt -> addIndexedItem( "pdgIdMother" , m_nPart , m_pdgIdMother ) ;
  }
  if ( sc.isSuccess() ) {
    sc = nt -> addIndexedItem( "pdgIdDau1" , m_nPart , m_pdgIdDau1 ) ;
  }
  if ( sc.isSuccess() ) {
    sc = nt -> addIndexedItem( "pdgIdDau2" , m_nPart , m_pdgIdDau2 ) ;
  }
  if ( sc.isSuccess() ) {
    sc = nt -> addIndexedItem( "pdgIdDau3" , m_nPart , m_pdgIdDau3 ) ;
  }
  if ( sc.isSuccess() ) {
    sc = nt -> addIndexedItem( "pdgIdDau4" , m_nPart , m_pdgIdDau4 ) ;
  }
  if ( sc.isSuccess() ) {
    sc = nt -> addIndexedItem( "pdgIdDau5" , m_nPart , m_pdgIdDau5 ) ;
  }
  if ( sc.isSuccess() ) {
    sc = nt -> addIndexedItem( "pdgIdDau6" , m_nPart , m_pdgIdDau6 ) ;
  }
  if ( sc.isSuccess() ) {
    sc = nt -> addIndexedItem( "indexMother" , m_nPart , m_indexMother ) ;
  }
  if ( sc.isSuccess() ) {
    sc = nt -> addIndexedItem( "indexInter" , m_nPart, m_indexInter ) ;
  }

  // Inter Block  
  if ( sc.isSuccess() ) {
    sc = nt -> addItem ( "NInter" , m_nInter , 0 , m_nInterMax ) ;
  }
  if ( sc.isSuccess() ) {
    sc = nt -> addIndexedItem ( "isBB" , m_nInter , m_isBB ) ;
  }

  // Event Variables  
  if ( sc.isSuccess() ) {
    sc = nt -> addItem ( "Mandelstam" , m_event , 0 , m_event_max ) ;
  }
  if ( sc.isSuccess() ) {
    sc = nt -> addIndexedItem ( "s_hat" , m_event , m_shat ) ;
  }
  if ( sc.isSuccess() ) {
    sc = nt -> addIndexedItem ( "t_hat" , m_event , m_that ) ;
  }
  if ( sc.isSuccess() ) {
    sc = nt -> addIndexedItem ( "u_hat" , m_event , m_uhat ) ;
  }
  if ( sc.isSuccess() ) {
    sc = nt -> addIndexedItem ( "pt_hat" , m_event , m_pthat ) ;
  }
  if ( sc.isSuccess() ) {
    sc = nt -> addIndexedItem ( "x1_Bjork" , m_event , m_x1 ) ;
    }
  if ( sc.isSuccess() ) {
    sc = nt -> addIndexedItem ( "x2_Bjork" , m_event , m_x2 ) ;
  }
  if ( sc.isSuccess() ) {
    sc = nt -> addIndexedItem ( "m_procId" , m_event , m_procId ) ;
  }

  if ( ! sc.isSuccess() ) {
    error() << "Error declaring NTuple" << endmsg ;
  }

  return sc ;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode GeneratorFullMonitor::execute() {

  debug() << "==> Execute" << endmsg;

  m_event = 0;
  LHCb::GenCollisions* collisions = get<LHCb::GenCollisions>( m_inputColl );
  for( LHCb::GenCollisions::iterator itC = collisions->begin(); 
         collisions->end() != itC; ++itC ) {
    debug() << "GenCollision ::: " << endmsg;
    debug() << (*itC) << " ," 
            << (*itC)->event()->pGenEvt()->signal_process_id() << " "
            << " sHat = " << (*itC)->sHat() << " "
            << " tHat = " << (*itC)->tHat() << " "
            << " uHat = " << (*itC)->uHat() << " "
            << " x1 = " << (*itC)->x1Bjorken() << " "
            << " x2 = " << (*itC)->x2Bjorken() << " "
            << endmsg;
    m_procId [ m_event ] = (*itC)->event()->pGenEvt()->signal_process_id();
    m_shat   [ m_event ] = (*itC)->sHat();
    m_that   [ m_event ] = (*itC)->tHat();
    m_uhat   [ m_event ] = (*itC)->uHat();
    m_pthat  [ m_event ] = (*itC)->ptHat() ;
    m_x1     [ m_event ] = (*itC)->x1Bjorken();
    m_x2     [ m_event ] = (*itC)->x2Bjorken();
    m_event++;
    if(m_event >= m_event_max){
      warning() << "Not all Hard Process info recorded." << endmsg;
      break;
    }    
  }

  // Get HepMCEvents
  LHCb::HepMCEvents* hepVect = get<LHCb::HepMCEvents>( m_inputHepMC );

  // Loop over events
  LHCb::HepMCEvents::iterator it ;
  // Initialize counters
  m_nPart = 0 ;
  m_nInter = 0 ;    
  for( it  = hepVect->begin(); it != hepVect->end(); it++ ) {
    m_isBB[ m_nInter ] = 0 ;
    debug() << "Generator process = " 
            << (*it)->pGenEvt()->signal_process_id()
            << endmsg;
    // loop over all vertices in event
    for( HepMC::GenEvent::vertex_const_iterator pVertex = 
           (*it)->pGenEvt()->vertices_begin();
         pVertex != (*it)->pGenEvt()->vertices_end();
         pVertex++ ) {
      // loop over outgoing particles, check it has status 
      // 1, 888 or 889
      for( HepMC::GenVertex::particles_out_const_iterator pParticle =
             ( *pVertex )->particles_out_const_begin( );
           ( *pVertex )->particles_out_const_end( ) != pParticle;
           ++pParticle ) {
        if( ( (*pParticle)->status( ) == 1 ) ||
            ( (*pParticle)->status( ) == 2 ) ||
            ( (*pParticle)->status( ) == 888 ) ||
            ( (*pParticle)->status( ) == 889 ) ||
            ( (*pParticle)->status( ) == 998 ) ) {
          if( ! (*pParticle)->production_vertex() ) {
            FillNtuple( (*pParticle), 0, 0 );
          } else {              
            if( (*pParticle)->production_vertex()->particles_in_size() 
                == 1 ) {
              LHCb::ParticleID pidM ( (*(*pParticle)->production_vertex()
                                       ->particles_in_const_begin())
                                       ->pdg_id() );
              LHCb::ParticleID pid ( (*pParticle)->pdg_id ( ) ) ;
              if( ( ( ( ! pidM.isHadron( ) ) &&
                      ( ! pidM.isLepton( ) ) &&
                      ( ! pidM.isNucleus( ) ) &&
                      ( pidM.abspid() != 22 ) ) ||
                    ( (*(*pParticle)->production_vertex()
                       ->particles_in_const_begin())->status() == 3 ) ) && 
                  ( ( pid.isHadron( ) ) ||
                    ( pid.isLepton( ) ) ||
                    ( pid.isNucleus( ) ) ||
                    ( pid.abspid() == 22 ) ) )
                  FillNtuple( (*pParticle) , 0 , 0 );
            }
            else {
              LHCb::ParticleID pid( (*pParticle)->pdg_id( ) );
              if( ( pid.isHadron( ) ) ||
                  ( pid.isLepton( ) ) ||
                  ( pid.isNucleus( ) ) ||
                  ( pid.abspid() == 22 ) ) 
                FillNtuple( (*pParticle), 0, 0 );
            }
          }                
        }
      }
    }
    m_nInter ++ ;
    if( m_nInter >= m_nInterMax ) break;
  }

  m_nTuple->write( );
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode GeneratorFullMonitor::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
// Fill Ntuple
//=============================================================================
void GeneratorFullMonitor::FillNtuple( HepMC::GenParticle * thePart ,
                                  int pdgMotherId ,
                                  int motherIndex ) 
{
  if ( m_nPart >= m_nPartMax ) return ;
  
  // Fill PART Block
  m_e [ m_nPart ] = thePart -> momentum() . e() ;
  m_px[ m_nPart ] = thePart -> momentum() . px() ;
  m_py[ m_nPart ] = thePart -> momentum() . py() ;
  m_pz[ m_nPart ] = thePart -> momentum() . pz() ;
  //
  if ( thePart -> production_vertex ( ) ) {
    m_vxProd [ m_nPart ] = thePart -> production_vertex() -> position() . x() ;
    m_vyProd [ m_nPart ] = thePart -> production_vertex() -> position() . y() ;
    m_vzProd [ m_nPart ] = thePart -> production_vertex() -> position() . z() ;
    m_vtProd [ m_nPart ] = thePart -> production_vertex() -> position() . t() ;
  } else {
    m_vxProd [ m_nPart ] = -10000000. ;
    m_vyProd [ m_nPart ] = -10000000. ;
    m_vzProd [ m_nPart ] = -10000000. ;
    m_vtProd [ m_nPart ] = -10000000. ;
  }
  // Decay Vertex
  if ( thePart -> end_vertex ( ) ) {
    m_vxDecay [ m_nPart ] = thePart -> end_vertex() -> position() . x() ;
    m_vyDecay [ m_nPart ] = thePart -> end_vertex() -> position() . y() ;
    m_vzDecay [ m_nPart ] = thePart -> end_vertex() -> position() . z() ;
    m_vtDecay [ m_nPart ] = thePart -> end_vertex() -> position() . t() ;
    m_nDau[ m_nPart ] = thePart -> end_vertex() -> particles_out_size() ;
    HepMC::GenVertex::particles_out_const_iterator theIt =
      thePart -> end_vertex() -> particles_out_const_begin( ) ;
    //
    if ( thePart -> end_vertex() -> particles_out_size() == 1 ) {
      m_pdgIdDau1 [ m_nPart ] = ( * theIt ) -> pdg_id () ;
      m_pdgIdDau2 [ m_nPart ] = 0 ;
      m_pdgIdDau3 [ m_nPart ] = 0 ;
      m_pdgIdDau4 [ m_nPart ] = 0 ;
      m_pdgIdDau5 [ m_nPart ] = 0 ;
      m_pdgIdDau6 [ m_nPart ] = 0 ;    
    } else if ( thePart -> end_vertex() -> particles_out_size() == 2 ) {
      m_pdgIdDau1 [ m_nPart ] = ( * theIt ) -> pdg_id () ;
      m_pdgIdDau2 [ m_nPart ] = ( * ++theIt ) -> pdg_id () ;
      m_pdgIdDau3 [ m_nPart ] = 0 ;
      m_pdgIdDau4 [ m_nPart ] = 0 ;
      m_pdgIdDau5 [ m_nPart ] = 0 ;
      m_pdgIdDau6 [ m_nPart ] = 0 ;    
    } else if ( thePart -> end_vertex() -> particles_out_size() == 3 ) {
      m_pdgIdDau1 [ m_nPart ] = ( * theIt ) -> pdg_id () ;
      m_pdgIdDau2 [ m_nPart ] = ( * ++theIt ) -> pdg_id () ;
      m_pdgIdDau3 [ m_nPart ] = ( * ++theIt ) -> pdg_id () ;
      m_pdgIdDau4 [ m_nPart ] = 0 ;
      m_pdgIdDau5 [ m_nPart ] = 0 ;
      m_pdgIdDau6 [ m_nPart ] = 0 ;    
    } else if ( thePart -> end_vertex() -> particles_out_size() == 4 ) {
      m_pdgIdDau1 [ m_nPart ] = ( * theIt ) -> pdg_id () ;
      m_pdgIdDau2 [ m_nPart ] = ( * ++theIt ) -> pdg_id () ;
      m_pdgIdDau3 [ m_nPart ] = ( * ++theIt ) -> pdg_id () ;
      m_pdgIdDau4 [ m_nPart ] = ( * ++theIt ) -> pdg_id () ;
      m_pdgIdDau5 [ m_nPart ] = 0 ;
      m_pdgIdDau6 [ m_nPart ] = 0 ;    
    } else if ( thePart -> end_vertex() -> particles_out_size() == 5 ) {
      m_pdgIdDau1 [ m_nPart ] = ( * theIt ) -> pdg_id () ;
      m_pdgIdDau2 [ m_nPart ] = ( * ++theIt ) -> pdg_id () ;
      m_pdgIdDau3 [ m_nPart ] = ( * ++theIt ) -> pdg_id () ;
      m_pdgIdDau4 [ m_nPart ] = ( * ++theIt ) -> pdg_id () ;
      m_pdgIdDau5 [ m_nPart ] = ( * ++theIt ) -> pdg_id () ;
      m_pdgIdDau6 [ m_nPart ] = 0 ;    
    } else if ( thePart -> end_vertex() -> particles_out_size() == 6 ) {
      m_pdgIdDau1 [ m_nPart ] = ( * theIt ) -> pdg_id () ;
      m_pdgIdDau2 [ m_nPart ] = ( * ++theIt ) -> pdg_id () ;
      m_pdgIdDau3 [ m_nPart ] = ( * ++theIt ) -> pdg_id () ;
      m_pdgIdDau4 [ m_nPart ] = ( * ++theIt ) -> pdg_id () ;
      m_pdgIdDau5 [ m_nPart ] = ( * ++theIt ) -> pdg_id () ;
      m_pdgIdDau6 [ m_nPart ] = ( * ++theIt ) -> pdg_id () ;    
    } else {
      m_pdgIdDau1 [ m_nPart ] = 0 ;
      m_pdgIdDau2 [ m_nPart ] = 0 ;
      m_pdgIdDau3 [ m_nPart ] = 0 ;
      m_pdgIdDau4 [ m_nPart ] = 0 ;
      m_pdgIdDau5 [ m_nPart ] = 0 ;
      m_pdgIdDau6 [ m_nPart ] = 0 ;    
    }
  } else {
    m_vxDecay [ m_nPart ] = 10000000. ;
    m_vyDecay [ m_nPart ] = 10000000. ;
    m_vzDecay [ m_nPart ] = 10000000. ;
    m_vtDecay [ m_nPart ] = 10000000. ;
    m_nDau [ m_nPart ] = 0 ;
    m_pdgIdDau1 [ m_nPart ] = 0 ;
    m_pdgIdDau2 [ m_nPart ] = 0 ;
    m_pdgIdDau3 [ m_nPart ] = 0 ;
    m_pdgIdDau4 [ m_nPart ] = 0 ;
    m_pdgIdDau5 [ m_nPart ] = 0 ;
    m_pdgIdDau6 [ m_nPart ] = 0 ;    
  }
  m_pdgId [ m_nPart ] = thePart -> pdg_id ( ) ;
  m_pdgIdMother [ m_nPart ] = pdgMotherId ;
  m_indexMother [ m_nPart ] = motherIndex ;
            
  m_indexInter [ m_nPart ] = m_nInter ;

  int index = m_nPart ;
  
  m_nPart ++ ;

  // Fill Info for daughters
  if ( thePart -> end_vertex( ) ) {
    for ( HepMC::GenVertex::particle_iterator theIterator =
            thePart -> end_vertex() -> particles_begin( HepMC::children ) ;
          theIterator !=
            thePart -> end_vertex() -> particles_end( HepMC::children ) ;
          theIterator++ ) {
      FillNtuple( (*theIterator) , thePart -> pdg_id ( ) , index + 1 ) ;
    }
  }
}
