//	============================================================
//
//	MEPOverflowWriterSvc.h
//	------------------------------------------------------------
//
//	Package   : GaudiOnline
//
//	Author    : Markus Frank
//
//	===========================================================
#ifndef GAUDIONLINE_MEPCONVERTERSVC_H
#define GAUDIONLINE_MEPCONVERTERSVC_H 1

// Framework include files
#include "GaudiKernel/IRunable.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiOnline/OnlineService.h"
#include "MDF/RawEventHelpers.h"
#include "MDF/RawEventDescriptor.h"
#include <map>
#include <list>
#include "stdio.h"

// Forward declarations
class IIncidentSvc;
namespace MBM
{
  class Consumer;
  class EventDesc;
}

/*
 *  LHCb namespace declaration
 */
namespace LHCb
{

  // Forward declarations
  class MEPManager;

  /** @class MEPOverflowWriterSvc MEPOverflowWriterSvc.h GaudiOnline/MEPOverflowWriterSvc.h
   *
   *  @author Markus Frank
   *  @version 1.0
   */
  class FileDescr
  {
  public:
    unsigned int m_RunNumber;
    int m_Handle;
    int m_Sequence;
    long m_BytesWritten;
    std::string m_FileName;
    int state;
    FileDescr()
    {
      m_Handle = m_Sequence = m_BytesWritten=0;
    }
  };

  class RunDesc
  {
  public:
    unsigned int m_RunNumber;
    long m_Files;
    long m_BytesWritten;
    FileDescr *m_CurrentFileDescr;
    long m_MEPs;
    RunDesc()
    {
      m_RunNumber = m_Files = m_BytesWritten = m_MEPs = 0;
    }
  };

  enum
  {
    C_OPEN=0,
    C_CLOSED=1
  };

  class MEPOverflowWriterSvc: public OnlineService,
      virtual public IRunable,
      virtual public IIncidentListener
  {
  public:
    //    typedef std::vector<LHCb::MEPFragment*> Frags;
    //    typedef std::map<unsigned int, Frags >  SubEvents;
        typedef std::vector<std::string>        Requirements;
  protected:
    /// Reference to MEP manager service
    MEPManager* m_mepMgr;
    MBM::Consumer* m_consumer;
    /// Flag indicating that MBM event retrieval is active
    bool m_receiveEvts;
    /// Property: printout frequence
    float m_freq;
    /// Property: Flag to register callback for MEP error event handling. 0=No handling, 1=save as MEP, 2=save as events
    int m_handleErrs;
    /// Property: 4rth. Word of trigger mask for sending ERROR MEPs.
    int m_routingBits;
    /// Property: Request specification
    Requirements m_req;
    /// Property: Name of output buffer for error events (requires handleErrs to be set)
    std::string m_errBuffer;
    /// Property: MEP manager service name/type
    std::string m_mepMgrName;

    /// Cached minimal allocation size for multi burst mode
    int m_minAlloc;
    /// Monitoring quantity: Number of MEPs processed
    long m_mepIn;
    /// Monitoring quantity: Number of MEPs processed
    long m_mepOut;
    /// Bytes Written to file
    long m_BytesOut;
    /// Number of files
    long m_NumFiles;
    /// Current Run Number (initialized to 0)
    unsigned int m_RunNumber;
    /// Name of the currently open file
    std::string m_FileName;
    ///File Prefix String. Filename wil be m_FilePrefix + Runnumber + _Sequence+.MEP
    std::string m_FilePrefix;
    ///Limit on the filesize in MB=1024*1024 Bytes
    long m_SizeLimit;
    FileDescr *m_FileDesc;
    ///List of runs and descriptors;
    std::map<unsigned int,RunDesc*> m_RunList;
    std::string m_node;

  public:
    /// Standard Constructor
    MEPOverflowWriterSvc(const std::string& name, ISvcLocator* svc);
    /// Standard Destructor
    virtual ~MEPOverflowWriterSvc();
    /// IInterface implementation: Query interface
    virtual StatusCode queryInterface(const InterfaceID& riid,
        void** ppvInterface);
    /// Incident handler implemenentation: Inform that a new incident has occured
    void handle(const Incident& inc);
    /// Service overload: initialize()
    virtual StatusCode initialize();
    /// Service overload: finalize()
    virtual StatusCode finalize();
    /// Process events
    virtual StatusCode run();
    /// get the Run Number from the current MEP
    virtual unsigned int getRunNumber(MEPEvent *me);
    /// Open a new file
    virtual FileDescr *openFile(unsigned int runnumber);
    /// Declare Monitoring information on a run
    virtual void CreateMonitoringInfo(unsigned int runn);
    /// get the file time string
    std::string FileTime();
    virtual void handleFileWriteError();
    virtual ssize_t Write(int fd, const void *buf, size_t n);
  };
} // End namespace LHCb
#endif //  GAUDIONLINE_MEPCONVERTERSVC_H
