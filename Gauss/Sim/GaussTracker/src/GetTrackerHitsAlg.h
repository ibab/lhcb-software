// $Id: GetTrackerHitsAlg.h,v 1.3 2006-07-27 12:59:07 gcorti Exp $
#ifndef GETTRACKERHITSALG_H 
#define GETTRACKERHITSALG_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// Forward declarations
class IGiGaSvc;
class IGiGaKineCnvSvc;
class DetectorElement;
class TrackerHit;
namespace LHCb {
  class MCHit;
}


/** @class GetTrackerHitsAlg GetTrackerHitsAlg.h
 *  
 *
 *  @author Gloria CORTI
 *  @date   2005-10-02
 */
class GetTrackerHitsAlg : public GaudiAlgorithm {
public: 
  /// Standard constructor
  GetTrackerHitsAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~GetTrackerHitsAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

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

  void fillHit( TrackerHit* g4Hit, LHCb::MCHit* mcHit );
  

private:

  std::string        m_gigaSvcName;      ///< Name of GiGa Service
  std::string        m_kineSvcName;      ///< Name of GiGaCnv Service
  IGiGaSvc*          m_gigaSvc;          ///< Pointer to GiGa Service
  IGiGaKineCnvSvc*   m_gigaKineCnvSvc;   ///< Pointer to GiGaKine Service

  bool               m_extendedInfo;     ///< Flag to fill MCHit or MCExtendedHit

  std::string        m_hitsLocation;     ///< Name of TES path for MCHits
  std::string        m_colName;          ///< Name of G4 hits collection
  
  std::string        m_detName;          ///< Detector PATH
  const DetectorElement* m_detector;     ///< Pointer to DetectorElement
  
};
#endif // GETTRACKERHITSALG_H
