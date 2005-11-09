// $Id: TrackSimpleExtraSelector.h,v 1.1 2005-11-09 14:43:46 erodrigu Exp $
#ifndef TrackSimpleExtraSelector_H
#define TrackSimpleExtraSelector_H 1

// Include files
#include "GaudiAlg/GaudiTool.h"

#include "ITrackExtraSelector.h"

#include <string>

/** @class TrExtraSelector TrExtraSelector.h "TrExtrapolator/TrExtraSelector.h"
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

  ~TrackSimpleExtraSelector();

  StatusCode initialize();

  virtual ITrackExtrapolator* select( const double zStart,
                                      const double zEnd ) const;

private:
  ITrackExtrapolator* m_extrapolator;
  std::string         m_extraName;
  
};

#endif // TrackSimpleExtraSelector_H
