// Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"

#include "TrackInterfaces/IMeasurementProvider.h"

// LHCb
// Det/VPDet
#include "VPDet/DeVP.h"
// Event/DigiEvent
#include "Event/VPCluster.h"
// Event/TrackEvent
#include "Event/Measurement.h"
#include "Event/StateVector.h"
// Tr/LHCbTrackInterfaces
#include "TrackInterfaces/IVPClusterPosition.h"

// Rec
// Tr/TrackFitEvent
#include "Event/VPMeasurement.h"
// Tr/TrackKernel
#include "TrackKernel/ZTrajectory.h"

/** @class VPMeasurementProvider VPMeasurementProvider.cpp
 *
 * Implementation of VPMeasurementProvider tool
 * see interface header for description
 *
 *  @author Victor Coco 
 *          based on Wouter Hulsbergen, Stefania Vecchi MuonMeasurementProvider
 *  @date   12/02/2010
 */

class VPMeasurementProvider : public GaudiTool, virtual public IMeasurementProvider,
                              virtual public IIncidentListener {

public:
  /// Constructor
  VPMeasurementProvider(const std::string& type,
                        const std::string& name,
                        const IInterface* parent);
  /// Destructor
  ~VPMeasurementProvider() {}
  
  StatusCode initialize() ;

  void handle(const Incident& incident);

  /// Measurement for single hits
  LHCb::Measurement* measurement(const LHCb::LHCbID& id, bool localY) const; 
  /// Measurement for single hits with known trajectory
  LHCb::Measurement* measurement(const LHCb::LHCbID& id, 
                                 const LHCb::ZTrajectory& ref, bool localY) const;
  double nominalZ(const LHCb::LHCbID& id) const;
  
  void addToMeasurements(const std::vector<LHCb::LHCbID>& lhcbids,
                         std::vector<LHCb::Measurement*>& measurements,
                         const LHCb::ZTrajectory& ref) const;

  StatusCode load(LHCb::Track&) const {
    return Error("MeasurementProviderBase::load not implemented");
  }

private:

  /// On-demand access to clusters.
  const LHCb::VPClusters* clusters() const;

  /// Pointer to detector element
  mutable DeVP* m_det;
  ToolHandle<IVPClusterPosition> m_positionTool;
  std::string m_clusterLocation;
  mutable LHCb::VPClusters* m_clusters;

};

DECLARE_TOOL_FACTORY(VPMeasurementProvider)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VPMeasurementProvider::VPMeasurementProvider(const std::string& type,
                                             const std::string& name,
                                             const IInterface* parent) :
    GaudiTool(type, name, parent),
    m_det(nullptr),
    m_positionTool("VPClusterPosition"),
    m_clusters(nullptr) {

  declareInterface<IMeasurementProvider>(this);
  declareProperty("ClusterLocation", 
                  m_clusterLocation = LHCb::VPClusterLocation::Default);

}

//=============================================================================
// Initialize
//=============================================================================
StatusCode VPMeasurementProvider::initialize() {
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) {
    return Error("Failed to initialize!", sc);
  }
  // Retrieve the position tool
  sc = m_positionTool.retrieve();
  if (sc.isFailure()) { 
    return Error("Failed to initialize position tool!", sc); 
  }
  // Retrieve the detector element
  m_det = getDet<DeVP>(DeVPLocation::Default);
  // Reset pointer to list of clusters at begin-event
  incSvc()->addListener(this, IncidentType::BeginEvent);
  return sc;

}

//=============================================================================
// Handle a begin-event incidence: Make sure clusters are reloaded.
//=============================================================================
void VPMeasurementProvider::handle(const Incident& incident) {

  if (IncidentType::BeginEvent == incident.type()) m_clusters = nullptr;

}

//=============================================================================
// Load clusters from the TES
//=============================================================================
const LHCb::VPClusters* VPMeasurementProvider::clusters() const {
  if (!m_clusters) {
    m_clusters = get<LHCb::VPClusters>(m_clusterLocation);
  }
  return m_clusters ;
}

//=============================================================================
// Create a measurement
//=============================================================================
LHCb::Measurement* VPMeasurementProvider::measurement(const LHCb::LHCbID& id, bool localY) const {

  if (!id.isVP()) {
    error() << "Not a VP measurement" << endmsg;
    return nullptr;
  } 
  const LHCb::VPCluster* cluster = clusters()->object(id.vpID());
  if (!cluster) {
    error() << "Cannot find cluster for id " << id << endmsg;
    return nullptr;
  }
  LHCb::VPMeasurement::VPMeasurementType dir = localY ? 
    LHCb::VPMeasurement::Y : LHCb::VPMeasurement::X;
  LHCb::VPPositionInfo info = m_positionTool->position(cluster);
  LHCb::Measurement* meas = new LHCb::VPMeasurement(*cluster, info, dir);
  return meas;

}

//=============================================================================
// Create a measurement with ref vector
//=============================================================================
LHCb::Measurement* VPMeasurementProvider::measurement(const LHCb::LHCbID& id, 
                                                      const LHCb::ZTrajectory& ref,
                                                      bool localY) const {

  if (!id.isVP()) {
    error() << "Not a VP measurement" << endmsg;
    return nullptr;
  } 
  const LHCb::VPCluster* cluster = clusters()->object(id.vpID());
  if (!cluster) {
    error() << "Cannot find cluster for id " << id << endmsg;
    return nullptr;
  }
  LHCb::VPMeasurement::VPMeasurementType dir = localY ? 
      LHCb::VPMeasurement::Y : LHCb::VPMeasurement::X;
  double z = nominalZ(id);
  LHCb::StateVector sv = ref.stateVector(z);
  LHCb::VPPositionInfo info = m_positionTool->position(cluster, sv.position(), sv.tx(), sv.ty());
  LHCb::Measurement* meas = new LHCb::VPMeasurement(*cluster, info, dir);
  return meas;

}

//=============================================================================
// Return the z-coordinate of this lhcb-id (w/o creating the hit)
//=============================================================================
double VPMeasurementProvider::nominalZ(const LHCb::LHCbID& id) const {
  return m_det->sensorOfChannel(id.vpID())->z();
}

//=============================================================================
// Create measurements for list of LHCbIDs
//=============================================================================
void VPMeasurementProvider::addToMeasurements(const std::vector<LHCb::LHCbID>& lhcbids,
                                              std::vector<LHCb::Measurement*>& measurements,
                                              const LHCb::ZTrajectory& ref) const {
  
  for (auto it = lhcbids.cbegin(), end = lhcbids.cend(); it != end; ++it) {
    measurements.push_back(measurement(*it, ref, false));
    measurements.push_back(measurement(*it, ref, true));
  }
}
