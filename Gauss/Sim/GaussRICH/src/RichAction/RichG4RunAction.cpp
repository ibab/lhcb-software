/// GaudiKernel
#include "GaudiKernel/PropertyMgr.h"
/// GiGa 
#include "GiGa/GiGaMACROs.h"
/// G4 
#include "G4UImanager.hh"
/// Local 
#include "RichG4RunAction.h"
#include "RichG4HistoDefineSet1.h"
#include "RichG4HistoDefineTimer.h"
#include "RichG4Counters.h"

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
  , m_defineRichG4HistoSet1(false)
    , m_defineRichG4HistoTimer(false)
    , m_aRichG4HistoSet1(0)
     ,  m_aRichG4HistoTimer(0)
{  
  declareProperty("BeginOfRunCommands", m_beginCmds );
  declareProperty("EndOfRunCommands"  , m_endCmds   );
  declareProperty("DefineRichG4HistoSet1", m_defineRichG4HistoSet1);
  declareProperty("DefineRichG4HistoTimer",  m_defineRichG4HistoTimer);
  

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
  // The part for interactive runnign is commented out.
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

  if(m_defineRichG4HistoSet1) {
    
    m_aRichG4HistoSet1 = new RichG4HistoDefineSet1();
  }

  if(m_defineRichG4HistoTimer) {
    
    m_aRichG4HistoTimer = new  RichG4HistoDefineTimer();
    
  }
 
 
  // Now to create the RichCounters.
 
  RichG4Counters* aRichCounter=  RichG4Counters::getInstance();

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
  // the part for the interactive running of G4 commented out.
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

