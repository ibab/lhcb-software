/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log: not supported by cvs2svn $ 
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
{ return GiGaEventActionBase::initialize() ; } ;

/// ===========================================================================
/// ===========================================================================
StatusCode GiGaEventActionEmpty::finalize   () 
{ return GiGaEventActionBase::initialize() ; } ;

/// ===========================================================================
/// ===========================================================================
void GiGaEventActionEmpty::BeginOfEventAction ( const G4Event* /* event */ ) 
{ Print(" BeginOfEventAction() method is invoked"); };

/// ===========================================================================
/// ===========================================================================
void GiGaEventActionEmpty::EndOfEventAction   ( const G4Event* /* event */ ) 
{ Print(" EndOfEventAction() method is invoked"); };

/// ===========================================================================
