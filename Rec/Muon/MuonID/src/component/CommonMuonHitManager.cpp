/** Implementation of CommonMuonHitManager.
 *
 * 2010-12-07: Roel Aaij
 */
#include "MuonID/CommonMuonHitManager.h"

#include <vector>
#include <algorithm>

#include "Event/MuonCoord.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/boost_allocator.h"
#include "MuonDet/DeMuonDetector.h"
#include "MuonDet/IMuonFastPosTool.h"

DECLARE_TOOL_FACTORY(CommonMuonHitManager)

CommonMuonHitManager::CommonMuonHitManager(const std::string& type,
                                           const std::string& name,
                                           const IInterface* parent)
    : GaudiTool(type, name, parent), m_muonDet{nullptr}, m_loaded{false} {
  declareInterface<CommonMuonHitManager>(this);
  declareProperty("MuonCoordLocation",
                  m_coordLocation = LHCb::MuonCoordLocation::MuonCoords);
  m_prepared.reset();
}

StatusCode CommonMuonHitManager::initialize() {
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;

  incSvc()->addListener(this, IncidentType::BeginEvent);

  m_muonDet =
      getDet<DeMuonDetector>("/dd/Structure/LHCb/DownstreamRegion/Muon");
  auto stations = m_muonDet->stations();
  m_stations.reserve(stations);
  assert(stations <= 5);

  // X region boundaries
  // Define the regions, the last ones are increased a little to make sure
  // everything is caught.
  const std::array<std::vector<double>, 5> x_regions{
      {{-3940, -1920, -960, -480, 480, 960, 1920, 3940},
       {-4900, -1200, -600, 600, 1200, 4900},
       {-5280, -1292, -644, 644, 1292, 5280},
       {-5656, -1392, -696, 696, 1392, 5656},
       {-6052, -1488, -744, 744, 1488, 6052}}};

  for (int i = 0; i != stations; ++i) {
    m_stations.emplace_back(m_muonDet, i, x_regions[i]);
  }
  return sc;
}

void CommonMuonHitManager::handle(const Incident& incident) {
  if (IncidentType::BeginEvent != incident.type()) return;

  m_prepared.reset();
  m_loaded = false;
  std::for_each(std::begin(m_coords), std::end(m_coords),
                [](std::vector<const LHCb::MuonCoord*>& v) { v.clear(); });
}

CommonMuonHitRange CommonMuonHitManager::hits(double xmin, unsigned int station,
                                              unsigned int region) {
  if (!m_prepared[station]) prepareHits(station);
  return m_stations[station].hits(xmin, region);
}

CommonMuonHitRange CommonMuonHitManager::hits(double xmin, double xmax,
                                              unsigned int station,
                                              unsigned int region) {
  if (!m_prepared[station]) prepareHits(station);
  return m_stations[station].hits(xmin, xmax, region);
}

unsigned int CommonMuonHitManager::nRegions(unsigned int station) const {
  return m_stations[station].nRegions();
}

const CommonMuonStation& CommonMuonHitManager::station(unsigned int id) const {
  return m_stations[id];
}

void CommonMuonHitManager::prepareHits(unsigned int station) {
  if (!m_loaded) loadCoords();

  CommonMuonHits hits;
  hits.reserve(m_coords[station].size());
  for (const auto& coord : m_coords[station]) {
    double x = 0., dx = 0., y = 0., dy = 0., z = 0., dz = 0.;
    StatusCode sc = m_muonDet->Tile2XYZ(coord->key(), x, dx, y, dy, z, dz);
    if (sc.isFailure()) {
      Warning("Impossible MuonTileID");
      continue;
    }
    hits.emplace_back(coord->key(), x, dx, y, dy, z, dz, coord->uncrossed());
  }
  // Put the hits in the station
  m_stations[station].setHits(std::move(hits));
  m_prepared.set(station, true);
}

void CommonMuonHitManager::loadCoords() {
  LHCb::MuonCoords* coords = getIfExists<LHCb::MuonCoords>(m_coordLocation);
  if (!coords) {
    Exception("Cannot retrieve MuonCoords ", StatusCode::FAILURE);
  }
  for (auto coord : *coords) {
    m_coords[coord->key().station()].emplace_back(coord);
  }
  m_loaded = true;
}
