// $Id: CaloSensDet.cpp,v 1.2 2002-12-13 16:52:57 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2002/12/07 21:19:14  ibelyaev
//  few optimization updates
//
// Revision 1.1  2002/12/07 14:41:44  ibelyaev
//  add new Calo stuff
//
// Revision 1.4  2002/05/07 12:21:36  ibelyaev
//  see $GIGAROOT/doc/release.notes  7 May 2002
//
// ============================================================================
/// SRD & STD 
#include <algorithm>
#include <vector>
/// CLHEP 
#include "CLHEP/Geometry/Point3D.h"
/// GaudiKernel
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h" 
#include "GaudiKernel/IDataProviderSvc.h"
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
// GiGaCnv 
#include "GiGaCnv/GiGaVolumeUtils.h"
// CaloDet
#include "CaloDet/DeCalorimeter.h"
/// local
#include "CaloSensDet.h"
#include "CaloHit.h"
#include "CaloSimHash.h"
///
#include "AIDA/IHistogram1D.h"

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
/** @fn numHits 
 *  fast evaluator of totan number of hits, including subhits 
 *  @author Vanya Belyaev
 *  @date   23/01/2001 
 */
inline size_t numHits( const CaloSensDet::HitMap& hitmap )
{
  typedef CaloSensDet::HitMap HitMap;
  size_t size = 0 ;
  for( HitMap::const_iterator entry = hitmap.begin() ;
       hitmap.end() != entry ; ++entry )
    {
      const CaloHit* hit = entry->second ;
      if( 0 != hit ) { size += hit->totalSize() ; }
    }
  return size ;
};
// ============================================================================

// ============================================================================
/// factory business 
// ============================================================================
IMPLEMENT_GiGaFactory( CaloSensDet );
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
  : GiGaSensDetBase     ( type , name , parent ) 
  , G4VSensitiveDetector( name  )
  , m_endVolumeName    ( "World" )  
  , m_end              ( 0       ) 
  , m_startVolumeNames (         ) 
  , m_start            (         )
  , m_volumesLocated   ( false   )
  ///
  , m_collectionName   ( "Hits"  ) 
  , m_table            ()
  , m_hitmap           ()
  //
  , m_caloName         ( DeCalorimeterLocation::Ecal ) 
  , m_calo             ( 0 ) 
  ///
  , m_zmin             (  1 * km )  // minimal z of forbidden zone 
  , m_zmax             ( -1 * km )  // maximal z of forbidden zone 
  ///
  , m_collection       ( 0 )
{
  ///
  setProperty     ( "DetectorDataProvider" ,  "DetectorDataSvc"  ) ;
  
  declareProperty ( "StartVolumes"         ,  m_startVolumeNames ) ;
  declareProperty ( "EndVolume"            ,  m_endVolumeName    ) ;
  declareProperty ( "CollectionName"       ,  m_collectionName   ) ;
  declareProperty ( "Detector"             ,  m_caloName         ) ;
  declareProperty ( "zMin"                 ,  m_zmin             ) ;
  declareProperty ( "zMax"                 ,  m_zmax             ) ;
  //
};
// ============================================================================

// ============================================================================
/// destructor 
// ============================================================================
CaloSensDet::~CaloSensDet(){};
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
  if( 0 == detSvc() ) { return Error("detSvc() points to NULL!") ; }
  m_calo = get( detSvc() , m_caloName , m_calo );
  if( 0 == m_calo   ) { return StatusCode::FAILURE               ; }
  
  ///
  return StatusCode::SUCCESS ;
};
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
  // finalize the base class 
  return GiGaSensDetBase::finalize();
};
// ============================================================================

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
};
// ============================================================================

// ============================================================================
/** method from G4 
 *  (Called at the begin of each event)
 *  @see G4VSensitiveDetector 
 *  @param HCE pointer to hit collection of current event 
 */
// ============================================================================
void CaloSensDet::Initialize( G4HCofThisEvent* HCE )
{
  ///
  if( !m_volumesLocated ) 
    {
      StatusCode sc = locateVolumes();
      if( sc.isFailure() ) { Error("Error from 'locateVolumes' method",sc); }
    }
  Assert( m_volumesLocated , "Could not locate volumes!");
  ///
  m_collection = new CaloHitsCollection ( SensitiveDetectorName , 
                                          collectionName[0]     ) ; 
  /// 
  const int id  = GetCollectionID( 0 )  ;
  HCE -> AddHitsCollection( id , m_collection );
  ///
  Print(" Initialize(): CollectionName='" + m_collection->GetName   () +
        "' for SensDet='"                 + m_collection->GetSDname () + 
        "'" , StatusCode::SUCCESS , MSG::VERBOSE                       ) ;
  ///
  m_hitmap.clear() ;
};
// ============================================================================

// ============================================================================
/** method from G4 
 *  (Called at the end of each event)
 *  @see G4VSensitiveDetector 
 *  @param HCE pointer to hit collection of current event 
 */
// ============================================================================
void CaloSensDet::EndOfEvent( G4HCofThisEvent* HCE ) 
{ m_hitmap.clear() ; };
// ============================================================================

// ============================================================================
/** process the hit
 *  @param step     pointer to current Geant4 step 
 *  @param history  pointert to touchable history 
 */
// ============================================================================
bool CaloSensDet::ProcessHits( G4Step* step , 
                               G4TouchableHistory* /* history */ ) 
{

  
  if( 0 == step ) { return false ; } 
  ///
  const double      edeposit  = step-> GetTotalEnergyDeposit                () ;
  const HepPoint3D& prePoint  = step-> GetPreStepPoint  () -> GetPosition   () ;
  const HepPoint3D& postPoint = step-> GetPostStepPoint () -> GetPosition   () ;
  const double      time      = step-> GetPreStepPoint  () -> GetGlobalTime () ;
  ///
  const G4Track*    track     = step -> GetTrack      () ;
  const int         trackID   = track-> GetTrackID    () ;
  const double      charge    = track-> GetDefinition () -> GetPDGCharge  () ;
  
  if( edeposit <= 0           ) { return false ; }
  if( 0 == int( charge * 10 ) ) { return false ; }
  
  G4TouchableHistory* tHist  = (G4TouchableHistory*)
    ( step->GetPreStepPoint()->GetTouchable() ) ;
  
  CaloSim::Path path ;
  path.reserve( 10 ) ;
  
  for( int level = 0 ; level < tHist->GetHistoryDepth() ; ++level ) 
    {
      const G4VPhysicalVolume* pv = tHist -> GetVolume( level ) ;
      if( 0 == pv ) { continue ; }                          // CONTINUE 
      const G4LogicalVolume*   lv = pv    -> GetLogicalVolume();
      if( 0 == lv ) { continue ; }                          // CONTINUE 
      
      //  start volume ? 
      if      (  m_end == lv  )    { break ; } // BREAK    
      // "useful" volume 
      else if ( !path.empty() ) 
        { path.push_back( tHist->GetReplicaNumber( level ) ); }
      else if ( m_start .end() != std::find( m_start .begin () , 
                                             m_start .end   () , lv ) )
        { path.push_back( tHist->GetReplicaNumber( level ) ); }
      // end volume ?
    }
  
  if( path.empty() ) 
    { Error("Replica Path is invalid!") ; return false ; }       // RETURN 
  
  // find the cellID 
  CaloCellID  cellID( m_table( path ) ) ;
  if( CaloCellID() == cellID ) 
    {
      cellID          = calo() -> Cell ( prePoint );
      m_table( path ) = cellID ;
    }
  if( CaloCellID() == cellID ) 
    { Error ("Invalid cell is found") ; return false ; }        // RETURN 
  
  // get the existing hit 
  CaloHit*&    hit = m_hitmap( cellID );                        // ATTENTION 
  if( 0 == hit )  // hit does not exists 
    {
      // create new hit 
      hit = new CaloHit      ( cellID ) ; 
      // add it into collection 
      m_collection -> insert ( hit    ) ;
    }
  
  // check the status of the track
  GaussTrackInformation* info = 
    gaussTrackInformation( track->GetUserInformation() );
  if( 0 == info ) 
    { Error("Invalid Track information") ; return false ; }     // RETURN
  
  /// ID of track to be stored 
  int sTrackID     = track -> GetParentID () ;
  // already marked to be stored:
  if     ( info -> toBeStored()     ) { sTrackID = trackID ; }
  else  
    {
      const double z0 = track->GetVertexPosition().z() ;
      // outside the "forbidden zone" 
      if ( z0 < m_zmin || z0 > m_zmax   ) { sTrackID = trackID ; }
    }
  /// hit is exist for given track? 
  CaloSubHit*& sub  = hit->hit( sTrackID ) ;                   // ATTENTION
  // create new subhit if needed 
  if( 0 == sub ) { sub = new CaloSubHit ( cellID , sTrackID ) ; }
  /// update the track informoation
  if( trackID == sTrackID ) { info->addToHits( sub ); }
  // add current energy deposition to the sub-hit
  CaloSubHit::Time tm = 0   ; 
  sub->add( tm , edeposit ) ;
  
  return true ;
  
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
