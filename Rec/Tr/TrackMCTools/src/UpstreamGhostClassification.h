#ifndef _UpstreamGhostClassification_H
#define _UpstreamGhostClassification_H

#include "TrackGhostClassificationBase.h"


namespace LHCb{
  class Track;
}

class UpstreamGhostClassification: public TrackGhostClassificationBase {

public:

  /// constructer
  UpstreamGhostClassification(const std::string& type,
                               const std::string& name,
                               const IInterface* parent);

  /** destructer */
  virtual ~UpstreamGhostClassification();


 private:

 virtual StatusCode specific(LHCbIDs::const_iterator& start, 
                       LHCbIDs::const_iterator& stop, LHCb::GhostTrackInfo& tinfo) const;


};



#endif
