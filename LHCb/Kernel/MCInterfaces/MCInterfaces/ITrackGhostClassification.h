#ifndef _ITrackGhostClassification_H
#define _ITrackGhostClassification_H

#include "GaudiKernel/IAlgTool.h"
#include <vector>


namespace LHCb{
  class Track;
  class LHCbID;
  class GhostTrackInfo;
}

static const InterfaceID IID_ITrackGhostClassification( "ITrackGhostClassification", 0,0 );

class ITrackGhostClassification: virtual public IAlgTool {

public:

 typedef std::vector<LHCb::LHCbID> LHCbIDs;

 
 /// Retrieve interface ID
 static const InterfaceID& interfaceID() { return IID_ITrackGhostClassification ; }

 /**
 *  Information on what a ghost track is....
 *  @param aTrack to link
 *  @param tinfo link info 
 */
 virtual void info(const LHCb::Track& aTrack, LHCb::GhostTrackInfo& tinfo) const= 0;


 /**
 *  Information on a list of LHCbIDs
 *  @param start first iterator 
 *  @param stop  last iterator
 *  @param tinfo link info  
 */
 virtual void info(LHCbIDs::const_iterator& start, 
                   LHCbIDs::const_iterator& stop, LHCb::GhostTrackInfo& tinfo) const= 0;


  /**
  *  Check this is a ghost .
  *  @param aTrack to link
  *  @return bool true if a ghost
  */
  virtual bool isGhost(const LHCb::Track& aTrack) const= 0;

  /**
   *  Check this is a ghost .
   *  @param aTrack to link
   *  @return bool true if a ghost
   */
  virtual bool isGhost(LHCbIDs::const_iterator& start, 
                       LHCbIDs::const_iterator& stop) const = 0;


};

#endif
