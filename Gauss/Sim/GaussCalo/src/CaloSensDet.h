// $Id: CaloSensDet.h,v 1.3 2003-05-14 08:43:42 robbep Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2002/12/13 16:52:57  ibelyaev
//  put updated versions of the packages
//
// Revision 1.1  2002/12/07 14:41:44  ibelyaev
//  add new Calo stuff
//
// ============================================================================
#ifndef       GIGA_CaloSensDet_H
#define       GIGA_CaloSensDet_H 1 
// ============================================================================
// GaudiKernel
/// Ntupel Svc 
#include "GaudiKernel/INTuple.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"
/// GiGa 
#include "GiGa/GiGaSensDetBase.h"
#include "GiGa/GiGaHashMap.h"
/// local
#include "CaloSim.h"
#include "CaloHit.h"
/// forward declarations 
template <class SD> class GiGaSensDetFactory;

class DeCalorimeter ;

/** @class CaloSensDet CaloSensDet.h CaloSensDet.h
 *
 *
 *  @author  Vanya Belyaev
 *  @date    23/01/2001
 */

class CaloSensDet: virtual public GiGaSensDetBase
{
  /// friend factory 
  friend class GiGaFactory<CaloSensDet>;
public:
  
  /// useful type for list of names 
  typedef std::vector<std::string>              Names   ;
  /// useful type for list of logical volumes 
  typedef std::vector<const G4LogicalVolume*>   Volumes ;
  // translator from Path to CellID 
  typedef GiGaHashMap<CaloSim::Path,CaloCellID> Table   ;
  typedef GiGaHashMap<CaloCellID,CaloHit*>      HitMap  ;
  
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
  
  /** process the hit
   *  @param step     pointer to current Geant4 step 
   *  @param history  pointert to touchable history 
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
  
protected:
  
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
  virtual ~CaloSensDet();
  
private:
  
  // no default constructor 
  CaloSensDet() ; 
  // no copy constructor
  CaloSensDet           ( const CaloSensDet& ); 
  // no assignement  
  CaloSensDet& operator=( const CaloSensDet& ) ;
  
protected: 
  
  // acess to detector element 
  const DeCalorimeter* calo() const  { return m_calo ; }
  
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
  Table                                       m_table               ;
  HitMap                                      m_hitmap              ;
  
  std::string                                 m_caloName            ;
  const DeCalorimeter*                        m_calo                ;

  double                                      m_zmin                ;
  double                                      m_zmax                ;
  
  CaloHitsCollection*                         m_collection          ;

  double                                      m_c1                  ;
  double                                      m_c2                  ;
  double                                      m_c1correction        ;

  double                                      m_a_local_inner_ecal  ;
  double                                      m_a_local_middle_ecal ;
  double                                      m_a_local_outer_ecal  ;

  double                                      m_ecalTimeHisto[2][25] ;

public:

  /** Correction factor from Birk's Law
   *  Factor = 1/(1+C1*dEdx/rho+C2*(dEdx/rho)^2)
   *  Where :
   *      - C1 = 0.013 g/MeV/cm^2 [Ref NIM 80 (1970) 239]
   *      - C2 = 9.6.10^-6 g^2/MeV^2/cm^4
   *      - dEdx in MeV/cm
   *      - rho = density in g/cm^3
   */
  double birkCorrection( G4Step* step )                             ;

  /** The energy deposited in the cell by the step contributes 
   *  - frac1*energy in the time bin timeBin (25 ns bins)
   *  - frac2*energy in the time bin timeBin+1
   *  the origin is the time of collision
   */
  void timeFraction( G4Step* step , const CaloCellID& cell ,
                     double& frac1 , double& frac2 , int& timeBin ) ;

  /** Correction due to the local non uniformity due to the light 
   *  collection efficiency in cell cell
   */
  double localNonUniformity( G4Step* step , const CaloCellID& cell ) ;  
  
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif  ///< GIGA_CaloSensDet_H
// ============================================================================
