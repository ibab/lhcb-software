// $Id: CaloProtoElectronMonitor.h,v 1.1 2009-12-11 17:07:40 odescham Exp $
#ifndef CALOPROTOELECTRONMONITOR_H 
#define CALOPROTOELECTRONMONITOR_H 1

// Include files
// from Gaudi
#include "CaloMoniAlg.h"
#include "Event/ProtoParticle.h"
#include "CaloUtils/ICaloElectron.h"
#include "CaloInterfaces/ICaloHypo2Calo.h"

/** @class CaloProtoElectronMonitor CaloProtoElectronMonitor.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2009-12-11
 */
class CaloProtoElectronMonitor : public CaloMoniAlg {
public: 
  /// Standard constructor
  CaloProtoElectronMonitor( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CaloProtoElectronMonitor( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

  ICaloElectron* m_caloElectron;
  ICaloHypo2Calo* m_toPrs ;   
  double m_eOpMin;
  double m_eOpMax;
  int m_eOpBin;
  double m_prsCut;
};
#endif // CALOPROTOELECTRONMONITOR_H
