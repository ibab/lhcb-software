/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.2  2001/07/27 14:29:01  ibelyaev
/// bug fix
///
/// Revision 1.1  2001/07/26 16:09:22  ibelyaev
/// GiGaRunActions budiness
///
/// 
/// ===========================================================================
/// GaudiKernel
#include "GaudiKernel/PropertyMgr.h"
/// GiGa 
#include "GiGa/GiGaRunActionFactory.h"
/// G4 
#include "G4UImanager.hh"
/// Local 
#include "GiGaRunActionCommand.h"

/// ===========================================================================
/** Implementation file for class : GiGaRunActionCommand
 *
 *   @author Vanya  Belyaev
 *   @date 25/07/2001 
 */
/// ===========================================================================

/// ===========================================================================
/// Factory business
/// ===========================================================================
static const GiGaRunActionFactory<GiGaRunActionCommand>         s_Factory;
const       IGiGaRunActionFactory&GiGaRunActionCommandFactory = s_Factory;

/// ===========================================================================
/** standard constructor
 *  @param Name  name of this concrete event action instance 
 *  @param Loc   pointer to service locator 
 */
/// ===========================================================================
GiGaRunActionCommand::GiGaRunActionCommand( const std::string& Name ,
                                                ISvcLocator*       Loc  )
  : GiGaRunActionBase( Name, Loc )
  , m_beginCmds ()   ///< empty default list! 
  , m_endCmds   ()   ///< empty default list! 
{  
  declareProperty("BeginOfRunCommands", m_beginCmds );
  declareProperty("EndOfRunCommands"  , m_endCmds   );
};

/// ===========================================================================
/// destructor 
/// ===========================================================================
GiGaRunActionCommand::~GiGaRunActionCommand()
{
  m_beginCmds .clear();
  m_endCmds   .clear();
};

/// ===========================================================================
/** initialization of event action object
 *  @return status code
 */
/// ===========================================================================
StatusCode GiGaRunActionCommand::initialize()
{
  StatusCode sc = GiGaRunActionBase::initialize();
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

/// ===========================================================================
/** finalization of event action object
 *  @return status code
 */
/// ===========================================================================
StatusCode GiGaRunActionCommand::finalize()
{
  ///
  Print("finalization");
  /// finalize the base class 
  return GiGaRunActionBase::finalize();
};

/// ===========================================================================
/** performe the action at the begin of each run 
 *  @param run pointer to Geant4 run object 
 */
/// ===========================================================================
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
          Print("BeginOfRunAction(): execute '"+(*iCmd)+"'");
          ui->ApplyCommand( *iCmd ); 
        }
    }
  ///
};

/// ===========================================================================
/** performe the action at the end of each run 
 *  @param run pointer to Geant4 run object 
 */
/// ===========================================================================
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
          Print("EndOfRunAction(): execute '"+(*iCmd)+"'");
          ui->ApplyCommand( *iCmd ); 
        }
    }  
  ///
};

/// ===========================================================================

