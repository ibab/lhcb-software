// $Id: ProductionAsymmetry.cpp,v 1.2 2008-07-26 18:02:08 robbep Exp $
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
#include "ProductionAsymmetry.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ProductionAsymmetry
//
// Based on Generator Analysis algorithm
// 2007-04-24 : R. Lambert
//
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( ProductionAsymmetry )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ProductionAsymmetry::ProductionAsymmetry( const std::string& name,
                              ISvcLocator* pSvcLocator)
  : GaudiHistoAlg( name , pSvcLocator )
  , m_minEta           ( 2.0 )
  , m_maxEta          ( 4.9 )
  , m_counter          ( 0 )
  , m_nEvents          ( 0 )
{

  declareProperty( "MinEta", m_minEta = 2.0); //LHCb min eta
  declareProperty( "MaxEta", m_maxEta = 4.9); //LHCb max eta
  declareProperty( "Input",  m_dataPath = LHCb::HepMCEventLocation::Default ); //Location of HepMC Events
  declareProperty( "ApplyTo", m_generatorName = "" ); //specify the generator to study
  declareProperty( "Signal", m_signalName = "B0 " ); //name of plots
  declareProperty( "SignalPIDs", m_hepPID); //list of PIDs to plot
  

}

//=============================================================================
// Destructor
//=============================================================================
ProductionAsymmetry::~ProductionAsymmetry() {}

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode ProductionAsymmetry::initialize() {

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

  //initialise vector of PIDs just in case... don't want a seg fault!
  if(m_hepPID.size() == 0)
  {
    m_hepPID.push_back(511); //B0 production asymmetry as standard
    
  }

  info() << "Looking for ..."
         << m_signalName << endmsg;
  
  
  for(unsigned int i=0; i<m_hepPID.size(); i++)
  {
    
    LHCb::ParticleID aPID(abs(m_hepPID[i]));  //store particles in this vector!
    m_sPID.push_back(aPID);
    info() << " signal _particle_ PID "
         << m_sPID[i].pid() << endmsg; 
    m_counters.push_back(0);
    m_asymcounters.push_back(0);
    m_partcounters.push_back(0);
    m_partcounters.push_back(0); //particle then antipaticle
    
  }

  
  return StatusCode::SUCCESS;

}

//=============================================================================
// Main execution
//=============================================================================
StatusCode ProductionAsymmetry::execute() {

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

        for(unsigned int i=0; i<m_sPID.size(); i++)
        {
          
        
          if(m_sPID[i].pid() == abs(hepMCpart->pdg_id()))
          {
            double pseudoRap =  hepMCpart->momentum().pseudoRapidity();
            //is the particle inside the acceptance?
            if( (pseudoRap > m_minEta) && (pseudoRap < m_maxEta) )
            {
              debug() << "found desired particle" << endmsg;

              m_counter++;
              m_counters[i]++;
              
              //get PID
              int aPID = hepMCpart->pdg_id();
              int aPIDtype = abs(aPID);
             
              debug() << "subtracting anti particles from particles" << endmsg;
              int par_or_anti = aPID/aPIDtype;           //-1 for antiparticle, 1 for particle

              m_asymcounters[i]+=par_or_anti;
              int isantiparticle = (1 - par_or_anti)/2;  //1 for antiparticle, 0 for particle

              m_partcounters[i*2 + isantiparticle]++;
            
              someplots(hepMCpart);
              
            }
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
StatusCode ProductionAsymmetry::finalize() {

  debug() << "==> Finalize" << endmsg;

  info() << "========================================================================" << endmsg;
  
  info() << m_counter << " " << m_signalName << " total " << " in LHCb acc. from " << m_nEvents << " events" << endmsg;
  
  for(unsigned int i=0; i<m_sPID.size();i++)
  {

    info() << m_counters[i] << " of type " << m_sPID[i].pid() << " with " 
           << m_asymcounters[i] << " more particles than antiparticles" << endmsg;

    info() << m_partcounters[i*2] << " of " << m_sPID[i].pid() << " and " 
           << m_partcounters[i*2 + 1] << " of " << -1* m_sPID[i].pid() << endmsg;

    double anasym = double(m_partcounters[i*2 + 1])/double(m_partcounters[i*2]) - 1.0;
    
    info() << "    dp = " << anasym << endmsg;
    

  }
  
  info() << "========================================================================" << endmsg;
 
  return GaudiHistoAlg::finalize();
  
}


//=============================================================================
//  BookHistos
//=============================================================================
StatusCode ProductionAsymmetry::someplots(const HepMC::GenParticle* plotme) 
{

  
   double pseudoRap =  plotme->momentum().pseudoRapidity();
   double energy = plotme->momentum().e()/Gaudi::Units::GeV;
   double pt = plotme->momentum().perp()/Gaudi::Units::GeV;
   double p =  plotme->momentum().rho()/Gaudi::Units::GeV;
            
   debug() << "ready to book histos" << endmsg;

   //get PID

   int aPID = plotme->pdg_id();
   std::string strPID = boost::lexical_cast<std::string>(aPID);

   //get PID type, PID(antiparticle) = -1*PID(particle)

   int aPIDtype = abs(aPID);
   std::string strPIDtype = boost::lexical_cast<std::string>(aPIDtype);

   //these names will be used to add to specific named plots.


   debug() << "subtracting anti particles from particles" << endmsg;
   int addorsub = aPID/aPIDtype;

   //plots for individual particles;
   std::string histname = m_signalName + strPID;
   std::string histtag = strPID;

   histname = m_signalName + strPID + " PseudoRapidity";
   histtag = "I" + strPID + "PsR";
   plot(pseudoRap, histtag, histname ,0.,8.,100);

   histname = m_signalName + strPID + " Energy";
   histtag =  "I" + strPID + "E";
   plot(energy, histtag, histname ,0.,350,200);

   histname = m_signalName + strPID + " Momentum";
   histtag = "I" + strPID + "p";
   plot(p, histtag, histname ,0.,350,200);

   histname = m_signalName + strPID + " Transverse Momentum";
   histtag = "I" + strPID + "pt";
   plot(pt, histtag, histname ,0.,50,200);

   //production asymmetry plots

   histname = m_signalName + strPIDtype + " dp PseudoRapidity";
   histtag = "dp" + strPIDtype + "PsR";
   plot(pseudoRap, histtag, histname ,0.,8.,100, addorsub);

   histname = m_signalName + strPIDtype + " dp Energy";
   histtag =  "dp" + strPIDtype + "E";
   plot(energy, histtag, histname ,0.,350,200, addorsub);

   histname = m_signalName + strPIDtype + " dp Momentum";
   histtag = "dp" + strPIDtype + "p";
   plot(p, histtag, histname ,0.,350,200, addorsub);

   histname = m_signalName + strPIDtype + " dp Transverse Momentum";
   histtag = "dp" + strPIDtype + "pt";
   plot(pt, histtag, histname ,0.,50,200, addorsub);

   //Totals plots

   histname = m_signalName + strPIDtype + " PseudoRapidity Total";
   histtag = "T" + strPIDtype + "PsR";
   plot(pseudoRap, histtag, histname ,0.,8.,100);

   histname = m_signalName + strPIDtype + " Energy Total";
   histtag =  "T" + strPIDtype + "E";
   plot(energy, histtag, histname ,0.,350,200);

   histname = m_signalName + strPIDtype + " Momentum Total";
   histtag = "T" + strPIDtype + "p";
   plot(p, histtag, histname ,0.,350,200);

   histname = m_signalName + strPIDtype + " Transverse Momentum Total";
   histtag = "T" + strPIDtype + "pt";
   plot(pt, histtag, histname ,0.,50,200);


   debug() << "histos booked" << endmsg;

   return StatusCode::SUCCESS;
  
}
