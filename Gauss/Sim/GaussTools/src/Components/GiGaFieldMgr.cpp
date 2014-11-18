// $Id: GiGaFieldMgr.cpp,v 1.4 2007-01-12 15:36:45 ranjard Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
//#include "GiGa/GiGaMACROs.h"

// GEANT4 
#include "G4ChordFinder.hh"
#include "G4FieldManager.hh"
#include "G4TransportationManager.hh"
#include "G4ChordFinder.hh"

// local 
#include "GiGaFieldMgr.h"

// ============================================================================
/** @file 
 *  Implementation file for class : GiGaFieldMgr
 *
 *  @date 2003-04-03 
 *  @author Vanya  BELYAEV Ivan.Belyaev@itep.ru
 */
// ============================================================================

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( GiGaFieldMgr )

// ============================================================================
/** standard constructor
 *  @see GiGaFieldMgrBase 
 *  @see GiGaBase 
 *  @see AlgTool 
 *  @param type type of the object (?)
 *  @param name name of the object
 *  @param parent  pointer to parent object
 */
// ============================================================================
GiGaFieldMgr::GiGaFieldMgr 
( const std::string& type   , 
  const std::string& name   , 
  const IInterface*  parent )  
  : GiGaFieldMgrBase ( type , name , parent ) 
  , m_fieldType      (       )
  , m_field          ( 0     ) 
{
  declareProperty( "MagneticField" , m_fieldType ) ;
} 


// ============================================================================
/// destructor 
// ============================================================================
GiGaFieldMgr::~GiGaFieldMgr (){} 


// ============================================================================
/** initialize the object 
 *  @see GiGaFieldMgrBase
 *  @see GiGaBase
 *  @see  AlgTool
 *  @see IAlgTool
 *  @return status code 
 */
// ============================================================================
StatusCode   GiGaFieldMgr::initialize     () 
{
  // initialize the base class 
  StatusCode sc = GiGaFieldMgrBase::initialize () ;
  if( sc.isFailure() ) 
    { return Error ( "Base class is not initialized properly!" , sc ) ; }
  
  m_field = tool<IGiGaFieldMgr> ( m_fieldType , this ) ;
  
  if( 0 == field() ) 
  { return Error("G4MagneticField* points to NULL!" ) ; }
  
  return StatusCode::SUCCESS ;
}



// ============================================================================
/** retrieve the magnetic field 
 *  @see IGiGaFieldMgr 
 *  @see G4MagneticField 
 *  @see G4Field
 *  @return poniter to magnetic field 
 */
// ============================================================================
G4MagneticField* GiGaFieldMgr::field    () const 
{
  if( 0 == m_field ) 
    { Error ( "field(): IGiGaFieldMgr*   points to NULL! " ) ; return 0 ; }
  G4MagneticField* mf = m_field -> field () ;
  if( 0 == mf )
    { Error ( "field(): G4MagneticField* points to NULL! " ) ; return 0 ; }
  return mf ;
}


// ============================================================================
// The END 
// ============================================================================
