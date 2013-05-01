#ifndef _VeloGhostClassification_H
#define _VeloGhostClassification_H

#include "TrackGhostClassificationBase.h"


namespace LHCb{
  class Track;
}

class VeloGhostClassification: public TrackGhostClassificationBase {

public:


  /// constructer
  VeloGhostClassification(const std::string& type,
                               const std::string& name,
                               const IInterface* parent);

  /** destructer */
  virtual ~VeloGhostClassification();


 private:

  StatusCode specific(LHCbIDs::const_iterator& start, 
                LHCbIDs::const_iterator& stop, LHCb::GhostTrackInfo& tinfo) const;


};



#endif
