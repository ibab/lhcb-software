// $Id: CaloSensDet.cpp,v 1.4 2003-06-05 08:27:56 robbep Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2003/05/14 08:43:42  robbep
// Addition of specific calo corrections in CaloSensDet :
// - Birk's law
// - timing
// - local non uniformity
//
// Revision 1.2  2002/12/13 16:52:57  ibelyaev
//  put updated versions of the packages
//
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
  , m_zmin             ( -1 * km )  // minimal z of forbidden zone 
  , m_zmax             (  1 * km )  // maximal z of forbidden zone 
  ///
  , m_collection       ( 0 )
  , m_c1               ( 0.013 * g/MeV/cm2 ) // 1st coef. of Birk's law
  , m_c2               ( 9.6E-6 * g*g/MeV/MeV/cm2/cm2 ) // 2nd coef
  //. of Birk's law
  , m_c1correction     ( 0.57142857 ) // correction of c1 for 2e charged part.
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
  setProperty     ( "DetectorDataProvider" ,  "DetectorDataSvc"  ) ;
  
  declareProperty ( "StartVolumes"         ,  m_startVolumeNames ) ;
  declareProperty ( "EndVolume"            ,  m_endVolumeName    ) ;
  declareProperty ( "CollectionName"       ,  m_collectionName   ) ;
  declareProperty ( "Detector"             ,  m_caloName         ) ;
  declareProperty ( "zMin"                 ,  m_zmin             ) ;
  declareProperty ( "zMax"                 ,  m_zmax             ) ;
  declareProperty ( "c1"                   ,  m_c1               ) ;
  declareProperty ( "c2"                   ,  m_c2               ) ;
  declareProperty ( "c1correction"         ,  m_c1correction     ) ;
  declareProperty ( "a_local_inner_ecal"   ,  m_a_local_inner_ecal ) ;
  declareProperty ( "a_local_middle_ecal"  ,  m_a_local_middle_ecal ) ;
  declareProperty ( "a_local_outer_ecal"   ,  m_a_local_outer_ecal ) ;
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
  //
  if( !m_volumesLocated ) {
    StatusCode sc = locateVolumes();
    if( sc.isFailure() ) { 
      Error("Error from 'locateVolumes' method",sc); 
    }
  }
  Assert( m_volumesLocated , "Could not locate volumes!");
  //
  
  m_collection = new CaloHitsCollection ( SensitiveDetectorName , 
                                          collectionName[0]     ) ; 
  //
  const int id  = GetCollectionID( 0 ) ;
  
  HCE -> AddHitsCollection( id , m_collection );
  
  //
  Print(" Initialize(): CollectionName='" + m_collection->GetName   () +
        "' for SensDet='"                 + m_collection->GetSDname () + 
        "'" , StatusCode::SUCCESS , MSG::VERBOSE                       ) ;
  //
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
  if     ( info -> toBeStored()     ) { 
    sTrackID = trackID ; 
  }  else {
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

  // Birk's Law Correction
  double ecorrected(edeposit) ;
  ecorrected *= birkCorrection(step) ;

  // Timing
  double frac1, frac2 ;
  int timeBin ;
  timeFraction( step , cellID , frac1 , frac2 , timeBin ) ;

  //Local NonUniformity
  ecorrected *= localNonUniformity( step , cellID ) ;

  if ( frac1 > 1E-7 ) sub->add( timeBin   , ecorrected*frac1 ) ;
  if ( frac2 > 1E-7 ) sub->add( timeBin+1 , ecorrected*frac2 ) ;

  return true ;
  
};
// ============================================================================
// Birk's Law
// ============================================================================
  /** Correction factor from Birk's Law
   *  Factor = 1/(1+C1*dEdx/rho+C2*(dEdx/rho)^2)
   *  Where :
   *      - C1 = 0.013 g/MeV/cm^2 [Ref NIM 80 (1970) 239]
   *      - C2 = 9.6.10^-6 g^2/MeV^2/cm^4
   *      - dEdx in MeV/cm
   *      - rho = density in g/cm^3
   */
double CaloSensDet::birkCorrection(G4Step* step) 
{
  double result = 1. ;
  if (step) {

    // Track
    const G4Track* track  = step->GetTrack() ;
    const double charge   = track->GetDefinition()->GetPDGCharge()  ;

    // Only charged tracks
    if (abs(charge)>0.1) {

      // Birk's law coefficients
      double C1       = m_c1 ;
      double C2       = m_c2 ;

      // Correction for charge 2 particles
      if (abs(charge)>1.5) {
        C1 *= m_c1correction ;
      }

      // Material
      const G4Material* mat = step->GetPreStepPoint()->GetMaterial() ;
      double density  = mat->GetDensity() ;

      // dEdx
      //      double dEdx = G4EnergyLossTables::GetDEDX(
      //                                        track->GetDefinition(),
      //                                        track->GetKineticEnergy(),
      //mat ) ;

      const G4MaterialCutsCouple *aMaterialCut = 
        track->GetMaterialCutsCouple() ;

      double dEdx = G4EnergyLossTables::GetDEDX(
                                                track->GetDefinition() ,
                                                track->GetKineticEnergy() ,
                                                aMaterialCut ) ;

      result = 1./(1. + C1*dEdx/density + C2*dEdx*dEdx/density/density ) ;
    }
  }
  
  return result ;
};

// ============================================================================
// Get Fraction of energy in consecutive time windows
// ============================================================================
  
  /** The energy deposited in the cell by the step contributes 
   *  - frac1*energy in the time bin timeBin (25 ns bins)
   *  - frac2*energy in the time bin timeBin+1
   *  the origin is the time of collision
   */

void CaloSensDet::timeFraction(G4Step* step, const CaloCellID& cell,
                               double& frac1, double& frac2, int& timeBin)
{
  frac1 = 1. ;
  frac2 = 0. ;
  timeBin = 0 ;

  // Only for ECAL for the moment
  if ( CaloCellCode::CaloNameFromNum( cell.calo() ) == "Ecal" ) {  

    // Time of the step in Geant = time since the primary event
    double timeOfStep = step->GetPreStepPoint()->GetGlobalTime() ;
    // Time of flight between z=0 and z=Ecal 
    double dist       = sqrt( m_calo->cellX(cell)*
                              m_calo->cellX(cell)
                              + m_calo->cellY(cell)*
                              m_calo->cellY(cell) 
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
// Local Non Uniformity
// ============================================================================
  /** Correction due to the local non uniformity due to the light 
   *  collection efficiency in cell cell
   */

double CaloSensDet::localNonUniformity(G4Step* step, const CaloCellID& cell) 
{
  
  // Only for ECal for the moment
  double correction = 1. ;
  
  if ( CaloCellCode::CaloNameFromNum( cell.calo() ) == "Ecal" ) {
  
    // Find the position of the step
    double x        = step->GetPreStepPoint()->GetPosition().x() ;
    double y        = step->GetPreStepPoint()->GetPosition().y() ;
    // Center of the cell
    double x0       = m_calo->cellX( cell ) ;
    double y0       = m_calo->cellY( cell ) ;
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
      1. - 
      A_local * 
      cos(2.*pi*(x-x0)/d) *
      cos(2.*pi*(y-y0)/d) ;
  }
  
  return correction ;
  
};



// ============================================================================
// The END 
// ============================================================================
