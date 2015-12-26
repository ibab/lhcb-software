/** Implementation of CommonMuonStation
 *
 * 2010-12-07: Roel Aaij
 */
#include "MuonID/CommonMuonStation.h"

#include <exception>
#include <vector>

#include <GaudiKernel/Range.h>
#include <MuonDet/DeMuonDetector.h>
#include <Kernel/MuonTileID.h>
#include <Event/MuonCoord.h>

#include "MuonID/CommonMuonHit.h"

CommonMuonStation::CommonMuonStation(DeMuonDetector* det, int station,
                                     std::vector<double> regions)
    : m_xboundaries{std::move(regions)},
      m_hits{},
      m_index{nRegionsY * (m_xboundaries.size() - 1) + 1},
      m_z{det->getStationZ(station)},
      m_ymax{det->getOuterY(station)},
      m_station{station} {}

CommonMuonHitRange CommonMuonStation::hits(double xmin,
                                           unsigned int region) const {
  auto first = std::find_if(m_index[region], m_index[region + 1],
                            [=](const CommonMuonHit& hit) {
    return (hit.x() + hit.dx() / 2.) > xmin;
  });
  return {first, m_index[region + 1]};
}

CommonMuonHitRange CommonMuonStation::hits(double xmin, double xmax,
                                           unsigned int region) const {
  auto first = std::find_if(m_index[region], m_index[region + 1],
                            [=](const CommonMuonHit& hit) {
    return (hit.x() + hit.dx() / 2.) > xmin;
  });
  auto last =
      std::find_if(first, m_index[region + 1], [=](const CommonMuonHit& hit) {
    // TODO: this weird asymmetry between first, last is for historical
    // reasons...
    return hit.x() > xmax;
        // return ( hit.x() - hit.dx() / 2.) > xmax;
  });
  return {first, last};
}

CommonMuonHitRange CommonMuonStation::hits(unsigned int region) const {
  return {m_index[region], m_index[region + 1]};
}

void CommonMuonStation::setHits(CommonMuonHits&& hts) {
  m_hits = std::move(hts);

  // TODO: first get rid of invalid hits ( x < xlow, y<ymin, y>ymax )
  // m_hits.erase( std::remove_if( std::begin(m_hits), std::end(m_hits), ... )
  // );
  auto by_x = [](const CommonMuonHit& lhs, const CommonMuonHit& rhs) {
    return lhs.x() < rhs.x();
  };
  auto y_lt_ = [](double ymax) {
    return [=](const CommonMuonHit& h) { return h.y() < ymax; };
  };
  auto x_lt_ = [](double xmax) {
    return [=](const CommonMuonHit& h) { return h.x() < xmax; };
  };

  auto id = std::begin(m_index);
  *id = std::begin(m_hits);
  // partition in x
  for (auto x = std::next(std::begin(m_xboundaries)); x != std::end(m_xboundaries);
       ++x) {
    auto imax = std::partition(*id, std::end(m_hits), x_lt_(*x));
    // within each x partition, partition in y
    for (unsigned j = 0; j < nRegionsY; ++j) {
      *std::next(id) = std::partition(*id, imax, y_lt_(ymin() + (j + 1) * dy()));
      // within each region, sort by x
      std::sort(*id, *std::next(id), by_x);
      ++id;  // next region...
    }
    assert(*id == imax);  // no invalid hits in y...
  }
  assert(*id == std::end(m_hits));
  assert(uint(std::distance(std::begin(m_index), id)) ==
         uint(m_index.size() - 1));
}
