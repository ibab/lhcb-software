// $Id: RichG4RunAction.cpp,v 1.10 2009-07-03 11:59:49 seaso Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

// G4
#include "G4UImanager.hh"

// Local
#include "RichG4RunAction.h"
#include "GaussRICH/RichG4HistoDefineSet1.h"
#include "RichG4HistoDefineSet2.h"
#include "GaussRICH/RichG4HistoDefineSet3.h"
#include "RichG4HistoDefineSet4.h"
#include "RichG4HistoDefineSet5.h"

#include "GaussRICH/RichG4HistoDefineTimer.h"
#include "GaussRICH/RichG4Counters.h"
#include "GaussRICH/RichG4GaussPathNames.h"
#include "GaussRICH/RichG4MatRadIdentifier.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichG4RunAction
//
// 2002-08-21 : Sajan Easo
// 2007-01-11 : Gloria Corti, modified for Gaudi v19
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( RichG4RunAction );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichG4RunAction::RichG4RunAction
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : GiGaRunActionBase( type , name , parent )
  , m_beginCmds ()   //  empty default list!
  , m_endCmds   ()   //  empty default list!
  , m_defineRichG4HistoSet1(false)
  , m_defineRichG4HistoSet2(false)
  , m_defineRichG4HistoSet3(false)
  , m_defineRichG4HistoSet4(false)
  , m_defineRichG4HistoSet5(false)
  , m_defineRichG4HistoTimer(false)
  , m_aRichG4HistoSet1(0)
  , m_aRichG4HistoSet2(0)
  , m_aRichG4HistoSet3(0)
  , m_aRichG4HistoSet4(0)
  , m_aRichG4HistoTimer(0),
    m_FirstTimeOfBeginRichRun(true)

{
  declareProperty("BeginOfRunCommands", m_beginCmds );
  declareProperty("EndOfRunCommands"  , m_endCmds   );
  declareProperty("DefineRichG4HistoSet1", m_defineRichG4HistoSet1);
  declareProperty("DefineRichG4HistoSet2", m_defineRichG4HistoSet2);
  declareProperty("DefineRichG4HistoSet3", m_defineRichG4HistoSet3);
  declareProperty("DefineRichG4HistoSet4", m_defineRichG4HistoSet4);
  declareProperty("DefineRichG4HistoSet5", m_defineRichG4HistoSet5);
  declareProperty("DefineRichG4HistoTimer",  m_defineRichG4HistoTimer);


}

//=============================================================================
// Destructor
//=============================================================================
RichG4RunAction::~RichG4RunAction()
{
  m_beginCmds .clear();
  m_endCmds   .clear();
}

//=============================================================================
// performe the action at the begin of each run
//=============================================================================
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
  //  The rich specific histo booking and counters are done only
  // at the first time this BeginRunAction is called.

  if(  m_FirstTimeOfBeginRichRun ) {

    // Also set the radiatormaterial numbers
    
    //  RichG4MatRadIdentifier* aRichG4MatRadIdentifier = RichG4MatRadIdentifier::RichG4MatRadIdentifierInstance();

  if(m_defineRichG4HistoSet1) {

    m_aRichG4HistoSet1 = new RichG4HistoDefineSet1();
  }
  if(m_defineRichG4HistoSet2) {

    m_aRichG4HistoSet2 = new RichG4HistoDefineSet2();
  }

  if(m_defineRichG4HistoSet3) {

    m_aRichG4HistoSet3 = new RichG4HistoDefineSet3();
  }
  if(m_defineRichG4HistoSet4) {

    m_aRichG4HistoSet4 = new RichG4HistoDefineSet4();
  }
  if(m_defineRichG4HistoSet5) {

    m_aRichG4HistoSet5 = new RichG4HistoDefineSet5();
  }


  if(m_defineRichG4HistoTimer) {

    m_aRichG4HistoTimer = new  RichG4HistoDefineTimer();

  }

  // Now to create the RichCounters.
  // done in Event action.
  // RichG4Counters* aRichCounter=  RichG4Counters::getInstance();

  m_FirstTimeOfBeginRichRun= false;
  }
  
}


//=============================================================================
// performe the action at the end of each run
//=============================================================================
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
}


//=============================================================================

