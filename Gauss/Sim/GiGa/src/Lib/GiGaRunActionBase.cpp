// $Id: GiGaRunActionBase.cpp,v 1.4 2002-04-09 17:16:50 ibelyaev Exp $ 
// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.3  2001/08/12 15:42:49  ibelyaev
/// improvements with Doxygen comments
///
/// Revision 1.2  2001/07/27 17:03:18  ibelyaev
/// improved printout
///
/// Revision 1.1  2001/07/26 14:24:46  ibelyaev
/// new base class for Run Action
/// 
// ============================================================================
/// GiGa 
#include "GiGa/GiGaRunActionBase.h"

// ============================================================================
/** @file
 *
 *  Implementation file for class : GiGaRunActionBase
 * 
 *  @date 26/07/2001 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 */
// ============================================================================

// ============================================================================
/** standard constructor
 *  @param Name name of this concrete instance of Run Action Object 
 *  @param Loc  pointer to Service Locator 
 */
// ============================================================================
GiGaRunActionBase::GiGaRunActionBase( const std::string Name ,
                                      ISvcLocator*      Loc  )
  : GiGaBase( Name , Loc )
{};

// ============================================================================
/// destructor
// ============================================================================
GiGaRunActionBase::~GiGaRunActionBase()
{};

// ============================================================================
/** identification 
 *  @return name of concrete inteface instance 
 */
// ============================================================================
const std::string& GiGaRunActionBase::name() const 
{ return GiGaBase::name() ; }

// ============================================================================
/** initialization method 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaRunActionBase::initialize  ()
{ 
  StatusCode sc = GiGaBase::initialize();   
  if( sc.isFailure() ) 
    { return Error("Could not initialize base class!",sc); }
  ///
  Print("GiGaRunActionBase initialized successfully" ,
        StatusCode::SUCCESS , MSG::DEBUG ) ;
  ///
  return StatusCode::SUCCESS;
};

// ============================================================================
/** finalization method 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaRunActionBase::finalize  ()
{
  /// 
  Print("GiGaRunActionBase finalization" ,
        StatusCode::SUCCESS , MSG::DEBUG ) ;
  ///
  return GiGaBase::finalize(); 
};

// ============================================================================
/** perform begin-of-run action
 *  @param run  pointer to Geant4 Run object
 */
// ============================================================================
void GiGaRunActionBase::BeginOfRunAction ( const G4Run* run )
{
  Print("'BeginOfRunAction' method", StatusCode::SUCCESS , MSG::DEBUG );  
  if( 0 == run ) { Warning("'BeginOfRunAction' G4Run* points to NULL!"); }
};

// ============================================================================
/** perform end-of-run action
 *  @param run  pointer to Geant4 Run object
 */
// ============================================================================
void GiGaRunActionBase::EndOfRunAction ( const G4Run* run )
{
  Print("'EndOfRunAction' method", StatusCode::SUCCESS , MSG::DEBUG );  
  if( 0 == run ) { Warning("'EndOfRunAction' G4Run* points to NULL!"); }
};

// ============================================================================
// The END 
// ============================================================================

