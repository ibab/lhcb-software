// $Id: GaussTrackActionZ.cpp,v 1.1 2004-02-20 19:35:30 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
// Include files 

// CLHEP
#include "CLHEP/Units/SystemOfUnits.h"

// GiGa
#include "GiGa/GiGaUtil.h"

// Geant4
#include "G4TrackingManager.hh"

// GaussTools 
#include "GaussTools/GaussTrajectory.h"
#include "GaussTools/GaussTrackInformation.h"

// local
#include "GaussTools/GaussTrackActionZ.h"
// ============================================================================

/** @file 
 *  Implementation file for class GaussTrackActionZ
 *
 *  @date 2004-02-19 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 */

// ============================================================================
/** standard constructor 
 *  @see GiGaTrackActionBase 
 *  @see GiGaBase 
 *  @see AlgTool 
 *  @param type type of the object (?)
 *  @param name name of the object
 *  @param parent  pointer to parent object
 */
// ============================================================================
GaussTrackActionZ::GaussTrackActionZ
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent ) 
  : GaussTrackActionBase ( type , name , parent ) 
  //
  , m_zMin ( -1 * km )  // use everywhere 
  , m_zMax (  1 * km )  // use everywhere
  //
{
  declareProperty ( "zMin" , m_zMin ) ;
  declareProperty ( "zMax" , m_zMax ) ;
}


// ============================================================================
/// Destructor
// ============================================================================
GaussTrackActionZ::~GaussTrackActionZ() {}


// ============================================================================
/** initialize the track action  
 *  @see GiGaTrackActionBase 
 *  @see GiGaBase 
 *  @see  AlgTool 
 *  @see IAlgTool 
 *  @return status code 
 */
// ============================================================================
StatusCode GaussTrackActionZ::initialize () 
{ 
  return GaussTrackActionBase::initialize() ; 
}


// ============================================================================
/** finalise the track action  
 *  @see GiGaTrackActionBase 
 *  @see GiGaBase 
 *  @see  AlgTool 
 *  @see IAlgTool 
 *  @return status code 
 */
// ============================================================================
StatusCode GaussTrackActionZ::finalize   () 
{ 
  return GaussTrackActionBase::finalize   () ; 
}


// ============================================================================
// The END 
// ============================================================================
