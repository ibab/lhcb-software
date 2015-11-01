// $Id: BOppositeMonitoring.cpp,v 1.1 2009-10-19 11:23:20 musy Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "BOppositeMonitoring.h"

//-----------------------------------------------------------------------------
// Implementation file for class : BOppositeMonitoring
// 
// 2009-10-08 : Marc Grabalosa Gandara
//-----------------------------------------------------------------------------

using namespace LHCb;
using namespace Gaudi::Units;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( BOppositeMonitoring )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
BOppositeMonitoring::BOppositeMonitoring( const std::string& name,
                                          ISvcLocator* pSvcLocator)
  : DaVinciHistoAlgorithm ( name , pSvcLocator ),
    m_forcedBtool(0)
{
}

//=============================================================================
// Destructor
//=============================================================================
BOppositeMonitoring::~BOppositeMonitoring() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode BOppositeMonitoring::initialize() {
  StatusCode sc = DaVinciHistoAlgorithm::initialize(); 
  if ( sc.isFailure() ) return sc;

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;
  
  //Identify B signal
  //m_forcedBtool = tool<IForcedBDecayTool> ( "ForcedBDecayTool", this );// doesn't work for MC12 data
  m_forcedBtool = tool<IBDecayTool> ( "BDecayTool", this );
  if( ! m_forcedBtool ) {
    fatal() << "Unable to retrieve BDecayTool tool "<< endmsg;
    return StatusCode::FAILURE;
  }
  
  //counters
  nmcpb=0;
  //B
  nB511=0;
  nB521=0;
  nB531=0;
  nB541=0;
  //B*
  nB513=0;
  nB523=0;
  nB533=0;
  nB543=0;
  //B**
  nB515=0;
  nB525=0;
  nB535=0;
  nB545=0;
  //barions
  nBarion=0;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode BOppositeMonitoring::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  // Event info
  RecHeader* evt = get<RecHeader> (RecHeaderLocation::Default);
  debug()<<">>>>>  Processing Event "<<evt->evtNumber()<<" (run: "<<evt->runNumber()<<")"<<endmsg;
  
  // Retrieve MCParticles
  MCParticles* mcpart = get<MCParticles> (MCParticleLocation::Default);
  debug() << "Nr of MCParticles retrieved="<< mcpart->size()<< endmsg;
  
  //check what is the B forced to decay
  const MCParticle* BS = m_forcedBtool->forcedB();
  if ( !BS ) {
    warning() << "Missing Forced Signal B meson in MC!"<< endmsg;
    //return StatusCode::SUCCESS;                      
  }
  if ( BS ) {
    debug()<<"BS, P: "<<BS->momentum().P()<<", Pt: "<<BS->pt()<<", PID: "<<BS->particleID().pid()<<endmsg;
  }
  
  //look for opposite mc particles
  MCParticles::const_iterator imc;
  for ( imc = mcpart->begin(); imc != mcpart->end(); imc++ ) {
    if( (*imc) != BS ) 
      if((*imc)->particleID().hasBottom()) {

        ++nmcpb;

        //info mcpart
        debug()<<"P: "<<(*imc)->momentum().P()<<", Pt: "<<(*imc)->pt()<<", PID: "<<(*imc)->particleID().pid()<<endmsg;

        //counters
        int aid = abs( (*imc)->particleID().pid() );
        //B
        if(aid==511) ++nB511;
        if(aid==521) ++nB521;
        if(aid==531) ++nB531;
        if(aid==541) ++nB541;
        if(aid==511 || aid==521 || aid==531 || aid==541) plot(aid, "Bpid", "Bpid", 500., 550. );
        //B*
        if(aid==513) ++nB513;
        if(aid==523) ++nB523;
        if(aid==533) ++nB533;
        if(aid==543) ++nB543;
        if(aid==513 || aid==523 || aid==533 || aid==543) plot(aid, "Bspid", "Bspid", 500., 550. );
        //B**
        if(aid==10511 || aid==10513 || aid==20513 || aid==515 ) { aid=515; ++nB515; }
        if(aid==10521 || aid==10523 || aid==20523 || aid==525 ) { aid=525; ++nB525; }
        if(aid==10531 || aid==10533 || aid==20533 || aid==535 ) { aid=535; ++nB535; }
        if(aid==10541 || aid==10543 || aid==20543 || aid==545 ) { aid=545; ++nB545; }
        if(aid==515 || aid==525 || aid==535 || aid==545) plot(aid, "Bsspid", "Bsspid", 500., 550. );
        //barions
        if(aid>5000 && aid<5599) ++nBarion, plot(aid, "Barionpid", "Barionpid", 5000., 5600. );

        //histos
        plot((*imc)->particleID().pid(), "pid", "pid", -22000., 22000. ); // Pid
      }
  }
  ////////////////////////////////////////////////////

 
  setFilterPassed(true);  // Mandatory. Set to true if event is accepted. 
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode BOppositeMonitoring::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;
  
  //info
  info()<<"######### Fraction of B opposite particle type"<<endmsg;
  double dmcpb = double ( nmcpb );
  info()<<nmcpb<<" mcp with B"<<std::setprecision(3)<<endmsg;
  info()<<"B_d  : "<<nB511/dmcpb<<" B^+  : "<<nB521/dmcpb<<" B_s  : "<<nB531/dmcpb<<" B_c  : "<<nB541/dmcpb<<endmsg;
  info()<<"B*_d : "<<nB513/dmcpb<<" B^*+ : "<<nB523/dmcpb<<" B*_s : "<<nB533/dmcpb<<" B*_c : "<<nB543/dmcpb<<endmsg;
  info()<<"B^*0_2  + B(H)^0_1  + B(L)^0_1  + B^*0_0  : "<<nB515/dmcpb<<endmsg;
  info()<<"B^*+_2  + B(H)^+_1  + B(L)^+_1  + B^*+_0  : "<<nB525/dmcpb<<endmsg;
  info()<<"B^*0_s2 + B(H)^0_s1 + B(L)^0_s1 + B^*0_s0 : "<<nB535/dmcpb<<endmsg;
  info()<<"B^*+_c2 + B(H)^+_c1 + B(L)^+_c1 + B^*+_c0 : "<<nB545/dmcpb<<endmsg;
  info()<<"Barions: "<<nBarion/dmcpb<<endmsg;
  info()<<"#########"<<endmsg;
  
  return DaVinciHistoAlgorithm::finalize();
}

//=============================================================================
