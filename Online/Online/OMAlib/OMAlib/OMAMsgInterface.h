// $Id: OMAMsgInterface.h,v 1.1 2008-03-11 18:23:26 ggiacomo Exp $
#ifndef OMALIB_OMAMSGINTERFACE_H 
#define OMALIB_OMAMSGINTERFACE_H 1

/** @class OMAMsgInterface OMAMsgInterface.h OMAlib/OMAMsgInterface.h
 *  Output interface for Online Monitoring Analysis messages 
 *
 *  @author Giacomo Graziani
 *  @date   2008-02-29
 */

#include <string>
class TH1;

class OMAMsgInterface {
public: 
  /// Standard constructor
  OMAMsgInterface( ); 

  virtual ~OMAMsgInterface( ); ///< Destructor

  
  typedef enum { INFO=3, WARNING, ALARM} OMAMsgLevel;
  typedef enum { LOCAL=0, ECS} OMAMsgMode;
  virtual void raiseMessage(OMAMsgLevel level,
                            std::string message,
                            std::string histogramName); 
  void setMessageMode(OMAMsgMode mode) 
  { msgMode = mode;}

protected:
  OMAMsgMode msgMode;
private:

};
#endif // OMALIB_OMAMSGINTERFACE_H
