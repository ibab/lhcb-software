// $Id: GeneratorFullMonitor.cpp,v 1.3 2004-05-14 07:06:04 robbep Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/SmartDataPtr.h"

// from Event
#include "Event/HepMCEvent.h"
#include "Event/HardInfo.h"

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
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<GeneratorFullMonitor>          s_factory ;
const        IAlgFactory& GeneratorFullMonitorFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GeneratorFullMonitor::GeneratorFullMonitor( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : Algorithm ( name , pSvcLocator )
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
    , m_nPartMax( 2000 ) 
    , m_nInterMax( 20 ) 
    , m_inputData( HepMCEventLocation::Default )
{
  declareProperty( "Input", m_inputData );
}

//=============================================================================
// Destructor
//=============================================================================
GeneratorFullMonitor::~GeneratorFullMonitor() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode GeneratorFullMonitor::initialize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "==> Initialise" << endreq;
  
  // Ntuple initialization

  if ( 0 == ntupleSvc() ) {
    msg << MSG::FATAL << "NtupleSvc is not available !" << endreq ;
    return StatusCode::FAILURE ;
  }

  NTupleFilePtr file ( ntupleSvc() , "/NTUPLES/FILE1" ) ;
  
  if ( ! file ) {
    msg << MSG::FATAL << "Could not access /NTUPLES/FILE1" 
        << endreq ;
    return StatusCode::FAILURE ;
  }
  
  NTuplePtr nt ( ntupleSvc() , "/NTUPLES/FILE1/MCTruth/1" ) ;
  if ( ! nt ) {
    nt = ntupleSvc() -> book ( "/NTUPLES/FILE1/MCTruth/1" ,
                               CLID_ColumnWiseTuple ,
                               "MCTruth" ) ;
  }
  if ( ! nt ) {
    msg << MSG::FATAL << "Could not create Ntuple (1)" 
        << endreq ;
    return StatusCode::FAILURE ;
  }
  
  m_nTuple = nt ;
  
  StatusCode sc ;

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
  if ( ! sc.isSuccess() ) {
    msg << MSG::ERROR << "Error declaring NTuple" << endreq ;
  }

  return sc ;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode GeneratorFullMonitor::execute() {

  MsgStream  msg( msgSvc(), name() );
  msg << MSG::DEBUG << "==> Execute" << endreq;
  SmartDataPtr<HardInfos> hardVect ( eventSvc(), HardInfoLocation::Default );
  if( !hardVect ) {
    msg << MSG::DEBUG << "This event has no Hard Process Info" 
        << endreq ;
  } 
  else {
    for( HardInfos::iterator itHard = hardVect->begin(); 
         hardVect->end() != itHard; ++itHard ) {
      msg << MSG::DEBUG << "Hard Info is ::: "
          << (*itHard)->event()->pGenEvt()->signal_process_id() << " " 
          << (*itHard)->sHat() << " " 
          << (*itHard)->tHat() << " " 
          << (*itHard)->uHat() << " " 
          << (*itHard)->ptHat() << " " 
          << (*itHard)->x1Bjorken() << " " 
          << (*itHard)->x2Bjorken()
          << endreq;
    }
  }


  // Get HepMCEvents
  SmartDataPtr< HepMCEvents > hepVect ( eventSvc() , m_inputData ) ;
  
  if ( ! hepVect ) {
    msg << MSG::WARNING << "This event has no HepMCEvent" 
        << endreq ;
    return StatusCode::FAILURE;
  } else {
    // Loop over events
    HepMCEvents::iterator it ;
    // Initialize counters
    m_nPart = 0 ;
    m_nInter = 0 ;
    
    for ( it  = hepVect -> begin() ;
          it != hepVect -> end  () ;
          it++ ) {
      m_isBB[ m_nInter ] = 0 ;
      msg << MSG::DEBUG << "Generator process = " 
          << (*it)->pGenEvt()->signal_process_id()
          << endreq;
      // loop over all vertices in event
      for ( HepMC::GenEvent::vertex_const_iterator pVertex = 
              (*it) -> pGenEvt() -> vertices_begin() ;
            pVertex != (*it) -> pGenEvt() -> vertices_end() ;
            pVertex++ ) {
        // loop over outgoing particles, check it has status 
        // 1, 888 or 889
        for ( HepMC::GenVertex::particles_out_const_iterator pParticle =
                ( *pVertex ) -> particles_out_const_begin ( ) ;
              ( *pVertex ) -> particles_out_const_end ( ) != pParticle ;
              ++pParticle ) {
          if ( ( (*pParticle) -> status ( ) == 1 ) ||
               ( (*pParticle) -> status ( ) == 2 ) ||
               ( (*pParticle) -> status ( ) == 888 ) ||
               ( (*pParticle) -> status ( ) == 889 ) ||
               ( (*pParticle) -> status ( ) == 998 ) ) {
            if ( ! (*pParticle) -> production_vertex() ) {
              FillNtuple ( (*pParticle) , 0 , 0 ) ;
            } else {              
              if ( (*pParticle) -> production_vertex() -> particles_in_size() 
                   == 1 ) {
                ParticleID pidM ( (*(*pParticle)->production_vertex()
                                  -> particles_in_const_begin())
                                  ->pdg_id() ) ;
                ParticleID pid ( (*pParticle) -> pdg_id ( ) ) ;
                if ( ( ( ( ! pidM.isHadron( ) ) &&
                         ( ! pidM.isLepton( ) ) &&
                         ( ! pidM.isNucleus( ) ) &&
                         ( pidM.abspid() != 22 ) ) ||
                       ( (*(*pParticle)->production_vertex()
                          -> particles_in_const_begin()) 
                         -> status() == 3 ) ) && 
                     ( ( pid.isHadron( ) ) ||
                       ( pid.isLepton( ) ) ||
                       ( pid.isNucleus( ) ) ||
                       ( pid.abspid() == 22 ) ) )
                  FillNtuple( (*pParticle) , 0 , 0 ) ;
              }
              else {
                ParticleID pid ( (*pParticle) -> pdg_id ( ) ) ;
                if ( ( pid.isHadron( ) ) ||
                     ( pid.isLepton( ) ) ||
                     ( pid.isNucleus( ) ) ||
                     ( pid.abspid() == 22 ) ) 
                  FillNtuple( (*pParticle) , 0 , 0 ) ;
              }
            }                
          }
        }
      }
      m_nInter ++ ;
      if ( m_nInter >= m_nInterMax ) break ;
    }
  }

  m_nTuple -> write ( ) ;
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode GeneratorFullMonitor::finalize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "==> Finalize" << endreq;

  return StatusCode::SUCCESS;
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
