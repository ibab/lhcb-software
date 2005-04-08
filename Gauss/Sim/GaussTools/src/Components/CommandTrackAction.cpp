/// Geant4 
#include "G4Track.hh"
#include "G4UImanager.hh"
/// GaudiKernel
#include "GaudiKernel/PropertyMgr.h"
/// GiGa 
#include "GiGa/GiGaMACROs.h"
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
IMPLEMENT_GiGaFactory( CommandTrackAction );
// ============================================================================

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
  : GiGaTrackActionBase( type , name , parent ), m_trackid(1)
{
  m_precommand.push_back("/tracking/verbose 1");
  m_postcommand.push_back("/tracking/verbose 0");
  
  declareProperty("TrackID", m_trackid);
  declareProperty("PreCommand", m_precommand);  
  declareProperty("PostCommand", m_postcommand);
};
// ============================================================================

// ============================================================================
/// destructor 
// ============================================================================
CommandTrackAction::~CommandTrackAction()
{
};
// ============================================================================

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
  m_test=false;
  return Print("Initialized successfully" , 
               StatusCode::SUCCESS        , MSG::VERBOSE);
} ;
// ============================================================================

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
};
// ============================================================================

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

  if (track->GetTrackID()==m_trackid) {
    m_test = true;
    for( std::vector<std::string>::const_iterator iCmd = m_precommand.begin();
         m_precommand.end() != iCmd; ++iCmd ) {
      ui->ApplyCommand(*iCmd);
    }
  }
  else if (m_test) {
    m_test = false;
    for(std::vector<std::string>::const_iterator jCmd = m_postcommand.begin(); 
        m_postcommand.end() != jCmd; ++jCmd ) {
      ui->ApplyCommand(*jCmd);
    }
  }
};

// ============================================================================
/**  perform action 
 *  @see G4UserTrackingAction
 *   @param pointer to  track object 
 */
// ============================================================================
void CommandTrackAction::PostUserTrackingAction ( const G4Track* ) 
{
}; 
// ============================================================================












