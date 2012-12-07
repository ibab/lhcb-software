// Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IIncidentListener.h"
// Det/VLDet
#include "VLDet/DeVL.h"
// Event/TrackEvent
#include "Event/StateVector.h"
#include "Event/TrackParameters.h"
// Tr/TrackInterfaces
#include "TrackInterfaces/IMeasurementProvider.h"
#include "TrackInterfaces/ITrackExtrapolator.h"
// Tr/TrackFitEvent
#include "Event/VLMeasurement.h"
// Tr/TrackKernel
#include "TrackKernel/ZTrajectory.h"

/** @class VLMeasurementProvider VLMeasurementProvider.cpp
 *
 */

class VLMeasurementProvider : public GaudiTool, 
                              virtual public IMeasurementProvider,
                              virtual public IIncidentListener {
public:
  
  /// Constructor
  VLMeasurementProvider(const std::string& type, const std::string& name,
                        const IInterface* parent);
  /// Destructor
  ~VLMeasurementProvider() {}
  
  StatusCode initialize();

  void handle(const Incident& incident) {
    if (IncidentType::BeginEvent == incident.type()) {
      m_clusters = 0;
      m_liteClusters = 0;
    }
  }
  LHCb::Measurement* measurement(const LHCb::LHCbID& id, 
                                 bool localY = false) const; 
  LHCb::Measurement* measurement(const LHCb::LHCbID& id, 
                                 const LHCb::ZTrajectory& reftraj, 
                                 bool localY = false) const;
  double nominalZ(const LHCb::LHCbID& id) const {
    const DeVLSensor* sensor = m_det->sensor(id.vlID().sensor());
    return sensor->z();
  }
     
  void addToMeasurements(const std::vector<LHCb::LHCbID>& lhcbids,
                         std::vector<LHCb::Measurement*>& measurements,
                         const LHCb::ZTrajectory& reftraj) const;
  
  StatusCode load(LHCb::Track&) const {
    warning() << "MeasurementProviderBase::load not implemented" << endmsg;
    return StatusCode::FAILURE;
  }

private:

  DeVL* m_det;
  ToolHandle<IVLClusterPosition> m_positiontool;
  LHCb::VLClusters* m_clusters;
  mutable LHCb::VLLiteCluster::VLLiteClusters* m_liteClusters;

};

DECLARE_TOOL_FACTORY(VLMeasurementProvider)

//=============================================================================
/// Constructor
//=============================================================================
VLMeasurementProvider::VLMeasurementProvider(const std::string& type,
                                             const std::string& name,
                                             const IInterface* parent)
  :  GaudiTool(type, name, parent), 
     m_det(0), m_positiontool("VLClusterPosition") {

  declareInterface<IMeasurementProvider>(this);

}

//=============================================================================
/// Initialization 
//=============================================================================
StatusCode VLMeasurementProvider::initialize() {

  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;
  // Retrieve the position tool.
  sc = m_positiontool.retrieve();
  if (sc.isFailure()) {
    error() << "Failed to retrieve position tool" << endmsg;
    return sc;
  }
  // Retrieve the detector element.
  m_det = getDet<DeVL>(DeVLLocation::Default);
  incSvc()->addListener(this, IncidentType::BeginEvent);
  return StatusCode::SUCCESS;

}

//=============================================================================
/// Create a measurement
//=============================================================================
LHCb::Measurement* VLMeasurementProvider::measurement(const LHCb::LHCbID& id,
                                                      bool localY) const {

  if (!id.isVL()) {
    error() << "LHCbID " << id << " is not a VL ID" << endmsg;
    return 0;
  }
  if (!m_liteClusters) {
    m_liteClusters = get<LHCb::VLLiteCluster::VLLiteClusters>(LHCb::VLLiteClusterLocation::Default);
  }
  const LHCb::VLLiteCluster* cluster = m_liteClusters->object(id.vlID());
  if (!cluster) {
    error() << "Cannot find cluster for id " << id << endmsg;
    return 0;
  }
  LHCb::Measurement* m = 0;
  m = new LHCb::VLMeasurement(cluster, m_det, *m_positiontool);
  return m;

}

//=============================================================================
/// Create a measurement with ref vector
//=============================================================================
LHCb::Measurement* VLMeasurementProvider::measurement(const LHCb::LHCbID& id,  
                                                      const LHCb::ZTrajectory& reftraj,
                                                      bool localY) const {
  
  if (!id.isVL()) {
    error() << "Not a VL measurement" << endmsg;
    return 0;
  }
  if (!m_liteClusters) {
    m_liteClusters = get<LHCb::VLLiteCluster::VLLiteClusters>(LHCb::VLLiteClusterLocation::Default);
  }
  const LHCb::VLLiteCluster* cluster = m_liteClusters->object(id.vlID());
  if (!cluster) {
    error() << "Cannot find cluster for id " << id << endmsg;
    return 0;
  }
  const double z = nominalZ(id);
  LHCb::StateVector ref = reftraj.stateVector(z);
  LHCb::Measurement* m = 0;
  m = new LHCb::VLMeasurement(cluster, m_det, *m_positiontool, ref);
  return m;

}

//=============================================================================
/// Create measurements for list of LHCbIDs
//=============================================================================
void VLMeasurementProvider::addToMeasurements(const std::vector<LHCb::LHCbID>& lhcbids,
                                              std::vector<LHCb::Measurement*>& measurements,
                                              const LHCb::ZTrajectory& reftraj) const {
    
  std::vector<LHCb::LHCbID>::const_iterator it;
  for (it = lhcbids.begin(); it != lhcbids.end(); ++it) {
    measurements.push_back(measurement(*it, reftraj));
  }

}
