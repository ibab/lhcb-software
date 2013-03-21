// $Id: GenMonitorAlg.cpp,v 1.16 2010-02-24 19:02:33 robbep Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/SystemOfUnits.h"

// From HepMC
#include "Event/HepMCEvent.h"
#include "HepMC/GenEvent.h"
#include "HepMC/GenParticle.h"

// From LHCb
#include "Kernel/ParticleID.h"

// local
#include "GenMonitorAlg.h"
#include "GaussGenUtil.h"

//-----------------------------------------------------------------------------
// Implementation file for class : GenMonitorAlg
//
// 2003-11-13 : Patrick Robbe
// 2005-04-11 : G.Corti
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( GenMonitorAlg );


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
  declareProperty( "Input",  m_dataPath = LHCb::HepMCEventLocation::Default );
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

  if( produceHistos() ) {
    bookHistos();
  }
  
  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode GenMonitorAlg::execute() {

  debug() << "==> Execute" << endmsg;  

  // Initialize counters
  int nParticles(0), nParticlesStable(0);
  int nParticlesStableCharged(0), nParChStabEtaAcc(0);
  int nPileUp(0);
  
  // Retrieve data from selected path
  SmartDataPtr< LHCb::HepMCEvents > hepMCptr( eventSvc() , m_dataPath );

  if( 0 == hepMCptr ) {
    info() << "No HepMCEvents at location " << m_dataPath << endmsg;
  } else {
    LHCb::HepMCEvents::iterator it ;
    for( it = hepMCptr->begin() ; it != hepMCptr->end(); ++it ) {

      // Check if monitor has to be applied to this event
      if( !m_generatorName.empty() ) {
        if( m_generatorName != (*it)->generatorName() ) {
          continue;
        }
      }
      debug() << "Monitor for " << (*it)->generatorName()
              << endmsg;
      
      // Plot process type 
      if( produceHistos() ) {
        m_hProcess->fill( (*it)->pGenEvt()->signal_process_id() );
      }

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
              if ( hepMCpart -> production_vertex() ) {
                m_hPrimX->fill( hepMCpart->production_vertex()->position().x()/
                                Gaudi::Units::mm );
                m_hPrimY->fill( hepMCpart->production_vertex()->position().y()/
                                Gaudi::Units::mm );
                m_hPrimZ->fill( hepMCpart->production_vertex()->position().z()/
                                Gaudi::Units::mm );
                m_hPrimZV->fill( hepMCpart->production_vertex()->position().z()/
                                 Gaudi::Units::mm );
                m_hPrimZE->fill( hepMCpart->production_vertex()->position().z()/
                                 Gaudi::Units::mm );
                m_hPrimT->fill( hepMCpart->production_vertex()->position().t()/
                                Gaudi::Units::ns );
                m_hPrimXvsZ->fill( hepMCpart->production_vertex()->position().z()/Gaudi::Units::mm,
                                   hepMCpart->production_vertex()->position().x()/Gaudi::Units::mm );                
                m_hPrimYvsZ->fill( hepMCpart->production_vertex()->position().z()/Gaudi::Units::mm,
                                   hepMCpart->production_vertex()->position().y()/Gaudi::Units::mm );                
              }
            } 
          }
          
          m_hPartP->fill( hepMCpart->momentum().rho()/
                          Gaudi::Units::GeV );
          m_hPartPDG->fill( hepMCpart->pdg_id() );
        }
        // Note that the following is really multiplicity of particle defined
        // as stable by Pythia just after hadronization: all particles known by
        // EvtGen are defined stable for Pythia (it will count rho and pi0  
        // and gamma all togheter as stable ...
        if( ( hepMCpart->status() != 2 ) && ( hepMCpart->status() != 3 ) ) {
          nParticlesStable++;
          if( produceHistos() ) {
            m_hProtoP->fill( hepMCpart->momentum().rho()/
                             Gaudi::Units::GeV );
            m_hProtoPDG->fill( hepMCpart->pdg_id() );
            m_hProtoLTime->fill( GaussGenUtil::lifetime( hepMCpart )/
                                 Gaudi::Units::mm );
          }
          // Charged stable particles meaning really stable after EvtGen
          LHCb::ParticleID pID( hepMCpart->pdg_id() );
          if( 0.0 != pID.threeCharge() ) {
            // A stable particle does not have an outgoing vertex
            if( !hepMCpart->end_vertex() ) {
             // should be the same as the following  
              //             if ( ( hepMCpart -> status() == 999 )  
              ++nParticlesStableCharged;
              double pseudoRap =  hepMCpart->momentum().pseudoRapidity();
              // in LHCb acceptance
              if( (pseudoRap > m_minEta) && (pseudoRap < m_maxEta) ) {
                ++nParChStabEtaAcc;
              }
              if( produceHistos() ) {
                m_hStableEta->fill( pseudoRap );
                m_hStablePt->fill( hepMCpart->momentum().perp()/
                                   Gaudi::Units::GeV );
              }
            }
          }    
        }
      }
    }
  }
  if( produceHistos() ) {
    m_hNPart->fill( nParticles );
    m_hNStable->fill( nParticlesStable );
    m_hNSCharg->fill( nParticlesStableCharged );
    m_hNSChEta->fill( nParChStabEtaAcc );
    m_hNPileUp->fill( nPileUp );
  }
  m_counter += nParticles ;
  m_counterstable += nParticlesStable ;
  m_counterCharged += nParticlesStableCharged;
  m_counterChInEta += nParChStabEtaAcc;
  m_nEvents++ ;
  
  debug() << "Event number " << m_nEvents << " contains "
          << nParticles << " particles" << endmsg;
  
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode GenMonitorAlg::finalize() {

  debug() << "==> Finalize" << endmsg;
  
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
         << endmsg;

  return GaudiHistoAlg::finalize();
  
}

//============================================================================
// Booking of histograms
//============================================================================
void GenMonitorAlg::bookHistos() 
{

  debug() << "==> Book histograms" << endmsg;
 
  m_hNPart   = book(  1, "Multiplicity all particles", 0., 2999., 300 );
  m_hNStable = book(  2, "Multiplicity protostable particles", 0., 2999., 300 );
  m_hNSCharg = book(  3, "Multiplicity stable charged particles", -0.5, 299.5, 300 );
  m_hNSChEta = book(  4, "Multiplicity stable charged particles in LHCb eta",
                      -0.5, 299.5, 300 );
  m_hProcess = book(  5, "Process type", -0.5, 5100.5, 5101);
  m_hNPileUp = book( 10, "Num. of primary interaction per bunch", -0.5, 10.5, 11 );
  m_hPrimX   = book( 11, "PrimaryVertex x (mm)", -1.0, 1.0, 200 );
  m_hPrimY   = book( 12, "PrimaryVertex y (mm)", -1.0, 1.0, 200 );
  m_hPrimZ   = book( 13, "PrimaryVertex z (mm)", -200., 200., 100 ); 
  m_hPrimZV  = book( 14, "PrimaryVertex z, all Velo (mm)", -1000., 1000., 100 );
  m_hPrimZE  = book( 15, "PrimaryVertex z, all Exp Area (mm)", -1000., 20000., 105 );
  m_hPrimT   = book( 16, "PrimaryVertex t (ns)", -75.5, 75.5, 151 );
  m_hPrimXvsZ = book2D( 17, "PrimaryVertex x vs z (mm)", -500., 500., 100, -1., 1., 100 );
  m_hPrimYvsZ = book2D( 18, "PrimaryVertex y vs z (mm)", -500., 500., 100, -1., 1., 100 );
  m_hPartP   = book( 21, "Momentum of all particles (GeV)", 0., 100., 100 );
  m_hPartPDG = book( 22, "PDGid of all particles", -4999., 5000., 10000 );
  m_hProtoP  = book( 31, "Momentum of protostable particles (GeV)", 0., 100., 100);
  m_hProtoPDG = book( 32, "PDGid of protostable particles", -4999., 5000., 10000 );
  m_hProtoLTime = book( 33, "Lifetime protostable particles (mm) ", -1., 20., 105 );
  m_hStableEta = book( 44, "Pseudorapidity stable charged particles", -15., 15., 150 );
  m_hStablePt  = book( 45, "Pt stable charged particles", 0., 20., 100 );

  return;

}

