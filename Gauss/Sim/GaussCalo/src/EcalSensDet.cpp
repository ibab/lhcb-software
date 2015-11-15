// $Id: EcalSensDet.cpp,v 1.12 2008-07-11 10:47:44 robbep Exp $
// Include files

// SRD & STD
#include <algorithm>
#include <vector>

// CLHEP
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Units/PhysicalConstants.h"

// GaudiKernel
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

// GiGa
#include "GiGa/GiGaHashMap.h"

// LHCb
#include "LHCbMath/LHCbMath.h"

// GaussTools
#include "GaussTools/GaussTrackInformation.h"

// Geant4
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

// local
#include "CaloHit.h"
#include "CaloSimHash.h"
#include "EcalSensDet.h"

//
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

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( EcalSensDet )

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
  : G4VSensitiveDetector( name  )
  , EHCalSensDet        ( type , name , parent )
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
}


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
  const HepGeom::Point3D<double>& prePoint      ,
  const double                   time ,
  const double                   deposit       ,
  const G4Track*                 track   ,
  const G4ParticleDefinition*    particle   ,
  const G4MaterialCutsCouple*    material   ,
  const G4Step*                  step          ) const
{

  if( 0 == hit || 0 == step ) { return StatusCode::FAILURE ; }

  // get the cell
  const LHCb::CaloCellID& cellID = hit->cellID() ;

  // Birk's Law Correction
  double ecorrected = deposit *
    birkCorrection( particle ,
                    track->GetKineticEnergy() ,
                    material ) ;

  //Local NonUniformity
  // if the cell is not valid do not apply the correction
  // (Anyway it will not be used)
  //if ( calo() -> valid ( cellID ) ) {
  ecorrected *= localNonUniformity( prePoint , cellID ) ;
  //}

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
        fractions.end() != ifr ; ++ifr ) {
    const double fr = *ifr ;
    if ( fr > 1.e-5 ) hit->add( slot , ecorrected * fr ) ;
    slot++ ;
  }

  return StatusCode::SUCCESS ;
}


// ============================================================================
// The END
// ============================================================================
