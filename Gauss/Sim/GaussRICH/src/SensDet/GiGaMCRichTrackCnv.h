// ============================================================================
#ifndef GIGACNV_GiGaRichTrackCnv_H
#define GIGACNV_GiGaRichTrackCnv_H  1
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
#include "G4TrajectoryContainer.hh"
#include "G4ParticleDefinition.hh"

/// forward declarations
template <class TYPE>
class CnvFactory;
class GiGaTrajectory;
class RichG4HitCollName;

class GiGaRichTrackCnv : public GiGaCnvBase {

  friend class CnvFactory<GiGaRichTrackCnv>;

protected:

  /** Standard Constructor
   *  @param loc pointer to service locator
   */
  GiGaRichTrackCnv( ISvcLocator* loc );

  /// virtual destructor
  virtual ~GiGaRichTrackCnv();

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

private: // methods

  GiGaRichTrackCnv () ; /// no default constructor
  GiGaRichTrackCnv( const GiGaRichTrackCnv& ) ; /// no copy
  GiGaRichTrackCnv& operator=( const GiGaRichTrackCnv& ) ; /// no assignment

private: // data

};

// ============================================================================
#endif   ///< GIGACNV_GiGaRichTrackCnv_H
// ============================================================================














