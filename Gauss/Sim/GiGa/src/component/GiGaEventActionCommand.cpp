// $Id: GiGaEventActionCommand.cpp,v 1.7 2002-04-25 13:02:04 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
/// GaudiKernel
#include "GaudiKernel/PropertyMgr.h"
/// GiGa 
#include "GiGa/GiGaEventActionFactory.h"
/// G4 
#include "G4UImanager.hh"
/// Local 
#include "GiGaEventActionCommand.h"

// ============================================================================
/**  @file 
 *
 *   Implementation file for class : GiGaEventActionCommand
 *
 *   @author Vanya  Belyaev Ivan.Belyaev@itep.ru
 *   @date 25/07/2001 
 */
// ============================================================================

// ============================================================================
// Factory business
// ============================================================================
IMPLEMENT_GiGaEventAction( GiGaEventActionCommand );

// ============================================================================
/** standard constructor
 *  @param Name  name of this concrete event action instance 
 *  @param Loc   pointer to service locator 
 */
// ============================================================================
GiGaEventActionCommand::GiGaEventActionCommand( const std::string& Name ,
                                                ISvcLocator*       Loc  )
  : GiGaEventActionBase( Name, Loc )
  , m_beginCmds ()   ///< empty default list! 
  , m_endCmds   ()   ///< empty default list! 
{  
  declareProperty("BeginOfEventCommands", m_beginCmds );
  declareProperty("EndOfEventCommands"  , m_endCmds   );
};

// ============================================================================
/// destructor 
// ============================================================================
GiGaEventActionCommand::~GiGaEventActionCommand()
{
  m_beginCmds .clear();
  m_endCmds   .clear();
};

// ============================================================================
/** initialization of event action object
 *  @return status code
 */
// ============================================================================
StatusCode GiGaEventActionCommand::initialize()
{
  StatusCode sc = GiGaEventActionBase::initialize();
  if( sc.isFailure() ) 
    { return Error("Could not initialize the base class", sc );}
  ///
  if( m_beginCmds.empty() ) 
    { Print("'BeginOfEventCommands' list is empty") ;}
  if( m_endCmds  .empty() ) 
    { Print("'EndOfEventCommands' list is empty"  ) ;}
  ///
  Print("initialized succesfully");
  ///
  return StatusCode::SUCCESS;
};

// ============================================================================
/** finalization of event action object
 *  @return status code
 */
// ============================================================================
StatusCode GiGaEventActionCommand::finalize()
{
  ///
  Print("finalization");
  /// finalize the base class 
  return GiGaEventActionBase::finalize();
};

// ============================================================================
/** performe the action at the begin of each event 
 *  @param event pointer to Geant4 event object 
 */
// ============================================================================
void GiGaEventActionCommand::BeginOfEventAction( const G4Event* event )
{
  if( 0 == event ) 
    { Warning("BeginOfEventAction:: G4Event* points to NULL!") ; }
  /// get Geant4 UI manager 
  G4UImanager* ui = G4UImanager::GetUIpointer() ;
  if( 0 == ui    ) 
    { Error("BeginOfEventAction:: G4UImanager* points to NULL!") ; return ; }
  else 
    {
      for( COMMANDS::const_iterator iCmd = m_beginCmds.begin() ;
           m_beginCmds.end() != iCmd ; ++iCmd ) 
        { 
          Print("BeginOfEventAction(): execute '"+(*iCmd)+"'");
          ui->ApplyCommand( *iCmd); 
        }
    }
  ///
};

// ============================================================================
/** performe the action at the end of each event 
 *  @param event pointer to Geant4 event object 
 */
// ============================================================================
void GiGaEventActionCommand::EndOfEventAction( const G4Event* event )
{
  if( 0 == event ) 
    { Warning("EndOfEventAction:: G4Event* points to NULL!") ; }
  /// get Geant4 UI manager 
  G4UImanager* ui = G4UImanager::GetUIpointer() ;
  if( 0 == ui    ) 
    { Error("EndOfEventAction:: G4UImanager* points to NULL!") ; }
  else
    { 
      for( COMMANDS::const_iterator iCmd = m_endCmds.begin() ;
           m_endCmds.end() != iCmd ; ++iCmd )
        { 
          Print("EndOfEventAction(): execute '"+(*iCmd)+"'");
          ui->ApplyCommand( *iCmd ); 
        }
    }  
  ///
};

// ============================================================================
// The END 
// ============================================================================

