// $Id: GetMCCkvInfoBase.h,v 1.5 2009-07-17 13:46:12 jonrob Exp $
#ifndef SENSDET_GetMCCkvInfoBase_H
#define SENSDET_GetMCCkvInfoBase_H 1

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

// Relations
#include "Relations/Relation1D.h"
//#include "Relations/RelationWeighted1D.h"
// RichDet
#include "RichDet/DeRichSystem.h"


// local
#include "GaussRICH/RichG4HitCollName.h"
#include "GaussCherenkov/CkvG4Hit.h"

/** @class GetMCCkvInfoBase GetMCCkvInfoBase.h
 *
 *  Base class for RICH GiGa converter algorithms
 *  
 *  @author Sajan EASO
 *  @date   2011-03-06
 */

class GetMCCkvInfoBase : public Rich::AlgBase
{ 

public:

  /// Standard constructor
  GetMCCkvInfoBase( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~GetMCCkvInfoBase( ) {}   ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode finalize  ();    ///< Algorithm finalization
  virtual StatusCode sysExecute();    ///< Algorithm system execute

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
      if(m_SuperRichFlag) m_RichG4HitCollectionName->setCollConfigWithSuperRich();

    }
    return m_RichG4HitCollectionName;
  }

  /// data location in TES (const)
  const std::string & dataLocationInTES() const { return m_dataToFill; }

  /// Which RICH is activate
  inline bool richIsActive( const Rich::DetectorType rich ) const
  {
    // return m_RICHes[rich];
    return (SuperRichFlag() || ( m_RICHes[rich]) );
  }

  /// Location of CkvG4Hit to MCRichHit relations
  inline std::string g4HitToMCRichHitRelLoc() const
  {
    return "/Event/MC/Rich/RichG4HitToMCRichHitRelation";
  }

  inline bool SuperRichFlag() const
  {  return m_SuperRichFlag;}
  
private:

  /// Fill collection data
  void getRichG4CollectionRange();

protected:

  /// Relations between RichG4Hits and MCRichHits
  typedef LHCb::Relation1D<const CkvG4Hit*,LHCb::MCRichHit> G4HitTable;

  /// Get the MCRichHit associated to a given CkvG4Hits
  const LHCb::MCRichHit * getMCRichHit( const CkvG4Hit* g4hit );

private:

  std::string        m_gigaSvcName;                      ///< Name of GiGa Service
  std::string        m_kineSvcName;                      ///< Name of GiGaKine Service
  mutable IGiGaSvc*          m_gigaSvc;                  ///< Pointer to GiGa Service
  mutable IGiGaKineCnvSvc*   m_gigaKineCnvSvc;           ///< Pointer to GiGaKine Service
  mutable RichG4HitCollName* m_RichG4HitCollectionName;  ///< G4 hit collections for RICH
  std::vector<int> m_colRange;                           ///< Collection data
  std::vector<bool> m_RICHes;                            ///< The RICH detectors to create data objects for
  G4HitTable * m_relationTable;                          ///< G4 hit to MCRichHit relation table
  bool m_SuperRichFlag;                                 // true for SuperRich, false for two-RICH option
protected:

  std::string      m_dataToFill;                         ///< Data location in TES to fill

};

#endif // SENSDET_GetMCCkvInfoBase_H
