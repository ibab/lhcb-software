#ifndef PACKEDOBJECTCHECKSUM_H
#define PACKEDOBJECTCHECKSUM_H 1

#include <boost/crc.hpp>

#include "Event/PackedTrack.h"
#include "Event/PackedCaloCluster.h"
#include "Event/PackedRichPID.h"
#include "Event/PackedMuonPID.h"
#include "Event/PackedCaloHypo.h"
#include "Event/PackedProtoParticle.h"

namespace PackedDataPersistence {

/** @class PackedDataChecksum PackedDataChecksum.h
 *  Helper class that calculates packed object checksums
 *
 *  @author Rosen Matev
 *  @date   2016-01-03
 */
class PackedDataChecksum {
public:

  void processObject(const LHCb::PackedTracks& object);
  void processObject(const LHCb::PackedCaloClusters& object);
  void processObject(const LHCb::PackedRichPIDs& object);
  void processObject(const LHCb::PackedMuonPIDs& object);
  void processObject(const LHCb::PackedCaloHypos& object);
  void processObject(const LHCb::PackedProtoParticles& object);

  /// Obtain the current value of the checksum
  int checksum() const { return m_result.checksum(); }
  /// Reset the checksum
  void reset() { m_result.reset(); }

private:

  template<typename T>
  void process(const T& x);

  template<typename T>
  void processVector(const std::vector<T>& v);

  template <typename ...Args>
  void processMany(Args&&... args);


  boost::crc_32_type m_result; 

};

}

#endif // PACKEDOBJECTCHECKSUM_H
