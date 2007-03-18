// $Id: GaussSensPlaneDet.h,v 1.4 2007-03-18 21:33:19 gcorti Exp $ 
#ifndef       GAUSS_GaussSensPlaneDet_H
#define       GAUSS_GaussSensPlaneDet_H 1 

// GaudiKernel
// Ntuple Svc 
#include "GaudiKernel/INTuple.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"

// GiGa 
#include "GiGa/GiGaSensDetBase.h"

// local
#include "CaloHit.h"


/** @class GaussSensPlaneDet GaussSensPlaneDet.h GaussSensPlaneDet.h
 *
 *  @author  Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date    23/01/2001
 */

class GaussSensPlaneDet: public GiGaSensDetBase
{ 
  /// friend factory 
  //  friend class GiGaFactory<GaussSensPlaneDet>;
  
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
   *    - determine the CaloCellID of the active cell
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
  
  
  /** standard constructor 
   *  @see GiGaSensDetBase 
   *  @see GiGaBase 
   *  @see AlgTool 
   *  @param type type of the object (?)
   *  @param name name of the object
   *  @param parent  pointer to parent object
   */
  GaussSensPlaneDet
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  /// destructor (virtual and protected)
  virtual ~GaussSensPlaneDet() {};
  
protected:
  
  /// keep all original links to G4Tracks/MCParticles 
  inline bool   keepLinks       () const { return m_keepLinks       ; }  
  /// only one entry ? 
  inline bool   oneEntry        () const { return m_oneEntry        ; }
  
  /// cut for photons to create hit 
  inline double cutForPhoton    () const { return m_cutForPhoton    ; }
  
  /// cut for e-   to create hit 
  inline double cutForElectron  () const { return m_cutForElectron  ; }
  
  /// cut for e+   to create hit 
  inline double cutForPositron  () const { return m_cutForPositron  ; }
  
  /// cut for muon to create hit 
  inline double cutForMuon      () const { return m_cutForMuon      ; }
  
  /// cut for other charged particle to create hit 
  inline double cutForCharged   () const { return m_cutForCharged   ; }
  
  /// cut for othe rneutral particle to create hit 
  inline double cutForNeutral   () const { return m_cutForNeutral   ; }
  
private:
  
  // no default constructor 
  GaussSensPlaneDet() ; 
  // no copy constructor
  GaussSensPlaneDet           ( const GaussSensPlaneDet& ); 
  // no assignement  
  GaussSensPlaneDet& operator=( const GaussSensPlaneDet& ) ;
  
protected: 
  
  std::string                   m_collectionName ;
  GaussSensPlaneHitsCollection* m_collection     ;
  
  bool   m_keepLinks       ;
  bool   m_oneEntry        ;  
  
  double m_cutForPhoton    ;
  double m_cutForElectron  ;
  double m_cutForPositron  ;
  double m_cutForMuon      ;
  double m_cutForCharged   ;
  double m_cutForNeutral   ;
  
private:
  
  // final statistics 
  bool   m_stat    ;
  long   m_events  ;
  double m_hits    ;
  double m_hits2   ;
  double m_hitsMin ;
  double m_hitsMax ;
  
};
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
#endif  ///< GAUSSCALO_GaussSensPlaneDet_H
// ============================================================================
