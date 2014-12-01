// $Id: ProductionAnalysis.cpp,v 1.2 2008-07-26 18:02:08 robbep Exp $
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
#include "ProductionAnalysis.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ProductionAnalysis
//
// Based on the Generator Analysis algorithm
// 2007-04-24 : R. Lambert
// 
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( ProductionAnalysis )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ProductionAnalysis::ProductionAnalysis( const std::string& name,
                              ISvcLocator* pSvcLocator)
  : GaudiHistoAlg( name , pSvcLocator )
  , m_minEta           ( 2.0 )
  , m_maxEta          ( 4.9 )
  , m_counter          ( 0 )
  , m_nEvents          ( 0 )
  , m_hepPID          ( 511 )
{

  declareProperty( "MinEta", m_minEta = 2.0);   //lhcb min eta
  declareProperty( "MaxEta", m_maxEta = 4.9);   //lhcb max eta
  declareProperty( "Input",  m_dataPath = LHCb::HepMCEventLocation::Default ); //location of HepMCEvent
  declareProperty( "ApplyTo", m_generatorName = "" ); //apply to a certain generator only
  declareProperty( "Signal", m_signalName = "B0 " ); //name to add to plots
  declareProperty( "SignalPID", m_hepPID = 511);    //signal PID
  

}

//=============================================================================
// Destructor
//=============================================================================
ProductionAnalysis::~ProductionAnalysis() {}

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode ProductionAnalysis::initialize() {

  StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiHistoAlg

  debug() << "==> Initialize" << endmsg;
  
  if ( m_generatorName.empty() ) 
  {
    info() << "Monitor will be applied to all events in container " 
           << m_dataPath << endmsg;
  }
  else 
  {
    info() << "Monitor will be applied to events produced with generator "
           << m_generatorName << " in container "
           << m_dataPath << endmsg; 
  }

  // option -> PID

  LHCb::ParticleID aPID(m_hepPID);
  m_sPID = aPID;

  info() << "Looking for "
         << m_signalName << " i.e. PID "
         << m_sPID.pid() << endmsg; 

  
  return StatusCode::SUCCESS;

}

//=============================================================================
// Main execution
//=============================================================================
StatusCode ProductionAnalysis::execute() {

  debug() << "==> Execute" << endmsg;

  
  // Retrieve data from selected path
  SmartDataPtr< LHCb::HepMCEvents > hepMCptr( eventSvc() , m_dataPath );

  if( 0 == hepMCptr ) 
  {
    info() << "No HepMCEvents at location " << m_dataPath << endmsg;
    return StatusCode::SUCCESS;
  }

  LHCb::HepMCEvents::iterator it ;
  for( it = hepMCptr->begin() ; it != hepMCptr->end(); ++it ) //loop over event container
  {

      // Check if monitor has to be applied to this event
      if( !m_generatorName.empty() ) 
      {
        if( m_generatorName != (*it)->generatorName() ) continue;
      }
      
      debug() << "Monitor for " << (*it)->generatorName()
              << endmsg;
 
      for( HepMC::GenEvent::particle_const_iterator 
             itp = (*it)->pGenEvt()->particles_begin();
           itp != (*it)->pGenEvt()->particles_end(); itp++ )  //loop over particles
      {
        debug() << "in particle loop" << endmsg;

        HepMC::GenParticle* hepMCpart = *itp;
        //is this particle of the desired type? 
        debug() << "PID is " << hepMCpart->pdg_id() << endmsg;
        
        if(m_sPID.pid() == hepMCpart->pdg_id())
        {
          double pseudoRap =  hepMCpart->momentum().pseudoRapidity();
          //is the particle inside the acceptance?
          if( (pseudoRap > m_minEta) && (pseudoRap < m_maxEta) )
          {
            debug() << "found desired particle" << endmsg;

            m_counter++;
  
              double energy = hepMCpart->momentum().e()/Gaudi::Units::GeV;
              double pt = hepMCpart->momentum().perp()/Gaudi::Units::GeV;
              double p =  hepMCpart->momentum().rho()/Gaudi::Units::GeV;
            
              debug() << "ready to book histos" << endmsg;

              //plot();
              std::string histname = m_signalName;
              histname = m_signalName + "PseudoRapidity";
              plot(pseudoRap, "PsR", histname ,0.,8.,100);
              histname = m_signalName + "Energy";
              plot(energy, "E", histname ,0.,350,100);

              histname = m_signalName + "Momentum";
              plot(p, "p", histname ,0.,350,100);

              histname = m_signalName + "Transverse Momentum";
              plot(pt, "pt", histname ,0.,50,100);

              debug() << "histos booked" << endmsg;
            
          }
          
        }
      }
  }
  
  m_nEvents++ ;
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode ProductionAnalysis::finalize() {

  debug() << "==> Finalize" << endmsg;

  info() << m_counter << " " << m_signalName << " with PID " << m_sPID.pid() 
         << " in LHCb acc. from " << m_nEvents << " events" << endmsg;
  
 
  return GaudiHistoAlg::finalize();
  
}

