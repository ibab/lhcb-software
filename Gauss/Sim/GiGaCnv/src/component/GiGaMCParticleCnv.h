// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.3  2001/07/24 11:13:56  ibelyaev
/// package restructurization(III) and update for newer GiGa
///
/// Revision 1.2  2001/07/15 20:45:11  ibelyaev
/// the package restructurisation
///
// ============================================================================
#ifndef GIGACNV_GIGAMCPARTICLECNV_H
#define GIGACNV_GIGAMCPARTICLECNV_H  1 
// ============================================================================
/// STL 
#include <set>
/// GaudiKernel
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
/// LHCbEvent 
#include "LHCbEvent/MCParticle.h"
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
class MCParticle;
class GiGaTrajectory;
///

/** @class GiGaMCParticleCnv GiGaMCParticleCnv.h
 *
 *  Converter of Geant4 trajectories into Gaudi MCParticles
 *
 *  @author  Vanya Belyaev
 *  @date    22/02/2001
 */

class GiGaMCParticleCnv: public GiGaCnvBase
{
  ///
  friend class CnvFactory<GiGaMCParticleCnv>; 
  /// 
 protected: 

  /** Standard Constructor
   *  @param loc pointer to service locator 
   */
  GiGaMCParticleCnv( ISvcLocator* loc );
  /// virtual destructor 
  virtual ~GiGaMCParticleCnv();
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
  GiGaMCParticleCnv () ; /// no default constructor 
  GiGaMCParticleCnv( const GiGaMCParticleCnv& ) ; /// no copy 
  GiGaMCParticleCnv& operator=( const GiGaMCParticleCnv& ) ; /// no assignement
  ///
private: 
  
  ///
};

// ============================================================================
#endif   ///< GIGACNV_GIGAMCPARTICLECNV_H
// ============================================================================














