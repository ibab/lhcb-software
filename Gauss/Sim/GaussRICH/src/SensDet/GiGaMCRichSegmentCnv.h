
/** @file GiGaRichSegmentCnv.h
 *
 *  Header file for GiGa converter : GiGaRichSegmentCnv
 *
 *  CVS History :
 *  $Id: GiGaMCRichSegmentCnv.h,v 1.3 2004-07-30 14:18:23 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *  Revision 1.2  2004/07/30 13:42:14  jonrob
 *  Add doxygen file documentation and CVS information
 *
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2004-03-29
 */

// ============================================================================
#ifndef GIGACNV_GiGaRichSegmentCnv_H
#define GIGACNV_GiGaRichSegmentCnv_H  1
// ============================================================================

// STL
#include <set>
#include <string>
#include <vector>

// GaudiKernel
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/IAddressCreator.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/LinkManager.h"
#include "GaudiKernel/GaudiException.h"

// LHCbEvent
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"
#include "Event/MCRichSegment.h"

// GiGa
#include "GiGa/IGiGaSvc.h"
#include "GiGaCnv/IGiGaHitsCnvSvc.h"
#include "GiGa/GiGaTrajectory.h"
#include "GiGa/GiGaUtil.h"
#include "GiGa/GiGaHitsByID.h"
#include "GiGa/GiGaHitsByName.h"

// GiGaCnv
#include "GiGaCnv/GiGaCnvBase.h"
#include "GiGaCnv/GiGaKineRefTable.h"
#include "GiGaCnv/GiGaCnvUtils.h"
#include "GiGaCnv/IGiGaHitsCnvSvc.h"
#include "GiGaCnv/IGiGaKineCnvSvc.h"
#include "GiGaCnv/GiGaKineRefTable.h"

// G4
#include "G4ParticleDefinition.hh"
#include "G4VHitsCollection.hh"
#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"

// CLHEP
#include "CLHEP/Geometry/Point3D.h"

// local
#include "RichG4HitCollName.h"
#include "RichG4Hit.h"

// forward declarations
template <class TYPE>
class CnvFactory;
class GiGaTrajectory;
class RichG4HitCollName;

/** @class GiGaMCRichSegmentCnv GiGaMCRichSegmentCnv.h
 *
 *  GiGa converter for the RICH event model object MCRichSegment
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2004-03-29
 */

class GiGaRichSegmentCnv : public GiGaCnvBase {

  friend class CnvFactory<GiGaRichSegmentCnv>;

protected:

  /** Standard Constructor
   *  @param loc pointer to service locator
   */
  GiGaRichSegmentCnv( ISvcLocator* loc );

  /// virtual destructor
  virtual ~GiGaRichSegmentCnv();

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

  GiGaRichSegmentCnv () ; ///< Default constructor
  GiGaRichSegmentCnv( const GiGaRichSegmentCnv& ) ; ///< Copy constructor
  GiGaRichSegmentCnv& operator=( const GiGaRichSegmentCnv& ) ; ///< assignment operator

private: // data

  /// Pointer to RichG4Hits
  RichG4HitCollName* m_RichG4HitCollectionName;

};

// ============================================================================
#endif   // GIGACNV_GiGaRichSegmentCnv_H
// ============================================================================














