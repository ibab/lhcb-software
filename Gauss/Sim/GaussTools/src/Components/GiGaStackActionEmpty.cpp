// $Id: GiGaStackActionEmpty.cpp,v 1.1 2002-12-12 15:19:32 witoldp Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2002/09/26 18:10:53  ibelyaev
//  repackageing: add all concrete implementations from GiGa
//
// Revision 1.8  2002/05/07 12:21:36  ibelyaev
//  see $GIGAROOT/doc/release.notes  7 May 2002
//
// ============================================================================
//
#include "G4Step.hh"
//
#include "GaudiKernel/MsgStream.h"
// GiGa 
#include "GiGa/GiGaMACROs.h"
// local
#include "GiGaStackActionEmpty.h"

/** @file 
 * 
 * Implementation fo class GiGaStackActionEmpty
 *  
 * @author Vanya Belyaev 
 */

// ============================================================================
// Factory
// ============================================================================
IMPLEMENT_GiGaFactory( GiGaStackActionEmpty ) ;

// ============================================================================
/** standard constructor 
 *  @see GiGaStackActionBase 
 *  @see GiGaBase 
 *  @see AlgTool 
 *  @param type type of the object (?)
 *  @param name name of the object
 *  @param parent  pointer to parent object
 */
// ============================================================================
GiGaStackActionEmpty::GiGaStackActionEmpty
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent ) 
  : GiGaStackActionBase( type , name , parent ){};
// ============================================================================

// ============================================================================
/// destructor 
// ============================================================================
GiGaStackActionEmpty::~GiGaStackActionEmpty(){};
// ============================================================================

// ============================================================================
/// G4 
// ============================================================================
G4ClassificationOfNewTrack 
GiGaStackActionEmpty::ClassifyNewTrack ( const G4Track* /* track */ )
{
  Print("ClassifyNewTrack method in invoked" , 
        StatusCode::SUCCESS                  , MSG::VERBOSE ) ;
  ///
  return fUrgent; 
};

// ============================================================================
/// G4 
// ============================================================================
void GiGaStackActionEmpty::NewStage         ()
{ Print("New Stage method in invoked"       , 
        StatusCode::SUCCESS                 , MSG::VERBOSE ) ; }; 

// ============================================================================
/// G4 
// ============================================================================
void GiGaStackActionEmpty::PrepareNewEvent  ()
{  Print("PrepareNewEvent method is invoked" , 
         StatusCode::SUCCESS                 , MSG::VERBOSE ) ; };

// ============================================================================
// The END 
// ============================================================================
