#ifndef MAKEMUONMEASUREMENTS_H 
#define MAKEMUONMEASUREMENTS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "TrackInterfaces/IMeasurementProvider.h"
#include "MuonDet/DeMuonDetector.h"

/** @class MakeMuonMeasurements MakeMuonMeasurements.h
 *  
 *
 *  @author Jose Angel Hernando Morata
 *  @author Xabier Cid Vidal
 *  @date   2008-07-16
 */
class MakeMuonMeasurements : public GaudiAlgorithm {
public: 
  /// Standard constructor
  MakeMuonMeasurements( const std::string& name, ISvcLocator* pSvcLocator );

  ~MakeMuonMeasurements( ) override = default; ///< Destructor

  StatusCode initialize() override;    ///< Algorithm initialization
  StatusCode execute   () override;    ///< Algorithm execution

protected:

  IMeasurementProvider*  m_measProvider = nullptr;
  DeMuonDetector*  m_mudet = nullptr;
  bool m_use_uncrossed = true;

private:

};
#endif // MAKEMUONMEASUREMENTS_H
