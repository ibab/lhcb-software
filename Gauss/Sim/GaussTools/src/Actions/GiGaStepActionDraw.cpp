// $Id: GiGaStepActionDraw.cpp,v 1.1 2002-09-26 18:10:53 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.10  2002/05/16 16:14:48  witoldp
// reversed colours for drawing charged particles
//
// Revision 1.9  2002/05/07 12:21:36  ibelyaev
//  see $GIGAROOT/doc/release.notes  7 May 2002
//
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
#include "GiGa/GiGaMACROs.h"
// local
#include "GiGaStepActionDraw.h"

/** @file 
 *  implementation of GiGaStepActionDraw 
 *  @author Vanya Belyaev Ivan.Belyave@itep.ru
 */

// ============================================================================
/// factory
// ============================================================================
IMPLEMENT_GiGaFactory( GiGaStepActionDraw );
// ============================================================================

// ============================================================================
/** standard constructor 
 *  @see GiGaStepActionBase 
 *  @see GiGaBase 
 *  @see AlgTool 
 *  @param type type of the object (?)
 *  @param name name of the object
 *  @param parent  pointer to parent object
 */
// ============================================================================
GiGaStepActionDraw::GiGaStepActionDraw
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent ) 
  : GiGaStepActionBase ( type , name , parent ) {};
// ============================================================================

// ============================================================================
/// destructor 
// ============================================================================
GiGaStepActionDraw::~GiGaStepActionDraw(){};
// ============================================================================

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
	( charge > 0.0 ) ? G4Colour( 1. , 0. , 0. ) : 
	( charge < 0.0 ) ? G4Colour( 0. , 0. , 1. ) : G4Colour( 0. , 1. , 0. ) ;
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

// ============================================================================
// The END 
// ============================================================================





