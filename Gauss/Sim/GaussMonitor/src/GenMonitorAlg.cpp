// $Id: GenMonitorAlg.cpp,v 1.1 2005-04-12 13:00:21 gcorti Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/SmartDataPtr.h"

// From HepMC
#include "Event/HepMCEvent.h"
#include "HepMC/GenEvent.h"
#include "HepMC/GenParticle.h"

// From LHCb
#include "Kernel/ParticleID.h"

// From EvtGen
#include "EvtGenBase/EvtConst.hh"

// local
#include "GenMonitorAlg.h"

//-----------------------------------------------------------------------------
// Implementation file for class : GenMonitorAlg
//
// 2003-11-13 : Patrick Robbe
// 2005-04-11 : G.Corti
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<GenMonitorAlg>          s_factory ;
const        IAlgFactory& GenMonitorAlgFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GenMonitorAlg::GenMonitorAlg( const std::string& name,
                              ISvcLocator* pSvcLocator)
  : GaudiAlgorithm  ( name , pSvcLocator ) ,
    m_counter       ( 0 ) ,
    m_counterstable ( 0 ) ,
    m_counterCharged( 0 ),
    m_counterChInEta( 0 ),
    m_nEvents       ( 0 ) {

  declareProperty( "MinEta", m_minEta = 2.0);
  declareProperty( "MaxEta", m_maxEta = 4.9);
  declareProperty( "FillHistos" , m_doHistos = false );
  declareProperty( "Input",       m_dataPath = "/Event/Gen/HepMCEvents" );
  
}

//=============================================================================
// Destructor
//=============================================================================
GenMonitorAlg::~GenMonitorAlg() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode GenMonitorAlg::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiHistoAlg

  debug() << "==> Initialize" << endmsg;

  if( !m_doHistos ) return StatusCode::SUCCESS;

  m_multiplicity        = histoSvc()->book( name(), 100, "Multiplicity", 
                                            300, 0., 2999. );
  m_multiplicitystable  = histoSvc()->book( name(), 110, 
                                            "Multiplicity ProtoStable",
                                            300, 0., 2999. );
  m_momentum            = histoSvc()->book( name(), 200, "Momentum",
                                            100, 0., 100. );
  m_momentumstable      = histoSvc()->book( name(), 210, 
                                            "Momentum ProtoStable" ,
                                            100, 0., 100. );
  m_pdgId               = histoSvc()->book( name(), 300, "PDGId" ,
                                            10000, -4999., 5000. );
  m_pdgIdstable         = histoSvc()->book( name(), 310, "PDGId ProtoStable" ,
                                            10000, -4999., 5000. );
  m_lifetimestable      = histoSvc()->book( name(), 410,
                                            "Lifetime Stable",
                                            100, -1.5e-10, 1.5e-10 ) ;
  m_multiplicityCharged = histoSvc()->book( name(), 120 ,
                                            "Multiplicity Charged Stable" ,
                                            300, -0.5, 299.5 );
  m_pseudoRapCharged    = histoSvc()->book( name(), 520,
                                            "Pseudorapidity Charged Stable",
                                            150, -15., 15. );
  m_ptCharged           = histoSvc()->book( name(), 620 , "Pt charged stable" ,
                                            100, 0., 20. );
  m_multChEtaAcc        = histoSvc()->book( name(), 130,
                                            "Multiplicity Charged Stable in LHCb eta " ,
                                            300, -0.5, 299.5 );
  
  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode GenMonitorAlg::execute() {

  debug() << "==> Execute" << endreq;

  // Initialize counters
  int nParticles(0), nParticlesStable(0);
  int nParticlesStableCharged(0), nParChStabEtaAcc(0);
  
  // Retrieve data from selected path
  SmartDataPtr< HepMCEvents > hepMCptr ( eventSvc() , m_dataPath );

  if ( 0 == hepMCptr ) {
    info() << "No HepMCEvents at location " << m_dataPath << endreq;
  } else {
    HepMCEvents::iterator it ;
    for ( it = hepMCptr->begin() ; it != hepMCptr->end(); ++it ) {
      for ( HepMC::GenEvent::particle_const_iterator 
              itp = ( *it ) -> pGenEvt() -> particles_begin() ;
            itp != ( *it ) -> pGenEvt() -> particles_end   () ;
            itp++ ) {
        HepMC::GenParticle* hepMCpart = *itp;
        nParticles++ ;
        if ( m_doHistos ) {  
          m_momentum -> fill( hepMCpart->momentum().vect().mag(), 1. ) ;
          m_pdgId    -> fill( hepMCpart->pdg_id(), 1. ) ;
        }
        // Note that the following is really multiplicity of particle defined as
        // stable by Pythia just after hadronization: all particles known by
        // EvtGen are defined stable for Pythia (it will count rho and pi0 and gamma all 
        // togheter as stable ...
        if ( ( hepMCpart -> status() != 2 ) && 
             ( hepMCpart -> status() != 3 ) ) {
          nParticlesStable++ ;
          if ( m_doHistos ) {
            m_momentumstable -> fill( hepMCpart->momentum().vect().mag(), 1.);
            m_pdgIdstable    -> fill( hepMCpart->pdg_id(), 1. );
            m_lifetimestable -> fill( lifetime( hepMCpart ), 1. );
          }
          // Charged stable particles meaning really stable after EvtGen
          ParticleID pID( hepMCpart->pdg_id() );
          if( 0.0 != pID.threeCharge() ) {
            // a particle does not have an outgoing vertex
            if( !hepMCpart->end_vertex() ) {
//             // should be the same as the following  
//             if ( ( hepMCpart -> status() == 999 )  
              ++nParticlesStableCharged;
              double pseudoRap =  hepMCpart->momentum().pseudoRapidity();
              // in LHCb acceptance
              if( (pseudoRap > m_minEta) && (pseudoRap < m_maxEta) ) {
                ++nParChStabEtaAcc;
              }
              if( m_doHistos ) {
                m_pseudoRapCharged->fill( pseudoRap , 1. );
                m_ptCharged->fill( hepMCpart->momentum().perp(), 1. );
              }
            }
          }    
        }
      }
    }
  }
  if( m_doHistos ) {
    m_multiplicity -> fill( nParticles , 1. ) ;  
    m_multiplicitystable -> fill( nParticlesStable, 1. ) ;
    m_multiplicityCharged -> fill( nParticlesStableCharged, 1. );
    m_multChEtaAcc->fill( nParChStabEtaAcc, 1.);   
  }
  m_counter += nParticles ;
  m_counterstable += nParticlesStable ;
  m_counterCharged += nParticlesStableCharged;
  m_counterChInEta += nParChStabEtaAcc;
  m_nEvents++ ;
  
  info() << "Event number " << m_nEvents << " contains "
         << nParticles << " particles" << endreq ;

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode GenMonitorAlg::finalize() {

  debug() << "==> Finalize" << endreq;
  
  info() << std::endl 
         << "======================== Generators Statistics ====================" 
         << std::endl
         << "=                                                                 ="
         << std::endl 
         << "= Number of particles generated: " << m_counter 
         << std::endl 
         << "= Number of events: " << m_nEvents
         << std::endl
         << "= Mean multiplicity: " << m_counter / ( double ) m_nEvents
         << std::endl 
         << "=                                                                 ="
         << std::endl 
         << "= Number of pseudo stable particles generated: " << m_counterstable
         << std::endl 
         << "= Number of events: " << m_nEvents
         << std::endl
         << "= Mean pseudo stable multiplicity: " 
         << m_counterstable / ( double ) m_nEvents
         << std::endl 
         << "=                                                                 ="
         << std::endl 
         << "= Number of charged stable particles generated: " << m_counterCharged
         << std::endl 
         << "= Number of events: " << m_nEvents
         << std::endl
         << "= Mean charged stable multiplicity: " 
         << m_counterCharged / ( double ) m_nEvents
         << std::endl 
         << "=                                                                 ="
         << std::endl 
         << "= Number of charged stable particles in LHCb eta " << m_counterChInEta
         << std::endl 
         << "= Number of events: " << m_nEvents
         << std::endl
         << "= Mean charged stable multiplicity in LHCb eta: " 
         << m_counterChInEta / ( double ) m_nEvents
         << std::endl 
         << "=                                                                 ="
         << std::endl 
         << "==================================================================="
         << endreq;

  return GaudiAlgorithm::finalize();
  
}

//=============================================================================
//  Lifetime of a stable particle
//=============================================================================
double GenMonitorAlg::lifetime( HepMC::GenParticle* thePart ) 
{
  if ( thePart->end_vertex() ) {
    HepLorentzVector thePosition = thePart->end_vertex()->position() ;
    Hep3Vector theBoost = thePart->momentum().boostVector() ;
    HepLorentzVector thePositionBoost = thePosition.boost( -theBoost ) ;
    return thePositionBoost.t() / EvtConst::c ;
  } else {
    return -1.e-10 ;
  }
}

