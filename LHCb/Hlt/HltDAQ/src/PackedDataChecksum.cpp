#include "PackedDataChecksum.h"

namespace PackedDataPersistence {

void PackedDataChecksum::processObject(const LHCb::PackedTracks& x) {
  // no packingVersion here
  process(x.version());
  processVector(x.tracks());
  processVector(x.states());
  processVector(x.ids());
  processVector(x.extras());
}

void PackedDataChecksum::processObject(const LHCb::PackedCaloClusters& x) {
  process(x.packingVersion());
  process(x.version());
  processVector(x.data());
  processVector(x.entries());
}

void PackedDataChecksum::processObject(const LHCb::PackedRichPIDs& x) {
  process(x.packingVersion());
  process(x.version());
  processVector(x.data());
}

void PackedDataChecksum::processObject(const LHCb::PackedMuonPIDs& x) {
  process(x.packingVersion());
  process(x.version());
  processVector(x.data());
}

void PackedDataChecksum::processObject(const LHCb::PackedCaloHypos& x) {
  process(x.packingVersion());
  process(x.version());
  processVector(x.hypos());
  processVector(x.refs());
}

void PackedDataChecksum::processObject(const LHCb::PackedProtoParticles& x) {
  process(x.packingVersion());
  process(x.version());
  processVector(x.protos());
  processVector(x.refs());
  processVector(x.extras());
}

template<typename T>
void PackedDataChecksum::process(const T& x) {
  m_result.process_bytes((void*)&x, sizeof(x));
}

template <typename ...Args>
void PackedDataChecksum::processMany(Args&&... args) {
  auto list = {(process(args),1)...};
  (void)list;
}

template<typename T>
void PackedDataChecksum::processVector(const std::vector<T>& v) {
  for (const auto& x: v)
    process(x);
}

// We need to define custom functions for some structures. This is because
// they have padding bytes, which are undefined and will spoil the checksum.
template<>
void PackedDataChecksum::process(const LHCb::PackedRichPID& x) {
  processMany(x.pidResultCode, x.dllEl, x.dllMu, x.dllPi, x.dllKa, x.dllPr,
              x.track, x.dllBt, x.key, x.dllDe);
}
template<>
void PackedDataChecksum::process(const LHCb::PackedCaloClusterEntry& x) {
  processMany(x.digit, x.status, x.fraction);
}
template<>
void PackedDataChecksum::process(const LHCb::PackedCaloCluster& x) {
  processMany(x.key, x.type, x.seed,
              x.pos_x, x.pos_y, x.pos_z, x.pos_e,
              x.pos_c0, x.pos_c1,
              x.pos_cov00, x.pos_cov11, x.pos_cov22,
              x.pos_cov10, x.pos_cov20, x.pos_cov21,
              x.pos_spread00, x.pos_spread11,
              x.pos_spread10,
              x.firstEntry, x.lastEntry);
}

}