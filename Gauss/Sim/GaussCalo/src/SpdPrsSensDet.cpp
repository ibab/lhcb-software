// $Id: SpdPrsSensDet.cpp,v 1.2 2003-07-07 16:27:46 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2003/07/07 08:21:12  ibelyaev
//  split the general CaloSensDet class
//
// ============================================================================
/// SRD & STD 
#include <algorithm>
#include <vector>
/// CLHEP 
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Units/PhysicalConstants.h"
/// GaudiKernel
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h" 
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/AlgFactory.h"
/// GiGa 
#include "GiGa/GiGaMACROs.h"
#include "GiGa/GiGaHashMap.h"
/// GaussTools 
#include "GaussTools/GaussTrackInformation.h"
/// Geant4 
#include "G4Step.hh"
#include "G4TouchableHistory.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4SDManager.hh"
#include "G4EnergyLossTables.hh"
// GiGaCnv 
#include "GiGaCnv/GiGaVolumeUtils.h"
// CaloDet
#include "CaloDet/DeCalorimeter.h"
/// local
#include "CaloHit.h"
#include "CaloSimHash.h"
#include "SpdPrsSensDet.h"
///
#include "AIDA/IHistogram1D.h"

// ============================================================================
/** @file 
 * 
 *  Implementation of class SpdPrsSensDet
 *  
 *  @author  Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @author  Patrick Robbe robbe@lal.in2p3.fr 
 *
 *  @date   23/01/2001 
 */
// ============================================================================

// ============================================================================
/// factory business 
// ============================================================================
IMPLEMENT_GiGaFactory( SpdPrsSensDet );
// ============================================================================

// ============================================================================
/** standard constructor 
 *  @see GiGaSensDetBase 
 *  @see GiGaBase 
 *  @see AlgTool 
 *  @param type type of the object (?)
 *  @param name name of the object
 *  @param parent  pointer to parent object
 */
// ============================================================================
SpdPrsSensDet::SpdPrsSensDet
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent ) 
  : CaloSensDet        ( type , name , parent ) 
  , G4VSensitiveDetector( name  )
{};
// ============================================================================

// ============================================================================
/** fill the hit with the concrete information about the energy and the time.
 *  The function is to be called from ProcessHits method.
 *
 *  @param hit        hit  to be filled with the correct information, 
 *                    the cellID information is accessible from the hit
 *  @param prePoint   PreStepPoint  for the given step 
 *  @param globalTime the global time of prestep point 
 *  @param deposit    raw energy deposition for the step 
 *  @param track      the actual G4Track pbject (decoded from G4Step)
 *  @param pdef       the actual particle type  (decoded from G4Step/G4Track)  
 *  @param material   the actual material       (decoded from G4Step) 
 *  @param step       the step itself, the most important 
 *                    information from the step 
 *                    is already decoded into prePoint,globalTime,track,
 *                    particle definition,material etc for efficiency reasons. 
 *
 */
// ============================================================================
StatusCode    SpdPrsSensDet::fillHitInfo 
( CaloSubHit*                    hit           ,
  const HepPoint3D&           /* prePoint   */ ,
  const double                /* globalTime */ , 
  const double                   deposit       ,
  const G4Track*              /* track      */ , 
  const G4ParticleDefinition* /* pdef       */ ,
  const G4MaterialCutsCouple* /* material   */ ,
  const G4Step*               /* step       */ ) const 
{
  
  if( 0 == hit ) { return StatusCode::FAILURE ; }
  
  // add the current energy deposition to the sub-hit
  CaloSubHit::Time tm = 0   ;
  
  hit->add ( tm , deposit ) ;
  
  return StatusCode::SUCCESS ;
};
// ============================================================================

// ============================================================================
/** The fractions of energy deposited in consequitive time-slots 
 *  in the give Ecal/Hcal cell
 *  @param time global time of energy deposition
 *  @param cell cellID of the cell 
 *  @param slot (out) the first time slot 
 *  @param fracs the vector of frractions for subsequent time-bins;
 *  @return StatuscCode 
 */
// ============================================================================
StatusCode SpdPrsSensDet::timing 
( const double             time      , 
  const CaloCellID&        cell      ,
  CaloSubHit::Time&        slot    ,
  CaloSensDet::Fractions&  fractions ) const 
{
  fractions.clear();
  slot = 1 ;
  fractions.push_back( 1.0 ) ;
  Warning ("timing(): not yet implemented in a proper way");
  return StatusCode::SUCCESS ;
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
