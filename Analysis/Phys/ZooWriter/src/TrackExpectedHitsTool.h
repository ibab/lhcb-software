// $Id: $
#ifndef INCLUDE_TRACKEXPECTEDHITSTOOL_H
#define INCLUDE_TRACKEXPECTEDHITSTOOL_H 1

#include "GaudiAlg/GaudiTool.h"

#include <vector>

class IHitExpectation;
class IVeloExpectation;
namespace LHCb {
    class Track;
    class HitPattern;
    class LHCbID;
}

static const InterfaceID IID_TrackExpectedHitsTool("TrackExpectedHitsTool", 1, 0);

/** @class TrackExpectedHitsTool TrackExpectedHitsTool.h
 *  
 *  determine the hits one would expect to be on a track based on its track
 *  parameters
 *
 *  factored into a tool from code in TrackComputeExpectedHits
 *
 * @author Manuel Tobias Schiller <schiller@physi.uni-heidelberg.de>
 * @date   2010-01-14
 */
class TrackExpectedHitsTool : public GaudiTool {

public:

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_TrackExpectedHitsTool; }

  /// Standard Constructor
  TrackExpectedHitsTool(const std::string& type,
                        const std::string& name,
                        const IInterface* parent);

  virtual ~TrackExpectedHitsTool(); ///< Destructor

  virtual StatusCode initialize(); ///< Tool initialization
  virtual StatusCode   finalize(); ///< Tool finalize

  virtual StatusCode execute(
	  const LHCb::Track& tr, LHCb::HitPattern& pattern,
	  std::vector<LHCb::LHCbID>& ids) const;

protected:


private:
  IVeloExpectation* m_veloExpectation;
  IHitExpectation* m_ttExpectation;
  IHitExpectation* m_itExpectation;
  IHitExpectation* m_otExpectation;

  bool m_forceVeloExpectation;

};
#endif // INCLUDE_TRACKEXPECTEDHITSTOOL_H

