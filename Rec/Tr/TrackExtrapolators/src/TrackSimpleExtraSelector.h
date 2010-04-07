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

class TrackSimpleExtraSelector: public GaudiTool,
                                virtual public ITrackExtraSelector {

public:
  TrackSimpleExtraSelector( const std::string& type, 
                            const std::string& name,
                            const IInterface* parent );

  virtual ~TrackSimpleExtraSelector();

  StatusCode initialize();

  virtual const ITrackExtrapolator* select( const double zStart,
                                      const double zEnd ) const;
  
private:
  const ITrackExtrapolator* m_extrapolator;
  std::string         m_extraName;
  
};

#endif // TrackSimpleExtraSelector_H
