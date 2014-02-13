// $Id: EventServerRunable.h,v 1.7 2008-10-14 08:37:21 frankb Exp $
//====================================================================
//  MooreTestSvc
//--------------------------------------------------------------------
//
//  Description: Runable to run Moore standalone on a single node.
//  Author     : M.Frank
//====================================================================
// $Header: $
#ifndef MOORESTANDALONE_UPIMOOREMONITOR_H
#define MOORESTANDALONE_UPIMOOREMONITOR_H

// Framework include files
#include "CPP/Interactor.h"
#include "Internals.h"

// C/C++ include files
#include <map>

// Forward declarations
class Event;
class IocSensor;

/*
 *   LHCb namespace declaration
 */
namespace LHCb {

  /** @class UpiMooreMonitor UpiMooreMonitor.h MooresStandalone/UpiMooreMonitor....h
   *
   *   Class definition of UpiMooreMonitor.
   *
   *
   *  @author Markus Frank
   *  @version 1.0
   */
  class UpiMooreMonitor : public Interactor  {
  protected:
    typedef MooreTest::UserStats UserStats;
    typedef MooreTest::Statistics Statistics;
    int m_id;
    int m_lines;
    int m_mode;
    bool m_update;
    Interactor* m_parent;
    Interactor* m_feed;
    IocSensor* m_ioc;
    enum Cmds {  
      COM_HEADER1 = 1,
      COM_HEADER2,
      COM_STATISTICS_1,
    };
  public:
    /// Initializing constructor
    UpiMooreMonitor(int mode, Interactor* parent, Interactor* feed);
    /// Standard destructor
    virtual ~UpiMooreMonitor();
    void setFeed(Interactor* feed);
    virtual void update(const Statistics& stat);
    virtual void handle (const Event& event);
  };
}
#endif // MOORESTANDALONE_UPIMOOREMONITOR_H
