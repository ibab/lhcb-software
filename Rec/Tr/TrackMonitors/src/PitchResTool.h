#ifndef INCLUDE_PITCHRESTOOL_H
#define INCLUDE_PITCHRESTOOL_H 1

#include "GaudiAlg/GaudiTool.h"

#include "TrackInterfaces/IPitchResTool.h"

#include <string>
#include <vector>
#include <utility>

// forward declaratrions
class ITrackExtrapolator;
namespace LHCb { class Track; }

/** @class PitchResTool PitchResTool.h
 * tool to calculate pitch residuals for all suitable pairs of OT hits
 * on a track
 *
 * @author Manuel Tobias Schiller <schiller@physi.uni-heidelberg.de>
 * @date   2008-06-05
 */
class PitchResTool : public GaudiTool, 
		     virtual public IPitchResTool {

public:

  /// Standard Constructor
  PitchResTool(const std::string& type,
               const std::string& name,
               const IInterface* parent);

  virtual ~PitchResTool(); ///< Destructor

  virtual StatusCode initialize(); ///< Tool initialization

  /// return a vector of pairs of (layer, pitch residual) from the OT hits on the track
  std::vector<std::pair<LHCb::OTChannelID, double> > calcPitchResiduals(const LHCb::Track* track) const;

protected:


private:
  std::string m_trackExtrapolatorName;
  ITrackExtrapolator* m_extrapolator;

};
#endif // INCLUDE_PITCHRESTOOL_H

