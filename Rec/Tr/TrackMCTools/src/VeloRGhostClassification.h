#ifndef _VeloRGhostClassification_H
#define _VeloRGhostClassification_H

#include "TrackGhostClassificationBase.h"


namespace LHCb{
  class Track;
}

class VeloRGhostClassification: public TrackGhostClassificationBase {

public:

  /// constructer
  VeloRGhostClassification(const std::string& type,
                               const std::string& name,
                               const IInterface* parent);

  /** destructer */
  virtual ~VeloRGhostClassification();
 

 private:

  virtual StatusCode specific(LHCbIDs::const_iterator& start, 
                        LHCbIDs::const_iterator& stop, LHCb::GhostTrackInfo& tinfo) const;


};



#endif
