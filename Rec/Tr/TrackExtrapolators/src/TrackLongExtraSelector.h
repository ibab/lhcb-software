// $Id: TrackLongExtraSelector.h,v 1.5 2007-11-01 14:39:30 mneedham Exp $
#ifndef TrackLongExtraSelector_H
#define TrackLongExtraSelector_H 1

// Include files
#include "GaudiAlg/GaudiTool.h"

// from TrackInterfaces
#include "TrackInterfaces/ITrackExtraSelector.h"

#include <string>

/** @class TrackLongExtraSelector "TrackLongExtraSelector.h"
*
*  Simple selection of one extrapolator
*
*/

class TrackLongExtraSelector: public GaudiTool,
                              virtual public ITrackExtraSelector {

public:
  TrackLongExtraSelector(const std::string& type, 
                         const std::string& name,
                         const IInterface* parent);

  virtual ~TrackLongExtraSelector();

  StatusCode initialize();

  virtual ITrackExtrapolator* select( const double zStart,
                                      const double zEnd ) const;

private:
   /// Extrapolators...
  double m_shortDist;
  double m_startT;
  double m_endVelo;
  /// extrapolator to use for short transport in mag field
  std::string m_shortFieldExtrapolatorName;
  std::string m_shortFieldExtrapolatorType;
 
  /// extrapolator to use for long transport in mag field
  std::string m_longFieldExtrapolatorName;
  std::string m_longFieldExtrapolatorType;
 
  ITrackExtrapolator* m_shortFieldExtrapolator;
  ITrackExtrapolator* m_longFieldExtrapolator;


};

#endif // TrackLongExtraSelector_H
