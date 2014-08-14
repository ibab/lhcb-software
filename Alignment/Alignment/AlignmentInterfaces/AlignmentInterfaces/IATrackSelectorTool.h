#ifndef ATRACKSELECTORTOOL_H
#define ATRACKSELECTORTOOL_H

#include "GaudiKernel/IAlgTool.h"

//*************************************
// Interface to a track selector tool for
// the alignment framework
// Created by: Johan Blouw, Physikalisches Institut, Heidelberg
// modified 17.10.2007 by M.Deissenroth
//******************************

static const InterfaceID IID_IATrackSelectorTool("IATrackSelector",1,0);

namespace LHCb {class Track;}

class IATrackSelectorTool : virtual public IAlgTool {
public:

  // Retrieve interface ID
  static const InterfaceID & interfaceID() { return IID_IATrackSelectorTool;   }

  // Accept a track when it fullfills certain criteria
  virtual bool accept( const LHCb::Track& aTrack ) = 0;

  // divide tracks in three classes (Through IT stations, through OT stations, crossing from one to the other
  virtual bool countTHits( const LHCb::Track& aTrack, 
                           int& nOTHits, 
                           int& nITHits ,
                           int& nTTHits) = 0;
  virtual StatusCode Reset() = 0;

  virtual void PrintUniformTD() = 0;

 protected:

 private:
};
#endif	  // ATRACKSELECTORTOOL_H 
