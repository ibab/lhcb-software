#ifndef ATRACKSELECTORTOOL_H
#define ATRACKSELECTORTOOL_H

#include "GaudiKernel/IAlgTool.h"

/* @class IATrackSelectorTool IATrackSelectorTool 
 * Interface to a track selector tool for
 * the alignment framework
 * @author : Johan Blouw, Physikalisches Institut, Heidelberg
*/

static const InterfaceID 
IID_IATrackSelectorTool("IATrackSelector",0,0);

namespace LHCb {
	class Track;
};

class IATrackSelectorTool : virtual public IAlgTool {
public:

  // Retrieve interface ID
  static const InterfaceID & interfaceID() {
    return IID_IATrackSelectorTool; 
  }

  virtual StatusCode initialize() = 0;

  /** Accepts a track when it fullfills certain criteria set via options by user
   * @input: LHCb::Track
   * @return: bool
  */
  virtual bool accept( const LHCb::Track& aTrack ) const = 0;

  /** divide tracks in three classes (Through IT stations, through OT stations, crossing from one to the other
   * @input: LHCb::Track, number of OT hits (nOTHits), number IThits (nITHits)
   * @return: integer which is used as identifier for the track-container to store the track
   *
  */
  virtual int traversesIT( LHCb::Track& aTrack, 
			   int& nOTHits, 
			   int& nITHits ) const = 0;

};
#endif	  // ATRACKSELECTORTOOL_H 
