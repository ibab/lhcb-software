// $Id: GetExtendedMCRichInfoAlg.h,v 1.1 2005-12-13 17:31:17 seaso Exp $
#ifndef SENSDET_GETEXTENDEDMCRICHINFOALG_H 
#define SENSDET_GETEXTENDEDMCRICHINFOALG_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "Event/MCRichSegment.h"
#include "Event/MCRichTrack.h"
#include "Event/MCRichOpticalPhoton.h"
#include "GiGaCnv/GiGaKineRefTable.h"
#include "GiGa/GiGaTrajectory.h"
#include "RichG4Hit.h"
 
 

// Forward declarations
class IGiGaSvc;
class IGiGaKineCnvSvc;
class  RichG4HitCollName;


/** @class GetExtendedMCRichInfoAlg GetExtendedMCRichInfoAlg.h SensDet/GetExtendedMCRichInfoAlg.h
 *  
 *
 *  @author Sajan EASO
 *  @date   2005-12-12
 */
      typedef std::pair< const MCParticle*, Rich::RadiatorType> HitDataListKey;
      typedef std::pair< const RichG4Hit*, int > HitDataType;
      typedef std::vector< HitDataType > G4HitList;
      typedef std::map< HitDataListKey, G4HitList > HitDataList;

class GetExtendedMCRichInfoAlg : public GaudiAlgorithm {
public: 
  /// Standard constructor
  GetExtendedMCRichInfoAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~GetExtendedMCRichInfoAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
  RichG4HitCollName* RichG4HitCollectionName()
  {
    return m_RichG4HitCollectionName ;
  }
  bool convertMCRichOpticalPhotonInfo() {return  m_convertMCOpticalPhotonInfo;}
  bool convertMCRichTrackSegmentInfo() {return m_convertMCRichTrackSegmentInfo;}
  bool convertMCRichTrackInfo() {return m_convertMCRichTrackInfo;}
  bool convertMCRichExtendedInfo() {return m_convertMCRichExtendedInfo;}

protected:
  /** accessor to GiGa Service
   *  @return pointer to GiGa Service
   */
  IGiGaSvc* gigaSvc() const { 
    return m_gigaSvc; 
  }
  /** accessor to kinematics  conversion service 
   *  @return pointer to kinematics conversion service
   */
  IGiGaKineCnvSvc*      kineSvc   () const { 
    return m_gigaKineCnvSvc;
  }
  std::vector<int> getRichG4CollectionRange();
  StatusCode  StoreExtendedRichTrackInfo(G4TrajectoryContainer* trajectories ,MCRichSegments * segments,
                                         MCRichTracks * richTracks,GiGaKineRefTable& table );
  StatusCode StoreExtendedSegmentInfo(HitDataList sortedHits,MCRichSegments * segments,MCRichOpticalPhotons* photons);
  MCRichOpticalPhoton * StoreExtendedPhotonInfo( RichG4Hit * g4hit);
  
private:
  std::string        m_gigaSvcName;      ///< Name of GiGa Service
  std::string        m_kineSvcName;      ///< Name of GiGaCnv Service
  IGiGaSvc*          m_gigaSvc;          ///< Pointer to GiGa Service
  IGiGaKineCnvSvc*   m_gigaKineCnvSvc;   ///< Pointer to GiGaKine Service

  std::string        m_richOpticalPhotonsLocation; ///< Name of TES path for MCRichOpticalPhoton
  std::string        m_richSegmentsLocation;  /// < Name of TES path for MCRichSegments
  std::string        m_richTracksLocation;   /// < Name of TES path for MCRichTracks
  std::string        m_richHitsLocation;     ///< Name of TES path for MCRichHits
 

  RichG4HitCollName* m_RichG4HitCollectionName;  // G4 hit collections for RICH
  // now for the extended informations for special RICH Analysis
  bool m_convertMCOpticalPhotonInfo;
  bool m_convertMCRichTrackSegmentInfo;
  bool m_convertMCRichTrackInfo; 
  bool m_convertMCRichExtendedInfo;
  std::vector<int> m_colRange;
 /// Count number of events processed
  unsigned long int m_nEvts;
 
 /// Count hits in each detector
 std::vector< unsigned long int > m_hitTallyPhot;
 std::vector< unsigned long int > m_hitTallySeg;
 unsigned long int m_hitTallyTrk;
 

};
#endif // SENSDET_GETEXTENDEDMCRICHINFOALG_H
