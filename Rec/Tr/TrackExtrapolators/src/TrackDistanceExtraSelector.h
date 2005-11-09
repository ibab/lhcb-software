// $Id: TrackDistanceExtraSelector.h,v 1.1 2005-11-09 14:43:46 erodrigu Exp $
#ifndef TrackDistanceExtraSelector_H
#define TrackDistanceExtraSelector_H 1

// Include files

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "ITrackExtraSelector.h"

#include <string>

/** @class TrExtraSelector TrExtraSelector.h "TrExtrapolator/TrExtraSelector.h"
*
*  Distance selection of one extrapolator
*
*/

class TrackDistanceExtraSelector: public GaudiTool,
                                  virtual public ITrackExtraSelector {

public:
  TrackDistanceExtraSelector(const std::string& type, 
                             const std::string& name,
                             const IInterface* parent);

  ~TrackDistanceExtraSelector();

  StatusCode initialize();

  virtual ITrackExtrapolator* select( const double zStart,
                                      const double zEnd ) const;

 private:
  /// Extrapolators...
  double m_shortDist;

  ITrackExtrapolator* m_shortFieldExtrapolator;
  ITrackExtrapolator* m_longFieldExtrapolator; 
 /// extrapolator to use for short transport in mag field
  std::string m_shortFieldExtrapolatorName;
  std::string m_shortFieldExtrapolatorType;
 
  /// extrapolator to use for long transport in mag field
  std::string m_longFieldExtrapolatorName;
  std::string m_longFieldExtrapolatorType;

};

#endif // TrackDistanceExtraSelector_H
