// $Id: GiGaSensDetPrint.cpp,v 1.4 2002-05-07 12:21:36 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
/// CLHEP 
#include "CLHEP/Geometry/Point3D.h"
/// GaudiKernel
#include "GaudiKernel/MsgStream.h"
/// GiGa 
#include "GiGa/GiGaMACROs.h"
/// Geant4 
#include "G4Step.hh"
#include "G4TouchableHistory.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
/// local
#include "GiGaSensDetPrint.h"


// ============================================================================
/** @file 
 * 
 *  Implementation of class GiGaSensDetPrint
 *  
 *  @author Vanya Belyaev
 *  @date   23/01/2001 
 */
// ============================================================================

// ============================================================================
/// factory business 
// ============================================================================
IMPLEMENT_GiGaFactory( GiGaSensDetPrint );
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
GiGaSensDetPrint::GiGaSensDetPrint
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent ) 
  : GiGaSensDetBase     ( type , name , parent ) 
  , G4VSensitiveDetector( name  )
{};
// ============================================================================

// ============================================================================
/// destructor 
// ============================================================================
GiGaSensDetPrint::~GiGaSensDetPrint(){};
// ============================================================================

// ============================================================================
/** process the hit
 *  @param step     pointer to current Geant4 step 
 *  @param history  pointert to touchable history 
 */
// ============================================================================
bool GiGaSensDetPrint::ProcessHits( G4Step* step , 
                                    G4TouchableHistory* /* history */ ) 
{
  if( 0 == step ) { return false ; } 
  
  double     edep = step->GetTotalEnergyDeposit();
  HepPoint3D pos  = step->GetPostStepPoint()->GetPosition(); 
  
  /// 
  G4TouchableHistory* TT =  
    (G4TouchableHistory*)(step->GetPreStepPoint()->GetTouchable());
  G4VPhysicalVolume*  PV =   TT->GetVolume();
  G4LogicalVolume*    LV =   PV->GetLogicalVolume();
  
  MsgStream log( msgSvc() , name() );
  log << MSG::INFO << "ProcessStep() " 
      << " Pos=("  << pos.x() << "," << pos.y() << "," << pos.z() << ")" 
      << " PV="    << PV->GetName() 
      << " LV="    << LV->GetName() 
      << " edep="  << edep << endreq; 
  
  return false;
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================

