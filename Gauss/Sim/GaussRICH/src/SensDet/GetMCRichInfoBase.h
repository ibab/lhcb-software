// $Id: GetMCRichInfoBase.h,v 1.4 2008-06-02 13:00:18 jonrob Exp $
#ifndef SENSDET_GetMCRichInfoBase_H
#define SENSDET_GetMCRichInfoBase_H 1

// base class
#include "RichKernel/RichAlgBase.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// from GiGa
#include "GiGa/IGiGaSvc.h"
#include "GiGa/GiGaTrajectory.h"
#include "GiGa/GiGaUtil.h"
#include "GiGa/GiGaHitsByID.h"
#include "GiGa/GiGaHitsByName.h"

// from GiGaCnv
#include "GiGaCnv/IGiGaKineCnvSvc.h"
#include "GiGaCnv/IGiGaCnvSvcLocation.h"
#include "GiGaCnv/GiGaKineRefTable.h"

// from Geant4
#include "G4TrajectoryContainer.hh"
#include "G4ParticleDefinition.hh"
#include "G4VHitsCollection.hh"
#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"

// from LHCb
#include "Event/MCRichTrack.h"
#include "Event/MCParticle.h"

// RichKernel
#include "RichKernel/RichStatDivFunctor.h"
#include "RichKernel/RichPoissonEffFunctor.h"

// local
#include "RichG4HitCollName.h"
#include "RichG4Hit.h"

/** @class GetMCRichInfoBase GetMCRichInfoBase.h
 *
 *  Base class for RICH GiGa converter algorithms
 *  
 *  @author Sajan EASO
 *  @date   2005-12-06
 */

class GetMCRichInfoBase : public RichAlgBase
{

public:

  /// Standard constructor
  GetMCRichInfoBase( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~GetMCRichInfoBase( ) {}   ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  /** accessor to GiGa Service on demand
   *  @return pointer to GiGa Service
   */
  inline IGiGaSvc* gigaSvc() const 
  {
    if ( !m_gigaSvc ) 
    { 
      m_gigaSvc = svc<IGiGaSvc>( m_gigaSvcName );
    }
    return m_gigaSvc;
  }

  /** accessor to kinematics  conversion service
   *  @return pointer to kinematics conversion service
   */
  inline IGiGaKineCnvSvc* kineSvc() const 
  {
    if ( !m_gigaKineCnvSvc ) 
    { 
      m_gigaKineCnvSvc = svc<IGiGaKineCnvSvc>( m_kineSvcName );
    }
    return m_gigaKineCnvSvc;
  }

  /// Access collection data
  inline const std::vector<int> & colRange() const { return m_colRange; }

  /// Access collection name data on demand
  inline RichG4HitCollName * RichG4HitCollectionName() const
  {
    if ( !m_RichG4HitCollectionName )
    { 
      m_RichG4HitCollectionName = new RichG4HitCollName(); 
    }
    return m_RichG4HitCollectionName;
  }

  /// data location in TES (const)
  const std::string & dataLocationInTES() const { return m_dataToFill; }

private:

  /// Fill collection data
  void getRichG4CollectionRange();

private:

  std::string        m_gigaSvcName;                      ///< Name of GiGa Service
  std::string        m_kineSvcName;                      ///< Name of GiGaKine Service
  mutable IGiGaSvc*          m_gigaSvc;                  ///< Pointer to GiGa Service
  mutable IGiGaKineCnvSvc*   m_gigaKineCnvSvc;           ///< Pointer to GiGaKine Service
  mutable RichG4HitCollName* m_RichG4HitCollectionName;  ///< G4 hit collections for RICH
  std::vector<int> m_colRange;                           ///< Collection data

protected:

  std::string      m_dataToFill;                         ///< Data location in TES to fill

};

#endif // SENSDET_GetMCRichInfoBase_H
