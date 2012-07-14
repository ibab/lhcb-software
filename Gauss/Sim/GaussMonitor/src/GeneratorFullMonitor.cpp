// $Id: GeneratorFullMonitor.cpp,v 1.8 2008-11-11 19:15:56 robbep Exp $
// Include files 

// local
#include "GeneratorFullMonitor.h"

#include <iostream>

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

// from Event
#include "Event/HepMCEvent.h"
#include "Event/GenCollision.h"
#include "Event/GenHeader.h"

// from Kernel
#include "Kernel/ParticleID.h"

// from HepMC
#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenParticle.h"

//-----------------------------------------------------------------------------
// Implementation file for class : GeneratorFullMonitor
//
// 2004-03-02 : Patrick Robbe
// 2007-01-30 : Paul Szczypka, add information on hard interaction
// 2007-02-16 : Gloria Corti, initialize m_event
// 2011-05-25 : Gloria Corti, add run and event number information
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( GeneratorFullMonitor );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GeneratorFullMonitor::GeneratorFullMonitor( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator )
    , m_nPartMax( 2000 ) 
    , m_nInterMax( 20 ) 
    , m_event_max( 5 ) {
  declareProperty( "HepMCEvents", 
                   m_inputHepMC = LHCb::HepMCEventLocation::Default);
  declareProperty( "Collisions", 
                   m_inputColl = LHCb::GenCollisionLocation::Default);
  declareProperty( "Header", m_inputHeader = LHCb::GenHeaderLocation::Default);
  
}

//=============================================================================
// Destructor
//=============================================================================
GeneratorFullMonitor::~GeneratorFullMonitor() {}; 

//=============================================================================
// Main execution
//=============================================================================
StatusCode GeneratorFullMonitor::execute() {
  debug() << "==> Execute" << endmsg;

  Tuple ntp = nTuple( 1 , "MCTruth" , CLID_ColumnWiseTuple ) ;

  int nEvent = 0;

  LHCb::GenHeader* header = get<LHCb::GenHeader>(m_inputHeader);
  int runN = header->runNumber();
  int evtN = header->evtNumber();
  ntp -> column( "runN", runN );
  ntp -> column( "evtN", evtN );

  LHCb::GenCollisions* collisions = get<LHCb::GenCollisions>( m_inputColl );
  
  std::vector< int > procId ;
  std::vector< double > shat , that , uhat , pthat , x1 , x2 ;
  
  for( LHCb::GenCollisions::iterator itC = collisions->begin(); 
         collisions->end() != itC ; ++itC ) {
    debug() << "GenCollision ::: " << endmsg;
    debug() << (*itC) << " ," 
            << (*itC)->event()->pGenEvt()->signal_process_id() << " "
            << " sHat = " << (*itC)->sHat() << " "
            << " tHat = " << (*itC)->tHat() << " "
            << " uHat = " << (*itC)->uHat() << " "
            << " x1 = " << (*itC)->x1Bjorken() << " "
            << " x2 = " << (*itC)->x2Bjorken() << " "
            << endmsg;
    procId.push_back( (*itC)->event()->pGenEvt()->signal_process_id() ) ;
    shat.push_back( (*itC)->sHat() ) ;
    that.push_back( (*itC)->tHat() ) ;
    uhat.push_back( (*itC)->uHat() ) ;
    pthat.push_back( (*itC)->ptHat() ) ;
    x1.push_back( (*itC)->x1Bjorken() ) ;
    x2.push_back( (*itC)->x2Bjorken() ) ; 
    nEvent++;
    if( nEvent >= m_event_max ) {
      warning() << "Not all Hard Process info recorded." << endmsg;
      break;
    }
  }

  ntp -> farray( "procId"   , procId , "NInter" , m_event_max ) ;
  ntp -> farray( "s_hat"    , shat   , "NInter" , m_event_max ) ;
  ntp -> farray( "t_hat"    , that   , "NInter" , m_event_max ) ;
  ntp -> farray( "u_hat"    , uhat   , "NInter" , m_event_max ) ;
  ntp -> farray( "pt_hat"   , pthat  , "NInter" , m_event_max ) ;
  ntp -> farray( "x1_Bjork" , x1     , "NInter" , m_event_max ) ;
  ntp -> farray( "x2_Bjork" , x2     , "NInter" , m_event_max ) ; 
  
  // Get HepMCEvents
  LHCb::HepMCEvents* hepVect = get<LHCb::HepMCEvents>( m_inputHepMC );

  // Loop over events
  LHCb::HepMCEvents::iterator it ;
  // Initialize counters
  m_nPart = 0 ;
  m_nInter = 0 ;
  
  m_e.clear() ;
  m_px.clear() ;
  m_py.clear() ;
  m_pz.clear() ;
  m_vxProd.clear() ;
  m_vyProd.clear() ;
  m_vzProd.clear() ;
  m_vtProd.clear() ;
  m_vxDecay.clear() ;
  m_vyDecay.clear() ;
  m_vzDecay.clear() ;
  m_vtDecay.clear() ;
  m_pdgId.clear() ;
  m_nDau.clear() ;
  m_pdgIdMother.clear() ;
  m_pdgIdDau1.clear()  ;
  m_pdgIdDau2.clear() ;
  m_pdgIdDau3.clear() ;
  m_pdgIdDau4.clear() ;
  m_pdgIdDau5.clear() ;
  m_pdgIdDau6.clear() ;
  m_indexMother.clear() ;
  m_indexInter.clear() ;
  
  std::vector< int > isBB ;
  
  for( it  = hepVect->begin(); it != hepVect->end(); it++ ) {
    isBB.push_back( 0 ) ;
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
                       ->particles_in_const_begin())->status() == 3 ) ) /*&& 
                  ( ( pid.isHadron( ) ) ||
                    ( pid.isLepton( ) ) ||
                    ( pid.isNucleus( ) ) ||
                   ( pid.abspid() == 22 ) )*/ ){
                    FillNtuple( (*pParticle) , 0 , 0 );
                  }
                  
            }
            else {
              
              LHCb::ParticleID pid( (*pParticle)->pdg_id( ) );
              if( ( pid.isHadron( ) ) ||
                  ( pid.isLepton( ) ) ||
                  ( pid.isNucleus( ) ) ||
                 ( pid.abspid() == 22 ) ){
                
                FillNtuple( (*pParticle), 0, 0 );
              }
                
            }
          }                
        }
      }
    }
    m_nInter ++ ;
    if( m_nInter >= m_nInterMax ) break;
  }
  
  ntp -> farray( "e" , m_e     , "NPart" , m_nPartMax ) ; 
  ntp -> farray( "px", m_px    , "NPart" , m_nPartMax ) ; 
  ntp -> farray( "py", m_py    , "NPart" , m_nPartMax ) ; 
  ntp -> farray( "pz", m_pz    , "NPart" , m_nPartMax ) ; 
  ntp -> farray( "vxProd", m_vxProd , "NPart" , m_nPartMax ) ; 
  ntp -> farray( "vyProd", m_vyProd , "NPart" , m_nPartMax ) ; 
  ntp -> farray( "vzProd", m_vzProd , "NPart" , m_nPartMax ) ; 
  ntp -> farray( "vtProd", m_vtProd , "NPart" , m_nPartMax ) ; 
  ntp -> farray( "vxDecay", m_vxDecay , "NPart" , m_nPartMax ) ; 
  ntp -> farray( "vyDecay", m_vyDecay , "NPart" , m_nPartMax ) ; 
  ntp -> farray( "vzDecay", m_vzDecay , "NPart" , m_nPartMax ) ; 
  ntp -> farray( "vtDecay", m_vtDecay , "NPart" , m_nPartMax ) ; 
  ntp -> farray( "pdgId", m_pdgId , "NPart" , m_nPartMax ) ; 
  ntp -> farray( "nDau", m_nDau , "NPart" , m_nPartMax ) ; 
  ntp -> farray( "pdgIdMother", m_pdgIdMother , "NPart" , m_nPartMax ) ; 
  ntp -> farray( "pdgIdDau1", m_pdgIdDau1 , "NPart" , m_nPartMax ) ; 
  ntp -> farray( "pdgIdDau2", m_pdgIdDau2 , "NPart" , m_nPartMax ) ; 
  ntp -> farray( "pdgIdDau3", m_pdgIdDau3 , "NPart" , m_nPartMax ) ; 
  ntp -> farray( "pdgIdDau4", m_pdgIdDau4 , "NPart" , m_nPartMax ) ; 
  ntp -> farray( "pdgIdDau5", m_pdgIdDau5 , "NPart" , m_nPartMax ) ; 
  ntp -> farray( "pdgIdDau6", m_pdgIdDau6 , "NPart" , m_nPartMax ) ; 
  ntp -> farray( "indexMother", m_indexMother , "NPart" , m_nPartMax ) ; 
  ntp -> farray( "indexInter", m_indexInter , "NPart" , m_nPartMax ) ;

  ntp -> write( );
  return StatusCode::SUCCESS;
}

//=============================================================================
// Fill Ntuple
//=============================================================================
void GeneratorFullMonitor::FillNtuple( HepMC::GenParticle * thePart ,
				       int pdgMotherId ,
                                       int motherIndex ) {
  if ( m_nPart >= m_nPartMax ) return ;
  
  // Fill PART Block
  m_e.push_back( thePart -> momentum() . e() ) ;
  m_px.push_back( thePart -> momentum() . px() ) ;
  m_py.push_back( thePart -> momentum() . py() ) ;
  m_pz.push_back( thePart -> momentum() . pz() ) ;
  //
  if ( thePart -> production_vertex ( ) ) {
    m_vxProd.push_back( thePart -> production_vertex() -> position() . x() ) ;
    m_vyProd.push_back( thePart -> production_vertex() -> position() . y() ) ;
    m_vzProd.push_back( thePart -> production_vertex() -> position() . z() ) ;
    m_vtProd.push_back( thePart -> production_vertex() -> position() . t() ) ;
  } else {
    m_vxProd.push_back( -10000000. ) ;
    m_vyProd.push_back( -10000000. ) ;
    m_vzProd.push_back( -10000000. ) ;
    m_vtProd.push_back( -10000000. ) ;
  }
  // Decay Vertex
  if ( thePart -> end_vertex ( ) ) {
    m_vxDecay.push_back( thePart -> end_vertex() -> position() . x() ) ;
    m_vyDecay.push_back( thePart -> end_vertex() -> position() . y() ) ;
    m_vzDecay.push_back( thePart -> end_vertex() -> position() . z() ) ;
    m_vtDecay.push_back( thePart -> end_vertex() -> position() . t() ) ;
    m_nDau.push_back( thePart -> end_vertex() -> particles_out_size() ) ;
    HepMC::GenVertex::particles_out_const_iterator theIt =
      thePart -> end_vertex() -> particles_out_const_begin( ) ;
    //
    if ( thePart -> end_vertex() -> particles_out_size() == 1 ) {
      m_pdgIdDau1.push_back( ( * theIt ) -> pdg_id () ) ;
      m_pdgIdDau2.push_back( 0 ) ;
      m_pdgIdDau3.push_back( 0 ) ;
      m_pdgIdDau4.push_back( 0 ) ;
      m_pdgIdDau5.push_back( 0 ) ;
      m_pdgIdDau6.push_back( 0 ) ; 
    } else if ( thePart -> end_vertex() -> particles_out_size() == 2 ) {
      m_pdgIdDau1.push_back( ( * theIt ) -> pdg_id () ) ;
      m_pdgIdDau2.push_back( ( * ++theIt ) -> pdg_id () ) ;
      m_pdgIdDau3.push_back( 0 ) ;
      m_pdgIdDau4.push_back( 0 ) ;
      m_pdgIdDau5.push_back( 0 ) ;
      m_pdgIdDau6.push_back( 0 ) ; 
    } else if ( thePart -> end_vertex() -> particles_out_size() == 3 ) {
      m_pdgIdDau1.push_back( ( * theIt ) -> pdg_id () ) ;
      m_pdgIdDau2.push_back( ( * ++theIt ) -> pdg_id () ) ;
      m_pdgIdDau3.push_back( ( * ++theIt ) -> pdg_id () ) ;
      m_pdgIdDau4.push_back( 0 ) ;
      m_pdgIdDau5.push_back( 0 ) ;
      m_pdgIdDau6.push_back( 0 ) ;
    } else if ( thePart -> end_vertex() -> particles_out_size() == 4 ) {
      m_pdgIdDau1.push_back( ( * theIt ) -> pdg_id () ) ;
      m_pdgIdDau2.push_back( ( * ++theIt ) -> pdg_id () ) ;
      m_pdgIdDau3.push_back( ( * ++theIt ) -> pdg_id () ) ;
      m_pdgIdDau4.push_back( ( * ++theIt ) -> pdg_id () ) ;
      m_pdgIdDau5.push_back( 0 ) ;
      m_pdgIdDau6.push_back( 0 ) ; 
    } else if ( thePart -> end_vertex() -> particles_out_size() == 5 ) {
      m_pdgIdDau1.push_back( ( * theIt ) -> pdg_id () ) ;
      m_pdgIdDau2.push_back( ( * ++theIt ) -> pdg_id () ) ;
      m_pdgIdDau3.push_back( ( * ++theIt ) -> pdg_id () ) ;
      m_pdgIdDau4.push_back( ( * ++theIt ) -> pdg_id () ) ;
      m_pdgIdDau5.push_back( ( * ++theIt ) -> pdg_id () ) ;
      m_pdgIdDau6.push_back( 0 ) ; 
    } else if ( thePart -> end_vertex() -> particles_out_size() == 6 ) {
      m_pdgIdDau1.push_back( ( * theIt ) -> pdg_id () ) ;
      m_pdgIdDau2.push_back( ( * ++theIt ) -> pdg_id () ) ;
      m_pdgIdDau3.push_back( ( * ++theIt ) -> pdg_id () ) ;
      m_pdgIdDau4.push_back( ( * ++theIt ) -> pdg_id () ) ;
      m_pdgIdDau5.push_back( ( * ++theIt ) -> pdg_id () ) ;
      m_pdgIdDau6.push_back( ( * ++theIt ) -> pdg_id () ) ; 
    } else {
      m_pdgIdDau1.push_back( 0 ) ;
      m_pdgIdDau2.push_back( 0 ) ;
      m_pdgIdDau3.push_back( 0 ) ;
      m_pdgIdDau4.push_back( 0 ) ;
      m_pdgIdDau5.push_back( 0 ) ;
      m_pdgIdDau6.push_back( 0 ) ; 
    }
  } else {
    m_vxDecay.push_back( 10000000. ) ;
    m_vyDecay.push_back( 10000000. ) ;
    m_vzDecay.push_back( 10000000. ) ;
    m_vtDecay.push_back( 10000000. ) ;
    m_nDau.push_back( 0 ) ;
    m_pdgIdDau1.push_back( 0 ) ;
    m_pdgIdDau2.push_back( 0 ) ;
    m_pdgIdDau3.push_back( 0 ) ;
    m_pdgIdDau4.push_back( 0 ) ;
    m_pdgIdDau5.push_back( 0 ) ;
    m_pdgIdDau6.push_back( 0 ) ; 
  }
  m_pdgId.push_back( thePart -> pdg_id ( ) ) ;
  m_pdgIdMother.push_back( pdgMotherId ) ;
  m_indexMother.push_back( motherIndex ) ;
            
  m_indexInter.push_back( m_nInter ) ;

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
