#ifndef COMMONMUONSTATION_H_
#define COMMONMUONSTATION_H_

#include <vector>

#include "GaudiKernel/Range.h"
#include "MuonDet/DeMuonDetector.h"

#include "MuonID/CommonMuonHit.h"

/** @class CommonMuonStation CommonMuonStation.h
 *  Class to hold regions of hits and provide some geometry information
 *
 *  Used to be Hlt/Hlt1Muons/Hlt1MuonStation.
 *
 *  @author Roel Aaij
 *  @author Kevin Dungs
 *  @date   2015-01-03
 */
class CommonMuonStation final {
 public:
  CommonMuonStation(DeMuonDetector* det, int station,
                    std::vector<double> regions);

  enum { nRegionsY = 7u };

  double z() const { return m_z; }

  unsigned int station() const { return m_station; }

  CommonMuonHitRange hits(double xmin, unsigned int region) const;
  CommonMuonHitRange hits(double xmin, double xmax, unsigned int region) const;

  CommonMuonHitRange hits(unsigned int region) const;

  unsigned int nRegions() const {
    return (m_xboundaries.size() - 1) * nRegionsY;
  }

  // TODO: generate range of id which overlap (xMin,xMax,yMin,yMax)...
  bool overlaps(unsigned id, double xmin, double xmax, double ymin_,
                double ymax) const {
    auto j = id % nRegionsY;
    auto i = id / nRegionsY;
    auto y = ymin() + j * dy();
    return !(xmax < m_xboundaries[i] || xmin > m_xboundaries[i + 1] ||
             ymin_ > y + dy() || ymax < y);
  }

 private:
  friend class CommonMuonHitManager;

  void setHits(CommonMuonHits&& hts);

  inline double dy() const { return 2 * m_ymax / nRegionsY; }
  inline double ymin() const { return -m_ymax; }

  const std::vector<double> m_xboundaries;
  CommonMuonHits m_hits;
  std::vector<typename CommonMuonHits::iterator>
      m_index;  // indices into m_hits

  double m_z;
  double m_ymax;

  const int m_station;
};
#endif  // COMMONMUONSTATION_H_
