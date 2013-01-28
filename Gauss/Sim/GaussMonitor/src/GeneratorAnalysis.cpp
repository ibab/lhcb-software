// $Id: GeneratorAnalysis.cpp,v 1.8 2010-02-24 19:02:33 robbep Exp $
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
#include "GeneratorAnalysis.h"
#include "GaussGenUtil.h"

//-----------------------------------------------------------------------------
// Implementation file for class : GeneratorAnalysis
//
// 2007-01-28 : P.Szczypka
// 2007-02-16 : G.Corti, clean up of code
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( GeneratorAnalysis );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GeneratorAnalysis::GeneratorAnalysis( const std::string& name,
                              ISvcLocator* pSvcLocator)
  : GaudiHistoAlg( name , pSvcLocator )
  , m_counter          ( 0 )
  , m_counterStable    ( 0 )
  , m_counterCharged   ( 0 )
  , m_counterChInEta   ( 0 )
  , m_nEvents          ( 0 )
  , m_nAllChargedStable( 0 )
  , m_nNeutralStable   ( 0 )
  , m_nChargedStable   ( 0 )
  , m_nAllParticles    ( 0 )
  , m_nBHistos         ( 0 )
  , m_nS0L0J0          ( 0 )
  , m_nS1L1J0          ( 0 )
  , m_nS1L0J1          ( 0 )
  , m_nS0L1J1          ( 0 )
  , m_nS1L1J1          ( 0 )
  , m_nS1L1J2          ( 0 )
  , m_stateB           ( 0 )
  , m_stateBStar       ( 0 )
  , m_stateBStarStar   ( 0 )
  , m_nMesonType       ( 0 )
  , m_nBdMeson         ( 0 )
  , m_nBuMeson         ( 0 )
  , m_nBsMeson         ( 0 )
  , m_nBcMeson         ( 0 )
  , m_nBbMeson         ( 0 )
  , m_nBBaryon         ( 0 )
  , m_nTbMeson         ( 0 )
  , m_nBParticles      ( 0 )
  , m_nParticles       ( 0 )
{

  declareProperty( "MinEta", m_minEta = 2.0);
  declareProperty( "MaxEta", m_maxEta = 4.9);
  declareProperty( "Input",  m_dataPath = LHCb::HepMCEventLocation::Default );
  declareProperty( "ApplyTo", m_generatorName = "" );
  
  // Set by default not to fill histograms for neutral particles
  declareProperty( "NeutralParticleHistos", m_neutralHistos = false );

}

//=============================================================================
// Destructor
//=============================================================================
GeneratorAnalysis::~GeneratorAnalysis() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode GeneratorAnalysis::initialize() {

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
    bookHistos(m_neutralHistos);
  }
  
  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode GeneratorAnalysis::execute() {

  debug() << "==> Execute" << endmsg;

  // Initialize counters
  int nPileUp(0);
  int nParticles(0), nStable(0), nProtoStable(0);
  int nChargedStable(0), nChargedStableAcceptance(0), nNeutralStableAcceptance(0);
  
  //Initalize particle variables
  double ptmax_det(0.), emax_det(0.), pmax_det(0.);
  double pmax_fr(0.),emax_fr(0.),ptmax_fr(0.);
  double pmax_neut(0.),emax_neut(0.),ptmax_neut(0.);

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
      
      for( HepMC::GenEvent::vertex_const_iterator pVertex = 
             (*it)->pGenEvt()->vertices_begin();
           pVertex != (*it)->pGenEvt()->vertices_end();
           pVertex++ ) {
        for( HepMC::GenVertex::particles_out_const_iterator pParticle =
               ( *pVertex )->particles_out_const_begin( );
             ( *pVertex )->particles_out_const_end( ) != pParticle;
             ++pParticle ) {   

          //Get Mother Particle ID
          LHCb::ParticleID m_mPID(0);
          if( (*pParticle)->production_vertex() ) {
            
            m_mPID.setPid( (*(*pParticle)->production_vertex()
                            -> particles_in_const_begin())->pdg_id() );
          }
          
          //Get current Particle ID
          LHCb::ParticleID m_dPID((*pParticle) -> pdg_id ( ));
          
          // Fill b hadron histos
          bHadronInfo(m_mPID,m_dPID);
        }
      }

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
        // Identify primary vertex and fill histograms
        if( !primFound ) {
          if( (hepMCpart->status() == 1) || (hepMCpart->status() == 888 ) ) {
            primFound = true;
            if( produceHistos() ) { 
              if ( hepMCpart -> production_vertex() ) {
                m_hPrimX->fill( hepMCpart->production_vertex()->position().x()/
                                Gaudi::Units::mm );
                m_hPrimY->fill( hepMCpart->production_vertex()->position().y()/
                                Gaudi::Units::mm );
                m_hPrimZ->fill( hepMCpart->production_vertex()->position().z()/
                                Gaudi::Units::mm );
                m_hPrimZZ->fill( hepMCpart->production_vertex()->position().z()/
                                 Gaudi::Units::mm );
              }
            }
            
          }
        }
        
        //Particle properties:
        double pseudoRap =  hepMCpart->momentum().pseudoRapidity();
        double energy = hepMCpart->momentum().e()/Gaudi::Units::GeV;
        double pt = hepMCpart->momentum().perp()/Gaudi::Units::GeV;
        double p =  hepMCpart->momentum().rho()/Gaudi::Units::GeV;
        int pID = hepMCpart->pdg_id();
        LHCb::ParticleID LHCb_pID( hepMCpart->pdg_id() );

        if(produceHistos()){
          m_hAllParticlesPRap->fill( pseudoRap );
          m_hAllParticlesEnergy->fill( energy );
          m_hAllParticlesP->fill( p );
          m_hAllParticlesPt->fill( pt );
          m_hAllParticlesPID->fill( pID );
          m_hPartP->fill( p );
          m_hPartPDG->fill( pID );
          //Why are these histo's here? Throwback from the original code...?
        }
        // Note that the following is really multiplicity of particle defined
        // as stable by Pythia just after hadronization: all particles known by
        // EvtGen are defined stable for Pythia (it will count rho and pi0  
        // and gamma all togheter as stable ...
        if( ( hepMCpart->status() != 2 ) && ( hepMCpart->status() != 3 ) ) {
          nProtoStable++;
          if( produceHistos() ) {
            m_hProtoP->fill( p );
            m_hProtoPDG->fill( pID );
            m_hProtoLTime->fill( GaussGenUtil::lifetime( hepMCpart )/
                                 Gaudi::Units::mm);
          }

          // A stable particle does not have an outgoing vertex
          if( !hepMCpart->end_vertex() ) {
            // should be the same as the following  
            //             if ( ( hepMCpart -> status() == 999 )  
            nStable++;
            if( 0 != int(LHCb_pID.threeCharge()) ) {
              nChargedStable++;
              if( produceHistos() ){
                m_hAllChargedStableP -> fill ( p );
                m_hAllChargedStablePt -> fill ( pt );
                m_hAllChargedStableEnergy -> fill ( energy );
                m_hAllChargedStablePRap -> fill ( pseudoRap );
                m_hAllChargedStablePID -> fill ( pID );
              }
              //Set maximum values              
              if(pt > ptmax_fr)  ptmax_fr = pt;
              //Max energy limited to overlook initial energy transfer
              if((energy > emax_fr) && (energy < 7000.)) emax_fr = energy;
              if(p > pmax_fr) pmax_fr = p;

              // in LHCb acceptance
              if( (pseudoRap > m_minEta) && (pseudoRap < m_maxEta) ) {
                //Within acceptance
                nChargedStableAcceptance++;
                if(p > pmax_det) pmax_det = p;
                if(pt > ptmax_det) ptmax_det = pt;
                if(energy > emax_det) emax_det = energy;
                if(produceHistos()){
                  m_hChargedStableP -> fill( p );
                  m_hChargedStablePt -> fill( pt );
                  m_hChargedStableEnergy -> fill( energy );
                  m_hChargedStablePRap -> fill( pseudoRap );
                  m_hChargedStablePID -> fill( pID );
                }
              }
            }else{
              if( (pseudoRap > m_minEta) && (pseudoRap < m_maxEta) ) {
                //Neutral and Stable within Acceptance
                nNeutralStableAcceptance++;
                if(pt > ptmax_neut) ptmax_neut = pt;
                if(p > pmax_neut) pmax_neut = p;
                if(energy > emax_neut) emax_neut = energy;             
                if(produceHistos() && m_neutralHistos ){
                  m_hNeutralStablePt->fill( pt );
                  m_hNeutralStableP->fill( p );
                  m_hNeutralStableEnergy->fill( energy ); 
                  m_hNeutralStablePID -> fill( pID );
                  m_hNeutralStablePRap -> fill( pseudoRap );
                }                  
              }
            }
            if( produceHistos() ) {
              m_hStableEta->fill( pseudoRap );
              m_hStablePt->fill( pt );
            }
          }
        }
      }
    }
  }
  if( produceHistos() ) {
    //All Particles
    m_hAllParticlesMult->fill(nParticles);

    //All Charged Stable PArticles
    m_hAllChargedStablePMax -> fill ( pmax_fr );
    m_hAllChargedStablePtMax -> fill( ptmax_fr );
    m_hAllChargedStableEnergyMax -> fill ( emax_fr );
    m_hAllChargedStableMult -> fill ( nChargedStable );

    //Charged Stable within EtaMin and Max
    m_hChargedStablePMax -> fill ( pmax_det );
    m_hChargedStablePtMax -> fill ( ptmax_det );
    m_hChargedStableEnergyMax -> fill ( emax_det );
    m_hChargedStableMult -> fill ( nChargedStableAcceptance );

    //Neutral Stable within EtaMin and Max
    if( m_neutralHistos ) {
      m_hNeutralStablePMax -> fill ( pmax_neut );
      m_hNeutralStablePtMax -> fill ( ptmax_neut );
      m_hNeutralStableEnergyMax -> fill ( emax_neut );
      m_hNeutralStableMult->fill ( nNeutralStableAcceptance );
    }
    
    //WHat are these histograms? Gloria?
    m_hNPart->fill( nParticles );
    m_hNStable->fill( nStable );
    m_hNSCharg->fill( nChargedStable );
    m_hNSChEta->fill( nChargedStableAcceptance );
    m_hNPileUp->fill( nPileUp );
  }
  m_counter += nParticles ;
  m_counterStable += nStable ;
  m_counterCharged += nChargedStable;
  m_counterChInEta += nChargedStableAcceptance;
  m_nEvents++ ;
  
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode GeneratorAnalysis::finalize() {

  debug() << "==> Finalize" << endmsg;
  if(produceHistos()) {
    normHistos();
  }

  // Number of B Meson states should be = totalBHadrons
  int totalBHadrons(m_nBdMeson + m_nBuMeson + m_nBsMeson
                    + m_nBcMeson + m_nBbMeson + m_nTbMeson + m_nBBaryon);
  int totalBMesonStates(m_stateB + m_stateBStar + m_stateBStarStar);

  //Set total states to avoid dividing by zero:
  if( !totalBHadrons ) totalBHadrons = 1;
  if( !totalBMesonStates ) totalBMesonStates = 1;

  info() << std::endl
         << "======================== B Hadron Statistics ===================" 
         << std::endl
         << "* B Hadron production counters                                 *" 
         << std::endl
         << "* Bd meson : " << m_nBdMeson
         << " [fraction : " << fraction( m_nBdMeson , totalBHadrons )
         << " +/- " << err_fraction( m_nBdMeson , totalBHadrons ) << " ]" 
         << std::endl
    
         << "* Bu meson : " << m_nBuMeson
         << " [fraction : " << fraction( m_nBuMeson , totalBHadrons )
         << " +/- " << err_fraction( m_nBuMeson , totalBHadrons ) << " ]" 
         << std::endl
    
         << "* Bs meson : " << m_nBsMeson
         << " [fraction : " << fraction( m_nBsMeson , totalBHadrons )
         << " +/- " << err_fraction( m_nBsMeson , totalBHadrons ) << " ]" 
         << std::endl
    
         << "* Bc meson : " << m_nBcMeson
         << " [fraction : " << fraction( m_nBcMeson , totalBHadrons )
         << " +/- " << err_fraction( m_nBcMeson , totalBHadrons ) << " ]" 
         << std::endl
    
         << "* Bb meson : " << m_nBbMeson
         << " [fraction : " << fraction( m_nBbMeson , totalBHadrons )
         << " +/- " << err_fraction( m_nBbMeson , totalBHadrons ) << " ]" 
         << std::endl
    
         << "* T  meson : " << m_nTbMeson
         << " [fraction : " << fraction( m_nTbMeson , totalBHadrons )
         << " +/- " << err_fraction( m_nTbMeson , totalBHadrons ) << " ]" 
         << std::endl
    
         << "* B Baryon : " << m_nBBaryon
         << " [fraction : " << fraction( m_nBBaryon , totalBHadrons )
         << " +/- " << err_fraction( m_nBBaryon , totalBHadrons ) << " ]" 
         << std::endl
    
         << "* Total    : " << totalBHadrons
         << " [fraction : " << fraction( totalBHadrons , totalBHadrons )
         << " +/- " << err_fraction( totalBHadrons , totalBHadrons ) << " ]" 
         << std::endl

         << "*                                                              *" 
         << std::endl
         
         << "* B Meson production state:                                    *" 
         << std::endl
         << "* Number of B   (L=0, J=0)     : " << m_stateB
         << " [fraction : " << fraction( m_stateB , totalBMesonStates )
         << " +/- " << err_fraction( m_stateB , totalBMesonStates ) <<" ]" 
         << std::endl
    
         << "* Number of B*  (L=0, J=1)     : " << m_stateBStar 
         << " [fraction : "  << fraction( m_stateBStar , totalBMesonStates)
         << " +/- " << err_fraction( m_stateBStar , totalBMesonStates ) <<" ]" 
         << std::endl
    
         << "* Number of B** (L=1, J=0,1,2) : " << m_stateBStarStar
         << " [fraction : " << fraction ( m_stateBStarStar , totalBMesonStates )
         << " +/- " << err_fraction( m_stateBStarStar , totalBMesonStates ) 
         <<" ]" << std::endl
    
         << "* Total         : " << totalBMesonStates
         << " [fraction : " << fraction ( totalBMesonStates , totalBMesonStates )
         << " +/- " << err_fraction( totalBMesonStates , totalBMesonStates ) 
         <<" ]" << std::endl
    
         << "*                                                              *" 
         << std::endl
    
         << "* B Meson Angular Momentum States:                             *" 
         << std::endl
         << "* S L J" << std::endl

         << "* 0 0 0 : " << m_nS0L0J0
         << " [fraction : " << fraction( m_nS0L0J0 , totalBMesonStates )
         << " +/- " << err_fraction( m_nS0L0J0 , totalBMesonStates ) <<" ]" 
         << std::endl

         << "* 1 1 0 : " << m_nS1L1J0
         << " [fraction : " << fraction( m_nS1L1J0 , totalBMesonStates )
         << " +/- " << err_fraction( m_nS1L1J0 , totalBMesonStates ) <<" ]" 
         << std::endl

         << "* 1 0 1 : " << m_nS1L0J1
         << " [fraction : " << fraction( m_nS1L0J1 , totalBMesonStates )
         << " +/- " << err_fraction( m_nS1L0J1 , totalBMesonStates ) <<" ]" 
         << std::endl

         << "* 0 1 1 : " << m_nS0L1J1
         << " [fraction : " << fraction( m_nS0L1J1 , totalBMesonStates )
         << " +/- " << err_fraction( m_nS0L1J1 , totalBMesonStates ) <<" ]" 
         << std::endl

         << "* 1 1 1 : " << m_nS1L1J1
         << " [fraction : " << fraction( m_nS1L1J1 , totalBMesonStates )
         << " +/- " << err_fraction( m_nS1L1J1 , totalBMesonStates ) <<" ]" 
         << std::endl

         << "* 1 1 2 : " << m_nS1L1J2
         << " [fraction : " << fraction( m_nS1L1J2 , totalBMesonStates )
         << " +/- " << err_fraction( m_nS1L1J2 , totalBMesonStates ) <<" ]" 
         << std::endl

         << "* Total : " << totalBMesonStates
         << " [fraction : " << fraction( totalBMesonStates , totalBMesonStates )
         << " +/- " << err_fraction( totalBMesonStates , totalBMesonStates ) 
         << " ]" <<std::endl
         << endmsg;
   
  info() << std::endl 
         << "======================== Generators Statistics ====================" 
         << std::endl
         << "=                                                                 ="
         << std::endl 
         << "= Number of particles generated: " << m_counter 
         << " m_nAllParticles = " << m_nAllParticles 
         << std::endl
         << std::endl 
         << "= Number of events: " << m_nEvents
         << std::endl
         << "= Mean multiplicity: " << m_counter / ( double ) m_nEvents
         << std::endl 
         << "=                                                                 ="
         << std::endl 
         << "= Number of pseudo stable particles generated: " << m_counterStable
         << std::endl 
         << "= Number of events: " << m_nEvents
         << std::endl
         << "= Mean pseudo stable multiplicity: " 
         << m_counterStable / ( double ) m_nEvents
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
void GeneratorAnalysis::bookHistos(bool neutral) 
{

  debug() << "==> Book histograms" << endmsg;
 
  int i(0);

  //
  debug() << "Booking B Hadron Histos" << endmsg;
  m_bMesonFraction = book(1000,"B Hadron Production Fractions",-0.,7.,7);
  m_pHisto.push_back( m_bMesonFraction );

  i++;
  m_nBHistos++;
  m_bMesonStates = book(1001,"B Meson Production Spin State",-0.,3.,3);
  m_pHisto.push_back( m_bMesonStates );

  i++;
  m_nBHistos++;
  m_bMesonStatesCode = book(1002,"B Meson Production Spin State Code",-0.,6.,6);
  m_pHisto.push_back( m_bMesonStatesCode );


  //General Histograms
  debug() << "Booking General Histograms." << endmsg;
  i++;
  m_hNPileUp = book( 150, "Num. of primary interaction per bunch", -0.5, 10.5, 11 );
  m_pHisto.push_back( m_hNPileUp );
  i++;
  m_hPrimX   = book( 101, "PrimaryVertex x (mm)", -0.5, 0.5, 100 );
  m_pHisto.push_back( m_hPrimX );
  i++;
  m_hPrimY   = book( 102, "PrimaryVertex y (mm)", -0.5, 0.5, 100 );
  m_pHisto.push_back( m_hPrimY );
  i++;
  m_hPrimZ   = book( 103, "PrimaryVertex z (mm)", -200., 200., 100 ); 
  m_pHisto.push_back( m_hPrimZ );
  i++;
  m_hPrimZZ  = book( 104, "PrimaryVertex z, all Velo (mm)", -1000., 1000., 100 );
  m_pHisto.push_back( m_hPrimZZ );
  i++;
  m_hNPart   = book(  105, "Multiplicity all particles", 0., 3000., 150 );
  m_pHisto.push_back( m_hNPart );
  i++;
  m_hNStable = book(  106, "Multiplicity protostable particles", 0., 3000., 75 );
  m_pHisto.push_back( m_hNStable );
  i++;
  m_hNSCharg = book(  107, "Multiplicity stable charged particles", 0., 500., 100 );
  m_pHisto.push_back( m_hNSCharg );
  i++;
  m_hNSChEta = 
    book(  108, "Multiplicity stable charged particles in LHCb eta", 0., 150., 75 );
  m_pHisto.push_back( m_hNSChEta );
  i++;
  m_hProcess = book(  109, "Process type", -0.5, 5100.5, 5101);
  m_pHisto.push_back( m_hProcess );
  i++;
  m_hPartP   = book( 120, "Momentum of all particles (GeV)", 0., 100., 100 );
  m_pHisto.push_back( m_hPartP );
  i++;
  m_hPartPDG = book( 121, "PDGid of all particles", -5000., 5000., 10000 );
  m_pHisto.push_back( m_hPartPDG );
  i++;
  m_hProtoP  = book( 130, "Momentum of protostable particles (GeV)", 0., 100., 100);
  m_pHisto.push_back( m_hProtoP );
  i++;
  m_hProtoPDG = book( 131, "PDGid of protostable particles", -5000., 5000., 10000 );
  m_pHisto.push_back( m_hProtoPDG );
  i++;
  m_hProtoLTime = 
    book( 132, "Lifetime protostable particles (mm)", -1., 20., 105 );
  m_pHisto.push_back( m_hProtoLTime );
  i++;
  m_hStableEta = 
    book( 133, "Pseudorapidity stable charged particles", -15., 15., 150 );
  m_pHisto.push_back( m_hStableEta );
  i++;
  m_hStablePt  = book( 140, "Pt stable charged particles (GeV)", 0., 5., 50 );
  m_pHisto.push_back(  m_hStablePt );


  // All Charged Stable Particle Histo's
  debug() << "Booking Histograms for all Charged Stable particles" << endmsg;
  i++;
  m_hAllChargedStableMult= 
    book(201,"All Eta: Charged Stable Multiplicity", 0., 500., 100);
  m_pHisto.push_back( m_hAllChargedStableMult );
  
  i++;
  m_hAllChargedStablePRap= 
    book(202,"All Eta: Charged Stable PseudoRapidity", -15., 15., 150);
  m_pHisto.push_back( m_hAllChargedStablePRap );
  
  i++;
  m_hAllChargedStableEnergy= 
    book(203,"All Eta: Charged Stable Energy (GeV)", 0., 100., 100);
  m_pHisto.push_back( m_hAllChargedStableEnergy );
  
  i++;
  m_hAllChargedStableEnergyMax= 
    book(204,"All Eta: Charged Stable Maximum Energy (GeV)", 0., 5000., 50);
  m_pHisto.push_back( m_hAllChargedStableEnergyMax );
  
  i++;
  m_hAllChargedStableP= 
    book(205,"All Eta: Charged Stable Momentum (GeV)", -0., 150., 150);
  m_pHisto.push_back( m_hAllChargedStableP );
  
  i++;
  m_hAllChargedStablePMax= 
    book(206,"All Eta: Charged Stable Maximum Momentum (GeV)", 0., 5000., 50);
  m_pHisto.push_back( m_hAllChargedStablePMax );
  
  i++;
  m_hAllChargedStablePt= 
    book(207,"All Eta: Charged Stable Transverse Momentum (GeV)", -0., 5., 250);
  m_pHisto.push_back( m_hAllChargedStablePt );
  
  i++;
  m_hAllChargedStablePtMax= 
    book(208,"All Eta: Charged Stable Maximum Transverse Momentum (GeV)",
         0., 10., 20);
  m_pHisto.push_back( m_hAllChargedStablePtMax );
  
  i++;
  m_hAllChargedStablePID = 
    book(209, "All Eta: Charged Stable particles PID", -5000., 5000., 10000);
  m_pHisto.push_back( m_hAllChargedStablePID );



  //Histograms for charged stable particles within the LHCb acceptance
  debug() << "Booking Histograms for Charged stable particles in the LHCb acceptance"
          << endmsg;
  i++;
  m_hChargedStableMult= 
    book(301,"Charged Stable Multiplicity in Detector", 0., 150., 30);
  m_pHisto.push_back( m_hChargedStableMult );
  
  //Histogram has 10 bins per rapidity unit 
  i++;
  m_hChargedStablePRap= 
    book(302,"Charged Stable PseudoRapidity in Detector", m_minEta, m_maxEta,
         int((m_maxEta - m_minEta)*10.) );
  m_pHisto.push_back( m_hChargedStablePRap );
  
  i++;
  m_hChargedStableEnergy= 
    book(303,"Charged Stable Energy in Detector (GeV)",0., 100., 100);
  m_pHisto.push_back( m_hChargedStableEnergy );
  
  i++;
  m_hChargedStableEnergyMax= 
    book(304,"Charged Stable Maximum Energy in Detector (GeV)", 0., 300., 75);
  m_pHisto.push_back( m_hChargedStableEnergyMax );
  
  i++;
  m_hChargedStableP= 
    book(305,"Charged Stable Momentum in Detector (GeV)", -0., 300., 150);
  m_pHisto.push_back( m_hChargedStableP );
  
  i++;
  m_hChargedStablePMax= 
    book(306,"Charged Stable Maximum Momentum in Detector (GeV)", 0., 300., 75);
  m_pHisto.push_back( m_hChargedStablePMax );
  
  i++;
  m_hChargedStablePt= 
    book(307,"Charged Stable Transverse Momentum in Detector (GeV)", -0., 5., 125);
  m_pHisto.push_back( m_hChargedStablePt );
  
  i++;
  m_hChargedStablePtMax= 
    book(308,"Charged Stable Maximum Transverse Momentum in Detector (GeV)",
         0., 10., 50);
  m_pHisto.push_back( m_hChargedStablePtMax );
  
  i++;
  m_hChargedStablePID = 
    book(309, "Charged Stable particles PID", -5000., 5000., 10000);
  m_pHisto.push_back( m_hChargedStablePID );
  
  
  
  //Histograms for all particles
  debug() << "Booking Histos for All Particles" << endmsg;
  i++;
  m_hAllParticlesMult= 
    book(401,"All Eta: Particle Multiplicity", 0., 2000., 200);
  m_pHisto.push_back( m_hAllParticlesMult );

  i++;
  m_hAllParticlesPRap= 
    book(402,"All Eta: Particle PseudoRapidity", -15., 15., 150);
  m_pHisto.push_back( m_hAllParticlesPRap );
  
  i++;
  m_hAllParticlesEnergy= 
    book(403,"All Eta: Particle Energy (GeV)", 0., 100., 100);
  m_pHisto.push_back( m_hAllParticlesEnergy );
  
  i++;
  m_hAllParticlesP= 
    book(404,"All Eta: Particle Momentum (GeV)", -0., 300., 300);
  m_pHisto.push_back( m_hAllParticlesP );
  
  i++;
  m_hAllParticlesPt= 
    book(405,"All Eta: Particle Transverse Momentum (GeV)", -0., 5., 125);
  m_pHisto.push_back( m_hAllParticlesPt );
  
  i++;
  m_hAllParticlesPID = 
    book(406, "All Eta: Particle particles PID", -5000., 5000., 10000);
  m_pHisto.push_back( m_hAllParticlesPID );
  
  
  // Histograms for neutral particles  
  if(neutral) {
    debug() << "Booking Histos for neutral particles in the LHCb acceptance" 
            << endmsg;
    i++;
    m_hNeutralStableMult= 
      book(501,"Neutral Stable Multiplicity in Detector", 0., 200., 100);
    m_pHisto.push_back( m_hNeutralStableMult );

    i++;
    m_hNeutralStablePRap= 
      book(502,"Neutral Stable PseudoRapidity in Detector", m_minEta, m_maxEta,
           int((m_maxEta - m_minEta)*10.) );
    m_pHisto.push_back( m_hNeutralStablePRap );

    i++;
    m_hNeutralStableEnergy= 
      book(503,"Neutral Stable Energy in Detector (GeV)", 0., 100., 100);
    m_pHisto.push_back( m_hNeutralStableEnergy );

    i++;
    m_hNeutralStableEnergyMax= 
      book(504,"Neutral Stable Maximum Energy in Detector (GeV)", 0., 200., 100);
    m_pHisto.push_back( m_hNeutralStableEnergyMax );

    i++;
    m_hNeutralStableP= 
      book(505,"Neutral Stable Momentum in Detector (GeV)", -0., 150., 75);
    m_pHisto.push_back( m_hNeutralStableP );

    i++;
    m_hNeutralStablePMax= 
      book(506,"Maximum Neutral Stable Momentum in Detector (GeV)", 0., 300., 150);
    m_pHisto.push_back( m_hNeutralStablePMax );

    i++;
    m_hNeutralStablePt= 
      book(507,"Neutral Stable Transverse Momentum in Detector (GeV)", -0., 5., 250);
    m_pHisto.push_back( m_hNeutralStablePt );

    i++;
    m_hNeutralStablePtMax= 
      book(508,"Neutral Stable Maximum Transverse Momentum in Detector (GeV)",
           0., 10., 50);
    m_pHisto.push_back( m_hNeutralStablePtMax );

    i++;
    m_hNeutralStablePID = 
      book(509, "Neutral Stable Particles PID", -5000., 5000., 10000);
    m_pHisto.push_back( m_hNeutralStablePID );
  }
  

  debug() << "Booked " << i+1 << " Histograms." << endmsg;
  
  return;
  
}

//============================================================================
//  Normalize Histograms
//============================================================================
void GeneratorAnalysis::normHistos() 
{
  debug() << "==> Normalize histograms" << endmsg;

  double binSize = 0;
  int i = -1;
  for( std::vector<AIDA::IHistogram1D*>::iterator iHisto = m_pHisto.begin();
       m_pHisto.end() != iHisto; ++iHisto ) {
    
    ++i;
    AIDA::IHistogram1D* pHisto = (*iHisto);
    if( (pHisto)->axis().isFixedBinning() ){ 
      binSize = ( pHisto->axis().upperEdge() - pHisto->axis().lowerEdge() ) /
        ( pHisto->axis().bins() );
      
      debug() << "Currently normalizing histo #: "<< i << " : " 
              << pHisto->title() << endmsg;    
      debug() << "nBins = " <<  pHisto->axis().bins()
              << ", Bins Width = " << pHisto->axis().binWidth(0)
              << ", Top range = " <<  pHisto->axis().upperEdge()
              << ", Lower Range = " << pHisto->axis().lowerEdge() << endmsg;
      if( (double)m_nEvents*binSize ){
        if( i <= m_nBHistos ){
          debug() << "Scale factor = " 
                  << 1./( (double)m_nMesonType*binSize) << endmsg;
          pHisto->scale(1./( (double)m_nMesonType*binSize ));            
        }else{
          debug() << "Scale factor = " 
                  << 1./( (double)m_nEvents*binSize) << endmsg;
          pHisto->scale(1./( (double)m_nEvents*binSize ));
        }
      }else{
        warning() << "Scale factor = 0, unable to normalise histo!" << endmsg;
      }
    } else {
      warning() << "Histo: " <<  pHisto->title() 
                << " not normalised (no fixed binSize)." << endmsg;
    }
  }
  
  return;
}

//============================================================================
// Obtain B Meson production information and fill histograms
//============================================================================
void GeneratorAnalysis::bHadronInfo( LHCb::ParticleID m_mPID, 
                                 LHCb::ParticleID m_dPID )
{
    if ( m_dPID.abspid() == 5 ) return ;
    if ( ! ( m_mPID.hasBottom() && m_mPID.abspid()!=5) ) {
    if(m_dPID.hasBottom()){
      m_nBParticles++;
      //Set default B Hadron Type
      bHadronType type = BdMeson;
      bool valid = false ;
      if(m_dPID.isMeson()){
        //Daughter has bottom and is a meson
        if(m_dPID.hasDown()){
          m_nBdMeson++;
          type = BdMeson;  
          valid = true ;        
        }else if(m_dPID.hasUp()){
          m_nBuMeson++;
          type = BuMeson;        
          valid = true ;
        }else if(m_dPID.hasStrange()){
          m_nBsMeson++;
          type = BsMeson;
          valid = true ;
        }else if(m_dPID.hasCharm()){
          m_nBcMeson++;
          type = BcMeson;
          valid = true ;
        }else if(m_dPID.hasTop()){
          m_nTbMeson++;            
          type = TbMeson;
          valid = true ;
        }else{
          m_nBbMeson++;
          type = BbMeson;
          valid = true ;
        }  
      } else if(m_dPID.isBaryon()){
        //Daughter is a B baryon
        type = BBaryon;
        valid = true ;
        if(    !( (m_mPID.isBaryon()) && (m_mPID.hasBottom()) )   ){
          //Mother not a BBaryon
          m_nBBaryon++;  
        }
      }
      if ( ! valid ) 
        warning() << "Unknown B code " << m_dPID << endreq ;
      if( produceHistos() )
       if ( valid ) m_bMesonFraction->fill((double)( (int) type ) - 0.1 );
    }
    
    //Set default meson spin state
    spinState spin = S0L0J0;
    if(m_dPID.isMeson() && m_dPID.hasBottom()){
      m_nMesonType++;
      if(5 == m_dPID.jSpin()){
        spin = S1L1J2;
        m_nS1L1J2++;
      }else if(3 == m_dPID.jSpin()){
        if(1 == m_dPID.lSpin()){
          if(1 == m_dPID.sSpin()){
            spin = S1L1J1;
            m_nS1L1J1++;
          }else if(0 == m_dPID.sSpin()){
            spin = S0L1J1;
            m_nS0L1J1++;
          }
        }else if(0 == m_dPID.lSpin()){
          spin = S1L0J1;    
          m_nS1L0J1++;
        }
      }else if(1 == m_dPID.jSpin()){
        if(1 == m_dPID.lSpin()){
          spin = S1L1J0;        
          m_nS1L1J0++;
        }else if(0 == m_dPID.lSpin()){
          spin = S0L0J0;        
          m_nS0L0J0++;
        }
      }

      //mind that B** is L=1 state, B* is L=0 and J=1, and B is L=0, J=0
      if(S0L0J0==spin){
        m_stateB++;
      }else if(S1L0J1==spin) {
        m_stateBStar++;   
      }else{
        m_stateBStarStar++;
      }

      if( produceHistos() ) {
        int state = 2; //L=1 state
        if (S0L0J0==spin) state = 0;
        else if (S1L0J1==spin) state = 1;
        m_bMesonStates->fill((double)state);
      }
      if( produceHistos() ) m_bMesonStatesCode->fill((double)spin);
    }
    }
}
