// $Id: GiGaRunActionCommand.cpp,v 1.1 2002-12-12 15:19:31 witoldp Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2002/09/26 18:10:52  ibelyaev
//  repackageing: add all concrete implementations from GiGa
//
// Revision 1.8  2002/05/07 12:21:35  ibelyaev
//  see $GIGAROOT/doc/release.notes  7 May 2002
//
// ============================================================================
/// GaudiKernel
#include "GaudiKernel/PropertyMgr.h"
/// GiGa 
#include "GiGa/GiGaMACROs.h"
/// G4 
#include "G4UImanager.hh"
/// Local 
#include "GiGaRunActionCommand.h"

// ============================================================================
/** @file 
 *
 *  Implementation file for class : GiGaRunActionCommand
 *
 *  @author Vanya  Belyaev Ivan.Belyaev@itep.ru
 *  @date 25/07/2001 
 */
// ============================================================================

// ============================================================================
/// Factory business
// ============================================================================
IMPLEMENT_GiGaFactory( GiGaRunActionCommand ) ;
// ============================================================================

// ============================================================================
/** standard constructor 
 *  @see GiGaPhysListBase
 *  @see GiGaBase 
 *  @see AlgTool 
 *  @param type type of the object (?)
 *  @param name name of the object
 *  @param parent  pointer to parent object
 */
// ============================================================================
GiGaRunActionCommand::GiGaRunActionCommand
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent ) 
  : GiGaRunActionBase( type , name , parent )
  , m_beginCmds ()   //  empty default list! 
  , m_endCmds   ()   //  empty default list! 
{  
  declareProperty("BeginOfRunCommands", m_beginCmds );
  declareProperty("EndOfRunCommands"  , m_endCmds   );
};
// ============================================================================

// ============================================================================
/// destructor 
// ============================================================================
GiGaRunActionCommand::~GiGaRunActionCommand()
{
  m_beginCmds .clear();
  m_endCmds   .clear();
};

// ============================================================================
/** performe the action at the begin of each run 
 *  @param run pointer to Geant4 run object 
 */
// ============================================================================
void GiGaRunActionCommand::BeginOfRunAction( const G4Run* run )
{
  if( 0 == run ) 
    { Warning("BeginOfRunAction:: G4Run* points to NULL!") ; }
  /// get Geant4 UI manager 
  G4UImanager* ui = G4UImanager::GetUIpointer() ;
  if( 0 == ui    ) 
    { Error("BeginOfRunAction:: G4UImanager* points to NULL!") ; return ; }
  else 
    {
      for( COMMANDS::const_iterator iCmd = m_beginCmds.begin() ;
           m_beginCmds.end() != iCmd ; ++iCmd ) 
        { 
          Print("BeginOfRunAction(): execute '" + (*iCmd) + "'" , 
                StatusCode::SUCCESS                             , MSG::DEBUG );
          ui->ApplyCommand( *iCmd ); 
        }
    }
};
// ============================================================================

// ============================================================================
/** performe the action at the end of each run 
 *  @param run pointer to Geant4 run object 
 */
// ============================================================================
void GiGaRunActionCommand::EndOfRunAction( const G4Run* run )
{
  if( 0 == run ) 
    { Warning("EndOfRunAction:: G4Run* points to NULL!") ; }
  /// get Geant4 UI manager 
  G4UImanager* ui = G4UImanager::GetUIpointer() ;
  if( 0 == ui    ) 
    { Error("EndOfRunAction:: G4UImanager* points to NULL!") ; }
  else
    { 
      for( COMMANDS::const_iterator iCmd = m_endCmds.begin() ;
           m_endCmds.end() != iCmd ; ++iCmd ) 
        { 
          Print("EndOfRunAction(): execute '" + (*iCmd) + "'" , 
                StatusCode::SUCCESS                           , MSG::DEBUG );
          ui->ApplyCommand( *iCmd ); 
        }
    }  
};
// ============================================================================

// ============================================================================
// The End
// ============================================================================

