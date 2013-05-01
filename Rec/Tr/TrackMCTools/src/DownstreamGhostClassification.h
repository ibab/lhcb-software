#ifndef _DownstreamGhostClassification_H
#define _DownstreamGhostClassification_H

#include "TrackGhostClassificationBase.h"


namespace LHCb{
  class Track;
}

class DownstreamGhostClassification: public TrackGhostClassificationBase {

public:

  /// constructer
  DownstreamGhostClassification(const std::string& type,
                               const std::string& name,
                               const IInterface* parent);

  /** destructer */
  virtual ~DownstreamGhostClassification();


 private:

 virtual StatusCode specific(LHCbIDs::const_iterator& start, 
                       LHCbIDs::const_iterator& stop, LHCb::GhostTrackInfo& tinfo) const;


};



#endif
