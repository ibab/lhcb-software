#ifndef COMMONMUONHIT_H_
#define COMMONMUONHIT_H_

#include "GaudiKernel/Range.h"
#include "GaudiKernel/boost_allocator.h"
#include "Kernel/MuonTileID.h"

/** @class CommonMuonHit CommonMuonHit.h
 * A container for a muon hit that holds a reference to the corresponding tile
 *as well as information on x, y, z and corresponding errors.
 * 
 * Used to be Hlt/Hlt1Muons/Hlt1MuonHit.
 *
 *  @author Roel Aaij
 *  @author Kevin Dungs
 *  @date   2015-01-03
 */
class CommonMuonHit final {
 public:
  CommonMuonHit() = delete;

  CommonMuonHit(const LHCb::MuonTileID& tile, double x, double dx, double y,
                double dy, double z, double dz, bool uncrossed)
      : m_params{{x, y, dx, z, dy, dz}}, m_tile{tile}, m_uncrossed{uncrossed} {}

  const LHCb::MuonTileID& tile() const { return m_tile; }
  unsigned int station() const { return m_tile.station(); }
  double x() const { return m_params[0]; }
  double dx() const { return m_params[2]; }
  double y() const { return m_params[1]; }
  double dy() const { return m_params[4]; }
  double z() const { return m_params[3]; }
  double dz() const { return m_params[5]; }
  bool uncrossed() const {return m_uncrossed; }

 private:
  std::array<double, 6> m_params;
  LHCb::MuonTileID m_tile;
  bool m_uncrossed;
};

typedef std::vector<CommonMuonHit> CommonMuonHits;
typedef std::vector<const CommonMuonHit*> CommonConstMuonHits;
typedef const Gaudi::Range_<CommonMuonHits> CommonMuonHitRange;
typedef const Gaudi::Range_<CommonConstMuonHits> CommonConstMuonHitRange;

#endif  // COMMONMUONHIT_H_
