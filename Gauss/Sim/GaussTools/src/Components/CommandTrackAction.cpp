/// Geant4 
#include "G4Track.hh"
#include "G4UImanager.hh"
/// GaudiKernel
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiKernel/PropertyMgr.h"
/// local
#include "CommandTrackAction.h"
///

/** @file 
 *  
 *  Implementation of class CommandTrackAction
 *
 *  @author Witek Pokorski
 */

// ============================================================================
/// factory business 
// ============================================================================
DECLARE_TOOL_FACTORY( CommandTrackAction )


// ============================================================================
/** standard constructor 
 *  @see GiGaTrackActionBase 
 *  @see GiGaBase 
 *  @see AlgTool 
 *  @param type type of the object (?)
 *  @param name name of the object
 *  @param parent  pointer to parent object
 */
// ============================================================================
CommandTrackAction::CommandTrackAction
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent ) 
  : GiGaTrackActionBase( type , name , parent )
{
  m_precommand.push_back("/tracking/verbose 1");
  m_postcommand.push_back("/tracking/verbose 0");
  
  declareProperty("PrintTrackIDs", m_printIDs = true );
  declareProperty("TrackInfos", m_trackInfos = false );
  declareProperty("TrackIDs", m_trackIDs );
  declareProperty("PreCommand", m_precommand);  
  declareProperty("PostCommand", m_postcommand);
}


// ============================================================================
/// destructor 
// ============================================================================
CommandTrackAction::~CommandTrackAction() {}


// ============================================================================
/** initialize the track action  
 *  @see GiGaTrackActionBase 
 *  @see GiGaBase 
 *  @see  AlgTool 
 *  @see IAlgTool 
 *  @return status code 
 */
// ============================================================================
StatusCode CommandTrackAction::initialize () 
{
  // initialize the base 
  StatusCode status = GiGaTrackActionBase::initialize() ; 
  if( status.isFailure() ) 
    { return Error("Could not intialize base class GiGaTrackActionBase!", 
                   status ) ; } 
  //
  ///
  m_test = false;
  return Print("Initialized successfully" , 
               StatusCode::SUCCESS        , MSG::VERBOSE);
}


// ============================================================================
/** finalize the action object 
 *  @see GiGaTrackActionBase 
 *  @see GiGaBase 
 *  @see  AlgTool 
 *  @see IAlgTool 
 *  @return status code
 */ 
// ============================================================================
StatusCode CommandTrackAction::finalize   () 
{ 
  // finilize the base 
  return GiGaTrackActionBase::finalize() ;
}


// ============================================================================
/**  perform action 
 *  @see G4UserTrackingAction
 *   @param pointer to  track opbject 
 */
// ============================================================================
void CommandTrackAction::PreUserTrackingAction ( const G4Track* track )
{
  // Is the track valid? Is tracking manager valid? 
  if( 0 == track || 0 == trackMgr() ) 
    { return ; }  
  
  /// get Geant4 UI manager 
  G4UImanager* ui = G4UImanager::GetUIpointer() ;  

  if ( m_printIDs ) printInfo( track, m_trackInfos );

  for ( std::vector<int>::const_iterator tkID = m_trackIDs.begin(); 
        m_trackIDs.end() != tkID; tkID++ ) {
    
    if ( track->GetTrackID() == (*tkID) ) {
      m_test = true;
      info() << "* Setting command for G4Track TrackID " << *tkID << endmsg;

      if ( !m_trackInfos ) printInfo( track );

      for(std::vector<std::string>::const_iterator iCmd = m_precommand.begin();
          m_precommand.end() != iCmd; iCmd++ ) {
        ui->ApplyCommand(*iCmd);
      }
    }
  }
  
}

// ============================================================================
/**  perform action 
 *  @see G4UserTrackingAction
 *   @param pointer to  track object 
 */
// ============================================================================
void CommandTrackAction::PostUserTrackingAction ( const G4Track* ) 
{
  if (m_test) {
    m_test = false;
    
    G4UImanager* ui = G4UImanager::GetUIpointer() ;  
    for(std::vector<std::string>::const_iterator jCmd = m_postcommand.begin(); 
        m_postcommand.end() != jCmd; ++jCmd ) {
      ui->ApplyCommand(*jCmd);
    }
  }
  
}

// ============================================================================
void CommandTrackAction::printInfo(const G4Track* track, bool tkInfo) {
  
  if ( tkInfo ) {
    info() << "*******************************************************" 
           << endmsg;
    info() << "* G4Track Information: "
           << "  Particle = " << track->GetDefinition()->GetParticleName()
           << ","
           << "   Track ID = " << track->GetTrackID()
           << ","
           << "   Parent ID = " << track->GetParentID()
           << endmsg;
    info() << "*                         Origin = ( " 
           << track->GetPosition().x()
           << ", " 
           << track->GetPosition().y() 
           <<  ", "
           << track->GetPosition().z()  
           << " ),   Momentum = ( " 
           << track->GetMomentum().x() 
           << ", " 
           << track->GetMomentum().y() 
           << ", "
           << track->GetMomentum().z() 
           << " )" << endmsg;
    info() << "*******************************************************";
  } else {
    info() << "* G4Track Information: "
           << "   Track ID = " << track->GetTrackID();
  }
  info() << endmsg;
  
}

