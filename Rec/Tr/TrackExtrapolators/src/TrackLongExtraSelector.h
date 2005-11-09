// $Id: TrackLongExtraSelector.h,v 1.2 2005-11-09 14:54:11 erodrigu Exp $
#ifndef TrackLongExtraSelector_H
#define TrackLongExtraSelector_H 1

// Include files
#include "GaudiAlg/GaudiTool.h"

// from TrackInterfaces
#include "TrackInterfaces/ITrackExtraSelector.h"

#include <string>

/** @class TrExtraSelector TrExtraSelector.h "TrExtrapolator/TrExtraSelector.h"
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

  ~TrackLongExtraSelector();

  StatusCode initialize();

  virtual ITrackExtrapolator* select( const double zStart,
                                      const double zEnd ) const;

private:
   /// Extrapolators...
  double m_shortDist;
  double m_startT;
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
