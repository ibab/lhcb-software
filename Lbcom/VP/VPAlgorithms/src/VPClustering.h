#ifndef VPCLUSTERING_H
#define VPCLUSTERING_H 1

#include "GaudiAlg/GaudiAlgorithm.h"

// Event/DigiEvent
#include "Event/VPDigit.h"
#include "Event/VPCluster.h"
#include "Event/VPLiteCluster.h"

// Kernel/LHCbKernel
#include "Kernel/VPChannelID.h"
#include "Kernel/FastClusterContainer.h"

// Det/VPDet
#include "VPDet/DeVP.h"

/** @class VPClustering.h 
 *
 *  @author Daniel Hynds 
 *  @date   2013/09/05 
 */

class VPClustering : public GaudiAlgorithm {

public:
  /// Standard constructor
  VPClustering(const std::string& name,ISvcLocator* pSvcLocator);
  /// Destructor
  virtual ~VPClustering();

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute();       ///< Algorithm execution

private:

  DeVP* m_det;

};
#endif 
