
/** @file GiGaRichTrackCnv.h
 *
 *  Header file for GiGa converter : GiGaRichTrackCnv
 *
 *  CVS History :
 *  $Id: GiGaMCRichTrackCnv.h,v 1.4 2005-02-17 13:32:16 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *  Revision 1.3  2005/01/19 10:38:52  jonrob
 *  add simple printout to GiGa converters
 *
 *  Revision 1.2  2004/07/30 13:42:14  jonrob
 *  Add doxygen file documentation and CVS information
 *
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2004-03-29
 */

// ============================================================================
#ifndef GIGACNV_GiGaRichTrackCnv_H
#define GIGACNV_GiGaRichTrackCnv_H  1
// ============================================================================

// STL
#include <set>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>

// CLHEP
#include "CLHEP/Geometry/Point3D.h"

// GaudiKernel
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/IAddressCreator.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/LinkManager.h"
#include "GaudiKernel/GaudiException.h"

// LHCbEvent
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"

// GiGa
#include "GiGa/IGiGaSvc.h"
#include "GiGa/GiGaTrajectory.h"
#include "GiGa/GiGaUtil.h"
#include "GiGa/GiGaHitsByID.h"
#include "GiGa/GiGaHitsByName.h"

// GiGaCnv
#include "GiGaCnv/GiGaCnvBase.h"
#include "GiGaCnv/IGiGaHitsCnvSvc.h"
#include "GiGaCnv/GiGaKineRefTable.h"
#include "GiGaCnv/GiGaCnvUtils.h"
#include "GiGaCnv/IGiGaHitsCnvSvc.h"
#include "GiGaCnv/IGiGaKineCnvSvc.h"
#include "GiGaCnv/GiGaKineRefTable.h"

// G4
#include "G4TrajectoryContainer.hh"
#include "G4ParticleDefinition.hh"
#include "G4VHitsCollection.hh"
#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"

// RichKernel
#include "RichKernel/RichStatDivFunctor.h"

// Rich Event
#include "Event/MCRichTrack.h"

// local
#include "GiGaMCRichTrackCnv.h"
#include "RichG4HitCollName.h"
#include "RichG4Hit.h"

// forward declarations
template <class TYPE>
class CnvFactory;
class GiGaTrajectory;
class RichG4HitCollName;

/** @class GiGaMCRichTrackCnv GiGaMCRichTrackCnv.h
 *
 *  GiGa converter for the RICH event model object MCRichTrack
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2004-03-29
 */

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

  GiGaRichTrackCnv () ; ///< Default constructor
  GiGaRichTrackCnv( const GiGaRichTrackCnv& ) ; ///< Copy constructor
  GiGaRichTrackCnv& operator=( const GiGaRichTrackCnv& ) ; ///< assignment operator

private: // data

  /// Count number of events processed
  unsigned long int m_nEvts;
  
  /// Count tracks
  unsigned long int m_hitTally;

};

// ============================================================================
#endif   // GIGACNV_GiGaRichTrackCnv_H
// ============================================================================














