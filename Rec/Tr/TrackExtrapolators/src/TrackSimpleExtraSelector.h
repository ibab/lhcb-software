// $Id: TrackSimpleExtraSelector.h,v 1.5 2010-04-07 21:08:38 wouter Exp $
#ifndef TrackSimpleExtraSelector_H
#define TrackSimpleExtraSelector_H 1

// Include files
#include "GaudiAlg/GaudiTool.h"

// from TrackInterfaces
#include "TrackInterfaces/ITrackExtraSelector.h"

#include <string>

/** @class TrackSimpleExtraSelector "TrackSimpleExtraSelector.h"
*
*  Simple selection of one extrapolator
*
*/

class TrackSimpleExtraSelector: public extends<GaudiTool, ITrackExtraSelector> {

public:
  TrackSimpleExtraSelector( const std::string& type, 
                            const std::string& name,
                            const IInterface* parent );

  StatusCode initialize() override;

  const ITrackExtrapolator* select( double zStart,
                                    double zEnd ) const override;
private:
  const ITrackExtrapolator* m_extrapolator;
  std::string         m_extraName;
};

#endif // TrackSimpleExtraSelector_H
