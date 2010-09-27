#define ENABLEEVENTVIEWER //commentout this line to disable the class
#ifdef  ENABLEEVENTVIEWER


#ifndef EventViewer_H
#define EventViewer_H 1

#include <iostream>
#include <vector>
#include <TLorentzVector.h>
#include "Tagger.h"
#include "Event.h"
#include "GenericTool.h"
#include "TEveVector.h"

using namespace std;

/** @class EventViewer
 *
 *  Tool to visulize the event
 *
 *  @author Marco Musy
 *  @date   2010-07-15
 */

class EventViewer : public GenericTool {

 public:

  EventViewer( );
  ~EventViewer( ); ///< Destructor

  void storeClone( Event& event);
  void DrawStoredEvents();
  void DrawLHCb(int opts=0);

 private:
  TEveVector  GetTEveVector( Float_t& eta, Float_t& phi );
  void DrawEvent(Event* event);

  vector< Event* > event_list;
  int m_ViewOption;
  Float_t length;

};

#endif 
#endif 
