/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $
/// ===========================================================================
/// $Log: not supported by cvs2svn $ 
/// ===========================================================================
/// GiGa 
#include "GiGa/GiGaRunActionBase.h"

/// ===========================================================================
/** Implementation file for class : GiGaRunActionBase
 * 
 *  @date 26/07/2001 
 *  @author Vanya Belyaev
 */
/// ===========================================================================

/// ===========================================================================
/** standard constructor
 *  @param Name name of this concrete instance of Run Action Object 
 *  @param Loc  pointer to Service Locator 
 */
/// ===========================================================================
GiGaRunActionBase::GiGaRunActionBase( const std::string Name ,
                                      ISvcLocator*      Loc  )
  : GiGaBase( Name , Loc )
{};

/// ===========================================================================
/// destructor
/// ===========================================================================
GiGaRunActionBase::~GiGaRunActionBase()
{};

/// ===========================================================================
/** identification 
 *  @return name of concrete inteface instance 
 */
/// ===========================================================================
const std::string& GiGaRunActionBase::name() const 
{ return GiGaBase::name() ; }

/// ===========================================================================
/** initialization method 
 *  @return status code 
 */
/// ===========================================================================
StatusCode GiGaRunActionBase::initialize  ()
{ return GiGaBase::initialize(); }

/// ===========================================================================
/** finalization method 
 *  @return status code 
 */
/// ===========================================================================
StatusCode GiGaRunActionBase::finalize  ()
{ return GiGaBase::finalize(); }

/// ===========================================================================
/** perform begin-of-run action
 *  @param run  pointer to Geant4 Run object
 */
/// ===========================================================================
void GiGaRunActionBase::BeginOfRunAction ( const G4Run* run )
{
  Print("'BeginOfRunAction' method", StatusCode::SUCCESS , MSG::DEBUG );  
  if( 0 == run ) { Warning("'BeginOfRunAction' G4Run* points to NULL!"); }
};

/// ===========================================================================
/** perform end-of-run action
 *  @param run  pointer to Geant4 Run object
 */
/// ===========================================================================
void GiGaRunActionBase::EndOfRunAction ( const G4Run* run )
{
  Print("'EndOfRunAction' method", StatusCode::SUCCESS , MSG::DEBUG );  
  if( 0 == run ) { Warning("'EndOfRunAction' G4Run* points to NULL!"); }
};

/// ===========================================================================

