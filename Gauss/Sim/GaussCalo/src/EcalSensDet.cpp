// $Id: EcalSensDet.cpp,v 1.6 2004-01-14 13:38:10 ranjard Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.5  2003/12/06 16:16:54  robbep
// Put the correct default parameters for non uniformity correction
//
// Revision 1.4  2003/12/05 08:59:58  robbep
// Put fonctions and constants for the non-uniformity. The local
// non-uniformity (sin-like amplitude) is set to 0 for the moment.
//
// Revision 1.3  2003/07/08 10:22:50  robbep
// Adaptation to the new GaussCalo interface
//
// Revision 1.2  2003/07/07 16:27:46  ibelyaev
//  substitupe G4Material with G4MaterialCutsCouple
//
// Revision 1.1  2003/07/07 08:21:09  ibelyaev
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
#include "EcalSensDet.h"
///
#include "AIDA/IHistogram1D.h"

// ============================================================================
/** @file 
 * 
 *  Implementation of class EcalSensDet
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
IMPLEMENT_GiGaFactory( EcalSensDet );
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
EcalSensDet::EcalSensDet
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent ) 
  : EHCalSensDet        ( type , name , parent ) 
    , G4VSensitiveDetector( name  )
  //
  //  , m_a_local_inner_ecal  ( 0.008 ) // local non uniformity amplitude 
  // inner ecal
  //  , m_a_local_middle_ecal  ( 0.008 ) // local non uniformity amplitude
  //  middle ecal
  //  , m_a_local_outer_ecal   ( 0.03  ) // local non uniformity amplitude 
  // outer ecal  
  // For the moment put 0 for the local N.U.
    , m_a_local_inner_ecal ( 0. ) 
    , m_a_local_middle_ecal ( 0. )
    , m_a_local_outer_ecal ( 0. )
    , m_a_global_inner_ecal  ( 0.0004  ) // global non uniformity amplitude 
  //inner ecal
    , m_a_global_middle_ecal ( 0.002  ) // global non uniformity amplitude
  //  middle ecal
    , m_a_global_outer_ecal  ( 0.03  ) // global non uniformity amplitude 
  // outer ecal
    , m_a_reflection_height ( 0.09 ) // reflection on the edges - height
    , m_a_reflection_width  ( 6. * mm ) // reflection on the edges - width
{
  ///
  declareProperty ( "a_local_inner_ecal"   ,  m_a_local_inner_ecal ) ;
  declareProperty ( "a_local_middle_ecal"  ,  m_a_local_middle_ecal ) ;
  declareProperty ( "a_local_outer_ecal"   ,  m_a_local_outer_ecal ) ;
  declareProperty ( "a_global_inner_ecal"   ,  m_a_global_inner_ecal ) ;
  declareProperty ( "a_global_middle_ecal"  ,  m_a_global_middle_ecal ) ;
  declareProperty ( "a_global_outer_ecal"   ,  m_a_global_outer_ecal ) ;
  declareProperty ( "a_reflection_height"  , m_a_reflection_height ) ;
  declareProperty ( "a_reflection_width"   , m_a_reflection_width  ) ;
  //
};
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
StatusCode    EcalSensDet::fillHitInfo 
( CaloSubHit*                    hit           ,
  const HepPoint3D&              prePoint      ,
  const double                   time , 
  const double                   deposit       ,
  const G4Track*                 track   , 
  const G4ParticleDefinition*    particle   ,
  const G4MaterialCutsCouple*    material   ,
  const G4Step*                  step          ) const 
{
  
  if( 0 == hit || 0 == step ) { return StatusCode::FAILURE ; }
  
  // get the cell 
  const CaloCellID& cellID = hit->cellID() ;

  // Birk's Law Correction
  double ecorrected = deposit *
    birkCorrection( particle ,
                    track->GetKineticEnergy() ,
                    material ) ;
  
  //Local NonUniformity
  // if the cell is not valid do not apply the correction
  // (Anyway it will not be used)
  if ( calo() -> valid ( cellID ) ) {
    ecorrected = ecorrected * localNonUniformity( prePoint , cellID ) ;
  }
  
  // Timing
  // Uses method in EHCalSensDet
  CaloSubHit::Time slot = 0   ;
  Fractions fractions ;
  StatusCode sc = timing( time , cellID , slot , fractions ) ;

  if ( sc.isFailure() ) {
    return Error( "Error in timing()" , sc ) ;
  }
  
  if ( fractions.empty() ) {
    Warning("The vector of fractions of energy is empty.") ;
  }
  
  for ( Fractions::const_iterator ifr = fractions.begin() ;
        fractions.end() != ifr ;
        ++ifr ) {
    const double fr = *ifr ;
    if ( fr > 1E-6 ) {
      hit->add( slot , ecorrected * fr ) ;
    }
    slot++ ;
  }
  
  return StatusCode::SUCCESS ;
};
// ============================================================================

// ============================================================================
// Local Non Uniformity
// ============================================================================
/** Correction due to the local non uniformity due to the light 
 *  collection efficiency in cell cell
 */

double EcalSensDet::localNonUniformity( const HepPoint3D& prePoint , 
                                        const CaloCellID& cell ) const 
{
  
  // Only for ECal for the moment
  double correction = 1. ;
  
  // Find the position of the step
  double x        = prePoint.x() ;
  double y        = prePoint.y() ;
  // Center of the cell
  double x0       = calo()->cellX( cell ) ;
  double y0       = calo()->cellY( cell ) ;

  // Distance between fibers 
  // and correction amplitude
  double d        = 10.1 * mm ;
  double A_local  = m_a_local_inner_ecal ; // in inner Ecal
  double A_global = m_a_global_inner_ecal ;

  // Cell size
  double cellSize = calo()->cellSize( cell ) ;

  // Assign amplitude of non uniformity as a function of the
  // Ecal region
  
  if ( cell.area() == 0 ) { // outer Ecal
    A_local  = m_a_local_outer_ecal ;
    A_global = m_a_global_outer_ecal ;
    d        = 15.25 * mm ;
  }
  else if ( cell.area() == 1 ) { // middle Ecal
    A_local  = m_a_local_middle_ecal ;
    A_global = m_a_global_middle_ecal ;
  }  
    
  // Local uniformity is product of x and y sine-like functions
  // The Amplitude of the sin-like function is a function of x and 
  // y
  correction += 
    A_local / 2. * 
    ( 1. - cos( 2.*pi * (x-x0)/d ) ) *
    ( 1. - cos( 2.*pi * (y-y0)/d ) ) ;

  // Global non uniformity

  correction += 
    A_global * ( x0 + cellSize / 2. - x ) * ( x - x0 + cellSize / 2. )
    / ( cellSize * cellSize / 4. )
    * ( y0 + cellSize / 2. - y ) * ( y - y0 + cellSize / 2. ) 
    / ( cellSize * cellSize / 4. ) ;


  // Light Reflexion on the edges
  correction += 
    m_a_reflection_height * 
    exp( - fabs ( x - x0 + cellSize / 2. ) / m_a_reflection_width ) 
    + m_a_reflection_height * 
    exp( - fabs ( x - x0 - cellSize / 2. ) / m_a_reflection_width ) 
    + m_a_reflection_height * 
    exp( - fabs ( y - y0 + cellSize / 2. ) / m_a_reflection_width ) 
    + m_a_reflection_height * 
    exp( - fabs ( y - y0 - cellSize / 2. ) / m_a_reflection_width ) ;
  
  return correction ;
};

// ============================================================================
// The END 
// ============================================================================
