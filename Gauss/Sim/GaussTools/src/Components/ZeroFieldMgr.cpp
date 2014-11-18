// $Id: ZeroFieldMgr.cpp,v 1.2 2007-01-12 15:36:58 ranjard Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

// include 
// GEANT4 
#include "G4ChordFinder.hh"
#include "G4FieldManager.hh"
#include "G4TransportationManager.hh"
#include "G4ChordFinder.hh"

// local 
#include "ZeroFieldMgr.h"

// ============================================================================
/** @file 
 *  Implementation file for class : ZeroFieldMgr
 *
 *  @date 2003-06-04
 *  @author Witek Pokorski
 */
// ============================================================================

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( ZeroFieldMgr )

// ============================================================================
/** standard constructor
 *  @see ZeroFieldMgrBase 
 *  @see GiGaBase 
 *  @see AlgTool 
 *  @param type type of the object (?)
 *  @param name name of the object
 *  @param parent  pointer to parent object
 */
// ============================================================================
ZeroFieldMgr::ZeroFieldMgr 
( const std::string& type   , 
  const std::string& name   , 
  const IInterface*  parent )  
  : GiGaFieldMgrBase ( type , name , parent ) 
  , m_field          ( 0     ) 
{
}


// ============================================================================
/// destructor 
// ============================================================================
ZeroFieldMgr::~ZeroFieldMgr (){}


// ============================================================================
/** initialize the object 
 *  @see ZeroFieldMgrBase
 *  @see GiGaBase
 *  @see  AlgTool
 *  @see IAlgTool
 *  @return status code 
 */
// ============================================================================
StatusCode   ZeroFieldMgr::initialize     () 
{
  // initialize the base class 
  StatusCode sc = GiGaFieldMgrBase::initialize () ;
  if( sc.isFailure() ) 
    { return Error ( "Base class is not initialized properly!" , sc ) ; }
  
  return StatusCode::SUCCESS ;
}


// ============================================================================
/** retrieve the magnetic field 
 *  @see IZeroFieldMgr 
 *  @see G4MagneticField 
 *  @see G4Field
 *  @return poniter to magnetic field 
 */
// ============================================================================
G4MagneticField* ZeroFieldMgr::field    () const 
{
  return 0;
}


// ============================================================================
// The END 
// ============================================================================
