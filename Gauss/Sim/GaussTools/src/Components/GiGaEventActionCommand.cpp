// $Id: GiGaEventActionCommand.cpp,v 1.2 2007-01-12 15:36:44 ranjard Exp $ 
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiKernel/PropertyMgr.h"

// G4 
#include "G4UImanager.hh"

// Local 
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

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( GiGaEventActionCommand )

// ============================================================================
/** standard constructor 
 *  @see GiGaBase 
 *  @see AlgTool 
 *  @param type type of the object (?)
 *  @param name name of the object
 *  @param parent  pointer to parent object
 */
// ============================================================================
GiGaEventActionCommand::GiGaEventActionCommand
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : GiGaEventActionBase( type , name , parent )
  , m_beginCmds ()   //  empty default list! 
  , m_endCmds   ()   //  empty default list! 
{  
  declareProperty("BeginOfEventCommands", m_beginCmds );
  declareProperty("EndOfEventCommands"  , m_endCmds   );
}


// ============================================================================
/// destructor 
// ============================================================================
GiGaEventActionCommand::~GiGaEventActionCommand()
{
  m_beginCmds .clear();
  m_endCmds   .clear();
}

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
          Print("BeginOfEventAction(): execute '" + (*iCmd) + "'" , 
                StatusCode::SUCCESS , MSG::DEBUG );
          ui->ApplyCommand( *iCmd); 
        }
    }
  ///
}


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
          Print("EndOfEventAction(): execute '" + (*iCmd) + "'" ,
                StatusCode::SUCCESS , MSG::DEBUG );
          ui->ApplyCommand( *iCmd ); 
        }
    }  
  ///
}


// ============================================================================
// The END 
// ============================================================================

