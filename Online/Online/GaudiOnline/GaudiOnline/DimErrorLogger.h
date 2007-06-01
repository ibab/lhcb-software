// $Id: DimErrorLogger.h,v 1.7 2007-06-01 13:49:45 frankm Exp $
#ifndef GAUDISVC_DIMERRORLOGGER_H
#define GAUDISVC_DIMERRORLOGGER_H 1

// Framework includes
#include "GaudiOnline/OnlineRunable.h"
#include "GaudiOnline/IErrorLogger.h"
#include "dic.hxx"
#include <map>

/*
 *   LHCb namespace declaration
 */
namespace LHCb  {
  
  // Forward declarations
  class DnsInfo;

  /** Class definition of DimErrorLogger.
    *
    *
    *  @author Markus Frank
    *  @version 1.0
    */
  class DimErrorLogger : public OnlineRunable, 
                         virtual public IErrorLogger,
                         virtual public DimInfoHandler
  {
  protected:
    friend class DnsInfo;
    typedef std::map<std::string,DimInfo*> Clients;
    /// Map of known clients
    Clients   m_clients;
    /// Pointer to DNS info handler to accept client notifications
    DnsInfo*  m_dns;

    /// Output format used by the service to print messages to stdout
    std::string m_outputFmt;
    /// Property: if true Service will print message to stdout.
    bool        m_outputMsg;
    /// Property: if true accept messages of level VERBOSE
    bool        m_verboseMsg;
    /// Property: if true accept messages of level DEBUG
    bool        m_debugMsg;
    /// Property: if true accept messages of level INFO
    bool        m_infoMsg;
    /// Property: if true accept messages of level WARNING
    bool        m_warningMsg;
    /// Property: if true accept messages of level ERROR
    bool        m_errorMsg;
    /// Property: if true accept messages of level FATAL
    bool        m_fatalMsg;
    /// Property: if true accept messages of level ALWAYS
    bool        m_successMsg;
    /// Property: flag to print DIM service name
    bool        m_printSvc;
    /// Property: Match node name with service name for subscription
    bool        m_matchNode;
    /// Property: if true accept messages from wildcard matching sources  (Default: "*")
    std::vector<std::string> m_acceptedSources;
    /// Property: if true refuse messages from wildcard matching sources  (Default: "*")
    std::vector<std::string> m_refusedSources;
    /// Property: if match, messages from these clients will not be accepted
    std::vector<std::string> m_refusedClients;
    /// Property: if match, messages from these clients will be accepted (Default: "*")
    std::vector<std::string> m_acceptedClients;
    /// Property: if match, messages will not be accepted
    std::vector<std::string> m_refusedMessages;

    /// Add handler for a given message source
    void addHandler(const std::string& name);
    /// Remove handler for a given message source
    void removeHandler(const std::string& name);
    /// Report Error to logger
    virtual void reportMessage(int typ, const std::string& src, const std::string& msg);

  public:
    /// Standard Constructor
    DimErrorLogger(const std::string& nam, ISvcLocator* svcLoc);
    /// Standard Destructor
    virtual ~DimErrorLogger();
    // IInterface implementation : queryInterface
    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);
    /// IService implementation: initialize the service
    virtual StatusCode initialize();
    /// IService implementation: finalize the service
    virtual StatusCode finalize();
    /// IRunable implementation : Run the class implementation
    virtual StatusCode run();
    /// DimInfoHandler implementation: service data change callback
    virtual void infoHandler();
    /// Report Error to logger
    virtual void report(int typ, const std::string& src, const std::string& msg);
  };
}      // End namespace LHCb
#endif // GAUDISVC_DIMERRORLOGGER_H
