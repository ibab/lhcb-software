#ifndef CALOPROTOELECTRONMONITOR_H 
#define CALOPROTOELECTRONMONITOR_H 1

// Include files
// from Gaudi
#include "CaloMoniAlg.h"
#include "Event/ProtoParticle.h"
#include "CaloUtils/ICaloElectron.h"
#include "TrackInterfaces/ITrackExtrapolator.h"

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


  ITrackExtrapolator* extrapolator(){return m_extrapolator;};

protected:

private:

  ITrackExtrapolator*  m_extrapolator;
  ICaloElectron* m_caloElectron;
  double m_eOpMin;
  double m_eOpMax;
  int m_eOpBin;
  double m_prsCut;
  bool m_pairing;
  std::string m_extrapolatorType;
  std::vector<int> m_tracks;
};
#endif // CALOPROTOELECTRONMONITOR_H
