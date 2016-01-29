#ifndef DISTMUIDTOOL_H
#define DISTMUIDTOOL_H 1

// Include files
#include <bitset>
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Chi2MuIDTool.h"


/** @class DistMuIDTool DistMuIDTool.h
 *
 *
 *  @author Jose Angel Hernando Morata
 *  @author Xabier Cid Vidal
 *  @date   2009-03-12
 */
class DistMuIDTool : public Chi2MuIDTool {
public:
  /// Standard constructor
  DistMuIDTool( const std::string& type,
                const std::string& name,
                const IInterface* parent);

  ~DistMuIDTool( ) override = default; ///< Destructor

  StatusCode muonQuality(LHCb::Track& muTrack, double& Quality) override;

private:
  StatusCode computeDistance(const LHCb::Track& muTrack,double& dist,
                             std::bitset<4> sts_used );

  bool m_applyIsmuonHits;


};
#endif // DISTMUIDTOOL_H
