
/** @file GiGaMCRichHitCnv.h
 *
 *  Header file for GiGa converter : GiGaMCRichHitCnv
 *
 *  CVS History :
 *  $Id: GiGaMCRichHitCnv.h,v 1.2 2004-07-30 13:42:13 jonrob Exp $
 *  $Log: not supported by cvs2svn $
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

  RichG4HitCollName* m_RichG4HitCollectionName;

};

// ============================================================================
#endif   // GIGACNV_GiGaMCRichHitCnv_H
// ============================================================================
