// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.6  2001/07/27 17:03:21  ibelyaev
/// improved printout
///
/// Revision 1.5  2001/07/23 13:12:28  ibelyaev
/// the package restructurisation(II)
///
/// Revision 1.4  2001/07/15 20:54:34  ibelyaev
/// package restructurisation
/// 
// ============================================================================
#define GIGA_GIGASTEPACTIONDRAW_CPP 1 
// ============================================================================
#include "G4Step.hh"
//
#include "G4VVisManager.hh"
#include "G4Polyline.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"
///
#include "GaudiKernel/MsgStream.h"
// GiGa 
#include "GiGa/GiGaStepActionFactory.h"
// local
#include "GiGaStepActionDraw.h"

///
static const GiGaStepActionFactory<GiGaStepActionDraw>          
s_GiGaStepActionDrawFactory;
const       IGiGaStepActionFactory& GiGaStepActionDrawFactory = 
s_GiGaStepActionDrawFactory;
///


// ============================================================================
/** standard constructor 
 *  @param name name of this ocncrete instance
 *  @param Loc  pointer to service locator 
 */
// ============================================================================
GiGaStepActionDraw::GiGaStepActionDraw( const std::string& Name , 
                                        ISvcLocator*       Loc  ) 
  : GiGaStepActionBase            ( Name , Loc ) 
{};

// ============================================================================
/// destructor 
// ============================================================================
GiGaStepActionDraw::~GiGaStepActionDraw(){};

// ============================================================================
/** initialization 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaStepActionDraw::initialize () 
{ 
  /// initialize the base class 
  StatusCode sc = GiGaStepActionBase::initialize() ; 
  if( sc.isFailure() ) 
    { return Error("Could not initialize base class",sc);}
  ///
  Print("initialized succesfully");
  ///
  return StatusCode::SUCCESS;
} ;

// ============================================================================
/** finalization 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaStepActionDraw::finalize   () 
{ 
  ///
  Print("finalisation");
  ///
  return GiGaStepActionBase::initialize() ; 
} ;

// ============================================================================
/** the "main" method
 *  @param aStep  pointer to current step object
 */
// ============================================================================
void GiGaStepActionDraw::UserSteppingAction( const G4Step* step )
{
  /// locate visual manager 
  G4VVisManager* visMgr  = G4VVisManager::GetConcreteInstance();
  ///
  if      ( 0 == visMgr    ) { Error("Could not locate G4VVisManager* !" ) ; }
  else if ( 0 == step      ) { Error("G4Step* points to NULL!"           ) ; } 
  else 
    {
      /// Define a line segment 
      G4double charge = step->GetTrack()->GetDefinition()->GetPDGCharge();
      ///
      G4Colour colour = 
	( charge < 0.0 ) ? G4Colour( 1. , 0. , 0. ) : 
	( charge > 0.0 ) ? G4Colour( 0. , 0. , 1. ) : G4Colour( 0. , 1. , 0. ) ;
      ///
      G4Polyline polyline;
      G4VisAttributes attribs( colour );
      polyline.SetVisAttributes( attribs );
      polyline.push_back( step->GetPreStepPoint  ()->GetPosition () );
      polyline.push_back( step->GetPostStepPoint ()->GetPosition () );
      /// 
      visMgr -> Draw( polyline ); 
      ///
    }  
  ///
};
///





