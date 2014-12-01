// 
// include files

// from CLEP
#include "CLHEP/Geometry/Point3D.h"

// from Geant4
#include "G4Step.hh"
#include "G4OpticalPhoton.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4Gamma.hh" 
#include "G4Timer.hh"

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
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

// from GaussTools
#include "GaussTools/GaussTrackInformation.h"

// from AIDA
#include "AIDA/IHistogram1D.h"

// local
#include "MonitorStepAction.h"

// stream
#include <fstream>
#include <cstdlib>


/* file 
 *  
 *  Implementation of class MonitorStepAction
 *
 *  author Olga Levitskaya, Gloria Corti 
 */

// ============================================================================
// Declaration of the Tool Factory
// ============================================================================

DECLARE_TOOL_FACTORY( MonitorStepAction )

// ============================================================================
// Standard constructor, initialize variables
// ============================================================================
/* 
 *  @see GiGaStepActionBase 
 *  @see GiGaBase 
 *  @see AlgTool 
 *  @param "type"- type of the object (?)
 *  @param "name" - name of the object
 *  @param "parent" - pointer to parent object
 */
// ============================================================================

MonitorStepAction::MonitorStepAction( const std::string& type,
                                      const std::string& name,
  				                            const IInterface*  parent ) 
  : GiGaStepActionBase ( type , name , parent )
    , m_nTracks(0)
    , m_first( true )
{
  // member variables initialization

  m_nSteps = 0;   // number of steps
  oldTrack = 0;   // sign of an old track
  old_name = " "; // old_name (name of current particle) is empty

}

// ============================================================================
// Destructor
// ============================================================================

MonitorStepAction::~MonitorStepAction()
{
  Vmsgr.close();
}

//============================================================================
// Initialize
//============================================================================
StatusCode MonitorStepAction::initialize()
{

  // book histograms

  m_hStepNumberMF = histoSvc()->book(name(),"StepNF1-3","Total Distribution of step numbers in MUON Filters(1-3)",
                                  21, -0.5, 20.5);
  
  m_hStepLengthMF = histoSvc()->book(name(),"StepLF1-3","Total Distribution of step length in MUON Filters(1-3) mm",
                                  100, 0., 1000.0);
  
  StatusCode status =  GiGaStepActionBase::initialize();
  return Print("Initialized successfully" , 
               StatusCode::SUCCESS         , MSG::VERBOSE);  
}

//============================================================================
// Finalize
//============================================================================
StatusCode MonitorStepAction::finalize()
{

  // fill one last time nstep histogram
  //m_hStepNumberMF -> fill(m_nSteps,1.0);
  always() << "Number of muon tracks = " << m_nTracks << endmsg;

  return GiGaStepActionBase::finalize();
  //  return StatusCode::SUCCESS;
}
			
// ============================================================================
// ============================================================================
/*  stepping action
 *  see G4UserSteppingAction
 *  param step Geant4 step
 */
// ============================================================================

void MonitorStepAction::UserSteppingAction ( const G4Step* theStep ) 
{
  
  // =========================================================================
  // Rerform Pre Action
  // =========================================================================
  
  // Get information about a track
  track = theStep->GetTrack();
  
  uinf = track->GetUserInformation(); 
  
  ginf = (GaussTrackInformation*) uinf;
  
  partdef = track->GetDefinition();
  
  thePreStepPoint = theStep->GetPreStepPoint();
  
  thePostStepPoint = theStep->GetPostStepPoint();
  
  Vol = thePreStepPoint->GetPhysicalVolume();
  
  
  //===========================================================================
  // Volume Registration
  //===========================================================================

  std::string volumeName = (std::string)Vol->GetName();
  ParticleName = partdef->GetParticleName();
  std::string volName = "";

  //  std::cout << "Volume name " << volumeName << std::endl;

  // If Muon filter find which one
  if ( volumeName.find("Muon/Filters/") != std::string::npos ) {
    volName = volumeName.substr(0,54);
  }

  // Do this only for muons 
  if ( abs(partdef->GetPDGEncoding()) != 13 ) {
    return;
  }
  
  // Check if this is a new or old track
  if ( track != oldTrack ) {

    m_nTracks++;
    
    debug() << "  New Track " << m_nTracks << endmsg;
    
    // Update Position vectors 
    initial_position = thePreStepPoint->GetPosition();      
    // Update initial track paramteters
    oldTrack = track;
    old_name = partdef->GetParticleName();

    m_oldvolname = volName; // is expected to change
    initial_volume = volumeName; // reinitialized at the beginning of each track only

      
    if ( ! m_first ) {
      // fill histogram with m_nSteps for all new tracks but very first one
     m_hStepNumberMF -> fill(m_nSteps,1.0);
      
      initial_position = position;

      m_first = false;
    }
    radius = 0.0;
    m_nSteps = 0;
  }

  // Find if we are in one of the muon filters
  if ( volumeName.find("Muon/Filters/") != std::string::npos ) {

    // Check if it is the same muon filter
    // reinitialize the values when the filter changes
    
    if (volName != m_oldvolname) {
      // temporary printing 28.05.08
      debug() << " STEP NUMBER "<< m_nSteps << " in old volume " << m_oldvolname << endmsg;
      debug() << " New volume is         " << volumeName << endmsg;
      // fill histogram with m_nSteps

      if ( m_nSteps == 0 && oldTrack != 0 ) {
        G4ThreeVector dir = oldTrack->GetVertexMomentumDirection();
        debug() << " Old Track       " << m_nTracks << endmsg;
        debug() << " Vertex Coordin. " << oldTrack->GetVertexPosition() << endmsg;
        debug() << " Vertex Direct.  " << dir << endmsg;
        debug() << " Theta(x,y)      " << dir.x()/dir.z() << ", " << dir.y()/dir.z() << endmsg;        
      }

      m_hStepNumberMF -> fill(m_nSteps,1.0);
      
      initial_position = position;
      m_oldvolname = volName;
      m_nSteps=0;
    }

    m_nSteps+=1;
    // temporary printing 28.05.08
    debug() << " STEP NUMBER "<< m_nSteps  
              << " in             " << volName << endmsg;
      
    position = thePreStepPoint->GetPosition();
    postposition = thePostStepPoint->GetPosition();
    DeltaPos = thePostStepPoint->GetPosition() - thePreStepPoint->GetPosition();
    // step size       
    radius = sqrt( DeltaPos[0]*DeltaPos[0] + DeltaPos[1]*DeltaPos[1]
                   + DeltaPos[2]*DeltaPos[2]  );

    //      std::cout <<" MSA "<< ParticleName.c_str() << " Volume "<< volName << std::endl;
    //std::cout <<" Cin."<< position << " Cout. "<<postposition << " radius "<< radius <<std::endl;
      //}
    
      
    // fill histogram with radius
       m_hStepLengthMF -> fill(radius);
       
  } 
 
}

// ============================================================================
// The END 
// ============================================================================
