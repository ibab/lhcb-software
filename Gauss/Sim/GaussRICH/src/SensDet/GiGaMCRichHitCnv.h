
/** @file GiGaMCRichHitCnv.h
 *
 *  Header file for GiGa converter : GiGaMCRichHitCnv
 *
 *  CVS History :
 *  $Id: GiGaMCRichHitCnv.h,v 1.4 2005-09-02 16:37:28 jonrob Exp $
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2004-03-29
 */

// ============================================================================
#ifndef GIGACNV_GiGaMCRichHitCnv_H
#define GIGACNV_GiGaMCRichHitCnv_H  1
// ============================================================================

// STL
#include <set>
// GaudiKernel
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
// LHCbEvent
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"
// GiGa
#include "GiGa/GiGaTrajectory.h"
// GiGaCnv
#include "GiGaCnv/GiGaCnvBase.h"
// G4
#include "G4ParticleDefinition.hh"

// forward declarations
template <class TYPE>
class CnvFactory;
class GiGaTrajectory;
class  RichG4HitCollName;

/** @class GiGaMCRichHitCnv GiGaMCRichHitCnv.h
 *
 *  GiGa converter for the RICH event model object MCRichHit
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2004-03-29
 */

class GiGaMCRichHitCnv : public GiGaCnvBase
{

  friend class CnvFactory<GiGaMCRichHitCnv>;

protected:

  /** Standard Constructor
   *  @param loc pointer to service locator
   */
  GiGaMCRichHitCnv( ISvcLocator* loc );

  /// virtual destructor
  virtual ~GiGaMCRichHitCnv();

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


  RichG4HitCollName* RichG4HitCollectionName()
  {
    return m_RichG4HitCollectionName ;
  }

private: // methods

  GiGaMCRichHitCnv () ; ///< Default constructor
  GiGaMCRichHitCnv( const GiGaMCRichHitCnv& ) ; ///< Copy constructor
  GiGaMCRichHitCnv& operator=( const GiGaMCRichHitCnv& ) ; ///< assignment operator

private: // data

  /// Data to convert
  RichG4HitCollName* m_RichG4HitCollectionName;

  /// Count number of events processed
  unsigned long int m_nEvts;

  /// overall count hits in each detector
  std::vector< unsigned long int > m_hitTally;

  /// overall hit count in each radiator
  std::vector< unsigned long int > m_radHits;

  /// overall number of invalid radiator flag hits per event
  std::vector< unsigned long int > m_invalidRadHits;

  /// number of hits with invalid RICH flag
  unsigned long int m_invalidRichHits;

  /// overall charged track hit count in each radiator
  std::vector< unsigned long int > m_ctkHits;

  /// overall scattered hit count in each radiator
  std::vector< unsigned long int > m_scatHits;
  
  /// overall background hit count in each radiator
  std::vector< unsigned long int > m_bkgHits;

  /// overall MCParticle-less hit count in each radiator
  std::vector< unsigned long int > m_nomcpHits;
  
};

// ============================================================================
#endif   // GIGACNV_GiGaMCRichHitCnv_H
// ============================================================================
