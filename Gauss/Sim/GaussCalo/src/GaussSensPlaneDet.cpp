// $Id: GaussSensPlaneDet.cpp,v 1.7 2008-07-11 10:47:44 robbep Exp $
// Include files

// SRD & STD
#include <algorithm>
#include <vector>

// CLHEP
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Units/PhysicalConstants.h"

// GaudiKernel
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/ToolFactory.h"

// GiGa
#include "GiGa/GiGaHashMap.h"

// GaussTools
#include "GaussTools/GaussTrackInformation.h"

// Geant4
#include "G4Step.hh"
#include "G4TouchableHistory.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4SDManager.hh"
#include "G4EnergyLossTables.hh"
#include "G4Material.hh"
#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4MuonPlus.hh"
#include "G4MuonMinus.hh"

// GiGaCnv
#include "GiGaCnv/GiGaVolumeUtils.h"

// CaloDet
#include "CaloDet/DeCalorimeter.h"

/// local
#include "GaussSensPlaneHit.h"
#include "GaussSensPlaneDet.h"

using CLHEP::HepLorentzVector;

// ============================================================================
/** @file
 *
 *  Implementation of class GaussSensPlaneDet
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   23/01/2001
 */
// ============================================================================

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( GaussSensPlaneDet )


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
GaussSensPlaneDet::GaussSensPlaneDet
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : G4VSensitiveDetector ( name  )
  , GiGaSensDetBase      ( type , name , parent )
  ///
  , m_collectionName     ( "Hits"  )
  , m_collection         ( 0 )
  ///
  , m_keepLinks ( false )
    , m_oneEntry  ( true  )
  ///
  , m_cutForPhoton   ( 50 * MeV )
  , m_cutForElectron ( 10 * MeV )
  , m_cutForPositron ( 10 * MeV )
  , m_cutForMuon     ( -1 * MeV )
  , m_cutForCharged  ( 10 * MeV )
    , m_cutForNeutral  ( 10 * MeV )
  ///
  , m_stat    (  true   )
  , m_events  (  0      )
  , m_hits    (  0      )
  , m_hits2   (  0      )
  , m_hitsMin (  1.e+10 )
  , m_hitsMax ( -1.e+10 )
{
  declareProperty ( "CollectionName"       ,  m_collectionName    ) ;
  //
  declareProperty ( "KeepAllLinks"         , m_keepLinks      ) ;
  declareProperty ( "OneEntry"             , m_oneEntry       ) ;
  declareProperty ( "CutForPhoton"         , m_cutForPhoton   ) ;
  declareProperty ( "CutForElectron"       , m_cutForElectron ) ;
  declareProperty ( "CutForPositron"       , m_cutForPositron ) ;
  declareProperty ( "CutForMuon"           , m_cutForMuon     ) ;
  declareProperty ( "CutForCharged"        , m_cutForCharged  ) ;
  declareProperty ( "CutForNeutral"        , m_cutForNeutral  ) ;
}


// ============================================================================
/** standard initialization (Gaudi)
 *  @see GiGaSensDetBase
 *  @see GiGaBase
 *  @see   AlgTool
 *  @see  IAlgTool
 *  @return statsu code
 */
// ============================================================================
StatusCode GaussSensPlaneDet::initialize   ()
{
  // initialze the base class
  StatusCode sc = GiGaSensDetBase::initialize();
  if( sc.isFailure() )
    { return Error("Could not initialize the base class!",sc);}
  //
  // clear collection name vector
  collectionName.clear  () ;
  collectionName.insert ( m_collectionName );
  ///
  return StatusCode::SUCCESS ;
}


// ============================================================================
/** standard finalization (Gaudi)
 *  @see GiGaSensDetBase
 *  @see GiGaBase
 *  @see   AlgTool
 *  @see  IAlgTool
 *  @return statsu code
 */
// ============================================================================
StatusCode GaussSensPlaneDet::finalize    ()
{
  if( m_stat )
    { /// statistical printout
      MsgStream log( msgSvc() , name() ) ;
      log << MSG::DEBUG <<
        format ( " <#Hits>/Min/Max=(%3d+-%3d)/%d/%4d "                  ,
                 (long) m_hits                                          ,
                 (long) sqrt ( fabs( m_hits2 - m_hits * m_hits ) )      ,
                 (long) m_hitsMin                                       ,
                 (long) m_hitsMax                                       )
           << endreq ;
    }
  // finalize the base class
  return GiGaSensDetBase::finalize();
}


// ============================================================================
/** method from G4
 *  (Called at the begin of each event)
 *  @see G4VSensitiveDetector
 *  @param HCE pointer to hit collection of current event
 */
// ============================================================================
void GaussSensPlaneDet::Initialize( G4HCofThisEvent* HCE )
{
  //
  m_collection =
    new GaussSensPlaneHitsCollection ( SensitiveDetectorName ,
                                       collectionName[0]     ) ;
  //
  const int id  = GetCollectionID( 0 ) ;

  HCE -> AddHitsCollection( id , m_collection );

  //
  Print (" Initialize(): CollectionName='" + m_collection->GetName   () +
         "' for SensDet='"                 + m_collection->GetSDname () +
         "'" , StatusCode::SUCCESS , MSG::VERBOSE                       ) ;
  //
}


// ============================================================================
/** method from G4
 *  (Called at the end of each event)
 *  @see G4VSensitiveDetector
 *  @param HCE pointer to hit collection of current event
 */
// ============================================================================
void GaussSensPlaneDet::EndOfEvent( G4HCofThisEvent* /* HCE */ )
{
  if( !m_stat ) { return ; }                               // RETURN
    /// increase the counter of processed events
  ++m_events ;
  const double f1 = 1.0 / ( (double) ( m_events     ) ) ;
  const double f2 =  f1 * ( (double) ( m_events - 1 ) ) ;

  if ( 0 == m_collection )
    { Warning ( " EndOfEvent(): HitCollection points to NULL " ) ; return ; }
  typedef std::vector<GaussSensPlaneHit*> Hits ;
  const Hits* hits = m_collection ->GetVector() ;
  if ( 0 == hits )
    { Error   (" EndOfEvent(): HitVector* points to NULL "     ) ; return ; }

  const size_t nhits = hits->size() ;
  m_hits    = m_hits  * f2 + nhits           * f1 ;
  m_hits2   = m_hits2 * f2 + nhits  * nhits  * f1 ;

  if ( nhits  > m_hitsMax   ) { m_hitsMax   = nhits  ; }
  if ( nhits  < m_hitsMin   ) { m_hitsMin   = nhits  ; }

  MsgStream log ( msgSvc() , name() ) ;
  log << MSG::DEBUG <<
    format ( " #GaussSensPlaneHits=%4d ", nhits ) << endreq ;
}


// ============================================================================
/** process the hit
 *  @param step     pointer to current Geant4 step
 *  @param history  pointert to touchable history
 */
// ============================================================================
bool GaussSensPlaneDet::ProcessHits( G4Step* step                      ,
                                     G4TouchableHistory* /* history */ )
{
  if( 0 == step ) { return false ; }
  ///
  const G4Track*              const track    = step     -> GetTrack      () ;
  const int                         trackID  = track    -> GetTrackID    () ;
  const G4ParticleDefinition* const particle = track    -> GetDefinition () ;
  const double                      charge   = particle -> GetPDGCharge  () ;

  const G4StepPoint* const          pre      = step    -> GetPreStepPoint  () ;
  const G4StepPoint* const          post     = step    -> GetPostStepPoint () ;

  // make hit only at first(last?) entry
  if ( oneEntry() && pre -> GetPhysicalVolume() == post -> GetPhysicalVolume() )
    { return false ; }                                               // RETURN

  // apply the cuts on energy
  const double                       eKine     = pre   -> GetKineticEnergy  () ;

  if      ( particle == G4Gamma::    Gamma     () && eKine < cutForPhoton   () )
    { return false ; }                                               // RETURN
  else if ( particle == G4Electron:: Electron  () && eKine < cutForElectron () )
    { return false ; }                                               // RETURN
  else if ( particle == G4Positron:: Positron  () && eKine < cutForPositron () )
    { return false ; }                                               // RETURN
  else if ( particle == G4MuonMinus::MuonMinus () && eKine < cutForMuon     () )
    { return false ; }                                               // RETURN
  else if ( particle == G4MuonPlus:: MuonPlus  () && eKine < cutForMuon     () )
    { return false ; }                                               // RETURN
  else if ( 0 != charge                           && eKine < cutForCharged  () )
    { return false ; }                                               // RETURN
  else if (                                          eKine < cutForNeutral  () )
    { return false ; }                                               // RETURN

  // check the status of the track
  GaussTrackInformation* info =
    gaussTrackInformation( track->GetUserInformation() );
  if( 0 == info )
    { Error("Invalid Track information") ; return false ; }     // RETURN

  // ID of the track to be stored
  const int sTrackID =
    keepLinks        () ? trackID :
    info->toBeStored () ? trackID : track -> GetParentID () ;

  // create new hit
  GaussSensPlaneHit* hit =
    new GaussSensPlaneHit
    ( sTrackID                                           ,
      LHCb::ParticleID       ( particle -> GetPDGEncoding () ) ,
      HepLorentzVector ( pre      -> GetPosition    () ,
                         pre      -> GetGlobalTime  () ) ,
      HepLorentzVector ( pre      -> GetMomentum    () ,
                         pre      -> GetTotalEnergy () ) ) ;

  // add it into collection
  m_collection -> insert ( hit    ) ;

  // update the track information
  if( trackID == sTrackID ) { info->addToHits( hit ) ; }

  return true ;
}

// ============================================================================
// The END
// ============================================================================
