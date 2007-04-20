#ifndef TALIGNMENTINTERFACE_H
#define TALIGNMENTINTERFACE_H 1

#include "GaudiKernel/IAlgTool.h"

//*************************************
// Interface to a track selector tool for
// the alignment framework
// Created by: Johan Blouw, Physikalisches Institut, Heidelberg
//******************************

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

  // Accept a track when it fullfills certain criteria
  virtual bool accept( const LHCb::Track& aTrack ) const = 0;

  // divide tracks in three classes (Through IT stations, through OT stations, crossing from one to the other
  virtual int traversesIT( LHCb::Track& aTrack, 
			   int& nOTHits, 
			   int& nITHits ) const = 0;

};
#endif	   
