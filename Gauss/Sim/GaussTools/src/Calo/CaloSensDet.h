// $Id: CaloSensDet.h,v 1.1 2002-12-07 14:41:44 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
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
   *  @return statsu code 
   */
  virtual StatusCode initialize   ();
  
  /** standard finalization (Gaudi) 
   *  @see GiGaSensDetBase
   *  @see GiGaBase 
   *  @see   AlgTool 
   *  @see  IAlgTool 
   *  @return statsu code 
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
  
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif  ///< GIGA_CaloSensDet_H
// ============================================================================
