// $Id: CaloSensDet.cpp,v 1.24 2008-07-11 10:47:44 robbep Exp $
// Include files

// SRD & STD
#include <algorithm>
#include <vector>
#include <sstream>

// LHCbDefintions
#include "CLHEP/Geometry/Point3D.h"

// from Gaudi
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/Stat.h"

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
#include "G4MaterialCutsCouple.hh"

// GiGaCnv
#include "GiGaCnv/GiGaVolumeUtils.h"

// CaloDet
#include "CaloDet/DeCalorimeter.h"

// local
#include "CaloSensDet.h"
#include "CaloHit.h"
#include "CaloSimHash.h"

// AIDA
#include "AIDA/IHistogram1D.h"

// Boost
#include "boost/format.hpp"

// ============================================================================
/** @file
 *
 *  Implementation of class CaloSensDet
 *
 *  @author Vanya Belyaev
 *  @date   23/01/2001
 */
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
CaloSensDet::CaloSensDet
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : G4VSensitiveDetector ( name  )
  , GiGaSensDetBase      ( type , name , parent )
  , m_endVolumeName      ( "World" )
  , m_end                ( 0       )
  , m_startVolumeNames   (         )
  , m_start              (         )
  , m_volumesLocated     ( false   )
  ///
  , m_collectionName     ( "Hits"  )
  , m_table              ()
  , m_hitmap             ()
  //
  , m_caloName           ( DeCalorimeterLocation::Ecal )
  , m_calo               ( 0 )
  , m_caloID             ( 0 )
  ///
  , m_zmin               ( -1 * CLHEP::km )  // minimal z of forbidden zone
  , m_zmax               (  1 * CLHEP::km )  // maximal z of forbidden zone
  ///
  , m_collection         ( 0 )
  // input histograms
  , m_histoNames         (   )
  , m_histos             (   )
  , m_histoSvc           ( 0 )
  //
  , m_birk_c1            ( 0.013  * CLHEP::g/CLHEP::MeV/CLHEP::cm2 ) // 1st coef. of Birk's
  , m_birk_c2            ( 9.6E-6 * CLHEP::g*CLHEP::g/CLHEP::MeV/CLHEP::MeV/CLHEP::cm2/CLHEP::cm2 ) // 2nd coef
  //. of Birk's law correction of c1 for 2e charged part.
  , m_birk_c1correction  ( 0.57142857                   )
  /// correction to t0
  , m_dT0                ( 0.5 * CLHEP::ns              )
{
  setProperty     ( "DetectorDataProvider" ,  "DetectorDataSvc"   ) ;

  declareProperty ( "StartVolumes"         ,  m_startVolumeNames  ) ;
  declareProperty ( "EndVolume"            ,  m_endVolumeName     ) ;
  declareProperty ( "CollectionName"       ,  m_collectionName    ) ;
  declareProperty ( "Detector"             ,  m_caloName          ) ;
  declareProperty ( "zMin"                 ,  m_zmin              ) ;
  declareProperty ( "zMax"                 ,  m_zmax              ) ;
  //
  declareProperty ( "BirkC1"               ,  m_birk_c1           ) ;
  declareProperty ( "BirkC1cor"            ,  m_birk_c1correction ) ;
  declareProperty ( "BirkC2"               ,  m_birk_c2           ) ;
  //
  declareProperty ( "dT0"                  ,  m_dT0               ) ;
  // input histograms(parametrization)
  declareProperty ( "Histograms"           ,  m_histoNames        ) ;
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
StatusCode CaloSensDet::initialize   ()
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
  m_calo = getDet<DeCalorimeter>(  m_caloName );
  if( 0 == m_calo   ) { return StatusCode::FAILURE                     ; }
  m_caloID   = CaloCellCode::CaloNumFromName( caloName()             ) ;
  if( 0 >  caloID() ) { return Error("Invalid detector name/number!" ) ; }
  ///
  m_histoSvc = svc<IHistogramSvc> ( "HistogramDataSvc" , true ) ;
  { // load all input histos
    for( Names::const_iterator ihist = m_histoNames.begin() ;
         m_histoNames.end() != ihist ; ++ihist )
      {
        SmartDataPtr<IHistogram1D> pHist( histoSvc() , *ihist );
        IHistogram1D* hist = pHist ;
        if( 0 == hist )
          { return Error("Cannot load histogram '"+(*ihist)+"'"); }
        m_histos.push_back ( hist ) ;
      }
    if ( histos().empty() )
    { Warning ( "Empty vector of input time-histograms" ) ; }
  }
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
StatusCode CaloSensDet::finalize    ()
{
  // reset the detector element
  m_calo         = 0 ;
  // clear the translation table
  m_table  .clear () ;
  // clear hit map
  m_hitmap .clear () ;
  // clear volumes
  m_start  .clear () ;
  // clear histograms
  m_histos .clear () ;
  // finalize the base class
  return GiGaSensDetBase::finalize();
}

// ============================================================================
/** helpful method to locate start and end volumes
 *  @return status code
 */
// ============================================================================
StatusCode  CaloSensDet::locateVolumes()
{
  // locate start volumes
  for( Names::const_iterator vol =  m_startVolumeNames.begin() ;
       m_startVolumeNames.end() != vol ; ++vol )
    {
      // look through converted volumes
      const G4LogicalVolume* lv = GiGaVolumeUtils::findLVolume   ( *vol );
      if( 0 == lv )
        { return Error("G4LogicalVolume* points to 0 for "+ (*vol) );}
      m_start.push_back( lv );
    }
  if( m_start.empty() ) { return Error("Size of 'StartVolumes' is 0 "); }
  // locate end volume : look through converted volumes
  m_end = GiGaVolumeUtils::findLVolume   ( m_endVolumeName );
  if( 0 == m_end )
    { return Error("G4LogicalVolume* points to 0 for '"+m_endVolumeName+"'");}
  // set flag
  m_volumesLocated = true ;
  //
  return StatusCode::SUCCESS ;
}

// ============================================================================
/** method from G4
 *  (Called at the begin of each event)
 *  @see G4VSensitiveDetector
 *  @param HCE pointer to hit collection of current event
 */
// ============================================================================
void CaloSensDet::Initialize( G4HCofThisEvent* HCE )
{
  //
  if( !m_volumesLocated )
  {
    StatusCode sc = locateVolumes();
    if ( sc.isFailure() ) { Error("Error from 'locateVolumes' method",sc); }
  }
  Assert ( m_volumesLocated , "Could not locate volumes!");
  //
  m_collection = new CaloHitsCollection ( SensitiveDetectorName ,
                                          collectionName[0]     ) ;
  //
  const int id  = GetCollectionID( 0 ) ;

  HCE -> AddHitsCollection( id , m_collection );

  //
  if ( msgLevel ( MSG::DEBUG ) )
  {
    debug() << " Initialize(): CollectionName='"
            << m_collection->GetName   ()
            << "' for SensDet='"
            << m_collection->GetSDname ()
            <<"'" << endreq ;
  }
  //
  m_hitmap.clear() ;
}

// ============================================================================
/** method from G4
 *  (Called at the end of each event)
 *  @see G4VSensitiveDetector
 *  @param HCE pointer to hit collection of current event
 */
// ============================================================================
void CaloSensDet::EndOfEvent ( G4HCofThisEvent* /* HCE */ )
{
  /// clear the map
  m_hitmap.clear();

  if ( !printStat() && !msgLevel ( MSG::DEBUG ) ) { return ; }

  if ( 0 == m_collection )
  { Warning ( " EndOfEvent(): HitCollection points to NULL " ) ; return ; }
  typedef std::vector<CaloHit*> Hits ;
  const Hits* hits = m_collection ->GetVector() ;
  if ( 0 == hits )
  { Error   (" EndOfEvent(): HitVector* points to NULL "     ) ; return ; }

  // initialize counters
  const size_t nhits = hits->size()  ;
  size_t nshits = 0 ;
  size_t nslots = 0 ;
  double energy = 0 ;

  // the loop over all hits
  for ( Hits::const_iterator ihit = hits->begin() ;
        hits->end() != ihit ; ++ihit )
  {
    const CaloHit* hit = *ihit ;
    if( 0 == hit ) { continue ; }                           // CONTINUE
    nshits += hit -> size      () ;
    nslots += hit -> totalSize () ;
    energy += hit -> energy    () ;
  }

  energy /= CLHEP::GeV ;                               // NB: rescale to GeV

  counter ( "#hits"    ) += nhits  ;
  counter ( "#subhits" ) += nshits ;
  counter ( "#tslots"  ) += nslots ;
  counter ( "#energy"  ) += energy ;

  if ( msgLevel ( MSG::DEBUG ) )
  {
    always() << boost::format
      ( " #Hits=%5d #SubHits=%5d #Slots=%5d Energy=%8.3g[GeV] " )
      % nhits % nshits % nslots % energy << endreq ;
  }

}

// ============================================================================
/** process the hit
 *  @param step     pointer to current Geant4 step
 *  @param history  pointert to touchable history
 */
// ============================================================================
bool CaloSensDet::ProcessHits
( G4Step* step                      ,
  G4TouchableHistory* /* history */ )
{

  if( 0 == step ) { return false ; }
  ///
  const double                      deposit  = step-> GetTotalEnergyDeposit () ;
  if( deposit <= 0            ) { return false ; }                 // RETURN

  const G4Track*              const track    = step     -> GetTrack      () ;
  const int                         trackID  = track    -> GetTrackID    () ;
  const G4ParticleDefinition* const particle = track    -> GetDefinition () ;
  const double                      charge   = particle -> GetPDGCharge  () ;

  if ( 0 == int ( charge * 10 ) ) { return false ; }               // RETURN

  const G4StepPoint* const          preStep  = step    -> GetPreStepPoint () ;
  const HepGeom::Point3D<double>&   prePoint = preStep -> GetPosition     () ;
  const double                      time     = preStep -> GetGlobalTime   () ;
  const G4MaterialCutsCouple* const material = preStep ->
    GetMaterialCutsCouple () ;

  const LHCb::CaloCellID cellID = cell ( preStep ) ;
  if ( LHCb::CaloCellID() == cellID ) { return false ; }

  // get the existing hit
  CaloHit*& hit = m_hitmap[ cellID ] ;
  if ( 0 == hit )
  {
    // create new hit
    hit = new CaloHit      ( cellID ) ;
    // add it into collection
    m_collection -> insert ( hit    ) ;
  }

  // check the status of the track
  GaussTrackInformation* info =
    gaussTrackInformation ( track->GetUserInformation() );
  if( 0 == info )
  { Error("Invalid Track information") ; return false ; }     // RETURN

  // ID of the track to be stored
  int sTrackID     = track -> GetParentID () ;

  // already marked to be stored:
  if     ( info -> toBeStored()     ) { sTrackID = trackID ; }
  else
  {
    // z-position of production vertex
    const double z0 = track->GetVertexPosition().z() ;
    // outside the "forbidden zone" ?
    if ( z0 < zMin() || z0 > zMax ()  ) { sTrackID = trackID ; }
  }

  // Does the hit exist for the given track?
  CaloSubHit*& sub  = hit->hit( sTrackID ) ;                   // ATTENTION
  // create new subhit if needed
  if ( 0 == sub ) { sub = new CaloSubHit ( cellID , sTrackID ) ; }
  // update the track information
  if ( trackID == sTrackID ) { info->addToHits ( sub ) ; }

  // perform the specific sub-detector action
  StatusCode sc = fillHitInfo ( sub       ,
                                prePoint  ,
                                time      ,
                                deposit   ,
                                track     ,
                                particle  ,
                                material  ,
                                step      ) ;

  if ( sc.isFailure() ){ Error("The SubHit information is not filled!",sc) ; }

  return true ;
}

// ============================================================================
