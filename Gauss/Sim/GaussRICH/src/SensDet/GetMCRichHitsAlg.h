// $Id: GetMCRichHitsAlg.h,v 1.1 2005-12-13 17:31:17 seaso Exp $
#ifndef SENSDET_GETMCRICHHITSALG_H 
#define SENSDET_GETMCRICHHITSALG_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// Forward declarations
class IGiGaSvc;
class IGiGaKineCnvSvc;
class  RichG4HitCollName;
 

/** @class GetMCRichHitsAlg GetMCRichHitsAlg.h SensDet/GetMCRichHitsAlg.h
 *  
 *
 *  @author Sajan EASO
 *  @date   2005-12-06
 */
class GetMCRichHitsAlg : public GaudiAlgorithm {
public: 
  /// Standard constructor
  GetMCRichHitsAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~GetMCRichHitsAlg( ); ///< Destructor

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
  std::vector<int> getRichG4CollectionRange();
private:
  std::string        m_gigaSvcName;      ///< Name of GiGa Service
  std::string        m_kineSvcName;      ///< Name of GiGaCnv Service
  IGiGaSvc*          m_gigaSvc;          ///< Pointer to GiGa Service
  IGiGaKineCnvSvc*   m_gigaKineCnvSvc;   ///< Pointer to GiGaKine Service
 
  std::string        m_richHitsLocation;     ///< Name of TES path for MCRichHits

  RichG4HitCollName* m_RichG4HitCollectionName;  // G4 hit collections for RICH
  std::vector<int> m_colRange;
  
  // now the variables used for the local monitoring. This may eventually
  // go into GaussMonitor. SE Nov 2005.
  /// Count number of events processed
  unsigned long int m_nEvts;
  /// overall count hits in each RICH detector
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
#endif // SENSDET_GETMCRICHHITSALG_H
