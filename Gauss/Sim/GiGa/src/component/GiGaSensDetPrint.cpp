// $Id: GiGaSensDetPrint.cpp,v 1.3 2002-04-25 13:02:05 ibelyaev Exp $ 
// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.2  2001/08/12 15:42:53  ibelyaev
/// improvements with Doxygen comments
///
/// Revision 1.1  2001/07/27 17:56:05  ibelyaev
/// add new component GiGaSensDetPrint
/// 
// ============================================================================
/// CLHEP 
#include "CLHEP/Geometry/Point3D.h"
/// GaudiKernel
#include "GaudiKernel/MsgStream.h"
/// GiGa 
#include "GiGa/GiGaSensDetFactory.h"
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
IMPLEMENT_GiGaSensDet( GiGaSensDetPrint );

// ============================================================================
/** standard consrtructor 
 *  @param Name name of sensitive detector 
 *  @param Loc  pointer to service Locator 
 */ 
// ============================================================================
GiGaSensDetPrint::GiGaSensDetPrint( const std::string& Name , 
                                    ISvcLocator* Loc ) 
  : GiGaSensDetBase     ( Name , Loc ) 
  , G4VSensitiveDetector( Name )
{};

// ============================================================================
/// destructor 
// ============================================================================
GiGaSensDetPrint::~GiGaSensDetPrint(){};

// ============================================================================
/** initialize the sensitive detector 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaSensDetPrint::initialize () 
{ 
  StatusCode sc = GiGaSensDetBase::initialize() ; 
  if( sc.isFailure() ) 
    { return Error("Could not initialize base class!",sc);}
  ///
  Print("initialized successfully");
  ///
  return StatusCode::SUCCESS;
} ;

// ============================================================================
/** finalize the sensitive detector 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaSensDetPrint::finalize   () 
{ 
  ///
  Print("finalization");
  ///
  return GiGaSensDetBase::initialize() ; 
} ;

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
// The END 
// ============================================================================

