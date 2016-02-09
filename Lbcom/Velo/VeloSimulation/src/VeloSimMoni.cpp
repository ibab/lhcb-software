// Include files 

// from Gaudi
#include "GaudiAlg/Tuples.h"
#include "GaudiKernel/SmartRefVector.h"
#include "Kernel/VeloChannelID.h"

// local
#include "VeloSimMoni.h"

// Velo Event classes
#include "Event/MCHit.h"
#include "Event/MCVeloFE.h"
#include "Kernel/IMCVeloFEType.h"

using namespace Gaudi::Units;

//-----------------------------------------------------------------------------
// Implementation file for class : VeloSimMoni
//
// 2005-11-03 : Tomasz Szumlak & Chris Parkes
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( VeloSimMoni )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VeloSimMoni::VeloSimMoni( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator ),
    m_mcVFEsCont(0),
    m_veloDet(0),
    m_feTypeTool(0),
    m_nMCVeloFE(0),
    m_nMCVeloFE2(0.),
    m_nMCVeloFEs(0),
    m_nMCVeloFEn(0),
    m_nMCVeloFEo(0),
    m_NumberOfEvents(0)
{
  declareProperty("PrintInfo",       m_printInfo = false );
  declareProperty("DetailedMonitor", m_detailedMonitor = false );
  declareProperty("VeloFEMoni",      m_veloFEMoni = true );
}
//=============================================================================
// Destructor
//=============================================================================
VeloSimMoni::~VeloSimMoni() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode VeloSimMoni::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;
  m_feTypeTool=tool<IMCVeloFEType>("MCVeloFEType/feTypeTool");

  m_veloDet = getDet<DeVelo>( DeVeloLocation::Default );

  setHistoTopDir("Velo/");
  //  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode VeloSimMoni::execute() {

  debug() << "==> Execute" << endmsg;
  //
  m_NumberOfEvents++;
  getData();
  if(m_veloFEMoni) VeloFEMonitor();
  //  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode VeloSimMoni::finalize() {

  debug() << "==> Finalize" << endmsg;
  //
  double errnMCVeloFE = 0;
  if( 0 != m_NumberOfEvents ) {
    m_nMCVeloFE/=m_NumberOfEvents;
    m_nMCVeloFE2/=m_NumberOfEvents;
    errnMCVeloFE=
      sqrt((m_nMCVeloFE2-(m_nMCVeloFE*m_nMCVeloFE))/m_NumberOfEvents);
  }
  //
  info()<< "------------------------------------------------------" <<endmsg;
  info()<< "                - VeloSimMoni table -                 " <<endmsg;
  info()<< "------------------------------------------------------" <<endmsg;
  info()<< "| Number of MCVeloFEs/Event: " << m_nMCVeloFE << " +/- " 
        << errnMCVeloFE <<endmsg;
  double allFEs=m_nMCVeloFEs+m_nMCVeloFEn+m_nMCVeloFEo;
  if(allFEs>0){
    info().precision(4);
    info()<< "| FEs from signal:                      " 
          << (m_nMCVeloFEs/allFEs)*100
          << "%" <<endmsg;
    info()<< "| FEs from noise:                       " 
          << (m_nMCVeloFEn/allFEs)*100
          << "%" <<endmsg;
    info()<< "| FEs from other, coupling + spillover: " 
          << (m_nMCVeloFEo/allFEs)*100
          << "%" <<endmsg;
  }else{
    info()<< "| ==> No MCVeloFEs found! " <<endmsg;
  }
  info()<< "------------------------------------------------------" <<endmsg;
  //
  return GaudiAlgorithm::finalize();  // must be called after all other actions
}
//
void VeloSimMoni::getData()
{
  debug()<< " ==> VeloSimMoni::getData" <<endmsg;
  // 
  m_mcVFEsCont=getIfExists<LHCb::MCVeloFEs>(LHCb::MCVeloFELocation::Default);
  if( NULL == m_mcVFEsCont ){
    error()<< "There is no VeloFEs in TES!" <<endmsg;
    m_veloFEMoni=false;
  }
  return;
}
//
void VeloSimMoni::VeloFEMonitor()
{
  debug()<< " ==> VeloSimMoni::VeloFEMonitor" <<endmsg;
  //  
  long size=m_mcVFEsCont->size();
  m_nMCVeloFE+=size;
  m_nMCVeloFE2+=double(size*size);
  plot(size, 100, "Number of MCVeloFE/event", 0., 10000., 50);
  // main loop over FEs container
  LHCb::MCVeloFEs::iterator ItFE;
  for(ItFE=m_mcVFEsCont->begin(); ItFE!=m_mcVFEsCont->end(); ItFE++){
    LHCb::MCVeloFE* localFE=(*ItFE);
    int feType=0;
    bool Signal=false, Noise=false, Other=false;
    m_feTypeTool->FEType(localFE, feType);
    if(!feType){ Signal=true; }
    else if(1==feType){ Noise=true; }
    else if(2==feType){ Other=true; }
    if(Signal) { m_nMCVeloFEs++; }
    if(Noise) { m_nMCVeloFEn++; }
    if(Other) { m_nMCVeloFEo++; }
    if(Other){
    debug()<< " sig: " << Signal << ", noise: " << Noise
          << ", so: " << Other <<endmsg;
    debug()<< " local FE strip: " << localFE->strip() <<endmsg;
    }
    // print some info if asked
    if(m_printInfo){
      info()<< " ==> MCVeloFE Monitor: "
            << " ChannelID: " << localFE->channelID()
            << ", sensor number: " << localFE->sensor()
            << ", strip number: " << localFE->strip()
            << "\n \t\t" << " Added noise: " << localFE->addedNoise()
            << ", added CM noise: " << localFE->addedCMNoise()
            << ", added pedestal: " << localFE->addedPedestal()
            << "\n \t\t" << " Added signal: " << localFE->addedSignal()
            << ", charge: " << localFE->charge()
      <<endmsg;
    }
  //
  plot(localFE->charge(), 101, 
       "Charge in Si strip (electrons)",
       -10000., 50000., 100);
  plot(localFE->addedSignal(), 102, 
       "Signal deposited in Si strip (electrons)",
       0., 50000., 100);
  plot(localFE->addedNoise(), 103, 
       "Noise added in Si strip (electrons)",
       -10000., 10000, 100);
  plot(localFE->addedCMNoise(), 113,
       "Common-Mode noise added in Si strip (electrons)",
       -20000., 20000., 100);
  plot(localFE->addedPedestal(), 114,
       "Pedestal added in Si strip (electrons)",
       0., 50000., 100);
  plot2D(localFE->sensor(), localFE->strip(), 115,
         "Sensor and strip number",
         0., 100., 0., 5000., 100, 50);
  //
  if(Signal){
    plot(localFE->charge(), 104, 
         "Signal dominated - charge in Si (electrons)",
         -10000., 50000., 100);
    plot(localFE->addedSignal(), 105, 
         "Signal domianted - Signal deposited in Si (electrons)",
         0., 50000., 100);
    plot(localFE->addedNoise(), 106,
         "Signal dominated - Noise added in Si strip (electrons)",
         -10000., -10000., 100);
    plot2D(localFE->sensor(), localFE->strip(), 116,
           "Signal dominated - sensor and strip number",
           0., 100., 0., 5000., 100, 50);
  }
  //
  if(Noise){
    plot(localFE->charge(), 107, 
         "Noise dominated - Charge in Si strip (electrons)",
         -10000., 50000., 100);
    plot(localFE->addedSignal(), 108,
         "Noise dominated - Signal deposited in Si strip (electrons)",
         0., 50000., 100);
    plot(localFE->addedNoise(), 109,
         "Noise domianted - Noise added in Si strip (electrons)",
         -10000., 10000., 100);
    plot2D(localFE->sensor(), localFE->strip(), 117,
           "Noise dominated - sensor and strip number",
           0., 100., 0., 5000., 100, 50);
  }
  //
  if(Other){
    plot(localFE->charge(), 110,
         "Other dominated - Charge in Si strip (electrons)",
         -10000., 50000., 100);
    plot(localFE->addedSignal(), 111,
         "Other dominated - Signal deposited in Si strip (electrons)",
         0., 50000., 100);
    plot(localFE->addedNoise(), 112,
         "Other dominated - Noise added in Si strip (electrons)",
         -10000., 10000., 100);
    plot2D(localFE->sensor(), localFE->strip(), 118,
           "Other dominated - sensor and strip number",
           0., 100., 0., 5000., 100, 50);
  }
  //
  // Get access to the MCVeloHits from which MCVeloFE was made, store energy
  //
  SmartRefVector<LHCb::MCHit> myMCVeloHits=localFE->mcHits();
  SmartRefVector<LHCb::MCHit>::iterator ItH;
  std::vector<double> myMCVeloHitsCharge=localFE->mcHitsCharge();
  std::vector<double>::iterator ItCh;
  //
  if(m_printInfo){
    info()<< "Test MCVeloFE: " << "MCVeloHit size:"
          << myMCVeloHits.size() <<endmsg;
  }
  plot(myMCVeloHits.size(), 119,
       "Number of MCVeloHits/MCVeloFE",
       -0.5, 5.5, 6);
  //
  double totalSignal=0.;
  ItCh=myMCVeloHitsCharge.begin();
  for(ItH=myMCVeloHits.begin(); ItH!=myMCVeloHits.end(); ItH++){
    if(m_printInfo){
      info()<< "Test MCVeloFE: " << "MCVeloHit, "
            << "charge in current strip (electrons)" << (*ItCh)
            << ", energy (eV)" << (*ItH)->energy()/eV <<endmsg;
    }
    totalSignal+=(*ItCh);
    plot((*ItCh), 120,
         "MC hit signal deposited in MCVeloFE",
         0., 50000., 100);
    ItCh++;
  }
  //
  plot(totalSignal, 121,
       "MC total signal deposited in MCVeloFE",
       0., 50000., 100);
  if(Signal){
    plot(totalSignal, 122,
         "Signal dominated - MC total signal deposited in MCVeloFE",
         0., 50000., 100);
  }
  //
  if(Noise){
    plot(totalSignal, 123,
         "Noise dominated - MC total signal deposited in MCVeloFE",
         0., 50000., 100);
  }
  //
  if(Other){
    plot(totalSignal, 124,
         "Other dominated - MC total signal deposited in MCVeloFE",
         0., 50000., 100);
  }
//
// Some other details to check
//
  if(m_detailedMonitor){
  LHCb::VeloChannelID channel=localFE->channelID();
  const DeVeloSensor* sens=m_veloDet->sensor(localFE->channelID().sensor());
  double sensorZ=sens->z()/cm;
  if(m_printInfo){
    info()<< "Channel: " << channel << ", sensor (from channel): "
          << channel.sensor() << ", sensor (from MCVeloFE): "
          << localFE->sensor() << ", Z position: "
          << sensorZ <<endmsg;
  }
  //
  if(sens->isR()){
    const DeVeloRType* rSens=dynamic_cast<const DeVeloRType*>(sens);
    double testRadius=rSens->rOfStrip(channel.strip());
    unsigned int zone=rSens->zoneOfStrip(channel.strip());
    if(m_printInfo){
      info()<< "Sensor: " << channel.sensor()
            << ", strip: " << channel.strip()
            << ", zone: " << zone
            << ", sensorZ: " << sensorZ
            << ", testRadius: " << testRadius
            << ", testRadius/cm: " << testRadius/cm <<endmsg;
    }
    //
    if(0==zone){
      plot2D(sensorZ, testRadius/cm, 125,
             "MCVeloFE R position vs. Z (cm), Zone 0",
             -20., 80., 0., 5., 1000, 50);
    }else if(1==zone){
      plot2D(sensorZ, testRadius/cm, 126,
             "MCVeloFE R position vs. Z (cm), Zone 1",
             -20., 80., 0., 5., 1000, 50);
    }else if(2==zone){
      plot2D(sensorZ, testRadius/cm, 127,
             "MCVeloFE R position vs. Z (cm), Zone 2",
             -20., 80., 0., 5., 1000, 50);
    }else if(3==zone){
      plot2D(sensorZ, testRadius/cm, 128,
             "MCVeloFE R position vs. Z (cm), Zone 3",
             -20., 80., 0., 5., 1000, 50);
    }
  }else if(sens->isPhi()){
    unsigned int strip=channel.strip();
    unsigned int zone=sens->zoneOfStrip(strip);
    const DeVeloPhiType* phiSens=dynamic_cast<const DeVeloPhiType*>(sens);
    double testRadius=phiSens->rMin(zone);
    double testPhi=phiSens->globalPhi(strip, 0., testRadius);
    if(m_printInfo){
      info()<< "Sensor: " << channel.sensor()
            << ", type: " << (sens->type())
            << ", strip: " << channel.strip()
            << ", sensorZ: " << sensorZ
            << ", testPhi: " << testPhi/degree
            <<endmsg;
    }
    //
    if(0==zone){
      plot2D(sensorZ, testPhi/degree, 129,
             "MCVeloFE Phi position vs. Z (cm), Inner zone",
             -20., 80., -180., 180., 1000, 60);
    }else{
      plot2D(sensorZ, testPhi/degree, 129,
             "MCVeloFE Phi position vs. Z (cm), Outer zone",
             -20., 80., -180., 180., 1000, 60);
    }
   }
  }
 }
  return;
}
//
