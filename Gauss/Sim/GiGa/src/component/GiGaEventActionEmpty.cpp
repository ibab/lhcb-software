/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.4  2001/07/23 13:12:26  ibelyaev
/// the package restructurisation(II)
/// 
/// ===========================================================================
// GiGa 
#include "GiGa/GiGaEventActionFactory.h"
// local
#include "GiGaEventActionEmpty.h"

/** implementationof class GiGaEventActionEmpty
 * 
 *  @author Vanya Belyaev 
 */

/// ===========================================================================
/// ===========================================================================
static const GiGaEventActionFactory<GiGaEventActionEmpty>         s_Factory;
const       IGiGaEventActionFactory&GiGaEventActionEmptyFactory = s_Factory;

/// ===========================================================================
/// ===========================================================================
GiGaEventActionEmpty::GiGaEventActionEmpty( const std::string& Name , 
                                            ISvcLocator* Loc ) 
  : GiGaEventActionBase( Name , Loc ) 
{};

/// ===========================================================================
/// ===========================================================================
GiGaEventActionEmpty::~GiGaEventActionEmpty(){};

/// ===========================================================================
/// ===========================================================================
StatusCode GiGaEventActionEmpty::initialize () 
{
  /// initialize the base class 
  StatusCode sc = GiGaEventActionBase::initialize() ; 
  if( sc.isFailure() ) 
    { return Error("Could not initialize the base class",sc); }
  ///
  Print("initialized succesfully");
  ///
  return StatusCode::SUCCESS;
} ;

/// ===========================================================================
/// ===========================================================================
StatusCode GiGaEventActionEmpty::finalize   () 
{ 
  ///
  Print("finalization");
  ///
  return GiGaEventActionBase::initialize() ; 
} ;

/// ===========================================================================
/// ===========================================================================
void GiGaEventActionEmpty::BeginOfEventAction ( const G4Event* /* event */ ) 
{ Print(" BeginOfEventAction() method is invoked"); };

/// ===========================================================================
/// ===========================================================================
void GiGaEventActionEmpty::EndOfEventAction   ( const G4Event* /* event */ ) 
{ Print(" EndOfEventAction() method is invoked"); };

/// ===========================================================================
