// ============================================================================
#ifndef GIGACNV_GiGaMuonHitCnv_H
#define GIGACNV_GiGaMuonHitCnv_H  1 
// ============================================================================
/// STL 
#include <set>
/// GaudiKernel
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
/// LHCbEvent 
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"
/// GiGa
#include "GiGa/GiGaTrajectory.h" 
/// GiGaCnv
#include "GiGaCnv/GiGaCnvBase.h" 
/// G4 
#include "G4ParticleDefinition.hh"

/// forward declarations 
template <class TYPE> 
class CnvFactory;
///
class GiGaTrajectory;
///


class GiGaMuonHitCnv: public GiGaCnvBase
{
  ///
  friend class CnvFactory<GiGaMuonHitCnv>; 
  /// 
protected:

  /** Standard Constructor
   *  @param loc pointer to service locator 
   */
  GiGaMuonHitCnv( ISvcLocator* loc );
  /// virtual destructor 
  virtual ~GiGaMuonHitCnv();
  ///
  
public:
  
  /** initialize the converter 
   *  @return status code
   */
  virtual StatusCode initialize() ;
  
  /** finalize  the converter 
   *  @return status code
   */
  virtual StatusCode finalize  () ;
 
  /** create the Object 
   *  @param Address address 
   *  @param Object object itself 
   *  @return status code 
   */
  virtual StatusCode createObj     ( IOpaqueAddress*     Address   , 
                                     DataObject*&        Object    ) ;
  
  /** update the Object 
   *  @param Address address 
   *  @param Object object itself 
   *  @return status code 
   */
  virtual StatusCode updateObj     ( IOpaqueAddress*     Address   , 
                                     DataObject*         Object    ) ; 

  /** fill the object references 
   *  @param Address address 
   *  @param Object object itself 
   *  @return status code 
   */
  virtual StatusCode fillObjRefs   ( IOpaqueAddress*     Address   , 
                                     DataObject*         Object    ) ;

  /** update the object references 
   *  @param Address address 
   *  @param Object object itself 
   *  @return status code 
   */
  virtual StatusCode updateObjRefs ( IOpaqueAddress*     Address   , 
                                     DataObject*         Object    ) ;
  
  /** retrieve the class identifier  for created object 
   *  @return class idenfifier for created object 
   */
  static const CLID&          classID();
  
  /** retrieve the storage type for created object 
   *  @return storage type  for created object 
   */
  static const unsigned char storageType() ; 
  ///
protected: 
  
private:
  ///
  GiGaMuonHitCnv () ; /// no default constructor 
  GiGaMuonHitCnv( const GiGaMuonHitCnv& ) ; /// no copy 
  GiGaMuonHitCnv& operator=( const GiGaMuonHitCnv& ) ; /// no assignement
  ///
private: 
  
  ///
};

// ============================================================================
#endif   ///< GIGACNV_GiGaMuonHitCnv_H
// ============================================================================














