// $Id: OMAMsgInterface.h,v 1.2 2008-08-11 08:05:15 ggiacomo Exp $
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
  /// set output message interface
  void setMessageMode(OMAMsgMode mode) {
    m_msgMode = mode;}
  /// get output message interface
  inline OMAMsgInterface::OMAMsgMode messageMode() {
    return m_msgMode;}
protected:
  OMAMsgMode m_msgMode;
};
#endif // OMALIB_OMAMSGINTERFACE_H
