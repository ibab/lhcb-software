/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log: not supported by cvs2svn $ 
/// ===========================================================================
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

/// ===========================================================================
/// ===========================================================================
static const GiGaStackActionFactory<GiGaStackActionEmpty>         s_Factory;
const       IGiGaStackActionFactory&GiGaStackActionEmptyFactory = s_Factory;

/// ===========================================================================
/// ===========================================================================
GiGaStackActionEmpty::GiGaStackActionEmpty( const std::string& Name , 
                                            ISvcLocator* Loc ) 
  : GiGaStackActionBase( Name , Loc ) 
{};

/// ===========================================================================
/// ===========================================================================
GiGaStackActionEmpty::~GiGaStackActionEmpty(){};

/// ===========================================================================
/// ===========================================================================
StatusCode GiGaStackActionEmpty::initialize () 
{ return GiGaStackActionBase::initialize() ; } ;

/// ===========================================================================
/// ===========================================================================
StatusCode GiGaStackActionEmpty::finalize   () 
{ return GiGaStackActionBase::initialize() ; } ;

/// ===========================================================================
/// ===========================================================================
G4ClassificationOfNewTrack 
GiGaStackActionEmpty::ClassifyNewTrack ( const G4Track* /* track */ )
{
  Print("ClassifyNewTrack method in invoked", 
        StatusCode::SUCCESS , MSG::DEBUG ) ;
  return fUrgent; 
};

/// ===========================================================================
/// ===========================================================================
void GiGaStackActionEmpty::NewStage         ()
{ Print("New Stage method in invoked"       , 
        StatusCode::SUCCESS , MSG::DEBUG ) ; }; 

/// ===========================================================================
/// ===========================================================================
void GiGaStackActionEmpty::PrepareNewEvent  ()
{  Print("PrepareNewEvent method is invoked" , 
         StatusCode::SUCCESS , MSG::DEBUG ) ; };

/// ===========================================================================
