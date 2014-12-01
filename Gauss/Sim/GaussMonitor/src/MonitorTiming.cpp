//
// include files

// from CLHEP
#include "CLHEP/Geometry/Point3D.h"

// from Geant4
#include "G4Step.hh"
#include "G4OpticalPhoton.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4Gamma.hh"
#include "G4Timer.hh"

// from Guadi
#include "GaudiKernel/DeclareFactoryEntries.h" //new
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/Stat.h"
#include "GaudiKernel/PropertyMgr.h"

// from GiGa 
#include "GiGa/IGiGaSvc.h"
//#include "GiGa/GiGaMACROs.h"

// from GaussTools
#include "GaussTools/GaussTrackInformation.h"

// from AIDA
#include "AIDA/IHistogram1D.h"

// local
#include "MonitorTiming.h"

// stream
#include <fstream>
#include <cstdlib>

/* file 
 *  
 *  Implementation of class MonitorTiming for timing studies
 *
 *  author Olga Levitskaya, Gloria Corti, James McCarthy
 */

// ============================================================================
// Declaration of the Tool Factory
// ============================================================================

DECLARE_TOOL_FACTORY( MonitorTiming )

// ============================================================================
// Standard constructor, initialize variables
// ============================================================================
/** standard constructor 
 *  @see GiGaStepActionBase 
 *  @see GiGaBase 
 *  @see AlgTool 
 *  @param type type of the object (?)
 *  @param name name of the object
 *  @param parent  pointer to parent object
 */
// ============================================================================

MonitorTiming::MonitorTiming( const std::string& type   ,
                                      const std::string& name   ,
                                      const IInterface*  parent ) 
  : GiGaStepActionBase ( type , name , parent )
  , m_TimerFileName()
{
  declareProperty ("TimerFileName", m_TimerFileName);
  declareProperty ("Volumes", knownVolumes);

  // timer instantiation
  totalTimer = new G4Timer;
  stepTimer = new G4Timer;

  theSteppingManager = new G4SteppingManager;
}

// ============================================================================
/// destructor
// ============================================================================
MonitorTiming::~MonitorTiming()
{
  delete totalTimer;
  delete stepTimer;
  delete theSteppingManager;
}

//=============================================================================
// Initialization
//=============================================================================	
StatusCode MonitorTiming::initialize()
{

  info()<<"*********** Initialising MonitorTimingTool ************"<<endmsg;

  m_TimerFileName2 = m_TimerFileName.data();

  //initialise the total cumulated time
  totalCumTime=0;

  std::stringstream sstream;
    
  for(size_t i=0;i<knownVolumes.size();i++){
    sstream<<"/"<<knownVolumes[i];
    knownVolumes_2.push_back(sstream.str());
    sstream.str("");
    //debug()<<"look for volume "<<i<<": "<<knownVolumes[i]<<endmsg;
  }

  totalTimer->Start();

  //initially set to true for the first loop
  firstStep=true;

  StatusCode status =  GiGaStepActionBase::initialize();
  return Print("Initialized successfully" , 
               StatusCode::SUCCESS         , MSG::VERBOSE);
}

StatusCode MonitorTiming::finalize()
{
  info()<<"*********** Finalising MonitorTimingTool ************"<<endmsg;

  stepTimer->Stop();

  //Get the total time for the simulation
  totalTimer->Stop();
  totalTime=totalTimer->GetRealElapsed();
  
  // output into file
  std:: ofstream table(m_TimerFileName2);
  if(!table.is_open())
  {
    info()<<"MonitorTiming:: Timer File not opended :: EXIT"<<endmsg;
    exit(1);
  }

  table<<"************** Volume Timing ***************"<<std::endl;
  
  for(size_t i=0;i<volumes.size();i++){
    debug()<<"Volume "<<i<<": "<<volumes[i]<<" cumulated time "<<cumulated_time_vol[i]<<" seconds"<<endmsg;
    totalCumTime+=cumulated_time_vol[i];
    table<<"Volume "<<i<<": "<<volumes[i]<<" cumulated time "<<cumulated_time_vol[i]<<" seconds"<<std::endl;
  }

  table<<std::endl<<"Total Time: "<<totalCumTime<<std::endl;

  debug()<<"********** Other Volumes **********"<<endmsg;
  table<<std::endl<<std::endl<<"************ Other Volumes ***********"<<std::endl;
  
  for(size_t i=0;i<otherVolumes.size();i++){
    debug()<<"Other Volume "<<i<<": "<<otherVolumes[i]<<endmsg;
    table<<"Other Volume "<<i<<": "<<otherVolumes[i]<<std::endl;
  }
  /*
  debug()<<"********** All Sub-volumes **********"<<endmsg;
  table<<std::endl<<std::endl<<"************ All Sub-volumes ***********"<<std::endl;

  for(size_t i=0;i<subVolumes.size();i++){
    debug()<<"Sub-Volume "<<i<<": "<<subVolumes[i]<<" cumulated time "<<cumulated_time_subvol[i]<<" seconds"<<endmsg;
    table<<"Sub-Volume "<<i<<": "<<subVolumes[i]<<" cumulated time "<<cumulated_time_subvol[i]<<" seconds"<<std::endl;
  }
  */
  debug()<<"********** Process Timing *********"<<endmsg;
  table<<std::endl<<std::endl<<"************** Process Timing **************"<<std::endl;
  for(size_t i=0;i<processes.size();i++){
    debug()<<"Process "<<i<<": "<<processes[i]<<" cumulated time "<<cumulated_time_proc[i]<<" seconds"<<endmsg;
    table<<"Process "<<i<<": "<<processes[i]<<" cumulated time "<<cumulated_time_proc[i]<<" seconds"<<std::endl;
  }

  info()<<"******* Total Cumulated Time: "<<totalCumTime<<" *********"<<endmsg;
  info()<<"******* Total Actual Time: "<<totalTime<<" *********"<<endmsg;
	
  Print("Finalization" , StatusCode::SUCCESS , MSG::VERBOSE );
  return  GiGaStepActionBase::finalize();
}


// ============================================================================
// ============================================================================
/*
 *            *****    ****     ****      ******   
 *           *        *    *    *    *    *
 *          *        *      *   *     *   ****
 *           *        *    *    *    *    *
 *            *****    ****     ****      ******
 */
// ============================================================================
/** stepping action
 *  @see G4UserSteppingAction
 *  @param step Geant4 step
 */
// ============================================================================

void MonitorTiming::UserSteppingAction ( const G4Step* theStep ) 
{  

  // I_  TIMER INITIALIZATION =====================================================

  //Check if this is the first step in the event
  if(firstStep){
    stepTime=0;
    firstStep=false;
  }
  else{
    stepTimer->Stop();
    stepTime=stepTimer->GetRealElapsed();
  }
  
  //Getting step information

  // III_ VOLUME TIMING ======================================================
    
  Vol=theStep->GetPreStepPoint()->GetPhysicalVolume();
  
  /*
    Get the volume name

    Loop over known detector names to check for matches in the volume name
    If none of the known detector names are in the volume name,
    add it to the otherVolumes vector
  */

  VolName=(std::string)Vol->GetName();

  // See if the volume is in the list of known detectors  
  bool foundKnownVol=false;
  for(size_t i=0;i<knownVolumes_2.size();i++){
    if(VolName.find(knownVolumes_2[i]) != std::string::npos){
      VolumeName=knownVolumes[i];
      foundKnownVol=true;
    }
  }

  bool newVolume=true; //boolean to check if G4 has been in this volume already

  if(!foundKnownVol){
    VolumeName="OtherVolume";
    for(size_t i=0;i<otherVolumes.size();i++){
      if(otherVolumes[i]==VolName){
        newVolume=false;
        break;
      }   
    }
    if(newVolume){
      otherVolumes.push_back(VolName);
      //debug()<<"New Volume: "<<VolName<<endmsg;
    }
  }

  newVolume=true; 

  // add to the cumulative time for the volume we are in
  // if this is the first time in the volume, add it to the volumes vector
  
  for(size_t i=0;i<volumes.size();i++){
    if(VolumeName==volumes[i]){
      cumulated_time_vol[i]+=stepTime;
      newVolume=false;
    }
  }

  if(newVolume){
    volumes.push_back(VolumeName);
    cumulated_time_vol.push_back(stepTime);
  }

  // Keep track of the full name of all sub-volumes
  newVolume=true;
  for(size_t i=0;i<subVolumes.size();i++){
    if(VolName==subVolumes[i]){ 
      cumulated_time_subvol[i]+=stepTime;
      newVolume=false; 
    }
  }
  if(newVolume){ 
    subVolumes.push_back(VolName); 
    cumulated_time_subvol.push_back(stepTime);
  }

  // PROCESS TIMING ==========================================

  processName=theStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();

  bool newProcess=true; //boolean to check if G4 has been in this volume already

  // add to the cumulative time for the process
  // if this is the first time for this process, add it to the processess vector
  
  for(size_t i=0;i<processes.size();i++){
    if(processName==processes[i]){
      cumulated_time_proc[i]+=stepTime;
      newProcess=false;
    }
  }

  if(newProcess){
    processes.push_back(processName);
    cumulated_time_proc.push_back(stepTime);
  }
  stepTimer->Start();
}

// ============================================================================
// ============================================================================
// The END 
// ============================================================================
