// $Id: EcalSensDet.cpp,v 1.2 2003-07-07 16:27:46 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
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
  , m_a_local_inner_ecal ( 0.004 ) // local non uniformity amplitude inner ecal
  , m_a_local_middle_ecal ( 0.004 ) // local non uniformity amplitude
  //  middle ecal
  , m_a_local_outer_ecal ( 0.012 ) // local non uniformity amplitude 
  // outer ecal
{
  // Initialisation of Histograms for timing of ECAL
  m_ecalTimeHisto[0][0]  = 1. ;
  m_ecalTimeHisto[0][1]  = 1. ;
  m_ecalTimeHisto[0][2]  = 0.99 ;
  m_ecalTimeHisto[0][3]  = 0.985 ;
  m_ecalTimeHisto[0][4]  = 0.98 ;
  m_ecalTimeHisto[0][5]  = 0.96 ;
  m_ecalTimeHisto[0][6]  = 0.935 ;
  m_ecalTimeHisto[0][7]  = 0.9 ;
  m_ecalTimeHisto[0][8]  = 0.86 ;
  m_ecalTimeHisto[0][9]  = 0.81 ;
  m_ecalTimeHisto[0][10] = 0.75 ;
  m_ecalTimeHisto[0][11] = 0.68 ;
  m_ecalTimeHisto[0][12] = 0.59 ;
  m_ecalTimeHisto[0][13] = 0.51 ;
  m_ecalTimeHisto[0][14] = 0.42 ;
  m_ecalTimeHisto[0][15] = 0.35 ;
  m_ecalTimeHisto[0][16] = 0.28 ;
  m_ecalTimeHisto[0][17] = 0.2  ;
  m_ecalTimeHisto[0][18] = 0.14 ;
  m_ecalTimeHisto[0][19] = 0.09 ;
  m_ecalTimeHisto[0][20] = 0.06 ;
  m_ecalTimeHisto[0][21] = 0.025 ;
  m_ecalTimeHisto[0][22] = 0.01 ;
  m_ecalTimeHisto[0][23] = 0. ;
  m_ecalTimeHisto[0][24] = 0. ;
  //
  m_ecalTimeHisto[1][0]  = 0. ;
  m_ecalTimeHisto[1][1]  = 0. ;
  m_ecalTimeHisto[1][2]  = 0. ;
  m_ecalTimeHisto[1][3]  = 0.005 ;
  m_ecalTimeHisto[1][4]  = 0.035 ;
  m_ecalTimeHisto[1][5]  = 0.065 ;
  m_ecalTimeHisto[1][6]  = 0.09 ;
  m_ecalTimeHisto[1][7]  = 0.13 ;
  m_ecalTimeHisto[1][8]  = 0.18 ;
  m_ecalTimeHisto[1][9]  = 0.24 ;
  m_ecalTimeHisto[1][10] = 0.31 ;
  m_ecalTimeHisto[1][11] = 0.36 ;
  m_ecalTimeHisto[1][12] = 0.44 ;
  m_ecalTimeHisto[1][13] = 0.51 ;
  m_ecalTimeHisto[1][14] = 0.59 ;
  m_ecalTimeHisto[1][15] = 0.67 ;
  m_ecalTimeHisto[1][16] = 0.74 ;
  m_ecalTimeHisto[1][17] = 0.8 ;
  m_ecalTimeHisto[1][18] = 0.85 ;
  m_ecalTimeHisto[1][19] = 0.91 ;
  m_ecalTimeHisto[1][20] = 0.94 ;
  m_ecalTimeHisto[1][21] = 0.96 ;
  m_ecalTimeHisto[1][22] = 0.985 ;
  m_ecalTimeHisto[1][23] = 0.99 ;
  m_ecalTimeHisto[1][24] = 1. ;
  ///
  declareProperty ( "a_local_inner_ecal"   ,  m_a_local_inner_ecal ) ;
  declareProperty ( "a_local_middle_ecal"  ,  m_a_local_middle_ecal ) ;
  declareProperty ( "a_local_outer_ecal"   ,  m_a_local_outer_ecal ) ;
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
  const double                /* globalTime */ , 
  const double                   deposit       ,
  const G4Track*              /* track    */   , 
  const G4ParticleDefinition* /* pdef     */   ,
  const G4MaterialCutsCouple* /* material */   ,
  const G4Step*                  step          ) const 
{
  
  if( 0 == hit || 0 == step ) { return StatusCode::FAILURE ; }
  
  // get the cell 
  const CaloCellID& cellID = hit->cellID() ;
  
  // add current energy deposition to the sub-hit
  CaloSubHit::Time tm = 0   ;
  
  // Birk's Law Correction
  double ecorrected = deposit ;
  
  ecorrected *= birkCorrection(step) ;
  
  // Timing
  double frac1, frac2 ;
  int timeBin ;
  timeFraction( step , cellID , frac1 , frac2 , timeBin ) ;
  
  //Local NonUniformity
  ecorrected *= localNonUniformity( step , cellID ) ;
  
  if ( frac1 > 1E-7 ) hit->add( timeBin   , ecorrected*frac1 ) ;
  if ( frac2 > 1E-7 ) hit->add( timeBin+1 , ecorrected*frac2 ) ;
  
  return StatusCode::SUCCESS ;
};
// ============================================================================

// ============================================================================
// Local Non Uniformity
// ============================================================================
/** Correction due to the local non uniformity due to the light 
 *  collection efficiency in cell cell
 */

double EcalSensDet::localNonUniformity( const G4Step*     step , 
                                        const CaloCellID& cell ) const 
{
  
  // Only for ECal for the moment
  double correction = 1. ;
  
  if ( CaloCellCode::CaloNameFromNum( cell.calo() ) == "Ecal" ) {
  
    // Find the position of the step
    double x        = step->GetPreStepPoint()->GetPosition().x() ;
    double y        = step->GetPreStepPoint()->GetPosition().y() ;
    // Center of the cell
    double x0       = calo()->cellX( cell ) ;
    double y0       = calo()->cellY( cell ) ;
    // Distance between fibers 
    // and correction amplitude
    double d        = 10.1 * mm ;
    double A_local  = m_a_local_inner_ecal ;

    // Assign amplitude of non uniformity as a function of the
    // Ecal region
    
    if ( cell.area() == 0 ) { // outer Ecal
      A_local = m_a_local_outer_ecal ;
      d       = 15.25 * mm ;
    }
    else if ( cell.area() == 1 ) { // middle Ecal
      A_local = m_a_local_middle_ecal ;
    }

    
    // Local uniformity is product of x and y sine-like functions
    // The Amplitude of the sin-like function is a function of x and 
    // y
    correction =
      1. + 
      A_local * 
      ( 1. - cos( 2.*pi * (x-x0)/d ) ) *
      ( 1. - cos( 2.*pi * (y-y0)/d ) ) ;
  }
  
  return correction ;
  
};

// ============================================================================
// Get Fraction of energy in consecutive time windows
// ============================================================================

/** The energy deposited in the cell by the step contributes 
 *  - frac1*energy in the time bin timeBin (25 ns bins)
 *  - frac2*energy in the time bin timeBin+1
 *  the origin is the time of collision
 */
void EcalSensDet::timeFraction ( const G4Step*     step    , 
                                 const CaloCellID& cell    ,
                                 double&           frac1   , 
                                 double&           frac2   , 
                                 int&              timeBin ) const 
{
  frac1 = 1. ;
  frac2 = 0. ;
  timeBin = 0 ;
  
  // Only for ECAL for the moment
  if ( CaloCellCode::CaloNameFromNum( cell.calo() ) == "Ecal" ) {  
    
    // Time of the step in Geant = time since the primary event
    
    double timeOfStep = step->GetPreStepPoint()->GetGlobalTime() ;
    // Time of flight between z=0 and z=Ecal 
    double dist       = sqrt( calo()->cellX(cell)*
                              calo()->cellX(cell)
                              + calo()->cellY(cell)*
                              calo()->cellY(cell) 
                              + (12490.*mm) * (12490.*mm) ) ;
    
    double tFlight = dist / c_light ;
    // substract 0.5 ns (from SICB)
    tFlight -= 0.5 * ns ;
    
    // Find bin number in term of 25ns sampling
    double tNow = timeOfStep - tFlight ;
    int bin     = (int) floor( tNow - floor( tNow / (25.*ns) ) * (25.*ns) ) ;
    
    // Frac1 = fraction of energy in first 25ns time window
    frac1 = m_ecalTimeHisto[0][bin] ;
    // Frac2 = fraction of energy in second 25 ns time window
    frac2 = m_ecalTimeHisto[1][bin] ;
    // timeBin = absolute bin number with respect to tFlight
    timeBin = (int) floor( tNow / (25.*ns) ) ;
  }
};

// ============================================================================
// The END 
// ============================================================================
