// $Id: Logger.h,v 1.3 2008-05-22 06:32:29 frankm Exp $
//====================================================================
//  ROLogger
//--------------------------------------------------------------------
//
//  Package    : ROLogger
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROLogger/ROLogger/Logger.h,v 1.3 2008-05-22 06:32:29 frankm Exp $
#ifndef ROLOGGER_LOGGER_H
#define ROLOGGER_LOGGER_H

// C++ include files
#include <vector>

// Framework include files
#include "CPP/Interactor.h"

/*
 *   ROLogger namespace declaration
 */
namespace ROLogger {

  /**@class Logger Logger.h ROLogger/Logger.h
   *
   *   Interface to DIM Error logger displays
   *
   *   @author M.Frank
   */
  class Logger : public Interactor {

  protected:
    /// Cluster definition
    typedef std::vector<std::string> Nodes;

    /// Logger service name to interact with
    std::string m_name;
  public:
    /// Standard constructor with object setup through parameters
    Logger(const std::string& name);
    /// Standard destructor
    virtual ~Logger();
    /// Display callback handler
    virtual void handle(const Event& ev);

    /// Shutdown logger display process
    void shutdown();

    /// Connect to messages of a given source
    int connectMessages(const std::vector<std::string>& name, const std::string& title);
    /// Connect to messages of a given source
    int connectMessages(bool con, const std::string& name);
    /// Connect to processing cluster of a given name
    int connectCluster(bool con, const std::string& name);

    /// Send data string to logger service
    int sendData(const std::string& msg);
  };
}      /* End namespace ROLogger */
#endif /* ROLOGGER_LOGGER_H */
