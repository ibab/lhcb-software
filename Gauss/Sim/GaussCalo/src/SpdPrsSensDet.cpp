// $Id: SpdPrsSensDet.cpp,v 1.3 2003-07-09 17:01:44 ibelyaev Exp $ 
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

#include "GaudiKernel/IHistogramSvc.h"

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
#include "AIDA/IAxis.h"
#include "AIDA/IHistogram1D.h"

// DetDesc
#include "DetDesc/IGeometryInfo.h"

// ============================================================================
/** @file 
 * 
 *  Implementation of class SpdPrsSensDet
 *  
 *  @author  Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @author  Grigori Rybkine Grigori.Rybkine@cern.ch 
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
  , m_BX ( 25. * ns )
  , m_numBXs (6)
  , m_sDelays ( 3, 0. )
  , m_fracMin ( 1e-8 )
  , m_multiChargedBirks ( true ) 
{
  declareProperty ( "BunchCrossing"         ,  m_BX ) ;
  declareProperty ( "NumberBXs"         ,  m_numBXs ) ;
  declareProperty ( "IntegrationDelays" ,  m_sDelays ) ;
  declareProperty ( "FracMin"         ,  m_fracMin ) ;
  declareProperty ( "MultiChargedBirks"         ,  m_multiChargedBirks ) ;
}
// ============================================================================

// ============================================================================
/** standard initialization (Gaudi) 
 *  @see GiGaSensDetBase
 *  @see GiGaBase 
 *  @see   AlgTool 
 *  @see  IAlgTool 
 *  @return statsu code 
 */
// ============================================================================
StatusCode SpdPrsSensDet::initialize   ()
{
  // initialze the base class 
  StatusCode sc = CaloSensDet::initialize();
  if( sc.isFailure() )
    { return Error("Could not initialize the base class!",sc);}

  ///
  return StatusCode::SUCCESS ;
}
// ============================================================================

// ============================================================================
/** standard finalization (Gaudi) 
 *  @see GiGaSensDetBase
 *  @see GiGaBase 
 *  @see   AlgTool 
 *  @see  IAlgTool 
 *  @return statsu code 
 */
// ============================================================================
StatusCode SpdPrsSensDet::finalize    ()
{

  m_sDelays.clear () ;

  // finalize the base class 
  return CaloSensDet::finalize();
}
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
  const double                globalTime /* globalTime */ , 
  const double                   deposit       ,
  const G4Track*              track /* track      */ , 
  const G4ParticleDefinition* particle /* pdef       */ ,
  const G4MaterialCutsCouple* material /* material   */ ,
  const G4Step*               /* step       */ ) const 
{

  if( 0 == hit ) { return StatusCode::FAILURE ; }
  
  const CaloCellID& cellID = hit->cellID();
  if( !calo()->valid( cellID ) ) 
    return Print("fillHitInfo(): Cell not valid"
                 , StatusCode::FAILURE, MSG::DEBUG );

  // Birks' Law Correction
  double edep(deposit) ;
  const double dEdX =
    G4EnergyLossTables::GetDEDX ( particle ,
                                  track->GetKineticEnergy(),
                                  material ) ;
  edep *= birksCorrection( particle -> GetPDGCharge(), dEdX,
                              material->GetMaterial()->GetDensity() );

  // add the current energy deposition to the sub-hit
  // smearing the energy deposition over a number of bunch crossings (timing)
  CaloSubHit::Time slot;
  CaloSensDet::Fractions frac;
  frac.reserve( m_numBXs ) ;

  StatusCode sc = timing( globalTime, cellID, slot, frac );
  if( sc.isFailure() ){ 
    return Error("Could not smear Edep!", sc);
  }
  else
    Print("Smeared Edep", StatusCode::SUCCESS, MSG::VERBOSE);

  for( unsigned int i = 0; i < frac.size(); i++ )
    if( frac[i] > m_fracMin ) hit->add( slot + i , edep*frac[i] ) ;

  return StatusCode::SUCCESS ;
}
// ============================================================================

// ============================================================================
/** The fractions of energy deposited in consequitive time-slots 
 *  for the given calorimeter cell
 *  @param time global time of energy deposition
 *  @param cell cellID of the cell 
 *  @param slot (out) the first time slot 
 *  @param fracs the vector of fractions for subsequent time-slots;
 *  @return StatusCode 
 */
// ============================================================================
StatusCode SpdPrsSensDet::timing 
( const double             time      , 
  const CaloCellID&        cell      ,
  CaloSubHit::Time&        slot    ,
  CaloSensDet::Fractions&  fractions ) const 
{
  const double locTime = time - t0( cell );
    
  // number of the current 25 ns bx w.r.t. local time
  slot = static_cast<CaloSubHit::Time>( trunc( locTime/m_BX ) );

  const double refTime = locTime - slot * m_BX;
    
  // which area the cell is in: 0-Outer, 1-Middle, 2-Inner
  const unsigned int area = cell.area();

  const IAxis & axis = histos()[area]->axis();
  const double lowerEdge = axis.lowerEdge();
  const double upperEdge = axis.upperEdge();

  unsigned int i; double t;
  for( i = 0, t = -m_BX + m_sDelays[area] - refTime;
       i < m_numBXs;
       i++, t += m_BX )
    if( lowerEdge < t && t < upperEdge )
      fractions.push_back( histos()[area]->binHeight( axis.coordToIndex(t) ) );
    else fractions.push_back(0.);

  return StatusCode::SUCCESS ;
}
// ============================================================================

// ============================================================================
// Birks' Law 
// ============================================================================
/* The phenomenological description of the response attenuation of organic
 * scintillators is known as Birks' law. [J.B.Birks. The theory and practice
 * of Scintillation Counting. Pergamon Press, 1964.]
 * ============================================================================
 * Correction factor from Birks' Law
 *  Factor = 1/(1+C1*dEdx/rho+C2*(dEdx/rho)^2)
 *      - dEdx in MeV/cm
 *      - rho = density in g/cm^3
 *  where [R.L.Craun and D.L.Smith. Nucl. Inst. and Meth. 80 (1970) 239-244]
 *      - C1 = 0.013 g/MeV/cm^2
 *      - C2 = 9.6 * 10^-6 g^2/MeV^2/cm^4
 * for multiply charged (>1) paricles, a better description can be obtained
 * by correcting C1: C1'= 7.2/12.6 * C1 = 0.5714 * C1
 */
// ============================================================================
/** evaluate the correction for Birks' law 
 *  @param charge   the charge of the particle 
 *  @param dEdX     the nominal dEdX in the material
 *  @param density 
 *  @return the correction coefficient 
 *  (Adapted from GEANT3 SUBROUTINE GBIRK(EDEP))
 */
// ============================================================================
double  SpdPrsSensDet::birksCorrection 
( const double      charge   ,
  const double      dEdX     , 
  const double      density  ) const 
{
  // --- no saturation law for neutral particles ---
  if (fabs(charge) <= 1.e-10) {
    Warning("birksCorrection for neutral particle!");
    return 1.0 ;
  }

  // --- get the values for the BIRKS coefficients ---
  double C1 = birk_c1();
  const double C2 = birk_c2();
  
  // --- correction for particles with more than 1 charge unit ---
  // --- based on alpha particle data
  //    (only apply for m_multiChargedBirks == true) ---
  if ( m_multiChargedBirks )
    if ( fabs( charge ) > 1.99999 ) C1 *= birk_c1cor();
  
  const double alpha = dEdX/ density ;
  
  return 1.0 / ( 1.0 + C1 * alpha + C2 * alpha * alpha ) ;
}
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
