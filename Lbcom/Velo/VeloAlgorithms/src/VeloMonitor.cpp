// Include files 
#include "VeloMonitor.h"

//STL
#include <vector>
// CLHEP
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Geometry/Point3D.h"
// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "Event/MCParticle.h"
#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/SmartRefVector.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "Relations/IAssociatorWeighted.h"
#include "GaudiKernel/IToolSvc.h"
#include "Kernel/LHCbMath.h"

//- Velo classes ----------------------------------------------
#include "Event/MCVeloHit.h"
#include "Event/MCVeloFE.h"
#include "Event/VeloFullDigit.h"
#include "Event/VeloCluster.h"
#include "Kernel/VeloChannelID.h"
#include "VeloDet/DeVelo.h"
#include "VeloKernel/VeloSimParams.h"
#include "VeloDet/DeVelo.h"
//--------------------------------------------------------------------

//--------------------------------------------------------------------

//--------------------------------------------------------------------
//	VeloMonitor: algorithm to produce some standard ouput
//                   for checking purposes. 
//
// Writes out some info. on MCVeloHits, MCVeloFEs, VeloFullDigits, VeloClusters
//
//	Authors :   Chris Parkes, Juan Palacios
//
//--------------------------------------------------------------------

// static data/functions can be shared by all instances of class.
static const AlgFactory<VeloMonitor>    Factory;
const IAlgFactory& VeloMonitorFactory = Factory;


// Algorithm parameters which can be set at run time must be declared.
// This should be done in the constructor.

VeloMonitor::VeloMonitor(const std::string& name, ISvcLocator* pSvcLocator) 
  : GaudiAlgorithm(name, pSvcLocator),
    m_printout (0),
    m_detailedMonitor   ( false ),
    m_testMCVeloHit     ( true  ),
    m_testPUMCVeloHit   ( true  ),
    m_testMCVeloFE      ( true  ),
    m_testVeloFullDigit ( true  ),
    m_testVeloCluster   ( true  ),
    m_resolution        ( false ),
    m_detElement        ( false ),
    m_drawSensors       ( false )
{
  declareProperty( "DetailedMonitor"         ,m_detailedMonitor );
  declareProperty( "TestMCVeloHit"           ,m_testMCVeloHit );
  declareProperty( "TestPUMCVeloHit"         ,m_testPUMCVeloHit );
  declareProperty( "TestMCVeloFE"            ,m_testMCVeloFE );
  declareProperty( "TestVeloFullDigit"       ,m_testVeloFullDigit );
  declareProperty( "TestVeloCluster"         ,m_testVeloCluster );
  declareProperty( "Resolution"              ,m_resolution );
  declareProperty( "Printout"                ,m_printout);
  declareProperty( "DetElement"              ,m_detElement);
  declareProperty( "DrawSensors"             ,m_drawSensors);

  Rndm::Numbers m_uniformDist;
 
}


StatusCode VeloMonitor::initialize() {
  
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << ">>> Initialize" << endmsg;

  m_NEvent=0;

  m_nMCVH2=0;
  m_nMCPH=0;
  m_nMCPH2=0;
  m_nMCFE=0;
  m_nMCFE2=0;
  m_nFD=0;
  m_nFD2=0;
  m_nVC=0;
  m_nVC2=0;
  m_nMCFEs=0;
  m_nMCFEn=0;
  m_nMCFEo=0;
  m_nVCs=0;
  m_nVCn=0;
  m_nVCo=0;
  m_nMCVH=0;
   
  m_nMCFDs=0;
  m_MCFD=0;
  m_nMCFDn=0;
  m_nMCFDo=0;
  m_nMCVCs=0;
  m_MCVC=0;
  m_nMCVCn=0;
  m_nMCVCo=0;
  m_nMCFD=0;
  m_nMCFD2=0;
  m_nMCVC=0;
  m_nMCVC2=0;

  bookHistograms();
  detElement();
  return StatusCode::SUCCESS;

}

StatusCode VeloMonitor::execute() {
  
  bool isDebug   = msgLevel( MSG::DEBUG   );
  if(isDebug) debug() << ">>> execute" << endmsg;

  m_NEvent++;

  if ( checkTests() ) {
    if(isDebug) debug() << ">>> Retrieve data" << endmsg;
    retrieveData();
    basicMonitor();
    if (m_detailedMonitor) {
      if (m_testMCVeloHit) testMCVeloHit();
      if (m_testPUMCVeloHit) testPileUpMCVeloHit();
      if (m_testMCVeloFE) testMCVeloFE();
      if (m_testVeloFullDigit) testVeloFullDigit();
      if (m_testVeloCluster) {
        testVeloCluster();
        if (m_resolution)  resolution(); // determine cluster resolution
      }
      if(m_detElement) {
        testDetElement();
        m_detElement = false;
      }
    }
  } else {
    error() << ">>> Skipping monitor event loop" << endmsg;
  }
  if(isDebug) debug() << ">>> execute complete" << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode VeloMonitor::finalize() {
  

  m_nMCFDs/=m_MCFD;  m_nMCFDn/=m_MCFD; m_nMCFDo/=m_MCFD; 
  m_nMCVCs/=m_MCVC;  m_nMCVCn/=m_MCVC; m_nMCVCo/=m_MCVC;

  m_nMCVH/=m_NEvent; m_nMCVH2/=m_NEvent; 
  double errnMCVH=sqrt(m_nMCVH2-m_nMCVH);
  m_nMCPH/=m_NEvent; m_nMCPH2/=m_NEvent; 
  double errnMCPH=sqrt(m_nMCPH2-m_nMCPH);
  m_nMCFE/=m_NEvent; m_nMCFE2/=m_NEvent; 
  double errnMCFE=sqrt(m_nMCFE2-m_nMCFE);
  m_nFD/=m_NEvent; m_nFD2/=m_NEvent; double errnFD=sqrt(m_nFD2-m_nFD);
  m_nVC/=m_NEvent; m_nVC2/=m_NEvent; double errnVC=sqrt(m_nVC2-m_nVC);

  info() << "-----------------------------------------------------------------"
	 << endmsg;
  info() << "       Velo Monitoring Table " << endmsg;
  info() << "-----------------------------------------------------------------"
	 << endmsg;
  info() << "Number of Velo MCVeloHits   / Event " << m_nMCVH <<"+/-" 
	 << errnMCVH
	 << endmsg;
  info() << "Number of Pileup MCVeloHits / Event " << m_nMCPH <<"+/-" 
	 << errnMCPH
	 << endmsg;
  info() << "Number of MCVeloFEs         / Event " << m_nMCFE <<"+/-" 
	 << errnMCFE
	 << endmsg;
  if ((m_nMCFEs+m_nMCFEn+m_nMCFEo)>0) {
    info() << "FES from signal " << m_nMCFEs/(m_nMCFEs+m_nMCFEn+m_nMCFEo) 
	   << "%\n"
	   << "                                    noise " 
	   << m_nMCFEn/(m_nMCFEs+m_nMCFEn+m_nMCFEo) << "%\n"
	   << "                     spillover / coupling " 
	   << m_nMCFEo/(m_nMCFEs+m_nMCFEn+m_nMCFEo) << "%"<< endmsg; 
  } else {
    info() << "No FEs found" << endmsg;
  }
  info() << "Number of VeloFullDigits    / Event " << m_nFD <<"+/-" << errnFD
	 << endmsg;
  info() << "Number of VeloClusters      / Event " << m_nVC <<"+/-" << errnVC
	 << endmsg;
  
  info() << "-----------------------------------------------------------------"
	 << endmsg;
  
  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

StatusCode VeloMonitor::retrieveData() {
  bool isDebug   = msgLevel( MSG::DEBUG   );

  if (m_testMCVeloHit) {
    if(isDebug) debug() << "Retrieving " 
			<< MCVeloHitLocation::Default << endmsg;
    if ( !exist<MCVeloHits>(MCVeloHitLocation::Default ) ) { 
      error() << " ----  No MCVeloHits container retrieved --- "
	      << endmsg;
      m_testMCVeloHit=false;
    }else{
      m_mchits = get<MCVeloHits>(MCVeloHitLocation::Default );
    }
  }

  if (m_testPUMCVeloHit) {
    if(isDebug) debug() << "Retrieving " 
			<< MCVeloHitLocation::PuVeto << endmsg;
    if ( !exist<MCVeloHits>(MCVeloHitLocation::PuVeto ) ) { 
      error() <<" ----  No PileUp MCVeloHits container retrieved --- "
	      << endmsg;
      m_testPUMCVeloHit=false;
    }else{
      m_pumchits=get<MCVeloHits>(MCVeloHitLocation::PuVeto);
    }
  }

  if (m_testMCVeloFE||m_testVeloCluster) {
    if(isDebug) debug() << "Retrieving " << MCVeloFELocation::Default 
			<< endmsg;
    if ( !exist<MCVeloFEs>(MCVeloFELocation::Default) ) { 
      error() << " ----  No MCVeloFEs container retrieved --- "
	      << endmsg;
      m_testMCVeloFE=false;
    }else{
      m_mcfes=get<MCVeloFEs>(MCVeloFELocation::Default);
    }
  }
  if (m_testVeloFullDigit) {
    if(isDebug) debug() << "Retrieving " << VeloFullDigitLocation::Default
			<< endmsg;
    if ( !exist<VeloFullDigits>(VeloFullDigitLocation::Default) ){
      error() << " ----  No VeloFullDigits container retrieved --- "
	      << endmsg;
      m_testVeloFullDigit=false;
    }else{
      m_digits=get<VeloFullDigits>(VeloFullDigitLocation::Default);
    }
  }
  if (m_testVeloCluster) {
    if(isDebug) debug() << "Retrieving " << VeloClusterLocation::Default 
			<<endmsg;
    if ( !exist<VeloClusters>(VeloClusterLocation::Default ) ) { 
      error() << " ----  No VeloClusters container retrieved --- "
	      << endmsg;
      m_testVeloCluster=false;
    }else{
    m_clusters=get<VeloClusters>(VeloClusterLocation::Default );
    }
  }
  
  if (m_printout>m_NEvent){
    info() << "***************************************************"
        << endmsg;
    if (m_testMCVeloHit) 
      info() << "********* Number of MCVeloHits found        " 
          << m_mchits->size() << "**********" << endmsg;
    if (m_testPUMCVeloHit) 
      info() << "********* Number of PileUp MCVeloHits found "
          << m_pumchits->size() << "**********" << endmsg;
    if (m_testMCVeloFE) 
      info() << "********* Number of MCVeloFEs found         "
          << m_mcfes->size() << "**********" << endmsg;
    if (m_testVeloFullDigit)
      info() << "********* Number of VeloFullDigits found    "
          << m_digits->size() << "**********" << endmsg;
    if (m_testVeloCluster)
      info() << "********* Number of VeloClusters found      "
          << m_clusters->size() << "**********" << endmsg;
    info() << "***************************************************"
        << endmsg;
  }
  return StatusCode::SUCCESS;
}

StatusCode VeloMonitor::bookHistograms() {

  if (m_testMCVeloHit) {
    // MCVeloHit
    m_MCVHNHits = histoSvc()->
      book("velo/1", "Number of MCVeloHits per event", 100, 0., 3000.);
    if (m_detailedMonitor) {
      m_MCVHEntryZX = histoSvc()->
	book("velo/4", "Particle entry point in Si ZX (cm)",
	     1000,-20.,80.,50,-10.,10.);
      m_MCVHEnergy = histoSvc()->
	book("velo/2", "Energy deposited in Si (eV)", 100, 0., 300000.);
      m_Thomas1 = histoSvc()->
	book("velo/1001", "Energy deposited in Si (MeV) Z=20cm", 1000, 0., 10.);
      m_Thomas2 = histoSvc()->
	book("velo/1002", "Energy deposited in Si (MeV) Z=70cm", 1000, 0., 10.);
      m_MCVHEntryXY = histoSvc()->
	book("velo/3", "Particle entry point in Si XY (cm)", 50, -10,10.,50,-10.,10.);
      m_MCVHExitXY = histoSvc()->
	book("velo/5", "Particle exit point in Si XY (cm)", 50, -10, 10.,50,-10.,10.);
      m_MCVHExitZX = histoSvc()->
	book("velo/6", "Particle exit point in Si ZX (cm)",1000,-20.,80.,50,-10.,10.);
      m_MCVHSensorZ = histoSvc()->
	book("velo/7", "Z position and Sensor number (cm)",1000,-20.,80.,200,0.,100.);
      m_MCVHTOF = histoSvc()->
	book("velo/8", "Time Of Flight (nS)", 100, 0., 5.);
      m_MCPEnergy = histoSvc()->
	book("velo/9", "Particle Energy (GeV)", 100, 0., 50.);
      // Store plots of hits for each type of sensor
      m_MCVHRRighXY = histoSvc()->
        book("velo/20", "Particle entry point in RRigh XY (cm)", 
             50, -10,10.,50,-10.,10.);
      m_MCVHRLeftXY = histoSvc()->
        book("velo/21", "Particle entry point in RLeft XY (cm)", 
             50, -10,10.,50,-10.,10.);
      m_MCVHPhiURXY = histoSvc()->
        book("velo/22", "Particle entry point in PhiUR XY (cm)", 
             50, -10,10.,50,-10.,10.);
      m_MCVHPhiDRXY = histoSvc()->
        book("velo/23", "Particle entry point in PhiDR XY (cm)", 
             50, -10,10.,50,-10.,10.);
      m_MCVHPhiULXY = histoSvc()->
        book("velo/24", "Particle entry point in PhiUL XY (cm)", 
             50, -10,10.,50,-10.,10.);
      m_MCVHPhiDLXY = histoSvc()->
        book("velo/25", "Particle entry point in PhiDR XY (cm)", 
             50, -10,10.,50,-10.,10.);
      m_MCVHRLeftZX = histoSvc()->
        book("velo/26", "Particle entry point in RLeft ZX (cm)",
             1000,-20.,80.,50,-10.,10.);
      m_MCVHRRighZX = histoSvc()->
        book("velo/27", "Particle entry point in RRigh ZX (cm)",
             1000,-20.,80.,50,-10.,10.);
    }
  }
  if (m_testMCVeloHit) {
    // pileup MCVeloHit
    m_MCVHPNHits = histoSvc()->
      book("velo/51", "PU:Number of MCVeloHits per event", 100, 0., 2000.);
    if (m_detailedMonitor) {
      m_MCVHPEntryZX = histoSvc()->
	book("velo/54", "PU:Particle entry point in Si ZX (cm)",
	     1000, -20., 80.,50,-10.,10.);
      m_MCVHPEnergy = histoSvc()->
	book("velo/52", "PU:Energy deposited in Si (eV)", 100, 0., 300000.);
      m_MCVHPEntryXY = histoSvc()->
	book("velo/53", "PU:Particle entry point in Si XY (cm)",
	     50, -10, 10.,50,-10.,10.);
      m_MCVHPExitXY = histoSvc()->
	book("velo/55", "PU:Particle exit point in Si XY (cm)",
	     50, -10, 10.,50,-10.,10.);
      m_MCVHPExitZX = histoSvc()->
	book("velo/56", "PU:Particle exit point in Si ZX (cm)", 
	     1000, -20., 80.,50,-10.,10.);
      m_MCVHPSensorZ = histoSvc()->
	book("velo/57", "PU:Z position and Sensor number (cm)", 
	     1000, -20., 80.,200,0.,100.);
      m_MCVHPTOF = histoSvc()->
	book("velo/58", "PU:Time Of Flight (nS)", 100, 0., 5.);
      m_MCPUPEnergy = histoSvc()->
	book("velo/59", "PU:Energy (eV)", 100, 0., 50.);
    }
  }

  if (m_testMCVeloFE && m_detailedMonitor) {
    // MCVeloFE
    m_MCVFEMCHits = histoSvc()->
      book("velo/101", "Number of MC hits per FE", 6, -0.5, 5.5);
    m_MCVFENHits = histoSvc()->
      book("velo/102", "Number of MCVeloFEs per event", 16, 0., 8000.);
    m_MCVFECharge = histoSvc()->
      book("velo/103", "Charge in Si strip (electrons)", 100, -10000., 50000.);
    m_MCVFECharge_s = histoSvc()->
      book("velo/104", "signal dominated -  Charge in Si strip (electrons)", 
           100, -10000., 50000.);
    m_MCVFECharge_n = histoSvc()->
      book("velo/105", "noise dominated - Charge in Si strip (electrons)",
           100, -10000., 50000.);
    m_MCVFECharge_o = histoSvc()->
      book("velo/106", "other dominated - Charge in Si strip (electrons)",
           100, -10000., 50000.);
    m_MCVFESignal = histoSvc()->
      book("velo/107", "Signal deposited in Si strip (electrons)",
           100, 0., 50000.);
    m_MCVFESignal_s = histoSvc()->
      book("velo/108", 
           "signal dominated - Signal deposited in Si strip (electrons)",
           100, 0., 50000.);
    m_MCVFESignal_n = histoSvc()->
      book("velo/109",
           "noise dominated - Signal deposited in Si strip (electrons)",
           100, 0., 50000.);
    m_MCVFESignal_o = histoSvc()->
      book("velo/110",
           "other dominated - Signal deposited in Si strip (electrons)",
           100, 0., 50000.);
    m_MCVFENoise = histoSvc()->
      book("velo/111", "Noise added in Si strip (electrons)",
           100, -10000., 10000.);
    m_MCVFENoise_s = histoSvc()->
      book("velo/112", "signal dominated - Noise added in Si strip (electrons)",
           100, -10000., 10000.);
    m_MCVFENoise_n = histoSvc()->
      book("velo/113", "noise dominated - Noise added in Si strip (electrons)",
           100, -10000., 10000.);
    m_MCVFENoise_o = histoSvc()->
      book("velo/114", "other dominated - Noise added in Si strip (electrons)",
           100, -10000., 10000.);
    m_MCVFECMNoise = histoSvc()->
      book("velo/115", "CM noise added in Si strip (electrons)",
           100, -20000., 20000.); 
    m_MCVFEPedestal = histoSvc()->
      book("velo/116", "Pedestal added in Si strip (electrons)",
           100, 0., 50000.);
    m_MCVFESensorStrip = histoSvc()->
      book("velo/117", "Sensor and strip number", 100, 0., 100.,50,0.,5000.);
    m_MCVFESensorStrip_s = histoSvc()->
      book("velo/118", "signal dominated - Sensor and strip number",
           100, 0., 100.,50,0.,5000.);
    m_MCVFESensorStrip_n = histoSvc()->
      book("velo/119", "noise dominated - Sensor and strip number",
           100, 0., 100.,50,0.,5000.);
    m_MCVFESensorStrip_o = histoSvc()->
      book("velo/120", "other dominated -  Sensor and strip number",
           100, 0., 100.,50,0.,5000.);
    m_MCVFEMCHitsSignal= histoSvc()->
      book("velo/121", "MC hit signal deposited in FE", 100, 0., 50000);
    m_MCVFEMCHitsTotalSignal= histoSvc()->
      book("velo/122", "MC total signal deposited in FE", 100, 0., 50000);
    m_MCVFEMCHitsTotalSignal_s= histoSvc()->
      book("velo/123", "signal dominated - MC total signal deposited in FE",
           100, 0., 50000);
    m_MCVFEMCHitsTotalSignal_n= histoSvc()->
      book("velo/124", "noise dominated -  MC total signal deposited in FE",
           100, 0., 50000);
    m_MCVFEMCHitsTotalSignal_o= histoSvc()->
      book("velo/125", "other dominated - MC total signal deposited in FE",
           100, 0., 50000);
    m_MCVFEZR0 = histoSvc()->
      book("velo/130", "MCVeloFE R position vs. Z (cm) Zone 0",
           1000,-20., 80.,50,0.,5.);
    m_MCVFEZR1 = histoSvc()->
      book("velo/131", "MCVeloFE R position vs. Z (cm) Zone 1",
           1000,-20., 80.,50,0.,5.);
    m_MCVFEZR2 = histoSvc()->
      book("velo/132", "MCVeloFE R position vs. Z (cm) Zone 2",
           1000,-20., 80.,50,0.,5.);
    m_MCVFEZR3 = histoSvc()->
      book("velo/133", "MCVeloFE R position vs. Z (cm) Zone 3",
           1000,-20., 80.,50,0.,5.);
    m_MCVFEZPhiInner = histoSvc()->
      book("velo/134", "MCVeloFE Phi position vs. Z (cm) Inner",
           1000, -20., 80.,60,-180.,180.);
    m_MCVFEZPhiOuter = histoSvc()->
      book("velo/135", "MCVeloFE Phi position vs. Z (cm) Outer",
           1000, -20., 80.,60,-180.,180.);
    m_MCVFEZPhiDirecInner = histoSvc()->
      book("velo/136", "MCVeloFE Phi direction vs. Z (cm) Inner",
           1000, -20., 80.,60,-180.,360.);
    m_MCVFEZPhiDirecOuter = histoSvc()->
      book("velo/137", "MCVeloFE Phi direction vs. Z (cm) Outer",
           1000, -20., 80.,60,-180.,360.);
    m_MCVFEZTrgPhiInner = histoSvc()->
      book("velo/138", "MCVeloFE Phi (trg) position vs. Z (cm) Inner",
           1000, -20., 80.,60,-180.,180.);
    m_MCVFEZTrgPhiOuter = histoSvc()->
      book("velo/139", "MCVeloFE Phi (trg) position vs. Z (cm) Outer",
           1000, -20., 80.,60,-180.,180.);
    m_MCVFEZTrgPhiDirecInner = histoSvc()->
      book("velo/140", "MCVeloFE Phi direction (trg) vs. Z (cm) Inner",
           1000, -20., 80.,60,-180.,360.);
    m_MCVFEZTrgPhiDirecOuter = histoSvc()->
      book("velo/141", "MCVeloFE Phi direction (trg) vs. Z (cm) Outer",
           1000, -20., 80.,60,-180.,360.);
  }
  if (m_testVeloFullDigit && m_detailedMonitor) {
    // VeloFullDigit
    m_VDNHits = histoSvc()->
      book("velo/201", "Number of VeloFullDigits per event", 100, 0., 2000.);
  
    m_VDADC  = histoSvc()->
      book("velo/202", "ADC counts after correction", 256, 0., 256.);

    m_VDRawADC = histoSvc()->
      book("velo/203", "ADC counts", 256, 0., 256.);

    m_VDPedestal = histoSvc()->
      book("velo/204", "subtracted pedestal", 100, 0., 1024.);

    m_VDCMNoise = histoSvc()->
      book("velo/205", "subtracted CM noise", 100, 0., 1024.);

    m_VDNoise = histoSvc()->
      book("velo/206", "noise", 100, 0., 1024.);

    m_VDRawNoise = histoSvc()->
      book("velo/207", "raw noise", 100, 0., 1024.);

    m_VDSensorStrip = histoSvc()->
      book("velo/208", "Sensor and strip number", 100, 0., 100.,50,0.,5000.);
  }
  if (m_testVeloCluster) {
    //Velo Cluster
    m_VCNHits = histoSvc()->
      book("velo/301", "Number of VeloClusters per event", 50, 0., 3000.);
    
    if (m_detailedMonitor) {     
      m_VCNHits_s = histoSvc()->
	book("velo/302", "signal dominated - Number of VeloClusters per event",
	     50, 0., 3000.);
      m_VCNHits_n = histoSvc()->
	book("velo/303", "noise dominated - Number of VeloClusters per event",
	     50, 0., 3000.);
      m_VCNHits_o = histoSvc()->
	book("velo/304", "other dominated - Number of VeloClusters per event",
	     50, 0., 3000.);

      m_VCADCSum = histoSvc()->
	book("velo/305", "ADC sum", 256, -0.5, 255.5);
      m_VCADCSum_s = histoSvc()->
	book("velo/306", "signal dominated - ADC sum", 256, -0.5, 255.5);
      m_VCADCSum_n = histoSvc()->
	book("velo/307", "noise dominated - ADC sum", 256, -0.5, 255.5);
      m_VCADCSum_o = histoSvc()->
	book("velo/308", "other dominated - ADC sum", 256, -0.5, 255.5);

      m_VCSize = histoSvc()->
	book("velo/309", "number of strips in cluster", 6, -0.5, 5.5);
      m_VCSize_s = histoSvc()->
	book("velo/310", "signal dominated - number of strips in cluster",6,-0.5,5.5);
      m_VCSize_n = histoSvc()->
	book("velo/311", "noise dominated - number of strips in cluster",6,-0.5, 5.5);
      m_VCSize_o = histoSvc()->
	book("velo/312", "other dominated - number of strips in cluster",6,-0.5, 5.5);

      m_VCSensorStrip = histoSvc()->
	book("velo/313", "Sensor and first strip number", 100, 0., 100.,50,0.,5000.);
      m_VCZR0 = histoSvc()->
        book("velo/314", "Cluster R position vs. Z (cm) Zone 0",
             1000,-20., 80.,50,0.,5.);
      m_VCZR1 = histoSvc()->
        book("velo/315", "Cluster R position vs. Z (cm) Zone 1",
             1000,-20., 80.,50,0.,5.);
      m_VCZR2 = histoSvc()->
        book("velo/316", "Cluster R position vs. Z (cm) Zone 2",
             1000,-20., 80.,50,0.,5.);
      m_VCZR3 = histoSvc()->
        book("velo/317", "Cluster R position vs. Z (cm) Zone 3",
             1000,-20., 80.,50,0.,5.);
      m_VCZPhiInner = histoSvc()->
        book("velo/318", "Cluster Phi position vs. Z (cm) Inner",
             1000, -20., 80.,60,-180.,180.);
      m_VCZPhiOuter = histoSvc()->
        book("velo/319", "Cluster Phi position vs. Z (cm) Outer",
             1000, -20., 80.,60,-180.,180.);
      m_VCZTrgPhiInner = histoSvc()->
        book("velo/320", "Cluster Phi (trg) position vs. Z (cm) Inner",
             1000, -20., 80.,60,-180.,180.);
      m_VCZTrgPhiOuter = histoSvc()->
        book("velo/321", "Cluster Phi (trg) position vs. Z (cm) Outer",
             1000, -20., 80.,60,-180.,180.);
    }
  }

  if (m_resolution && m_detailedMonitor) {
    // resolution
    m_RRes = histoSvc()->
      book("velo/401", "R Sensor resolution (microns)", 50, -50., +50.);
    m_RResd = histoSvc()->
      book("velo/405", "R Sensor resolution (strip fraction)", 100, -5., +5.);
    m_RRes1s = histoSvc()->
      book("velo/407", "R Sensor resolution 1 strip cluster (microns)",
           50, -50., +50.);
    m_RRes2s = histoSvc()->
      book("velo/408", "R Sensor resolution 2 strip cluster (microns)",
           50, -50., +50.);
    m_RRes3s = histoSvc()->
      book("velo/410", "R Sensor resolution 3 strip cluster (microns)",
           50, -50., +50.);
    m_RCluster = histoSvc()->
      book("velo/409", "R Sensor N strips in cluster", 6, -0.5, 5.5);

    m_PhiRes = histoSvc()->
      book("velo/402", "Phi Sensor resolution (microns)", 100, -50., +50.);
    m_PhiResd = histoSvc()->
      book("velo/406", "Phi Sensor resolution (strip fraction)", 100, -5., +5.);
    m_RAngle = histoSvc()->
      book("velo/403", "Track theta (radians)", 50, 0., 0.75);
    m_RAngle1s = histoSvc()->
      book("velo/422", "Track theta 1 strip (radians)", 50, 0., 0.75);
    m_RAngle2s = histoSvc()->
      book("velo/423", "Track theta 2 strip (radians)", 50, 0., 0.75);
    m_RAngle3s = histoSvc()->
      book("velo/424", "Track theta 3 strip (radians)", 50, 0., 0.75);

    m_RResAngle= histoSvc()->
      book("velo/404", "R Sensor resolution vs track theta",
           100, -20., +20., 10, -0.0, +0.25);

  }
  if(m_detElement){
    // check geometry
    m_detRRigh = histoSvc()->book("velo/501","RRigh",101,-50.,50.,101,-50.,50.);
    m_detRLeft = histoSvc()->book("velo/502","RLeft",101,-50.,50.,101,-50.,50.);
    m_detPhiUL = histoSvc()->book("velo/503","PhiUL",101,-50.,50.,101,-50.,50.);
    m_detPhiDL = histoSvc()->book("velo/504","PhiDL",101,-50.,50.,101,-50.,50.);
    m_detPhiUR = histoSvc()->book("velo/505","PhiUR",101,-50.,50.,101,-50.,50.);
    m_detPhiDR = histoSvc()->book("velo/506","PhiDR",101,-50.,50.,101,-50.,50.);
    // strip no.
    m_stripRRigh = histoSvc()->book("velo/520","RRigh strip",2048,0.,2047.);
    m_stripRLeft = histoSvc()->book("velo/521","RLeft strip",2048,0.,2047.);
    m_stripPhiUL = histoSvc()->book("velo/522","PhiUL strip",2048,0.,2047.);
    m_stripPhiDL = histoSvc()->book("velo/523","PhiDL strip",2048,0.,2047.);
    m_stripPhiUR = histoSvc()->book("velo/524","PhiUR strip",2048,0.,2047.);
    m_stripPhiDR = histoSvc()->book("velo/525","PhiDR strip",2048,0.,2047.);
    // check residual
    m_residualR   = histoSvc()->book("velo/511","residual R",51,-0.05,0.05);
    m_residualPhi = histoSvc()->book("velo/513","residual Phi",51,-0.05,0.05);
    // check strips radii
    m_rOfStrips = histoSvc()->book("velo/517","R of strips",2048,0.,2047.);
  }
  return StatusCode::SUCCESS;
}
StatusCode VeloMonitor::testMCVeloHit() {
  bool isDebug   = msgLevel( MSG::DEBUG   );

  if(isDebug) debug() << "testMCVeloHit" << endmsg;
  //  Define an iterator to the MC VELO hit vector and loop through it  
  MCVeloHits::iterator it;

  for (it=m_mchits->begin(); it!=m_mchits->end(); it++) {
    // printout some info. for VELO hit
    if (m_printout>m_NEvent){
      info() << "testMCVeloHit: \n"
	  << "    sensor " << (*it)->sensor() 
	  << " \n"
	  << "    entry " << ((*it)->entry()).x()/mm
	  << "mm, "  << ((*it)->entry()).y()/mm
	  << "mm, "  << ((*it)->entry()).z()/mm
	  << "mm. \n" 
	  << "    exit  " << ((*it)->exit()).x()/mm
	  << "mm, " << ((*it)->exit()).y()/mm
	  << "mm, " << ((*it)->exit()).z()/mm
	  << "mm. \n" 
	  << "    energy " << (*it)->energy()/eV
	  << "eV, "
	  << " TOF " << (*it)->timeOfFlight()/ns
	  << "ns." 
	  << endmsg;
    }
    // fill histograms
    m_MCVHEnergy->fill((*it)->energy()/eV,1.0);
    if ( (*it)->sensor()==50 || (*it)->sensor()==51 ) {
      m_Thomas1->fill((*it)->energy()/MeV,1.0);
    }
    if ( (*it)->sensor()==78 || (*it)->sensor()==79 ) {
      m_Thomas2->fill((*it)->energy()/MeV,1.0);
    }
    m_MCVHEntryZX->fill((*it)->entry().z()/cm,(*it)->entry().x()/cm,1.0);
    m_MCVHEntryXY->fill((*it)->entry().x()/cm,(*it)->entry().y()/cm,1.0);
    m_MCVHExitXY->fill((*it)->exit().x()/cm,(*it)->exit().y()/cm,1.0);
    m_MCVHExitZX->fill((*it)->exit().z()/cm,(*it)->exit().x()/cm,1.0);
    m_MCVHSensorZ->fill((*it)->entry().z()/cm,(*it)->sensor(),1.0);
    m_MCVHTOF->fill((*it)->timeOfFlight()/ns,1.0);
    // fill xy plots for each type of sensor
    //    unsigned int sensor=(*it)->sensor();
    unsigned int sensor=m_velo->sensorNumber((*it)->entry());
    double x=(*it)->entry().x()/cm;
    double y=(*it)->entry().y()/cm;
    double z=(*it)->entry().z()/cm;
    if(m_velo->isRightSensor(sensor)){ // +ve x first
      if(m_velo->isRSensor(sensor)){
        m_MCVHRRighXY->fill(x,y,1.0);
        m_MCVHRRighZX->fill(z,x,1.0);
      } else if(m_velo->isPhiSensor(sensor)){
        if(m_velo->isDownstreamSensor(sensor)){
          m_MCVHPhiDRXY->fill(x,y,1.0);
        } else {
          m_MCVHPhiURXY->fill(x,y,1.0);
        }
      }
    } else { // now -ve x
      if(m_velo->isRSensor(sensor)){
        m_MCVHRLeftXY->fill(x,y,1.0);
        m_MCVHRLeftZX->fill(z,x,1.0);
      } else if(m_velo->isPhiSensor(sensor)){
        if(m_velo->isDownstreamSensor(sensor)){
          m_MCVHPhiDLXY->fill(x,y,1.0);
        } else {
          m_MCVHPhiULXY->fill(x,y,1.0);
        }
      }
    }

    // access MCParticle which made the hit, and write out 4-mom
    MCParticle* myMCParticle=(*it)->mcParticle();
    if (0!=myMCParticle){
      //noise, coupling & spillover hits won't have MCParticles
      HepLorentzVector fmom = myMCParticle->momentum();
      m_MCPEnergy->fill(fmom.e()/GeV);

      if (m_printout>m_NEvent){
        info() << "testMCVeloHit:" <<" MCParticle " 
	    <<  "p_x " << fmom.px()/GeV 
	    << " p_y " << fmom.py()/GeV 
	    << " p_z " << fmom.pz()/GeV 
	    << " E "   << fmom.e()/GeV << endmsg;
      }
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode VeloMonitor::testPileUpMCVeloHit() {

  //  Define an iterator to the MC VELO hit vector and loop through it  
  MCVeloHits::iterator it;

  for (it=m_pumchits->begin(); it!=m_pumchits->end(); it++) {
    // printout some info. for VELO hit
    if (m_printout>m_NEvent){
      info() << "test PileUp MCVeloHit:"
	  << "entry " << ((*it)->entry()).x()/cm << 
	" " << ((*it)->entry()).y()/cm << 
	" " << ((*it)->entry()).z()/cm 
	  << " exit " << ((*it)->exit()).x()/cm << 
	" " << ((*it)->exit()).y()/cm << 
	" " << ((*it)->exit()).z()/cm 
	  << " energy (eV)" << (*it)->energy()/eV
	  << " TOF (ns)" << (*it)->timeOfFlight()/ns 
	  << " sensor " << (*it)->sensor() 
	  << endmsg;
    }
    // fill histograms
    m_MCVHPEnergy->fill((*it)->energy()/eV,1.0);
    m_MCVHPEntryZX->fill((*it)->entry().z()/cm,(*it)->entry().x()/cm,1.0);
    m_MCVHPEntryXY->fill((*it)->entry().x()/cm,(*it)->entry().y()/cm,1.0);
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
	info() << "test pile up MCVeloHit:" <<" MCParticle " 
	    <<  "p_x " << fmom.px()/GeV 
	    << " p_y " << fmom.py()/GeV 
	    << " p_z " << fmom.pz()/GeV 
	    << " E "   << fmom.e()/GeV << endmsg;
      }
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode VeloMonitor::testMCVeloFE() {

  bool isDebug   = msgLevel( MSG::DEBUG   );

  m_MCVFENHits->fill(m_mcfes->size(),1.);
  //  Define an iterator to the MC VELO FE vector and loop through it  
  MCVeloFEs::iterator itdv;
  for (itdv=m_mcfes->begin(); itdv!=m_mcfes->end(); itdv++) {
    MCVeloFE* FE = (*itdv);
    bool signal=false, noise=false, other=false;
    FEType(FE,signal,noise,other);
    // printout some info.
    if (m_printout>m_NEvent){
      info() << "testMCVeloFE:"
          << " channel ID " << (*itdv)->channelID()
          << " sensorNumber " << (*itdv)->sensor() 
          << " strip Number " << (*itdv)->strip()
          << " Added Noise " << (*itdv)->addedNoise()
          << " Added CM Noise " << (*itdv)->addedCMNoise()
          << " Added Pedestal " << (*itdv)->addedPedestal()
          << " Added Signal " << (*itdv)->addedSignal()
          << " Charge " << (*itdv)->charge()
          << endmsg;
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
    if (signal) m_MCVFESensorStrip_s->fill( (*itdv)->sensor(),
                                            (*itdv)->strip(),1.0);
    if (noise)  m_MCVFESensorStrip_n->fill( (*itdv)->sensor(),
                                            (*itdv)->strip(),1.0);
    if (other)  m_MCVFESensorStrip_o->fill( (*itdv)->sensor(),
                                            (*itdv)->strip(),1.0);

    // access MCHit(s) from which MC FE was made and write out energy
    SmartRefVector<MCVeloHit> myMCVeloHits=(*itdv)->mcVeloHits();
    SmartRefVector<MCVeloHit>::iterator itmh;
    std::vector<double> myMCVeloHitscharge=(*itdv)->mcVeloHitsCharge();
    std::vector<double>::iterator itch;

    if (m_printout>m_NEvent) {
      info() << "testMCVeloFE:" << "MCVeloHit " <<
        "size " << myMCVeloHits.size() << endmsg; 
    }
    m_MCVFEMCHits->fill(myMCVeloHits.size(),1.);
    if (signal) m_MCVFESensorStrip_s->fill( (*itdv)->sensor(),
                                            (*itdv)->strip(),1.0);
    if (noise)  m_MCVFESensorStrip_n->fill( (*itdv)->sensor(),
                                            (*itdv)->strip(),1.0);
    if (other)  m_MCVFESensorStrip_o->fill( (*itdv)->sensor(),
                                            (*itdv)->strip(),1.0);

    double totalSignal=0.;
    itch=myMCVeloHitscharge.begin();
    for (itmh=myMCVeloHits.begin(); itmh!=myMCVeloHits.end(); itmh++) {
      if (m_printout>m_NEvent) {
        info() << "testMCVeloFE:" << "MCVeloHit " 
            <<" charge in this strip (electrons) " << (*itch) 
            <<" energy (eV) " << (*itmh)->energy()/eV << endmsg;
      }
      totalSignal+=(*itch);
      m_MCVFEMCHitsSignal->fill((*itch),1.);
      itch++;
    }
    m_MCVFEMCHitsTotalSignal->fill(totalSignal,1.);
    if (signal) m_MCVFEMCHitsTotalSignal_s->fill(totalSignal,1.);
    if (noise)  m_MCVFEMCHitsTotalSignal_n->fill(totalSignal,1.);
    if (other)  m_MCVFEMCHitsTotalSignal_o->fill(totalSignal,1.);
    if(m_detailedMonitor) {
      VeloChannelID channel=(*itdv)->channelID();
      double sensorZ=m_velo->zSensor(channel.sensor())/cm;
      if(isDebug) debug() << "Channel " << (*itdv)->channelID() 
			  << " sensor " << channel.sensor() << " " << (*itdv)->sensor() 
			  << " z position " << sensorZ
			  << endreq;
      if (m_velo->isRSensor(channel.sensor())){
        double testRadius=m_velo->rOfStrip(channel);
        unsigned int zone=m_velo->zoneOfStrip(channel);
        if(isDebug) debug() << "sensor " << channel.sensor() 
			    << " strip " << channel.strip()
			    << " zone " << zone
			    << " sensorZ " << sensorZ 
			    << " testRadius " << testRadius 
			    << " testRadius/cm " << testRadius/cm << endmsg;
        if(0 == zone) {
          m_MCVFEZR0->fill(sensorZ, testRadius/cm,1.0);
        } else if(1 == zone) {
          m_MCVFEZR1->fill(sensorZ, testRadius/cm,1.0);
        } else if(2 == zone) {
          m_MCVFEZR2->fill(sensorZ, testRadius/cm,1.0);
        } else if(3 == zone) {
          m_MCVFEZR3->fill(sensorZ, testRadius/cm,1.0);
        }
      } else if(m_velo->isPhiSensor(channel.sensor())) {
        unsigned int zone=m_velo->zoneOfStrip(channel);
        double testRadius=m_velo->rMin(channel.sensor(),zone);
        double testPhi=m_velo->phiOfStrip(channel,testRadius);
        double trgPhi=m_velo->trgPhiOfStrip(channel,testRadius);
        double testPhiDirec=m_velo->angleOfStrip(channel);
        double trgPhiDirec=m_velo->trgPhiDirectionOfStrip(channel);
        if(isDebug) debug() << "sensor " << channel.sensor() 
			    << " type " << (m_velo->type(channel.sensor()))
			    << " strip " << channel.strip()
			    << " sensorZ " << sensorZ 
			    << " testPhi " << testPhi/degree 
			    << " trgPhi " << trgPhi/degree
			    << " testPhiDirec " << testPhiDirec/degree 
			    << " trgPhiDirec " << trgPhiDirec/degree <<endmsg;
        if (0==zone){
          m_MCVFEZPhiInner->fill(sensorZ, testPhi/degree,1.0);
          m_MCVFEZTrgPhiInner->fill(sensorZ, trgPhi/degree,1.0);
          m_MCVFEZPhiDirecInner->fill(sensorZ, testPhiDirec/degree,1.0);
          m_MCVFEZTrgPhiDirecInner->fill(sensorZ, trgPhiDirec/degree,1.0);}
        else {
          m_MCVFEZPhiOuter->fill(sensorZ, testPhi/degree,1.0);
          m_MCVFEZTrgPhiOuter->fill(sensorZ, trgPhi/degree,1.0);
          m_MCVFEZPhiDirecOuter->fill(sensorZ, testPhiDirec/degree,1.0);
          m_MCVFEZTrgPhiDirecOuter->fill(sensorZ, trgPhiDirec/degree,1.0);
        }
      }
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode VeloMonitor::testVeloFullDigit() {

  m_VDNHits->fill(m_digits->size(),1.);

  //  Define an iterator to the VELO digit vector and loop through it  
  VeloFullDigits::iterator itdv;
  for (itdv=m_digits->begin(); itdv!=m_digits->end(); itdv++) {
    // printout some info.
    if (m_printout>m_NEvent){
      info() << "testVeloFullDigit:" 
	  << " sensorNumber " << (*itdv)->sensor() 
	  << " strip Number " << (*itdv)->strip()
	  << " ADCValue " << (*itdv)->adcValue()
	  << " rawADCValue " << (*itdv)->rawADCValue()
	  << " subtractedPedestal " << (*itdv)->subtractedPedestal()
	  << " subtractedCM " << (*itdv)->subtractedCM()
	  << " noise " << (*itdv)->noise()
	  << " rawNoise " << (*itdv)->rawNoise()
	  << endmsg;
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

  bool isDebug   = msgLevel( MSG::DEBUG   );

  //  Define an iterator to the VELO cluster vector and loop through it  
  VeloClusters::iterator itcv;
  int nVeloCluster_s=0, nVeloCluster_n=0, nVeloCluster_o=0;
  for (itcv=m_clusters->begin(); itcv!=m_clusters->end(); itcv++) {
    // cluster from signal or noise or other ?
    bool signal=false, noise=false, other=false;
    VeloCluster* cluster = (*itcv);
    clusterType(cluster,signal,noise,other);

    // printout some info. for the cluster
    if(isDebug) debug() << "testVeloCluster:" 
			<< " sensorNumber " << (*itcv)->sensor() 
			<< " First strip in cluster " << (*itcv)->strip(0)
			<< endmsg;
    // and the strips in the cluster
    int size=(*itcv)->size(); // number of strips
    if (m_printout>m_NEvent){
      for (int i=0;i<size;i++) {
	if(isDebug) debug() << " testVeloCluster:"
			    << " strip " <<   (*itcv)->strip(i) << " / " << size 
			    << " signal " <<  (*itcv)->adcValue(i);
      }
      if(isDebug) debug() << endmsg;
    }

    float adcSum=0.;
    for (int i=0;i<size;i++) {adcSum+=(*itcv)->adcValue(i);}
    m_VCSensorStrip->fill((*itcv)->sensor(),(*itcv)->strip(0));
    m_VCADCSum->fill(adcSum);
    if (signal) {
      m_VCADCSum_s->fill(adcSum);
      nVeloCluster_s++;
    }
    if (noise) {
      m_VCADCSum_n->fill(adcSum);
      nVeloCluster_n++;
    }
    if (other) {
      m_VCADCSum_o->fill(adcSum);
      nVeloCluster_o++;
    }

    m_VCSize->fill(size);
    if (signal) m_VCSize_s->fill(size);
    if (noise) m_VCSize_n->fill(size);
    if (other) m_VCSize_o->fill(size);

  }
  m_VCNHits_s->fill(nVeloCluster_s);
  m_VCNHits_n->fill(nVeloCluster_n);
  m_VCNHits_o->fill(nVeloCluster_o);
  // to relate a veloCluster strip to mcfe(s)  use the strip and sensor number
  return StatusCode::SUCCESS;

}
//==============================================================================
// Test the one of each sensor type...
//=======================================================================
StatusCode VeloMonitor::testDetElement(){

  bool isVerbose =  msgLevel( MSG::VERBOSE );
  if(isVerbose){
    verbose() << "Testing detector element" << endmsg;
    m_vpVeloR=m_velo->vpRSensors();
    m_vpVeloPhi=m_velo->vpPhiSensors();
    std::vector<unsigned int> testSensors;
    testSensors.clear();
    // Get sensor number for first two R sensors
    for (unsigned int i=0; i<2; i++){
      testSensors.push_back(m_vpVeloR[i]->sensorNumber());
      
    }
    // Get number for first 4 phi sensors
    for (unsigned int i=0; i<4; i++){
      testSensors.push_back(m_vpVeloPhi[i]->sensorNumber());
    }
    for(int ix=-50; ix<50; ix++){
      double x=ix;
      for(int iy=-50; iy<50; iy++){
        double y=iy;
        // Loop over sensors to test.....
        for(unsigned int iTest=0; iTest<testSensors.size(); iTest++){
          unsigned int sensor=testSensors[iTest];
          std::string type=m_velo->type(sensor);
          HepPoint3D testPoint(x,y,m_velo->zSensor(sensor));
          VeloChannelID channel;
          double localOffset,pitch;
          StatusCode scPoint = m_velo->pointToChannel(testPoint,sensor,channel,
						      localOffset,pitch);
          double residual,chi2;
          if(scPoint){
            StatusCode scResidual = m_velo->residual(testPoint,channel,residual,
                                                     chi2);
            if(scResidual) {
              if(m_velo->isRSensor(sensor)) {
                m_residualR->fill(residual,1.0);
              } else if(m_velo->isPhiSensor(sensor)) {
                m_residualPhi->fill(residual,1.0);
              }
            }
            if("RRigh" == type) {
              m_detRRigh->fill(testPoint.x(),testPoint.y());
              m_stripRRigh->fill(channel.strip(),1.0);
            } else if("RLeft" == type){
              m_detRLeft->fill(testPoint.x(),testPoint.y());
              m_stripRLeft->fill(channel.strip(),1.0);
            } else if("PhiDR" == type){
              m_detPhiDR->fill(testPoint.x(),testPoint.y()); 
              m_stripPhiDR->fill(channel.strip(),1.0);
            } else if("PhiUR" == type){
              m_detPhiUR->fill(testPoint.x(),testPoint.y());
              m_stripPhiUR->fill(channel.strip(),1.0);
            } else if("PhiUL" == type){
              m_detPhiUL->fill(testPoint.x(),testPoint.y());
              m_stripPhiUL->fill(channel.strip(),1.0);
            } else if("PhiDL" == type){
              m_detPhiDL->fill(testPoint.x(),testPoint.y());
              m_stripPhiDL->fill(channel.strip(),1.0);
            }
          }
        }
      }
    }
    // Loop over strips for R Sensor
    for(unsigned int strip=0;strip<m_velo->numberStrips(4);strip++){
      VeloChannelID channel(4,strip,VeloChannelID::RType);
      double radius=m_velo->rOfStrip(channel);
      verbose() <<"testR; strip " << strip << " radius " << radius
		<< std::endl;
      m_rOfStrips->fill(strip,radius);
    }
    for(unsigned int sensor=2; sensor<testSensors.size(); sensor++){
      for(unsigned int strip=0;strip<m_velo->numberStrips(testSensors[sensor]);
          strip++){
        VeloChannelID channel(testSensors[sensor],strip,VeloChannelID::PhiType);
        unsigned int zone=m_velo->zoneOfStrip(channel);
        double radius=m_velo->rMin(channel.sensor(),zone);
        double testPhi,trgPhi,testDirection,trgDirection;
        testPhi=m_velo->phiOfStrip(channel,radius);
        trgPhi=m_velo->trgPhiOfStrip(channel,radius);
        testDirection=m_velo->angleOfStrip(channel,radius);
        trgDirection=m_velo->trgPhiDirectionOfStrip(channel,radius);
        verbose() <<"testPhi; sensor " << testSensors[sensor] 
		  << " z " << (m_velo->zSensor(testSensors[sensor]))
		  << " strip " << strip 
		  << " zone " << zone
		  << " radius " << radius
		  << " phi " <<  testPhi/degree
		  << " trg " <<  trgPhi/degree
		  << " dirc " <<  testDirection/degree
		  << " trg " <<  trgDirection/degree
		  << std::endl;
      }
    }
    // Write out associated sensors
    for(unsigned int index=0;index<m_velo->numberSensors();index++){
      unsigned int sensor=m_velo->sensorNumber(index);
      std::vector<unsigned int> assocSensor;
      StatusCode sc = m_velo->sensorAssociated(sensor,assocSensor);
      if(sc) {
        verbose() << "Sensor " << sensor << ":";
        for(unsigned int iassoc=0;iassoc<assocSensor.size();iassoc++){
          verbose() << " iassoc " << iassoc << " " << assocSensor[iassoc];
        }
        verbose() << endreq;
        for(unsigned int iassoc=0;iassoc<assocSensor.size();iassoc++){
          if(m_velo->xSide(sensor) == m_velo->xSide(assocSensor[iassoc])){
            double stripMin,stripMax,pitch,offset;
            /*            verbose()
			  << "Inputs; sensor " << sensor << " R " << radius
                          << " sensor " << rSensor << " zone " << 0 
                          << " angularTol " << angularTol << endmsg;*/
            m_velo->trgPhiMatchingStrips(assocSensor[iassoc],8.17,sensor,0,0,
                                         stripMin,stripMax,pitch,offset);
          }
        }
      } else {
        verbose() << "No associated sensors for " << sensor 
            << endreq;
      }
      // Loop over all sensors and write out coordinates of strips
      if(m_drawSensors){
        for(unsigned int strip=0;strip<m_velo->numberStrips(sensor);strip++){
          if(m_velo->isRSensor(sensor)){
            double z,radius,phiMin,phiMax;
            StatusCode sc=m_velo->stripLimitsR(sensor,strip,z,radius,phiMin,
                                               phiMax);
            if(sc.isSuccess()){
              if(0 > phiMax || 0 > phiMin) {
                phiMax += 2*pi;
                phiMin += 2*pi;
              }
              if(phiMin > phiMax) {
                double temp = phiMax;
                phiMax = phiMin;
                phiMin = temp;
              }
              verbose() << "RSensor, number " << sensor 
                  << " Strip " << strip << " Limits " << radius 
                  << " " << phiMin/degree << " " << phiMax/degree << endreq;
            }
          } else if(m_velo->isPhiSensor(sensor)){
            HepPoint3D start;
            HepPoint3D end;
            StatusCode sc=m_velo->stripLimitsPhi(sensor,strip,start,end);
            if(sc.isSuccess()){
              verbose() << "PhiSensor, number " << sensor 
                  << " Strip " << strip << " Limits; x1 "
                  << start.x() << " y1 " << start.y() << " z1 " << start.z() 
                  << " x2 " <<  end.x() << " y2 " << end.y() 
                  << " z2 " << end.z() << endreq;
            }
          }
        }
      }
    }
  }
  return StatusCode::SUCCESS;
}
VeloChannelID VeloMonitor::weightedMean(VeloCluster* cluster, double& fraction){
  // calculated the weighted mean "position" of the cluster, 
  // in units of strip. 
  
  int Nstrips = cluster->size();
  int iDiff;
  double centre=0.;
  double total=0.;
  StatusCode sc;
  for(int i=0; i<Nstrips; i++){
    sc=m_velo->channelDistance(cluster->channelID(0),cluster->channelID(i),
                               iDiff);
    centre+=float(iDiff+100)*cluster->adcValue(i);
    total+=cluster->adcValue(i);
  }
  centre /= total;
  centre -= 100;
  iDiff=int(LHCbMath::round(centre));
  fraction=centre-iDiff;
  VeloChannelID iDiffID;
  sc= m_velo->neighbour(cluster->channelID(0),iDiff,iDiffID);
  if (sc) {
    return iDiffID;
  } else {
    return VeloChannelID(0);
  }
}

StatusCode VeloMonitor::basicMonitor() {

  bool isDebug   = msgLevel( MSG::DEBUG   );

  // fill a few basic histograms and store numbers for the monitoring table

  double size=0;
  // MCVeloHits
  if (m_testMCVeloHit) {
    if ( 0 == m_mchits ) { 
      error() << " ----  No MCVeloHits container retrieved --- "
          << endmsg;
      return StatusCode::FAILURE;
    } else {
      if(isDebug) debug() << "Retrieved MCVeloHits in standard monitor()"<<endmsg;
    }
    size=m_mchits->size(); m_nMCVH+= size; m_nMCVH2+= size*size;
    m_MCVHNHits->fill(size,1.0);
  }
  if (m_testPUMCVeloHit) {
    // MCVeloPileupHits
    if ( 0 == m_pumchits ) { 
      error() <<" ----  No PileUp MCVeloHits container retrieved --- "
          << endmsg;
      return StatusCode::FAILURE;
    } else {
      if(isDebug) debug() << "Retrieved PileUp MCVeloHits in standard monitor()"
			  << endmsg;
    }
    size=m_pumchits->size(); m_nMCPH+= size; m_nMCPH2+= size*size; 
    m_MCVHPNHits->fill(size,1.0);
  }
  // MCVeloFEs
  if (m_testMCVeloFE) {
    if ( 0== m_mcfes ) { 
      error() << " ----  No MCVeloFEs container retrieved --- "
          << endmsg;
      return StatusCode::FAILURE;
    } else {
      if(isDebug) debug() << "Retrieved MCVeloFEs in standard monitor()" <<endmsg;
    }
    size=m_mcfes->size(); m_nMCFE+= size; m_nMCFE2+= size*size; 
    // split FEs into sub-categories
    //  Define an iterator to the MC VELO FE vector and loop through it  
    MCVeloFEs::iterator itdv;
    for (itdv=m_mcfes->begin(); itdv!=m_mcfes->end(); itdv++) {
      // mainly signal or spillover/coupling or noise ?
      bool noise=false, signal=false, other=false;
      if (fabs((*itdv)->addedNoise()) > fabs((*itdv)->addedSignal())) 
        noise=true; // noise
      MCVeloFE* FE = (*itdv);
      FEType(FE, signal,noise,other); 
      if (signal) m_nMCFEs++; 
      if (noise) m_nMCFEn++; 
      if (other) m_nMCFEo++;
    }
  }
  //VeloFullDigits
  if (m_testVeloFullDigit) {
    if ( 0== m_digits ) { 
      error() << " ----  No VeloFullDigits container retrieved --- "
          << endmsg;
      return StatusCode::FAILURE;
    } else {
      if(isDebug) debug() << "Retrieved VeloFullDigits in standard monitor()"
			  << endmsg;
    }
    size=m_digits->size(); m_nFD+= size; m_nFD2+= size*size; 
  }
  //VeloClusters
  if (m_testVeloCluster) {
    if ( 0== m_clusters ) { 
      error() << " ----  No VeloCluster container retrieved --- "
          << endmsg;
      return StatusCode::FAILURE;
    } else {
      if(isDebug) debug() << "Retrieved VeloClusters in standard monitor()"
			  << endmsg;
    }
    size=m_clusters->size(); m_nVC+= size; m_nVC2+= size*size; 
    m_VCNHits->fill(size,1.);
    //split Clusters into sub-categories
    VeloClusters::iterator itcv;
    for (itcv=m_clusters->begin(); itcv!=m_clusters->end(); itcv++) {
      // cluster from signal or noise or other ?
      bool signal=false, noise=false, other=false, spillover=false;
      VeloCluster* cluster = (*itcv);
      clusterType(cluster,signal,noise,spillover);
      if (signal) m_nVCs++; if (noise) m_nVCn++; if (other) m_nVCo++;
      // calculate weighted centre of cluster
      double  fractionCentW;
      VeloChannelID IntCentW = weightedMean((*itcv), fractionCentW);
      unsigned int zone;
      double testRadius;
      double sensorZ=m_velo->zSensor((*itcv)->sensor())/cm;
      if (m_velo->isRSensor((*itcv)->sensor())) {
        testRadius=m_velo->rOfStrip(IntCentW,fractionCentW);
        zone=m_velo->zoneOfStrip(IntCentW);
        if(isDebug) debug() << " sensorZ " << sensorZ 
			    << " strip " << IntCentW.strip()
			    << " zone " << zone
			    << " testRadius " << testRadius 
			    << " testRadius/cm " << testRadius/cm 
			    << endmsg;
        if (m_detailedMonitor){
          if(0 == zone) {
            m_VCZR0->fill(sensorZ, testRadius/cm,1.0);
          } else if(1 == zone) {
            m_VCZR1->fill(sensorZ, testRadius/cm,1.0);
          } else if(2 == zone) {
            m_VCZR2->fill(sensorZ, testRadius/cm,1.0);
          } else if(3 == zone) {
            m_VCZR3->fill(sensorZ, testRadius/cm,1.0);
          }
        }
      } else if(m_velo->isPhiSensor((*itcv)->sensor())) {
        zone=m_velo->zoneOfStrip(IntCentW);
        testRadius=m_velo->rMin((*itcv)->sensor(),zone);
        double testPhi=m_velo->phiOfStrip(IntCentW,fractionCentW,testRadius);
        double trgPhi=m_velo->trgPhiOfStrip(IntCentW,fractionCentW,testRadius);
        if(isDebug) debug() << " sensorZ " << sensorZ
			    << " sensor " << IntCentW.sensor()
			    << " strip " << IntCentW.strip()+fractionCentW
			    << " zone " << zone
			    << " testRadius " << testRadius 
			    << " testPhi " << testPhi/degree 
			    << " trgPhi " << trgPhi/degree 
			    << endmsg;
        if (m_detailedMonitor){
          if (0==zone){
            m_VCZPhiInner->fill(sensorZ, testPhi/degree,1.0);
            m_VCZTrgPhiInner->fill(sensorZ, trgPhi/degree,1.0);}
          else {
            m_VCZPhiOuter->fill(sensorZ, testPhi/degree,1.0);
            m_VCZTrgPhiOuter->fill(sensorZ, trgPhi/degree,1.0);}
        }
      }
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode VeloMonitor::FEType(MCVeloFE* FE, bool& signal, 
                               bool& noise, bool& other ) {
  // mainly signal or spillover/coupling or noise ?

  if (fabs(FE->addedNoise()) > 0.0 && FE->addedSignal()==0.0) { 
    noise=true; // noise
    return StatusCode::SUCCESS;
  }
  if ((fabs(FE->addedSignal())>0.)&&(FE->mcVeloHits().size()!=0)) {
    signal=true; // signal from this event
    return StatusCode::SUCCESS;
  }
  if ((fabs(FE->addedSignal())>0.)&&(FE->mcVeloHits().size()==0)) {
    other=true; // spillover or coupling
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}

StatusCode VeloMonitor::clusterType(VeloCluster* aCluster,bool& signal, 
                                    bool& noise, bool& other ) {
  // mainly signal or spillover/coupling or noise ?
  //    noise=false, signal=false, other=false;

  // find main FE in cluster
  MCVeloFE* FE=0;
  int NStrips=aCluster->size();
  for (int iStrip=0; iStrip<NStrips; iStrip++){
    // get MCVeloFE for strip
    VeloChannelID channelID = aCluster->channelID(iStrip);
    MCVeloFE* anFE = m_mcfes->object(channelID);
    if( 0 == anFE ) {
      warning() << "Not found FE for channelID "<< channelID<< endmsg;
    }
    else {
      if (iStrip==0) FE=anFE;
      if (anFE->charge() > FE->charge()) FE=anFE;
    }
  }

  signal = false;
  noise = false;
  other = false;

  if ( 0 != FE ) {FEType(FE, signal, noise, other); }

  return StatusCode::SUCCESS;

}

StatusCode VeloMonitor::detElement(){

  m_velo = getDet<DeVelo>("/dd/Structure/LHCb/Velo" );

  if (m_detElement) {
    info() << "===========================================" << endmsg;
    unsigned int nonPUSensors=m_velo->numberNonPUSensors();
    for (unsigned int iSensor=0; iSensor < nonPUSensors; iSensor++){
      info() << "iSensor " << iSensor << " / " << nonPUSensors << endmsg;
      unsigned int sensor=m_velo->sensorNumber(iSensor);
      info() << "Sensor " << sensor << "  Z position " 
          << m_velo->zSensor(sensor)/mm << endmsg;
    }
    info() << "===========================================" << endmsg;
  }
  return StatusCode::SUCCESS;
}

StatusCode VeloMonitor::resolution() {

  bool isDebug   = msgLevel( MSG::DEBUG   );

  if(isDebug) debug() << " resolution() " << endmsg;
  if ( 0== m_clusters ) { 
    error() << " ----  No VeloCluster container retrieved --- "
        << endmsg;
    return StatusCode::FAILURE;
  }

  VeloClusters::iterator itcv;
  for (itcv=m_clusters->begin(); itcv!=m_clusters->end(); itcv++) {
    if(isDebug) debug() << "cluster loop" <<endmsg;

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
      error() << " ----  MCHit Associator Not retrieved --- "<<endmsg;
      return StatusCode::FAILURE;
    }
    const Table* table = associator->direct();
    if (0==table){ return StatusCode::FAILURE;}

    Range range1 = table->relations(*itcv);
    for (iterator relation=range1.begin(); relation !=range1.end(); relation++){
      // loop over relations
      MCVeloHit * hit = relation->to ();
      // calculate distance between point and cluster
      HepPoint3D point=hit->entry()+hit->exit();
      point/=2.;
      bool valid=false;
      double pitch=0.,fraction=0.;
      //calculate point on path
      VeloChannelID entryChan;
      StatusCode sc=m_velo->pointToChannel(point,entryChan,fraction,pitch);
      int IntChanDist;
      sc=m_velo->channelDistance(entryChan,IntCentW,IntChanDist);
      if(sc) valid=true;
      double distance=IntChanDist+(fractionCentW-fraction);
      if(isDebug) debug() << valid << " distance in strips " << distance 
			  << " in microns " << distance*pitch/micron << endmsg;
      HepLorentzVector FMom = hit->mcParticle()->momentum();
      // fill resolution histograms
      if (m_velo->isRSensor((*itcv)->sensor())&&valid){
	//          m_RRes->fill(distance*pitch/micron,1.0);
	if (((FMom.e()/GeV)>2.)){
	  m_RRes->fill(distance*pitch/micron,1.0);
	  if ((*itcv)->size()==1) m_RRes1s->fill(distance*pitch/micron,1.0);
	  if ((*itcv)->size()==2) m_RRes2s->fill(distance*pitch/micron,1.0);
	  if ((*itcv)->size()==3) m_RRes3s->fill(distance*pitch/micron,1.0);
	  m_RCluster->fill((*itcv)->size(),1.0);             
	  m_RResd->fill(distance,1.0);
	  double theta=FMom.theta();
	  if (theta> pi/2) {
	    theta-=pi;
	    theta=-1*theta;
	  }
	  m_RResAngle->fill(distance*pitch/micron,theta,1.0);
	  m_RAngle->fill(theta,1.0);
	  if ((*itcv)->size()==1) m_RAngle1s->fill(theta,1.0);
	  if ((*itcv)->size()==2) m_RAngle2s->fill(theta,1.0);
	  if ((*itcv)->size()==3) m_RAngle3s->fill(theta,1.0);
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

StatusCode VeloMonitor::checkTests() {
  if (0==(  m_testMCVeloHit
	    + m_testPUMCVeloHit
	    + m_testMCVeloFE
	    + m_testMCVeloFE
	    + m_testVeloFullDigit
	    + m_testVeloCluster) ) {
    error() << "VeloMonitor asked to monitor nothing!!!" 
	    << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}
