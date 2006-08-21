// $Id: HPDMonitorApp.h,v 1.1.1.1 2006-08-21 13:49:59 ukerzel Exp $
#ifndef HPDMONITORAPP_H 
#define HPDMONITORAPP_H 1

// Include files

//
// ROOT 
//
#include "TApplication.h"
#include "TTimer.h"
#include "TObject.h"


//
// HPD monitoring GUI
//
#include "HPDGui.h"

/** @class HPDMonitorApp HPDMonitorApp.h
 *  
 *
 *  @author Ulrich Kerzel
 *  @date   2006-08-07
 *
 *  This is the steering class for the HPD monitoring GUI
 *  It is necessary to have it because the GUI class
 *  cannot inherit from TObject as well as the GUI main class.
 *  However, to be able to use the timer, the
 *  "HandleTimer" method of a class derived from TObject
 *  must be overridden.
 *  Besides this, this class only initialises the 
 *  ROOT application, starts the timer and GUI
 *  and passes the referece to the timer to the GUI
 *  so that the timer can be controlled from there.
 *
 */

class HPDMonitorApp: public TObject {

public:
  // N.B. timer rate in ms
  HPDMonitorApp(int timerRate = 200,
		int guiWidth  = 800,
		int guiHeight = 600);
  ~HPDMonitorApp();

  Bool_t HandleTimer(TTimer *timer);

  void Start();
  

  
protected:

private:
  TApplication *m_TApplication;

  TTimer       *m_Timer;
  int           m_timerRate;
  
  HPDGui       *m_HPDGui;

}; //class HPDMonitorApp

#endif // HPDMONITORAPP_H
