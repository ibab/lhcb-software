// $Id: GenMonitorAlg.cpp,v 1.4 2005-06-23 17:57:26 gcorti Exp $
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
  : GaudiHistoAlg( name , pSvcLocator ) ,
    m_counter       ( 0 ) ,
    m_counterstable ( 0 ) ,
    m_counterCharged( 0 ),
    m_counterChInEta( 0 ),
    m_nEvents       ( 0 ) {

  declareProperty( "MinEta", m_minEta = 2.0);
  declareProperty( "MaxEta", m_maxEta = 4.9);
  declareProperty( "Input",  m_dataPath = "/Event/Gen/HepMCEvents" );
  declareProperty( "ApplyTo", m_generatorName = "" );
  
  // Set by default not to fill histograms for this algorithm
  setProduceHistos( false );
  
}

//=============================================================================
// Destructor
//=============================================================================
GenMonitorAlg::~GenMonitorAlg() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode GenMonitorAlg::initialize() {

  StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiHistoAlg

  debug() << "==> Initialize" << endmsg;
  
  if ( m_generatorName.empty() ) {
    info() << "Monitor will be applied to all events in container " 
           << m_dataPath << endmsg;
  } else {
    info() << "Monitor will be applied to events produced with generator "
           << m_generatorName << " in container "
           << m_dataPath << endmsg; 
  }

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
  int nPileUp(0);
  
  // Retrieve data from selected path
  SmartDataPtr< HepMCEvents > hepMCptr( eventSvc() , m_dataPath );

  if( 0 == hepMCptr ) {
    info() << "No HepMCEvents at location " << m_dataPath << endreq;
  } else {
    HepMCEvents::iterator it ;
    for( it = hepMCptr->begin() ; it != hepMCptr->end(); ++it ) {

      // Check if monitor has to be applied to this event
      if( !m_generatorName.empty() ) {
        if( m_generatorName != (*it)->generatorName() ) {
          continue;
        }
      }
      debug() << "Monitor for " << (*it)->generatorName()
              << endmsg;
      
      bool primFound = false;
      nPileUp++;
      for( HepMC::GenEvent::particle_const_iterator 
             itp = (*it)->pGenEvt()->particles_begin();
           itp != (*it)->pGenEvt()->particles_end(); itp++ ) {
        HepMC::GenParticle* hepMCpart = *itp;
        nParticles++ ;
        if( produceHistos() ) { 
          // Identify primary vertex and fill histograms
          if( !primFound ) {
            if( (hepMCpart->status() == 1) || (hepMCpart->status() == 888 ) ) {
              primFound = true;
              plot( hepMCpart->production_vertex()->position().x(), 11,
                    "PrimaryVertex x (mm)", -0.5, 0.5 );
              plot( hepMCpart->production_vertex()->position().y(), 12,
                    "PrimaryVertex y (mm)", -0.5, 0.5 );
              plot( hepMCpart->production_vertex()->position().z(), 13,
                    "PrimaryVertex z (mm)", -200., 200. );
              plot( hepMCpart->production_vertex()->position().z(), 14,
                    "PrimaryVertex z, all Velo (mm)", -1000., 1000. );
            }
          }
          plot( hepMCpart->momentum().vect().mag(), 21, 
                "Momentum of all particles (GeV)", 0., 100. );
          plot( hepMCpart->pdg_id(), 22,
                "PDGid of all particles", -4999., 5000., 10000 );
        }
        // Note that the following is really multiplicity of particle defined
        // as stable by Pythia just after hadronization: all particles known by
        // EvtGen are defined stable for Pythia (it will count rho and pi0  
        // and gamma all togheter as stable ...
        if( ( hepMCpart->status() != 2 ) && ( hepMCpart->status() != 3 ) ) {
          nParticlesStable++;
          if( produceHistos() ) {
            plot( hepMCpart->momentum().vect().mag(), 31,
                  "Momentum of protostable particles (GeV)", 0., 100. );
            plot( hepMCpart->pdg_id(), 32,
                  "PDGid of protostable particles", -4999., 5000., 10000 );
            plot( lifetime( hepMCpart ), 33,
                  "Lifetime protostable particles", -1.5e-10, 1.5e-10 );
          }
          // Charged stable particles meaning really stable after EvtGen
          ParticleID pID( hepMCpart->pdg_id() );
          if( 0.0 != pID.threeCharge() ) {
            // A stable particle does not have an outgoing vertex
            if( !hepMCpart->end_vertex() ) {
//             // should be the same as the following  
//             if ( ( hepMCpart -> status() == 999 )  
              ++nParticlesStableCharged;
              double pseudoRap =  hepMCpart->momentum().pseudoRapidity();
              // in LHCb acceptance
              if( (pseudoRap > m_minEta) && (pseudoRap < m_maxEta) ) {
                ++nParChStabEtaAcc;
              }
              if( produceHistos() ) {
                plot( pseudoRap, 44,
                      "Pseudorapidity stable charged particles",
                      -15., 15., 150 );
                plot( hepMCpart->momentum().perp(), 45, 
                      "Pt stable charged particles", 0., 20. );
              }
            }
          }    
        }
      }
    }
  }
  if( produceHistos() ) {
    plot( nParticles, 1, "Multiplicity all particles", 0., 2999., 300 );
    plot( nParticlesStable, 2,
          "Multiplicity protostable particles", 0., 2999., 300 );
    plot( nParticlesStableCharged, 3,
          "Multiplicity stable charged particles", -0.5, 299.5, 300 );
    plot( nParChStabEtaAcc, 4,
          "Multiplicity stable charged particles in LHCb eta",
          -0.5, 299.5, 300 );
    plot( nPileUp, 10, "Num. of primary interaction per bunch", -0.5, 10.5, 11 );
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

  return GaudiHistoAlg::finalize();
  
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

