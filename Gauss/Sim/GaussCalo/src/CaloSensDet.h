// $Id: CaloSensDet.h,v 1.15 2007-03-18 21:33:19 gcorti Exp $
#ifndef       GAUSSCALO_CaloSensDet_H
#define       GAUSSCALO_CaloSensDet_H 1

// GaudiKernel
#include "GaudiKernel/HashMap.h"

// GiGa
#include "GiGa/GiGaSensDetBase.h"

// AIDA
#include "AIDA/IHistogram1D.h"

// local
#include "CaloSim.h"
#include "CaloSimHash.h"
#include "CaloHit.h"

// GEANT4
#include "G4Material.hh"
#include "G4MaterialCutsCouple.hh"

#include "CLHEP/Geometry/Transform3D.h"

// forward declarations
class DeCalorimeter ;  // CaloDet
class IHistogramSvc ;  // GaudiKernel

/** @class CaloSensDet CaloSensDet.h CaloSensDet.h
 *
 *
 *  @author  Vanya Belyaev
 *  @date    23/01/2001
 */
class CaloSensDet: public GiGaSensDetBase
{
public:
  /// useful type for list of names
  typedef std::vector<std::string>              Names      ;
  /// useful type for list of logical volumes
  typedef std::vector<const G4LogicalVolume*>   Volumes    ;
  /// translator from Path to CellID
  typedef GaudiUtils::HashMap<CaloSim::Path,LHCb::CaloCellID> Table  ;
  typedef GaudiUtils::HashMap<LHCb::CaloCellID,CaloHit*>      HitMap ;
  /// type for all histograms
  typedef std::vector<AIDA::IHistogram1D*>            Histos     ;
  /// the typedef for vector of fractions
  typedef std::vector<double>                   Fractions  ;

public :

  /** standard initialization (Gaudi)
   *  @see GiGaSensDetBase
   *  @see GiGaBase
   *  @see   AlgTool
   *  @see  IAlgTool
   *  @return status code
   */
  virtual StatusCode initialize   ();

  /** standard finalization (Gaudi)
   *  @see GiGaSensDetBase
   *  @see GiGaBase
   *  @see   AlgTool
   *  @see  IAlgTool
   *  @return status code
   */
  virtual StatusCode finalize    ();

public:

  /** process the hit.
   *  The method is invoked by G4 for each step in the
   *  sensitive detector. This implementation performs the
   *  generic (sub-detector) independent action:
   *    - decode G4Step, G4Track information
   *    - determine the LHCb::CaloCellID of the active cell
   *    - determine is the hit to be associated with
   *      the track or its parent track
   *    - find/create CaloHit object for the given cell
   *    - find/create CaloSubHit object for given track
   *    - invoke fillHitInfo method for filling the hit with
   *      the actual (subdetector-specific) informaton
   *      (Birk's corrections, local/global non-uniformity
   *      corrections, timing, etc)
   *  @param step     pointer to current Geant4 step
   *  @param history  pointert to touchable history
   *  @attention One should not redefine this method for specific sub-detectors.
   */
  virtual bool ProcessHits
  ( G4Step*             step    ,
    G4TouchableHistory* history ) ;

  /** method from G4
   *  (Called at the begin of each event)
   *  @see G4VSensitiveDetector
   *  @param HCE pointer to hit collection of current event
   */
  virtual void Initialize( G4HCofThisEvent* HCE ) ;

  /** method from G4
   *  (Called at the end of each event)
   *  @see G4VSensitiveDetector
   *  @param HCE pointer to hit collection of current event
   */
  virtual void EndOfEvent( G4HCofThisEvent* HCE ) ;

protected:

  /** helpful method to locate start and end volumes
   *  @return status code
   */
  StatusCode locateVolumes();

public:

  /** standard constructor
   *  @see GiGaSensDetBase
   *  @see GiGaBase
   *  @see AlgTool
   *  @param type type of the object (?)
   *  @param name name of the object
   *  @param parent  pointer to parent object
   */
  CaloSensDet
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;

  /// destructor (virtual and protected)
  virtual ~CaloSensDet() {};

private:

  // no default constructor
  CaloSensDet() ;
  // no copy constructor
  CaloSensDet           ( const CaloSensDet& );
  // no assignement
  CaloSensDet& operator=( const CaloSensDet& ) ;

protected:

  /// acess to detector element
  inline const DeCalorimeter* calo     () const  { return m_calo       ; }

  /// return the (De)Calorimeter name
  inline const std::string&   caloName () const { return m_caloName    ; }

  /// return the (De)Calorimeter ID
  inline int                  caloID   () const  { return m_caloID     ; }

  /// access to hit collection
  inline CaloHitsCollection*  hits     () const  { return m_collection ; }

  /// access to translation table
  inline Table&               table    ()        { return m_table      ; }

  /// access to hit map
  inline HitMap&              hitmap   ()        { return m_hitmap     ; }

  /// access to minimal z
  inline double               zMin     () const  { return m_zmin       ; }

  /// access to maximal z
  inline double               zMax     () const  { return m_zmax       ; }

  /// the additive correction to 't0' evaluation
  inline double               dT0      () const  { return m_dT0        ; }

  /** the evaluation of t0 for given calorimetr cell
   *  @param cellID the calorimeter cell identifier
   *  @return "time"-parameter for the given cell
   *           it is evaluated per cell basis in DeCalorimeter
   */
  inline double               t0       ( const LHCb::CaloCellID& cellID ) const ;

  /// the first coefficient of Birks's law
  inline double               birk_c1    () const { return m_birk_c1 ; }
  /// the second coefficient of Birks's law
  inline double               birk_c2    () const { return m_birk_c2 ; }
  /// the correction to the first coefficient of Birks's law
  inline double               birk_c1cor () const
  { return m_birk_c1correction ; }

  /// the accessor to all time-histograms      (const version)
  inline const Histos&        histos     () const { return m_histos     ; }
  /// the accessor to all time-histograms  (non-const version)
  inline       Histos&        histos     ()       { return m_histos     ; }
  /// the accessor to the names of  histograms
  inline const Names&         histoNames () const { return m_histoNames ; }

  /// accessor to histogram service
  inline IHistogramSvc*       histoSvc   () const { return m_histoSvc   ; }

protected:

  /** helper method to locate the Calorimeter cell to which
   *  G4 point belongs to
   *  @param point G4 point
   *  @retuen calorimeter cell identifier
   */
  inline LHCb::CaloCellID cell ( const G4StepPoint* point ) const ;

protected:

  /** fill the hit with the concrete information about the energy and the time.
   *  The function is to be called from ProcessHits method.
   *
   *  @param hit        hit  to be filled with the correct information,
   *                    the cellID information is accessible from the hit
   *  @param prePoint   PreStepPoint  for the given step
   *  @param globalTime the global time of prestep point
   *  @param deposit    raw energy deposition for the step
   *  @param track      the actual G4Track pbject (decoded from G4Step)
   *  @param pdef       the actual particle type (decoded from G4Step/G4Track)
   *  @param material   the actual material       (decoded from G4Step)
   *  @param step       the step itself, the most important
   *                    information from the step
   *                    is already decoded into prePoint,globalTime,track,
   *                    particle definition,material
   *                    etc for efficiency reasons.
   *
   *  @attention all subdetector specific action
   *    (Birk's,timing, non-uniformities, etc ) need to be applied here!
   *
   */
  virtual StatusCode    fillHitInfo
  ( CaloSubHit*                 hit         ,
    const HepGeom::Point3D<double>& prePoint    ,
    const double                globalTime  ,
    const double                deposit     ,
    const G4Track*              track       ,
    const G4ParticleDefinition* pdef        ,
    const G4MaterialCutsCouple* material    ,
    const G4Step*               step        ) const = 0 ;

  /** The fractions of energy deposited in consequitive time-bins
   *  for the given calorimeter cell
   *  @param time global time of energy deposition
   *  @param cell cellID of the cell
   *  @param bin (out) the first time bin
   *  @param fracs the vector of fractions for subsequent time-bins;
   *  @return StatuscCode
   */
  virtual StatusCode timing
  ( const double      time      ,
    const LHCb::CaloCellID& cell      ,
    CaloSubHit::Time& slot      ,
    Fractions&        fractions ) const  = 0 ;

protected:

  /** Correction factor from Birk's Law
   *  @param step current G4step
   *  @return the correction factor
   */
  inline double birkCorrection ( const G4Step* step ) const ;

  /** Birk's correction for given particle with given kinetic energy
   *  for the given material
   *  @param  particle pointer to particle definition
   *  @param  Ekine    particle kinetic energy
   *  @param  maerial  pointer ot teh material
   */
  inline double  birkCorrection
  ( const G4ParticleDefinition* particle ,
    const double                eKine    ,
    const G4MaterialCutsCouple* material ) const ;

  /** evaluate the correction for Birk's law
   *  @param charge   the charge of the particle
   *  @param dEdX     the nominal dEdX in the material
   *  @param material the pointer ot teh material
   *  @return the correction coefficient
   */
  inline double  birkCorrection
  ( const double      charge   ,
    const double      dEdX     ,
    const G4Material* material ) const ;

  /** evaluate the correction for Birk's law
   *  @param charge   the charge of the particle
   *  @param dEdX     the nominal dEdX in the material
   *  @param density  the density ot the material
   *  @return the correction coefficient
   */
  inline double  birkCorrection
  ( const double      charge   ,
    const double      dEdX     ,
    const double      density  ) const ;

private:

  std::string                                 m_endVolumeName       ;
  const G4LogicalVolume*                      m_end                 ;

  // definition logical volumes where will be analysed signal
  Names                                       m_startVolumeNames    ;
  Volumes                                     m_start               ;

  bool                                        m_volumesLocated      ;

  /// name for created collection
  std::string                                 m_collectionName      ;

  // translation table
  mutable Table                               m_table               ;
  HitMap                                      m_hitmap              ;

  std::string                                 m_caloName            ;
  const DeCalorimeter*                        m_calo                ;
  int                                         m_caloID              ;

  double                                      m_zmin                ;
  double                                      m_zmax                ;

  CaloHitsCollection*                         m_collection          ;

private:

  /// the vector of histogram names/addresses
  Names                                      m_histoNames ;
  /// all histograms
  Histos                                     m_histos     ;
  /// histogram service
  IHistogramSvc*                             m_histoSvc   ;

private:

  // the first coefficient of Birk's law                    (c1)
  double                                      m_birk_c1             ;
  // the second coefficient of Birk's law                   (c2)
  double                                      m_birk_c2             ;
  // the correction to the first coefficient of Birk's law  (c1')
  double                                      m_birk_c1correction   ;

private:

  // the additive correction for the evaluation of t0
  double   m_dT0    ;

};
// ============================================================================

// ============================================================================
/** the evaluation of t0 for given calorimetr cell
 *  @param cellID the calorimeter cell identifier
 *  @return "time"-parameter for the given cell
 *           it is evaluated per cell basis in DeCalorimeter
 */
// ============================================================================
inline double CaloSensDet::t0       ( const LHCb::CaloCellID& cellID ) const
{ return calo()->cellTime( cellID ) - dT0 () ; }
// ============================================================================
/** helper method to locate the Calorimeter cell to which
 *  G4 point belongs to
 *  @param point G4 point
 *  @retuen calorimeter cell identifier
 */
// ============================================================================
inline LHCb::CaloCellID CaloSensDet::cell ( const G4StepPoint* point ) const
{
  // current solution!
  // LHCb::CaloCellID id = calo() -> Cell ( point->GetPosition() ) ;
  // return id ;

  G4TouchableHistory* tHist  =
    (G4TouchableHistory*) point->GetTouchable()  ;

  const int nLevel = tHist->GetHistoryDepth() ;
  CaloSim::Path path ;
  path.reserve ( nLevel ) ;

  for ( int  level = 0 ; level < nLevel ; ++level )
  {
    const G4VPhysicalVolume* pv = tHist->GetVolume ( level ) ;
    if      (  0     == pv  ) { continue ; }                        // CONTINUE
    const G4LogicalVolume*   lv = pv -> GetLogicalVolume() ;
    if      (  0     == lv  ) { continue ; }                        // CONTINUE
    // start volume ??
    if      (  m_end == lv  ) { break    ; }                        // BREAK
    // useful volume ?
    if      ( !path.empty() ) { path.push_back( pv ) ; }
    else if ( m_start.end() !=
              std::find ( m_start.begin () ,
                          m_start.end   () , lv ) ) { path.push_back( pv ) ; }
  }

  if ( path.empty() )
  { Error ( "Volume path is invalid(empty) " ) ; return LHCb::CaloCellID() ; }

  // find the appropriate ID
  Table::iterator ifound = m_table.find ( path ) ;
  // already in the table ?
  if ( m_table.end() != ifound ) { return ifound->second ; }    // RETURN

  // if not: add it into the table!
  // CLHEP -> ROOT
  HepGeom::Transform3D mtrx( *(tHist->GetRotation()) , tHist->GetTranslation() );
  auto heppoint = mtrx * HepGeom::Point3D<double>{};
  const Gaudi::XYZPoint p ( heppoint.x(),heppoint.y(),heppoint.z() );
  // get the right cell
  const CellParam* par = calo()->Cell_( p ) ;
  if ( 0 == par || !par->valid() )
  {
    m_table.insert ( Table::value_type( path , LHCb::CaloCellID() ) ) ;
    return LHCb::CaloCellID() ;                                   // RETURN
  }
  //
  m_table.insert ( Table::value_type( path , par-> cellID() ) ) ;
  //
  return par->cellID() ;
}

// ============================================================================
/** evaluate the correction for Birk's law
 *  @param charge   the charge of the particle
 *  @param dEdX     the nominal dEdX in the material
 *  @param density
 *  @return the correction coefficient
 */
// ============================================================================
inline double  CaloSensDet::birkCorrection
( const double      charge   ,
  const double      dEdX     ,
  const double      density  ) const
{
  const double C1 =
    fabs( charge )  < 1.5 ? birk_c1() : birk_c1() * birk_c1cor() ;
  const double C2 = birk_c2() ;

  const double alpha = dEdX/ density ;

  return 1.0 / ( 1.0 + C1 * alpha + C2 * alpha * alpha ) ;
}

// ============================================================================
/** evaluate the correction for Birk's law
 *  @param charge   the charge of the particle
 *  @param dEdX     the nominal dEdX in the material
 *  @param material the pointer ot teh material
 *  @return the correction coefficient
 */
// ============================================================================
inline double  CaloSensDet::birkCorrection
( const double      charge   ,
  const double      dEdX     ,
  const G4Material* material ) const
{
  if ( 0 == material )
  { Error("birkCorrection(): invalid material " ) ; return 1. ; } // RETURN
  return birkCorrection( charge , dEdX , material->GetDensity() ) ;
}

// ============================================================================
/** Birk's correction for given particle with given kinetic energy
 *  for the given material
 *  @param  particle pointer to particle definition
 *  @param  Ekine    particle kinetic energy
 *  @param  maerial  pointer ot teh material
 */
// ============================================================================
inline double CaloSensDet::birkCorrection
( const G4ParticleDefinition* particle ,
  const double                eKine    ,
  const G4MaterialCutsCouple* material ) const
{
  if (  0 == particle || 0 == material )
  { Error("birkCorrection(): invalid parameters " ) ; return 1.0 ; } // RETURN

  const double charge = particle -> GetPDGCharge() ;
  if ( fabs ( charge ) < 0.1                      ) { return 1.0 ; } // EEUTRN

  // get the nominal dEdX
  const double dEdX  =
    G4EnergyLossTables::GetDEDX ( particle , eKine , material ) ;

  // make an actual evaluation
  return birkCorrection
    ( charge ,
      dEdX   ,
      material->GetMaterial()->GetDensity() ) ;
}

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
// ============================================================================
inline double CaloSensDet::birkCorrection ( const G4Step* step ) const
{
  if ( !step ) { return 1 ; }                               // RETURN

  // Track
  const G4Track* track  = step->GetTrack() ;
  const double charge   = track->GetDefinition()->GetPDGCharge()  ;

  // Only for charged tracks
  if ( fabs( charge ) < 0.1 ) { return 1 ; }                 // RETURN

  // make an actual evaluation
  return birkCorrection
    ( track->GetDefinition         () ,
      track->GetKineticEnergy      () ,
      track->GetMaterialCutsCouple () ) ;
}
// ============================================================================




// ============================================================================
// The END
// ============================================================================
#endif  ///< GAUSSCALO_CaloSensDet_H
// ============================================================================
