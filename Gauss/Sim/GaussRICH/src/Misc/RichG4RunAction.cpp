/// GaudiKernel
#include "GaudiKernel/PropertyMgr.h"
/// GiGa 
#include "GiGa/GiGaMACROs.h"
/// G4 
#include "G4UImanager.hh"
/// Local 
#include "RichG4RunAction.h"
#include "RichG4Histo.h"

// ============================================================================
// SE 21-8-2002
// ============================================================================

// ============================================================================
/// Factory business
// ============================================================================
IMPLEMENT_GiGaFactory( RichG4RunAction ) ;
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
RichG4RunAction::RichG4RunAction
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
RichG4RunAction::~RichG4RunAction()
{
  m_beginCmds .clear();
  m_endCmds   .clear();
};

// ============================================================================
/** performe the action at the begin of each run 
 *  @param run pointer to Geant4 run object 
 */
// ============================================================================
void RichG4RunAction::BeginOfRunAction( const G4Run* run )
{
  if( 0 == run ) 
    { Warning("BeginOfRunAction:: G4Run* points to NULL!") ; }

  /// get Geant4 UI manager 
  //  G4UImanager* ui = G4UImanager::GetUIpointer() ;
  // if( 0 == ui    ) 
  //  { Error("BeginOfRunAction:: G4UImanager* points to NULL!") ; return ; }
  // else 
  //  {
  //    for( COMMANDS::const_iterator iCmd = m_beginCmds.begin() ;
  //         m_beginCmds.end() != iCmd ; ++iCmd ) 
  //      { 
  //        Print("BeginOfRunAction(): execute '" + (*iCmd) + "'" , 
  //              StatusCode::SUCCESS                             , MSG::DEBUG );
  //        ui->ApplyCommand( *iCmd ); 
  //      }
  //  }

    m_aRichG4Histo= new RichG4Histo(svcLoc());

};
// ============================================================================

// ============================================================================
/** performe the action at the end of each run 
 *  @param run pointer to Geant4 run object 
 */
// ============================================================================
void RichG4RunAction::EndOfRunAction( const G4Run* run )
{
  if( 0 == run ) 
    { Warning("EndOfRunAction:: G4Run* points to NULL!") ; }
  /// get Geant4 UI manager 
  //  G4UImanager* ui = G4UImanager::GetUIpointer() ;
  // if( 0 == ui    ) 
  //  { Error("EndOfRunAction:: G4UImanager* points to NULL!") ; }
  // else
  //  { 
  //    for( COMMANDS::const_iterator iCmd = m_endCmds.begin() ;
  //          m_endCmds.end() != iCmd ; ++iCmd ) 
  //      { 
  //        Print("EndOfRunAction(): execute '" + (*iCmd) + "'" , 
  //              StatusCode::SUCCESS                           , MSG::DEBUG );
  //        ui->ApplyCommand( *iCmd ); 
  //      }
  //  }  
};
// ============================================================================

// ============================================================================
// The End
// ============================================================================

