// ============================================================================
#ifndef GIGACNV_GiGaRichOpticalPhotonCnv_H
#define GIGACNV_GiGaRichOpticalPhotonCnv_H  1
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
class GiGaTrajectory;
class RichG4HitCollName;

class GiGaRichOpticalPhotonCnv: public GiGaCnvBase {

  friend class CnvFactory<GiGaRichOpticalPhotonCnv>;

protected:

  /** Standard Constructor
   *  @param loc pointer to service locator
   */
  GiGaRichOpticalPhotonCnv( ISvcLocator* loc );

  /// virtual destructor
  virtual ~GiGaRichOpticalPhotonCnv();

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

  /** retrieve the class identifier  for created object
   *  @return class idenfifier for created object
   */
  static const CLID& classID();

  /** retrieve the storage type for created object
   *  @return storage type  for created object
   */
  static const unsigned char storageType() ;

  RichG4HitCollName* RichG4HitCollectionName()
  {
    return m_RichG4HitCollectionName;
  }

private: // methods

  GiGaRichOpticalPhotonCnv () ; /// no default constructor
  GiGaRichOpticalPhotonCnv( const GiGaRichOpticalPhotonCnv& ) ; /// no copy
  GiGaRichOpticalPhotonCnv& operator=( const GiGaRichOpticalPhotonCnv& ) ; /// no assignment

private: // data

  /// Pointer to RichG4Hits
  RichG4HitCollName* m_RichG4HitCollectionName;

};

// ============================================================================
#endif   ///< GIGACNV_GiGaRichOpticalPhotonCnv_H
// ============================================================================














