// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.5  2001/07/27 17:03:21  ibelyaev
/// improved printout
///
/// Revision 1.4  2001/07/23 13:12:28  ibelyaev
/// the package restructurisation(II)
/// 
// ============================================================================
//
#include "G4Step.hh"
//
#include "GaudiKernel/MsgStream.h"
// GiGa 
#include "GiGa/GiGaStackActionFactory.h"
// local
#include "GiGaStackActionEmpty.h"

/** Implementation fo class GiGaStackActionEmpty
 *  
 * @author Vanya Belyaev 
 */

// ============================================================================
// ============================================================================
static const GiGaStackActionFactory<GiGaStackActionEmpty>         s_Factory;
const       IGiGaStackActionFactory&GiGaStackActionEmptyFactory = s_Factory;

// ============================================================================
// ============================================================================
GiGaStackActionEmpty::GiGaStackActionEmpty( const std::string& Name , 
                                            ISvcLocator* Loc ) 
  : GiGaStackActionBase( Name , Loc ) 
{};

// ============================================================================
// ============================================================================
GiGaStackActionEmpty::~GiGaStackActionEmpty(){};

// ============================================================================
// ============================================================================
StatusCode GiGaStackActionEmpty::initialize () 
{ 
  /// initialize the base class 
  StatusCode sc = GiGaStackActionBase::initialize() ; 
  if( sc.isFailure() )
    { return Error("Could not initialize the base class ",sc); }
  ///
  Print("initialized succesfully");
  ///
  return StatusCode::SUCCESS;
} ;

// ============================================================================
// ============================================================================
StatusCode GiGaStackActionEmpty::finalize   () 
{ 
  ///
  Print("finalization");
  ///
  return GiGaStackActionBase::initialize() ; 
} ;

// ============================================================================
// ============================================================================
G4ClassificationOfNewTrack 
GiGaStackActionEmpty::ClassifyNewTrack ( const G4Track* /* track */ )
{
  ///
  Print("ClassifyNewTrack method in invoked", 
        StatusCode::SUCCESS , MSG::DEBUG ) ;
  ///
  return fUrgent; 
};

// ============================================================================
// ============================================================================
void GiGaStackActionEmpty::NewStage         ()
{ Print("New Stage method in invoked"       , 
        StatusCode::SUCCESS , MSG::DEBUG ) ; }; 

// ============================================================================
// ============================================================================
void GiGaStackActionEmpty::PrepareNewEvent  ()
{  Print("PrepareNewEvent method is invoked" , 
         StatusCode::SUCCESS , MSG::DEBUG ) ; };

// ============================================================================
