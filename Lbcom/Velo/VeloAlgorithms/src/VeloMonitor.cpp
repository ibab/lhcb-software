// Include files 
#include "VeloMonitor.h"

//STL
#include <vector>
// CLHEP
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Geometry/Point3D.h"
// Gaudi
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "Event/MCParticle.h"
#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/SmartRefVector.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "Relations/IAssociatorWeighted.h"
#include "GaudiKernel/IToolSvc.h"

//- Velo classes ----------------------------------------------
#include "Event/MCVeloHit.h"
#include "Event/MCVeloFE.h"
#include "Event/VeloFullDigit.h"
#include "Event/VeloCluster.h"
#include "Kernel/VeloChannelID.h"
#include "VeloDet/DeVelo.h"
#include "VeloKernel/VeloSimParams.h"
#include "VeloKernel/VeloRound.h"
#include "VeloDet/DeVelo.h"
//--------------------------------------------------------------------

//--------------------------------------------------------------------

//--------------------------------------------------------------------
//
//	VeloMonitor: algorithm to illustrate the functionality of
//                     the Velo classes
//
// Writes out some info. on MCVeloHits, MCVeloFEs
//
//	Author :   Chris Parkes
//
//--------------------------------------------------------------------

// static data/functions can be shared by all instances of class.
static const AlgFactory<VeloMonitor>    Factory;
const IAlgFactory& VeloMonitorFactory = Factory;


// Algorithm parameters which can be set at run time must be declared.
// This should be done in the constructor.

VeloMonitor::VeloMonitor(const std::string& name, ISvcLocator* pSvcLocator) 
 : Algorithm(name, pSvcLocator),
   m_printout (10),
   m_testMCVeloHit     ( false ),
   m_testMCVeloFE      ( true ),
   m_testVeloFullDigit ( false ),
   m_testVeloCluster   ( false ),
   m_resolution        ( false )

 {
   declareProperty( "TestMCVeloHit"           ,m_testMCVeloHit );
   declareProperty( "TestMCVeloFE"            ,m_testMCVeloFE );
   declareProperty( "TestVeloFullDigit"       ,m_testVeloFullDigit );
   declareProperty( "TestVeloCluster"         ,m_testVeloCluster );
   declareProperty( "Resolution"              ,m_resolution );
   declareProperty( "Printout"                ,m_printout);

   Rndm::Numbers m_uniformDist;
 
}


StatusCode VeloMonitor::initialize() {
  
   MsgStream log(messageService(), name());
   log << MSG::INFO << ">>> Initialize" << endreq;

   // book histos
   if (m_testMCVeloHit) {
   // MCVeloHit
     m_MCVHNHits = histoSvc()->
       book("velo/1", "Number of MCVeloHits per event", 100, 0., 3000.);
     m_MCVHEnergy = histoSvc()->
       book("velo/2", "Energy deposited in Si (eV)", 100, 0., 300000.);
     m_MCVHEntryXY = histoSvc()->
       book("velo/3", "Particle entry point in Si XY (eV)", 50, -10, 10.,50,-10.,10.);
     m_MCVHEntryZX = histoSvc()->
       book("velo/4", "Particle entry point in Si ZX (eV)", 1000, -20., 80.,50,-10.,10.);
     m_MCVHExitXY = histoSvc()->
       book("velo/5", "Particle exit point in Si XY (eV)", 50, -10, 10.,50,-10.,10.);
     m_MCVHExitZX = histoSvc()->
       book("velo/6", "Particle exit point in Si ZX (eV)", 1000, -20., 80.,50,-10.,10.);
     m_MCVHSensorZ = histoSvc()->
       book("velo/7", "Z position and Sensor number (eV)", 1000, -20., 80.,50,0.,100.);
     m_MCVHTOF = histoSvc()->
       book("velo/8", "Time Of Flight (nS)", 100, 0., 5.);
     m_MCPEnergy = histoSvc()->
       book("velo/9", "Particle Energy (GeV)", 100, 0., 50.);

      // pileup MCVeloHit

     m_MCVHPNHits = histoSvc()->
       book("velo/1001", "PU:Number of MCVeloHits per event", 100, 0., 2000.);
     m_MCVHPEnergy = histoSvc()->
       book("velo/1002", "PU:Energy deposited in Si (eV)", 100, 0., 300000.);
     m_MCVHPEntryXY = histoSvc()->
       book("velo/1003", "PU:Particle entry point in Si XY (eV)", 50, -10, 10.,50,-10.,10.);
     m_MCVHPEntryZX = histoSvc()->
       book("velo/1004", "PU:Particle entry point in Si ZX (eV)", 1000, -20., 80.,50,-10.,10.);
     m_MCVHPExitXY = histoSvc()->
       book("velo/1005", "PU:Particle exit point in Si XY (eV)", 50, -10, 10.,50,-10.,10.);
     m_MCVHPExitZX = histoSvc()->
       book("velo/1006", "PU:Particle exit point in Si ZX (eV)", 1000, -20., 80.,50,-10.,10.);
     m_MCVHPSensorZ = histoSvc()->
       book("velo/1007", "PU:Z position and Sensor number (eV)", 1000, -20., 80.,50,0.,100.);
     m_MCVHPTOF = histoSvc()->
       book("velo/1008", "PU:Time Of Flight (nS)", 100, 0., 5.);

     m_MCPUPEnergy = histoSvc()->
       book("velo/1009", "PU:Energy (eV)", 100, 0., 50.);
   }
   if (m_testMCVeloFE) {
   // MCVeloFE
     m_MCVFEMCHits = histoSvc()->
       book("velo/10", "Number of MC hits per FE", 6, -0.5, 5.5);

     m_MCVFENHits = histoSvc()->
       book("velo/11", "Number of MCVeloFEs per event", 16, 0., 8000.);
     m_MCVFECharge = histoSvc()->
       book("velo/12", "Charge in Si strip (electrons)", 100, -10000., 50000.);
     m_MCVFECharge_s = histoSvc()->
       book("velo/1112", "s Charge in Si strip (electrons)", 100, -10000., 50000.);
     m_MCVFECharge_n = histoSvc()->
       book("velo/1212", "n Charge in Si strip (electrons)", 100, -10000., 50000.);
     m_MCVFECharge_o = histoSvc()->
       book("velo/1312", "o Charge in Si strip (electrons)", 100, -10000., 50000.);
     m_MCVFESignal = histoSvc()->
       book("velo/13", "Signal deposited in Si strip (electrons)", 100, 0., 50000.);
     m_MCVFESignal_s = histoSvc()->
       book("velo/1113", "s Signal deposited in Si strip (electrons)", 100, 0., 50000.);
     m_MCVFESignal_n = histoSvc()->
       book("velo/1213", "n Signal deposited in Si strip (electrons)", 100, 0., 50000.);
     m_MCVFESignal_o = histoSvc()->
       book("velo/1313", "o Signal deposited in Si strip (electrons)", 100, 0., 50000.);
     m_MCVFENoise = histoSvc()->
       book("velo/14", "Noise added in Si strip (electrons)", 100, -10000., 10000.);
     m_MCVFENoise_s = histoSvc()->
       book("velo/1114", "s Noise added in Si strip (electrons)", 100, -10000., 10000.);
     m_MCVFENoise_n = histoSvc()->
       book("velo/1214", "n Noise added in Si strip (electrons)", 100, -10000., 10000.);
     m_MCVFENoise_o = histoSvc()->
       book("velo/1314", "o Noise added in Si strip (electrons)", 100, -10000., 10000.);
     m_MCVFECMNoise = histoSvc()->
       book("velo/15", "CM noise added in Si strip (electrons)", 100, -20000., 20000.); 
     m_MCVFEPedestal = histoSvc()->
       book("velo/16", "Pedestal added in Si strip (electrons)", 100, 0., 50000.);
     m_MCVFESensorStrip = histoSvc()->
       book("velo/17", "Sensor and strip number", 100, 0., 100.,50,0.,5000.);
     m_MCVFESensorStrip_s = histoSvc()->
       book("velo/1117", "s Sensor and strip number", 100, 0., 100.,50,0.,5000.);
     m_MCVFESensorStrip_n = histoSvc()->
       book("velo/1217", "n Sensor and strip number", 100, 0., 100.,50,0.,5000.);
     m_MCVFESensorStrip_o = histoSvc()->
       book("velo/1317", "o Sensor and strip number", 100, 0., 100.,50,0.,5000.);

     m_MCVFEMCHitsSignal= histoSvc()->
       book("velo/18", "MC signal deposited in FE", 100, 0., 50000);
     m_MCVFEMCHitsSignal_s= histoSvc()->
       book("velo/1118", "s MC signal deposited in FE", 100, 0., 50000);
     m_MCVFEMCHitsSignal_n= histoSvc()->
       book("velo/1218", "n MC signal deposited in FE", 100, 0., 50000);
     m_MCVFEMCHitsSignal_o= histoSvc()->
       book("velo/1318", "o MC signal deposited in FE", 100, 0., 50000);
     m_MCVFEMCHitsTotalSignal= histoSvc()->
       book("velo/19", "MC total signal deposited in FE", 100, 0., 50000);
     m_MCVFEMCHitsTotalSignal_s= histoSvc()->
       book("velo/1119", "s MC total signal deposited in FE", 100, 0., 50000);
     m_MCVFEMCHitsTotalSignal_n= histoSvc()->
       book("velo/1219", "n MC total signal deposited in FE", 100, 0., 50000);
     m_MCVFEMCHitsTotalSignal_o= histoSvc()->
       book("velo/1319", "o MC total signal deposited in FE", 100, 0., 50000);
   }
   if (m_testVeloFullDigit) {
     // VeloFullDigit
     m_VDNHits = histoSvc()->
       book("velo/21", "Number of VeloFullDigits per event", 100, 0., 2000.);
  
     m_VDADC  = histoSvc()->
       book("velo/22", "ADC counts after correction", 256, 0., 256.);

     m_VDRawADC = histoSvc()->
       book("velo/23", "ADC counts", 256, 0., 256.);

     m_VDPedestal = histoSvc()->
       book("velo/24", "subtracted pedestal", 100, 0., 1024.);

     m_VDCMNoise = histoSvc()->
       book("velo/25", "subtracted CM noise", 100, 0., 1024.);

     m_VDNoise = histoSvc()->
       book("velo/26", "noise", 100, 0., 1024.);

     m_VDRawNoise = histoSvc()->
       book("velo/27", "raw noise", 100, 0., 1024.);

     m_VDSensorStrip = histoSvc()->
       book("velo/28", "Sensor and strip number", 100, 0., 100.,50,0.,5000.);
   }
   if (m_testVeloCluster) {
   //Velo Cluster
     m_VCNHits = histoSvc()->
       book("velo/31", "Number of VeloClusters per event", 50, 0., 3000.);
     m_VCNHits_s = histoSvc()->
       book("velo/1131", "s Number of VeloClusters per event", 50, 0., 3000.);
     m_VCNHits_n = histoSvc()->
       book("velo/1231", "n Number of VeloClusters per event", 50, 0., 3000.);
     m_VCNHits_o = histoSvc()->
       book("velo/1331", "o Number of VeloClusters per event", 50, 0., 3000.);

     m_VCADCSum = histoSvc()->
       book("velo/32", "ADC sum", 256, -0.5, 255.5);
     m_VCADCSum_s = histoSvc()->
       book("velo/1132", "s ADC sum", 256, -0.5, 255.5);
     m_VCADCSum_n = histoSvc()->
       book("velo/1232", "n ADC sum", 256, -0.5, 255.5);
     m_VCADCSum_o = histoSvc()->
       book("velo/1332", "o ADC sum", 256, -0.5, 255.5);

     m_VCSize = histoSvc()->
       book("velo/33", "number of strips in cluster", 6, -0.5, 5.5);
     m_VCSize_s = histoSvc()->
       book("velo/1133", "number of strips in cluster", 6, -0.5, 5.5);
     m_VCSize_n = histoSvc()->
       book("velo/1233", "number of strips in cluster", 6, -0.5, 5.5);
     m_VCSize_o = histoSvc()->
       book("velo/1333", "number of strips in cluster", 6, -0.5, 5.5);

     m_VCSensorStrip = histoSvc()->
       book("velo/34", "Sensor and first strip number", 100, 0., 100.,50,0.,5000.);
   }

   if (m_resolution) {
     //efficiency
     m_VCEfficiencyN = histoSvc()->
       book("velo/41", "deposited MC energy", 100, 0.,500000.);
     m_VCEfficiencyT = histoSvc()->
       book("velo/141", "deposited MC energy with Cluster reconstructed", 100,0.,500000.);
     m_VCEfficiencyNP = histoSvc()->
       book("velo/42", "particle true momentum ", 100, 0.,10.);
     m_VCEfficiencyTP = histoSvc()->
       book("velo/142", "particle true momentum with Cluster reconstructed", 100, 0.,10.);
     m_VCEfficiencyNXY = histoSvc()->
       book("velo/43", "Particle entry point in Si XY ",100,-5, 5.,100,-5.,5.);
     m_VCEfficiencyNRZ = histoSvc()->
       book("velo/44", "Particle entry point in Si ZR ", 1000, -20., 80.,100,0.,5.);
     m_VCEfficiencyTXY = histoSvc()->
       book("velo/143", "Particle entry point in Si XY with Cluster reconstructed", 100,-5, 5.,100,-5.,5.);
     m_VCEfficiencyTRZ = histoSvc()->
       book("velo/144", "Particle entry point in Si ZR with Cluster reconstructed", 1000, -20.,
	    80.,100,0.,5.);


     // resolution
     m_RRes = histoSvc()->
       book("velo/101", "R Sensor resolution (microns)", 50, -50., +50.);
     m_RResd = histoSvc()->
       book("velo/105", "R Sensor resolution (strip fraction)", 100, -5., +5.);
     m_RRes1s = histoSvc()->
       book("velo/107", "R Sensor resolution 1 strip cluster (microns)", 50, -50., +50.);
     m_RRes2s = histoSvc()->
       book("velo/108", "R Sensor resolution 2 strip cluster (microns)", 50, -50., +50.);
     m_RRes3s = histoSvc()->
       book("velo/110", "R Sensor resolution 3 strip cluster (microns)", 50, -50., +50.);
     m_RCluster = histoSvc()->
       book("velo/109", "R Sensor N strips in cluster", 6, -0.5, 5.5);

     m_PhiRes = histoSvc()->
       book("velo/102", "Phi Sensor resolution (microns)", 100, -50., +50.);
     m_PhiResd = histoSvc()->
       book("velo/106", "Phi Sensor resolution (strip fraction)", 100, -5., +5.);
     m_RAngle = histoSvc()->
       book("velo/103", "Track theta", 10, -0., +0.25);
     m_RAngle1s = histoSvc()->
       book("velo/122", "Track theta 1 strip", 10, -0., +0.25);
     m_RAngle2s = histoSvc()->
       book("velo/123", "Track theta 2 strip", 10, -0., +0.25);
     m_RAngle2s = histoSvc()->
       book("velo/124", "Track theta 2 strip", 10, -0., +0.25);

     m_RResAngle= histoSvc()->
       book("velo/104", "R Sensor resolution vs track theta", 100, -20., +20., 10, -0.0, +0.25);

     m_randomTest = histoSvc()->
       book("velo/121", "random number test", 150, 0., +15000.);
   }

      // det element
      m_RvsStrip = histoSvc()->
      book("velo/111", "R Sensor R vs strip number", 100, 0., 5., 100, 0., 2048.);
      m_PhivsStrip = histoSvc()->
      book("velo/112", "Phi Sensor Phi vs strip number", 100, -1.6, 1.6, 100, 0., 2048.);
      m_PhiPlot = histoSvc()->
      book("velo/113", "Phi Sensor Phi edge strips", 100, -1. ,5.,  100, -5.0, 5.0);

      StatusCode scr2=m_uniformDist.initialize( randSvc(), Rndm::Flat(0.,1.0));
      if (scr2.isFailure()) return scr2;


  SmartDataPtr<DeVelo> velo( detDataService(), "/dd/Structure/LHCb/Velo" );
   
  if ( ! velo ) {
    log << MSG::ERROR << "Unable to retrieve Velo detector element." << endreq;
    return StatusCode::FAILURE;
   }

  m_velo = velo;

  m_NEvent=0; // event counter        
  return StatusCode::SUCCESS;
}



StatusCode VeloMonitor::execute() {
  
  MsgStream log(messageService(), name());
  log << MSG::INFO << ">>> execute" << endreq;

  m_NEvent++;
  
  if (m_testMCVeloHit)     testMCVeloHit();
  if (m_testMCVeloHit)     testPileUpMCVeloHit();
  if (m_testMCVeloFE)      testMCVeloFE();
  if (m_testVeloFullDigit) testVeloFullDigit();
  if (m_testVeloCluster)   testVeloCluster();

  if (m_NEvent==1)  detElement();
  if (m_resolution){
    associators();
    efficiency();
  //random();
    resolution(); // determine cluster resolution
  }
  log << MSG::INFO << ">>> execute complete" << endreq;
  return StatusCode::SUCCESS;
}

StatusCode VeloMonitor::testMCVeloHit() {
  ////////////////////////////////////////////////////
 // Access geant hits in silicon - MCVeloHits
  ////////////////////////////////////////////////////
   MsgStream log(messageService(), name());
   log << MSG::INFO << ">>> testMCVeloHit" << endreq;

   //  typedef ObjectVector<MCVeloHit> MCVeloHits;
  SmartDataPtr<MCVeloHits> mchits(eventSvc(),MCVeloHitLocation::Default  );

  if ( 0 == mchits ) { 
    log << MSG::ERROR << " ----  No MCVeloHits container retrieved --- " << endreq;
    return StatusCode::FAILURE;
  }

  if (m_printout>m_NEvent){
  log << MSG::INFO << "***************************************************" << endreq;
  log << MSG::INFO << "********* Number of MCVeloHits found " << mchits->size() << "**********" << endreq;
  log << MSG::INFO << "***************************************************" << endreq;
  }

  //  Define an iterator to the MC VELO hit vector and loop through it  
  MCVeloHits::iterator it;
  m_MCVHNHits->fill(mchits->size(),1.0);
  for (it=mchits->begin(); it!=mchits->end(); it++) {
    // printout some info. for VELO hit
  if (m_printout>m_NEvent){
    log << MSG::INFO << "testMCVeloHit:"
	<< "entry " << ((*it)->entry()).x()/cm << 
                " " << ((*it)->entry()).y()/cm << 
                " " << ((*it)->entry()).z()/cm 
        << " exit " << ((*it)->exit()).x()/cm << 
                " " << ((*it)->exit()).y()/cm << 
                " " << ((*it)->exit()).z()/cm 
        << " energy (eV)" << (*it)->energy()/eV
	<< " TOF (ns)" << (*it)->timeOfFlight()/ns 
      	<< " sensor " << (*it)->sensor() 
        << endreq;
  }
    // fill histograms
    m_MCVHEnergy->fill((*it)->energy()/eV,1.0);
    m_MCVHEntryXY->fill((*it)->entry().x()/cm,(*it)->entry().y()/cm,1.0);
    m_MCVHEntryZX->fill((*it)->entry().z()/cm,(*it)->entry().x()/cm,1.0);
    m_MCVHExitXY->fill((*it)->exit().x()/cm,(*it)->exit().y()/cm,1.0);
    m_MCVHExitZX->fill((*it)->exit().z()/cm,(*it)->exit().x()/cm,1.0);
    m_MCVHSensorZ->fill((*it)->entry().z()/cm,(*it)->sensor(),1.0);
    m_MCVHTOF->fill((*it)->timeOfFlight()/ns,1.0);
 
    // access MCParticle which made the hit, and write out 4-mom
    MCParticle* myMCParticle=(*it)->mcParticle();
    if (0!=myMCParticle){
      //noise, coupling & spillover hits won't have MCParticles
      HepLorentzVector fmom = myMCParticle->momentum();
      m_MCPEnergy->fill(fmom.e()/GeV);

      if (m_printout>m_NEvent){
        log << MSG::INFO << "testMCVeloHit:" <<" MCParticle " 
                     <<  "p_x " << fmom.px()/GeV 
	             << " p_y " << fmom.py()/GeV 
	             << " p_z " << fmom.pz()/GeV 
	             << " E "   << fmom.e()/GeV << endreq;
      }
    }
  }

 return StatusCode::SUCCESS;
}

StatusCode VeloMonitor::testPileUpMCVeloHit() {
  ////////////////////////////////////////////////////
 // Access geant hits in silicon - MCVeloHits
  ////////////////////////////////////////////////////
   MsgStream log(messageService(), name());
   log << MSG::INFO << ">>> testPileUpMCVeloHit" << endreq;

   //  typedef ObjectVector<MCVeloHit> MCVeloHits;
  SmartDataPtr<MCVeloHits> mchits(eventSvc(),MCVeloHitLocation::PuVeto  );

  if ( 0 == mchits ) { 
    log << MSG::ERROR << " ----  No PileUp MCVeloHits container retrieved --- " << endreq;
    return StatusCode::FAILURE;
  }

  if (m_printout>m_NEvent){

  log << MSG::INFO << "***************************************************" << endreq;
  log << MSG::INFO << "********* Number of PileUp MCVeloHits found " << mchits->size() << "**********" << endreq;
  log << MSG::INFO << "***************************************************" << endreq;
  }

  //  Define an iterator to the MC VELO hit vector and loop through it  
  MCVeloHits::iterator it;
  m_MCVHPNHits->fill(mchits->size(),1.0);
  for (it=mchits->begin(); it!=mchits->end(); it++) {
    // printout some info. for VELO hit
  if (m_printout>m_NEvent){
    log << MSG::INFO << "test PileUp MCVeloHit:"
	<< "entry " << ((*it)->entry()).x()/cm << 
                " " << ((*it)->entry()).y()/cm << 
                " " << ((*it)->entry()).z()/cm 
        << " exit " << ((*it)->exit()).x()/cm << 
                " " << ((*it)->exit()).y()/cm << 
                " " << ((*it)->exit()).z()/cm 
        << " energy (eV)" << (*it)->energy()/eV
	<< " TOF (ns)" << (*it)->timeOfFlight()/ns 
      	<< " sensor " << (*it)->sensor() 
        << endreq;
  }
    // fill histograms
    m_MCVHPEnergy->fill((*it)->energy()/eV,1.0);
    m_MCVHPEntryXY->fill((*it)->entry().x()/cm,(*it)->entry().y()/cm,1.0);
    m_MCVHPEntryZX->fill((*it)->entry().z()/cm,(*it)->entry().x()/cm,1.0);
    m_MCVHPExitXY->fill((*it)->exit().x()/cm,(*it)->exit().y()/cm,1.0);
    m_MCVHPExitZX->fill((*it)->exit().z()/cm,(*it)->exit().x()/cm,1.0);
    m_MCVHPSensorZ->fill((*it)->entry().z()/cm,(*it)->sensor(),1.0);
    m_MCVHPTOF->fill((*it)->timeOfFlight()/ns,1.0);
 
    // access MCParticle which made the hit, and write out 4-mom
    MCParticle* myMCParticle=(*it)->mcParticle();
    if (0!=myMCParticle){
      //noise, coupling & spillover hits won't have MCParticles
      HepLorentzVector fmom = myMCParticle->momentum();
      m_MCPUPEnergy->fill(fmom.e()/GeV);

      if (m_printout>m_NEvent){
        log << MSG::INFO << "test pile up MCVeloHit:" <<" MCParticle " 
                     <<  "p_x " << fmom.px()/GeV 
	             << " p_y " << fmom.py()/GeV 
	             << " p_z " << fmom.pz()/GeV 
	             << " E "   << fmom.e()/GeV << endreq;
      }
    }
  }

 return StatusCode::SUCCESS;
}



StatusCode VeloMonitor::testMCVeloFE() {

  ////////////////////////////////////////////////////
  // Access MC si. + front-end simulation - MCVeloFE
  ////////////////////////////////////////////////////
   MsgStream log(messageService(), name());
   log << MSG::INFO << ">>> testMCVeloFE" << endreq;

  SmartDataPtr<MCVeloFEs> mcfes(eventSvc(),  MCVeloFELocation::Default);

  if ( 0== mcfes ) { 
    log << MSG::ERROR << " ----  No MCVeloFEs container retrieved --- " << endreq;
    return StatusCode::FAILURE;
  }

  if (m_printout>m_NEvent){
  log << MSG::INFO << "***************************************************" << endreq;
  log << MSG::INFO << "*********Number of MCVeloFEs found " 
      << mcfes->size() << "********" << endreq;
  log << MSG::INFO << "***************************************************" << endreq;
  }

  m_MCVFENHits->fill(mcfes->size(),1.);
  //  Define an iterator to the MC VELO FE vector and loop through it  
  MCVeloFEs::iterator itdv;
  for (itdv=mcfes->begin(); itdv!=mcfes->end(); itdv++) {
    // mainly signal or spillover/coupling or noise ?
      bool noise=false, signal=false, other=false;
      if (fabs((*itdv)->addedNoise()) > fabs((*itdv)->addedSignal())) noise=true; // noise
      else {
	  double signaltot=0.;
        for(std::vector<double>::iterator itch= (*itdv)->mcVeloHitsCharge().begin();
            itch!= (*itdv)->mcVeloHitsCharge().end(); itch++){             
	    signaltot+=(*itch);
	}
        signal = (signaltot > ((*itdv)->addedSignal()/2.)); // > 50% mc hits
        other = (!signal); // < 50% mc hits, hence spillover or cross-coupling dominates
      }

    // printout some info.
    if (m_printout>m_NEvent){
    log << MSG::INFO << "testMCVeloFE:"
      	<< " sensorNumber " << (*itdv)->sensor() 
        << " strip Number " << (*itdv)->strip()
        << " Added Noise " << (*itdv)->addedNoise()
        << " Added CM Noise " << (*itdv)->addedCMNoise()
        << " Added Pedestal " << (*itdv)->addedPedestal()
        << " Added Signal " << (*itdv)->addedSignal()
        << " Charge " << (*itdv)->charge()
        << endreq;
    }
    m_MCVFECharge->fill((*itdv)->charge(),1.);
    if (signal) m_MCVFECharge_s->fill((*itdv)->charge(),1.);
    if (noise)  m_MCVFECharge_n->fill((*itdv)->charge(),1.);
    if (other)  m_MCVFECharge_o->fill((*itdv)->charge(),1.);

    m_MCVFESignal->fill((*itdv)->addedSignal(),1.);
    if (signal) m_MCVFESignal_s->fill((*itdv)->addedSignal(),1.);
    if (noise)  m_MCVFESignal_n->fill((*itdv)->addedSignal(),1.);
    if (other)  m_MCVFESignal_o->fill((*itdv)->addedSignal(),1.);

    m_MCVFENoise->fill((*itdv)->addedNoise(),1.);
    if (signal) m_MCVFENoise_s->fill((*itdv)->addedNoise(),1.);
    if (noise)  m_MCVFENoise_n->fill((*itdv)->addedNoise(),1.);
    if (other)  m_MCVFENoise_o->fill((*itdv)->addedNoise(),1.);

    m_MCVFECMNoise->fill((*itdv)->addedCMNoise(),1.);
    m_MCVFEPedestal->fill((*itdv)->addedPedestal(),1.);
    m_MCVFESensorStrip->fill((*itdv)->sensor(),(*itdv)->strip(),1.0);
    if (signal) m_MCVFESensorStrip_s->fill((*itdv)->sensor(),(*itdv)->strip(),1.0);
    if (noise)  m_MCVFESensorStrip_n->fill((*itdv)->sensor(),(*itdv)->strip(),1.0);
    if (other)  m_MCVFESensorStrip_o->fill((*itdv)->sensor(),(*itdv)->strip(),1.0);

    // access MCHit(s) from which MC FE was made and write out energy
    SmartRefVector<MCVeloHit> myhits=(*itdv)->mcVeloHits();
    SmartRefVector<MCVeloHit>::iterator itmh;
    std::vector<double> myhitscharge=(*itdv)->mcVeloHitsCharge();
    std::vector<double>::iterator itch;

      if (m_printout>m_NEvent) {
      log << MSG::INFO << "testMCVeloFE:" << "MCVeloHit " <<
	  "size " << myhits.size() << endreq; }
      m_MCVFEMCHits->fill(myhits.size(),1.);
      if (signal) m_MCVFESensorStrip_s->fill((*itdv)->sensor(),(*itdv)->strip(),1.0);
      if (noise)  m_MCVFESensorStrip_n->fill((*itdv)->sensor(),(*itdv)->strip(),1.0);
      if (other)  m_MCVFESensorStrip_o->fill((*itdv)->sensor(),(*itdv)->strip(),1.0);

      double totalSignal=0.;
      itch=myhitscharge.begin();
      for (itmh=myhits.begin(); itmh!=myhits.end(); itmh++) {
        if (m_printout>m_NEvent) {
        log << MSG::INFO << "testMCVeloFE:" << "MCVeloHit " 
	  <<" charge in this strip (electrons) " << (*itch) 
	    <<" energy (eV) " << (*itmh)->energy()/eV << endreq;}

	totalSignal+=(*itch);    
        m_MCVFEMCHitsSignal->fill((*itch),1.);
        if (signal) m_MCVFEMCHitsSignal_s->fill((*itch),1.);
        if (noise)  m_MCVFEMCHitsSignal_n->fill((*itch),1.);
        if (other)  m_MCVFEMCHitsSignal_o->fill((*itch),1.);

        itch++;
      }
        m_MCVFEMCHitsTotalSignal->fill(totalSignal,1.);
        if (signal) m_MCVFEMCHitsTotalSignal_s->fill(totalSignal,1.);
        if (noise)  m_MCVFEMCHitsTotalSignal_n->fill(totalSignal,1.);
        if (other)  m_MCVFEMCHitsTotalSignal_o->fill(totalSignal,1.);
  }

 return StatusCode::SUCCESS;
}

StatusCode VeloMonitor::testVeloFullDigit() {
  ///////////////////////////////////////////////////////////////////
 // Access rec. cluster information as output onto DAQ - VeloCluster //
  ///////////////////////////////////////////////////////////////////

   MsgStream log(messageService(), name());
   log << MSG::INFO << ">>> testVeloFullDigit" << endreq;

  SmartDataPtr<VeloFullDigits> digits(eventSvc(), VeloFullDigitLocation::Default );

  if ( 0== digits ) { 
    log << MSG::ERROR << " ----  No VeloFullDigits container retrieved --- " << endreq;
    return StatusCode::FAILURE;
  }

  if (m_printout>m_NEvent){
    log << MSG::INFO << "***************************************************" << endreq;
    log << MSG::INFO << "*********Number of VeloFullDigits found " << digits->size() << "********" << endreq;
    log << MSG::INFO << "***************************************************" << endreq;
  }
  m_VDNHits->fill(digits->size(),1.);

  //  Define an iterator to the VELO digit vector and loop through it  
  VeloFullDigits::iterator itdv;
  for (itdv=digits->begin(); itdv!=digits->end(); itdv++) {
    // printout some info.
    if (m_printout>m_NEvent){
      log << MSG::INFO << "testVeloFullDigit:" 
      	<< " sensorNumber " << (*itdv)->sensor() 
        << " strip Number " << (*itdv)->strip()
        << " ADCValue " << (*itdv)->adcValue()
        << " rawADCValue " << (*itdv)->rawADCValue()
        << " subtractedPedestal " << (*itdv)->subtractedPedestal()
        << " subtractedCM " << (*itdv)->subtractedCM()
        << " noise " << (*itdv)->noise()
        << " rawNoise " << (*itdv)->rawNoise()
        << endreq;
    }

    m_VDSensorStrip->fill((*itdv)->sensor(),(*itdv)->strip(),1.0); 
    m_VDADC->fill((*itdv)->adcValue());
    m_VDRawADC->fill((*itdv)->rawADCValue());
    m_VDPedestal->fill((*itdv)->subtractedPedestal());
    m_VDCMNoise->fill((*itdv)->subtractedCM());
    m_VDNoise->fill((*itdv)->noise());
    m_VDRawNoise->fill((*itdv)->rawNoise());
  }
// to relate a VeloFullDigit to an mcdigit  use the strip and sensor number 

 return StatusCode::SUCCESS;

}

StatusCode VeloMonitor::testVeloCluster() {
  ///////////////////////////////////////////////////////////////////
 // Access rec. cluster information as output onto DAQ - VeloCluster //
  ///////////////////////////////////////////////////////////////////

   MsgStream log(messageService(), name());
   log << MSG::INFO << ">>> testVeloCluster" << endreq;
   bool signal=false, noise=false, other=false;

  SmartDataPtr<VeloClusters> clusters(eventSvc(), VeloClusterLocation::Default );

  if ( 0== clusters ) { 
    log << MSG::ERROR << " ----  No VeloCluster container retrieved --- " << endreq;
    return StatusCode::FAILURE;
  }
  m_VCNHits->fill(clusters->size(),1.);

  if (m_printout>m_NEvent){
    log << MSG::INFO << "***************************************************" << endreq;
    log << MSG::INFO << "*********Number of VeloClusters found " << clusters->size() << "********" << endreq;
    log << MSG::INFO << "***************************************************" << endreq;
  }
  //  Define an iterator to the VELO cluster vector and loop through it  
  VeloClusters::iterator itcv;
  for (itcv=clusters->begin(); itcv!=clusters->end(); itcv++) {
      // cluster from signal or noise or other ?

    // printout some info. for the cluster
    log << MSG::INFO << "testVeloCluster:" 
      	<< " sensorNumber " << (*itcv)->sensor() 
        << " First strip in cluster " << (*itcv)->strip(0)
        << endreq;
    // and the strips in the cluster
	int size=(*itcv)->size(); // number of strips
    if (m_printout>m_NEvent){
    for (int i=0;i<size;i++) {
    log << MSG::INFO << " testVeloCluster:"
        << " strip " <<   (*itcv)->strip(i) << " / " << size 
	<< " signal " <<  (*itcv)->adcValue(i);
    }
    log << MSG::INFO << endreq;
    }
    float adcSum=0.;
    for (int i=0;i<size;i++) {adcSum+=(*itcv)->adcValue(i);}
    m_VCSensorStrip->fill((*itcv)->sensor(),(*itcv)->strip(0));
    m_VCADCSum->fill(adcSum);
    if (signal) m_VCADCSum_s->fill(adcSum);
    if (noise) m_VCADCSum_n->fill(adcSum);
    if (other) m_VCADCSum_o->fill(adcSum);

    m_VCSize->fill(size);
    if (signal) m_VCSize_s->fill(size);
    if (noise) m_VCSize_n->fill(size);
    if (other) m_VCSize_o->fill(size);

  }
// to relate a veloCluster strip to mcfe(s)  use the strip and sensor number
 return StatusCode::SUCCESS;

}

  VeloChannelID VeloMonitor::weightedMean(VeloCluster* cluster, double& fraction){
      // calculated the weighted mean "position" of the cluster, 
      // in units of strip. 
 
   int Nstrips = cluster->size();
   int iDiff;
   double centre=0.;
   double total=0.;
   bool valid;
   for(int i=0; i<Nstrips; i++){
     iDiff=m_velo->neighbour(cluster->channelID(0),cluster->channelID(i),valid);
     centre+=float(iDiff+100)*cluster->adcValue(i);
     total+=cluster->adcValue(i);
   }
   centre /= total;
   centre -= 100;
   iDiff=int(VeloRound::round(centre));
   fraction=centre-iDiff;
   //   cout << "fraction" << fraction << endl;
   return m_velo->neighbour(cluster->channelID(0),iDiff,valid);
}

StatusCode VeloMonitor::efficiency() {

  // check MC truth associators
  MsgStream log(messageService(), name());
  
  log << MSG::INFO << "efficiency measurement" <<endreq;

  // get clusters
  SmartDataPtr<VeloClusters> clusters(eventSvc(), VeloClusterLocation::Default );
  if ( 0== clusters ) { 
    log << MSG::ERROR << " ----  No VeloCluster container retrieved --- " << endreq;
    return StatusCode::FAILURE;
  }

  // get MCVeloHit

  SmartDataPtr<MCVeloHits> mchits(eventSvc(),MCVeloHitLocation::Default  );
  if (0 == mchits){ 
    log << MSG::WARNING << "Failed to find MCVeloHits" << endreq;
    return StatusCode::FAILURE;
  } 
  MCVeloHits::iterator itmc;
  for (itmc=mchits->begin(); itmc!=mchits->end(); itmc++) {
      log << MSG::DEBUG << "MCVeloHits loop" <<endreq;
      MCVeloHit* hit = (*itmc);
      log << MSG::INFO << "MCVeloHits x/y/z " << (hit->entry()).x()/cm << 
	" " << (hit->entry()).y()/cm << 
        " " << (hit->entry()).z()/cm << endreq;

      double EntryFraction,ExitFraction;
      double pitch;
      bool EntryValid, ExitValid;
      VeloChannelID entryChan=m_velo->channelID(hit->entry(),EntryFraction,
                                          pitch,EntryValid);
      VeloChannelID exitChan=m_velo->channelID(hit->exit(),ExitFraction,
                                          pitch,ExitValid);
      const MCParticle * mcpart = (*itmc)->mcParticle();
      HepLorentzVector fmom = mcpart->momentum();
      double pmom = sqrt(fmom.px()*fmom.px()+fmom.py()*fmom.py()+fmom.pz()*fmom.pz()); 
      double theta = atan(sqrt(fmom.px()*fmom.px()+fmom.py()*fmom.py())/pmom); 
      
// count only particles in LHCb acceptance
      if (EntryValid&&ExitValid&&theta<0.4&&theta>0.015){
    
        const MCParticle * mcpart = (*itmc)->mcParticle();
        HepLorentzVector fmom = mcpart->momentum();
        double pmom = sqrt(fmom.px()*fmom.px()+fmom.py()*fmom.py()+fmom.pz()*fmom.pz()); 

        typedef IAssociatorWeighted<VeloCluster,MCVeloHit,double> MyAsct;
        const std::string Type = "VeloCluster2MCHitAsct";
        const std::string Name="VeloCluster2MCHit";
        MyAsct* associator = 0;
        StatusCode sc = toolSvc()->retrieveTool (Type, Name, associator);
        if ( sc.isFailure() ) { 
          log << MSG::ERROR << " ----  MC VeloHit Associator Not retrieved --- " << endreq;
          return StatusCode::FAILURE;
        }     

      // get cluster for MCHit - using inverse relations table
        typedef MyAsct::InverseType InvTable;
        typedef InvTable::Range InvRange;
        const InvTable* invtable = associator->inverse();
        if (0==invtable){ return StatusCode::FAILURE;}

	InvRange range3 = invtable->relations(*itmc);
        if( range3.empty()){
	  m_VCEfficiencyN->fill((*itmc)->energy()/eV);
          m_VCEfficiencyNP->fill(pmom/GeV);
          m_VCEfficiencyNXY->fill((*itmc)->entry().x()/cm,(*itmc)->entry().y()/cm,1.0);
          m_VCEfficiencyNRZ->fill((*itmc)->entry().z()/cm,sqrt((*itmc)->entry().x()*(*itmc)->entry().x() + 
    +(*itmc)->entry().y()*(*itmc)->entry().y())/cm,1.0);
        }
	else
	  {
	    m_VCEfficiencyT->fill((*itmc)->energy()/eV);
	    m_VCEfficiencyTP->fill(pmom/GeV);
	    
	    m_VCEfficiencyTXY->fill((*itmc)->entry().x()/cm,(*itmc)->entry().y()/cm,1.0);
          m_VCEfficiencyTRZ->fill((*itmc)->entry().z()/cm,sqrt((*itmc)->entry().x()*(*itmc)->entry().x() + 
    +(*itmc)->entry().y()*(*itmc)->entry().y())/cm,1.0);
	  }
      }
  }  

  return StatusCode::SUCCESS;
}

StatusCode VeloMonitor::associators() {
   // check MC truth associators
  MsgStream log(messageService(), name());
  
  log << MSG::INFO << "testing associators" <<endreq;

  // get clusters
  SmartDataPtr<VeloClusters> clusters(eventSvc(), VeloClusterLocation::Default );
  if ( 0== clusters ) { 
    log << MSG::ERROR << " ----  No VeloCluster container retrieved --- " << endreq;
    return StatusCode::FAILURE;
  }

  ///////////////////////////////////
  // Cluster to MCVelo Hit Associator
  ///////////////////////////////////
  VeloClusters::iterator itcv;
  for (itcv=clusters->begin(); itcv!=clusters->end(); itcv++) {
    log << MSG::DEBUG << "cluster loop" <<endreq;
     

     // get MCHit for cluster - using relations table
    typedef IAssociatorWeighted<VeloCluster,MCVeloHit,double> MyAsct;
    typedef MyAsct::DirectType Table;
    typedef Table::Range Range;
    typedef Table::iterator iterator;
 
    const std::string Type = "VeloCluster2MCHitAsct";
    const std::string Name="VeloCluster2MCHit";
    MyAsct* associator = 0;
    StatusCode sc = toolSvc()->retrieveTool (Type, Name, associator);
    if ( sc.isFailure() ) { 
      log << MSG::ERROR << " ----  MC VeloHit Associator Not retrieved --- " << endreq;
      return StatusCode::FAILURE;
    }     
    const Table* table = associator->direct();
    if (0==table){ return StatusCode::FAILURE;}

    Range range1 = table->relations(*itcv);
    log << MSG::INFO << " MCVeloHIt associator Number of relations is " << range1.size() << endreq;
       
    for (iterator relation =
	   range1.begin(); range1.end() != relation ; ++relation)
      {
	MCVeloHit * hit = relation->to ();
	double w = relation->weight ();
	log << MSG::INFO << "MCVeloHit found from relation with weight " << w <<  " hit x/y/z " << (hit->entry()).x()/cm << 
	  " " << (hit->entry()).y()/cm << 
	  " " << (hit->entry()).z()/cm << endreq;
      }

    /////////////////////
    //MCParticles associator
    /////////////////////
    typedef IAssociatorWeighted<VeloCluster,MCParticle,double> MyPAsct;
    typedef MyPAsct::DirectType PTable;
    typedef PTable::Range PRange;
    typedef PTable::iterator Piterator;
    
    const std::string PType = "VeloCluster2MCParticleAsct";
    const std::string PName= "VeloCluster2MCParticle";
    MyPAsct* PAssociator = 0;
    StatusCode Psc = toolSvc()->retrieveTool (PType,PName, PAssociator);
    if ( Psc.isFailure() ) { 
      log << MSG::ERROR << " ----  MCParticle Associator Not retrieved --- " << endreq;
      return StatusCode::FAILURE;
    }  
    
    const PTable* Ptable = PAssociator->direct();

    if (0==Ptable){ return StatusCode::FAILURE;}

    PRange Prange = Ptable->relations(*itcv);
    log << MSG::INFO << "MCParticle associator number of relations is " << Prange.size() << endreq;

    for (Piterator Prelation =
	   Prange.begin(); Prange.end() != Prelation ; ++Prelation)
      {
	MCParticle * particle = Prelation->to ();
	double w = Prelation->weight ();
	HepLorentzVector fmom = particle->momentum();
	log << MSG::INFO << "MCParticle found from relation with weight "
	    << w <<  " particle px/py/pz "  
	    <<  "p_x " << fmom.px()/GeV 
	    << " p_y " << fmom.py()/GeV 
	    << " p_z " << fmom.pz()/GeV 
	    << " E "   << fmom.e()/GeV << endreq;
      }


  }

  return StatusCode::SUCCESS;
}


StatusCode VeloMonitor::detElement(){
  MsgStream log(messageService(), name());

  // test r detector
  {
  int sensor=40;
  double z=m_velo->zSensor(sensor);
  //  double y=0.;
  double pitch;

  log << MSG::INFO << m_velo->nbStrips() << endreq;

  for(int strip=0; strip < (m_velo->nbStrips()); strip++){
    double dstrip =(double) strip;
    int zone;
    double r=m_velo->rOfStrip(dstrip,zone);
    double phi=0.;
    if (zone==0) phi = +1.04;
    if (zone==1) phi = +0.52;
    if (zone==2) phi = +0.79;
    if (zone==3) phi = -0.52;
    if (zone==4) phi = -1.04;
    if (zone==5) phi = -0.79;

    //    int zone=1;
    //   double dstrip = double(strip);
    //   double r=m_velo->rOfStrip( dstrip, zone );
    double x=r*cos(phi);
    double y=r*sin(phi);
    m_RvsStrip->fill(r/cm,strip);
    HepPoint3D point(-x,y,z);
    //    bool valid;
    double striprtn=m_velo->stripNumber(sensor, point, pitch);
    bool valid; double fraction;
    VeloChannelID entryChan=m_velo->channelID(point,fraction,pitch,valid);
    log << MSG::INFO << "RDet: input strip number " << strip 
        << " output strip number " << striprtn << " " << entryChan.strip() << " zone " << zone << " phi " << phi << " r " << r << endreq; 
  }
  }

//   // test phi detector
  {
  int sensor=38;
  double z=m_velo->zSensor(sensor);
  //  double y=0.;
  double pitch;

  log << MSG::INFO << m_velo->nbStrips() << endreq;

  for(int strip=0; strip < (m_velo->nbStrips()); strip++){
    double dstrip = double(strip);
    double radius = (strip<=681) ? 1.0*cm : 2.0*cm;
    double phi= m_velo->phiOfStrip ( dstrip, radius, sensor );
    m_PhivsStrip->fill(phi,strip);
    double x=radius*cos(phi);
    double y=radius*sin(phi);
    HepPoint3D point(x,y,z);
    //    bool valid;
    double striprtn=m_velo->stripNumber(sensor, point, pitch);
    log << MSG::INFO << "PhiDet: input strip number " << strip 
        << " output strip number " << striprtn << "  " << phi 
        << " "<< x << " " << y << " " << z << endreq; 
  }
  }
  // plot phi vs r

     int sensor=35;
  for (int iline=0; iline<11;iline++) {
      double dstrip=0.;
      switch (iline) {
        case 0: dstrip=0.;
	    break;
        case 1: dstrip=150.;
	    break;
        case 2: dstrip=340.;
	    break;
        case 3: dstrip=490.;
            break;
        case 4: dstrip=681.;
            break;
        case 5: dstrip=682.;
            break;
        case 6: dstrip=982.;
	    break;
        case 7: dstrip=1282.;
	    break;
        case 8: dstrip=1582.;
            break;
        case 9: dstrip=1882.;
            break;
        case 10: dstrip=2047.;   
            break;
      }

  for (int ipt=0;ipt<100;ipt++){
      double inner= (iline <5)? 0.8:1.72;
      double outer= (iline <5)? 1.72:4.0;
      double radius=(inner+ (outer-inner)*(ipt/100.))*cm;
      double phi= m_velo->phiOfStrip ( dstrip, radius, sensor );
      double x=radius*cos(phi)/cm;
      double y=radius*sin(phi)/cm;
      //      cout << "edge:phi " << phi << " r " << radius << " x " << x 
      //           << " y " << y << endl;  
      m_PhiPlot->fill(x,y);
  }

  }
 return StatusCode::SUCCESS;
}


StatusCode VeloMonitor::random() {
  //test 1/E^2 form of rndm number generation

  static bool first=true;
  if (first){
    double Tmin=1000.;
    double Tmax=10000.;
    for (int icnt=0; icnt<10000; icnt++){
      // generate delta ray energy 
      // dN/DE=k*1/E^2 for relativistic incident particle
      // E(r)=1/r, where r is uniform in range 1/Tmin < r < 1/Tmax
      // but Tmax bounded by energy left to allocate, so following is
      // not truly correct   
      double range=((1./Tmin) - (1./Tmax));
      double offset=1./Tmax;  
      double uniform = m_uniformDist()*range+offset;
      double charge=1./uniform;
      m_randomTest->fill(charge);
    }
    first=false;
  }
  return StatusCode::SUCCESS;
}

StatusCode VeloMonitor::resolution() {

  // routine to plot cluster resolution
  MsgStream log(messageService(), name());
  
  log << MSG::DEBUG << "resolution" <<endreq;

  // get clusters
  SmartDataPtr<VeloClusters> clusters(eventSvc(), VeloClusterLocation::Default );
  if ( 0== clusters ) { 
    log << MSG::ERROR << " ----  No VeloCluster container retrieved --- " << endreq;
    return StatusCode::FAILURE;
  }

  VeloClusters::iterator itcv;
  for (itcv=clusters->begin(); itcv!=clusters->end(); itcv++) {
    log << MSG::DEBUG << "cluster loop" <<endreq;
     

    // calculate weighted centre of cluster
    double  fractionCentW;
    VeloChannelID IntCentW = weightedMean((*itcv), fractionCentW);    

    // get MCHit for cluster - using relations table
    typedef IAssociatorWeighted<VeloCluster,MCVeloHit,double> MyAsct;
    typedef MyAsct::DirectType Table;
    typedef Table::Range Range;
    typedef Table::iterator iterator;
 
    const std::string Type = "VeloCluster2MCHitAsct";
    const std::string Name="VeloCluster2MCHit";
    MyAsct* associator = 0;
    StatusCode sc = toolSvc()->retrieveTool (Type, Name, associator);
    if ( sc.isFailure() ) { 
      log << MSG::ERROR << " ----  MCHit Associator Not retrieved --- " << endreq;
      return StatusCode::FAILURE;
    }     
    const Table* table = associator->direct();
    if (0==table){ return StatusCode::FAILURE;}

    Range range1 = table->relations(*itcv);
    for (iterator relation=range1.begin(); relation != range1.end(); relation++) {
      // loop over relations
      MCVeloHit * hit = relation->to ();
      // calculate distance between point and cluster
      HepPoint3D point=hit->entry()+hit->exit();
      point/=2.;
      bool valid;
      double pitch=0.,fraction=0.;       
      VeloChannelID entryChan=m_velo->channelID(point,fraction,pitch,valid); //calculate point on path
      int IntChanDist=m_velo->neighbour(entryChan,IntCentW,valid);
      double distance=IntChanDist+(fractionCentW-fraction);
      //       cout << "fraction " << fraction << " fractionCentW " << fractionCentW << " fraction diff " << (fraction-fractionCentW);
      log << MSG::INFO << valid << " distance in strips " << distance << " in microns " << distance*pitch/micron << endreq;
      HepLorentzVector FMom = hit->mcParticle()->momentum();
      // fill resolution histograms
      if (m_velo->isRSensor((*itcv)->sensor())&&valid){
	//          m_RRes->fill(distance*pitch/micron,1.0);
         
	// res vs angle,40um pitch
	//          cout << "pitch " << pitch/micron << " e " << FMom.e()/GeV << endl;
	if (((FMom.e()/GeV)>2.)&&((pitch/micron)>39.9)&&((pitch/micron)<40.1)){
	  //cout << "fill" << endl;
	  m_RRes->fill(distance*pitch/micron,1.0);
	  if ((*itcv)->size()==1) m_RRes1s->fill(distance*pitch/micron,1.0);
	  if ((*itcv)->size()==2) m_RRes2s->fill(distance*pitch/micron,1.0);
	  if ((*itcv)->size()==3) m_RRes3s->fill(distance*pitch/micron,1.0);
	  m_RCluster->fill((*itcv)->size(),1.0);             
	  m_RResd->fill(distance,1.0);
	  double theta=FMom.theta();
	  double pi=3.14159;
	  if (theta> pi/2.) theta-=pi;
	  m_RResAngle->fill(distance*pitch/micron,theta,1.0);
	  m_RAngle->fill(theta,1.0);
	  if ((*itcv)->size()==1) m_RAngle1s->fill(theta,1.0);
	  if ((*itcv)->size()==2) m_RAngle2s->fill(theta,1.0);
	  if ((*itcv)->size()==3) m_RAngle2s->fill(theta,1.0);
	}
      }

      if ((!m_velo->isRSensor((*itcv)->sensor()))&&valid){ 
	m_PhiRes->fill(distance*pitch/micron,1.0);
	m_PhiResd->fill(distance,1.0);
      }
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode VeloMonitor::finalize() {
  
  MsgStream log(messageService(), name());
  log << MSG::INFO << ">>>> finalize" << endreq;

  return StatusCode::SUCCESS;
}









