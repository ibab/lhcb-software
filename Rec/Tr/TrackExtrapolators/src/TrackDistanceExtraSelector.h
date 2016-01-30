#ifndef TrackDistanceExtraSelector_H
#define TrackDistanceExtraSelector_H 1

// Include files

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from TrackInterfaces
#include "TrackInterfaces/ITrackExtraSelector.h"

#include <string>

/** @class TrackDistanceExtraSelector "TrackDistanceExtraSelector.h"
*
*  Distance selection of one extrapolator
*
*/

class TrackDistanceExtraSelector: public extends<GaudiTool, ITrackExtraSelector> {

public:
  TrackDistanceExtraSelector(const std::string& type, 
                             const std::string& name,
                             const IInterface* parent);

  ~TrackDistanceExtraSelector() override;

  StatusCode initialize() override;

  const ITrackExtrapolator* select( double zStart,
                                    double zEnd ) const override;

 private:
  double m_shortDist;
  const ITrackExtrapolator* m_shortDistanceExtrapolator;
  const ITrackExtrapolator* m_longDistanceExtrapolator; 
  /// extrapolator to use for short transport in mag field
  std::string m_shortDistanceExtrapolatorType;
  /// extrapolator to use for long transport in mag field
  std::string m_longDistanceExtrapolatorType;
};

#endif // TrackDistanceExtraSelector_H
