// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/GaudiOnline/MBMMonitorSvc.h,v 1.2 2007-01-24 19:21:08 frankb Exp $
//	====================================================================
//  MBMMonitorSvc.cpp
//	--------------------------------------------------------------------
//
//	Author    : Markus Frank
//
//	====================================================================
#ifndef GAUDIONLINE_MBMMONITORSVC_H
#define GAUDIONLINE_MBMMONITORSVC_H

#include "GaudiOnline/OnlineService.h"
#include "MBM/bmstruct.h"
#include <map>

// Declaration of the interface ID. 
static const InterfaceID IID_IMBMMonitorSvc("IMBMMonitorSvc", 0, 0); 

/*
 *    LHCb namespace
 */
namespace LHCb    {

  /** @class MBMMonitorSvc MBMMonitorSvc.cpp  GaudiOnline/MBMMonitorSvc.h
    *
    * Conversion service for the online data.
    * 
    * @author  M.Frank
    * @version 1.0
    * @date    01/01/2005
    */
  class MBMMonitorSvc : public OnlineService  {

    // Forward declarations
    struct MBMDescriptor;

    /** @struct Client
      *
      */
    struct Client  {
      struct  ProcessThin {     /// Total size is 4 bytes; to be published as int
        /// -- Byte 0
        unsigned pid:16;    /// Process ID
        /// -- Byte 3
        unsigned type:2;    /// Process type '0'=unknown '1'=producer '2'=consumer 
        unsigned state:4;   /// Process state (enum MBM::BufferStates)
        unsigned active:1;  /// Flag to indicate process state
        unsigned inuse:1;   /// Flag to indicate if slot is in use
        /// -- Byte 4
        unsigned reqs:4;    /// Requirements summary
        unsigned spare:4;
      };
      struct ProcessFat  {
        char type;          /// Process type '0'=unknown '1'=producer '2'=consumer 
        char state;         /// Process state (enum MBM::BufferStates)
        char active;
        char inuse;
        int  pid;
        int  reqs;
      };
      int  partid;      /// Partition ID
      int  events;
      float utime;
      float stime;
      char name[32];
#ifdef _USE_THIN_DEVICES
      ProcessThin proc;
#else
      ProcessFat  proc;
#endif

      void init();
      void revoke(MBMDescriptor* dsc);
      void read(MBMDescriptor* dsc,const USER& us);
      void publish(MBMDescriptor* dsc,const std::string& client_name);
    };
    struct Info  {
      MBMDescriptor* dsc;

      int evtProduced;
      int evtPending;
      int evtSeen;
      int evtMax;
      int spaceTotal;
      int spaceFree;
      int userTotal;
      int userMax;
      char bmid[32];

      void publish();
      void publish(const std::string& new_user);
      void read();
      void revoke();
      void revoke(const char* client_name);
      void resetClients();
      void checkClients();
      Client clients[15];
    };
    typedef std::map<std::string, Client*>  MBMClients;
    struct MBMDescriptor   {
      char              active;
      const char*       name;
      ServerBMID        id;
      Info              info;
      MBMClients        clients;
      MBMMonitorSvc*    svc;
      MBMDescriptor();
      int  map();
      void unmap();
      void publish();
      void read();
      void revoke();
    };
    typedef std::map<std::string, MBMDescriptor*> MBMBuffers;

    MBMBuffers     m_buffers;
    /// Array of MBM descriptors for monitoring
    MBMDescriptor* m_bms;
    /// Pointer to data in MBM summary section
    BUFFERS*       m_buf;
    /// Handle to summary global section
    lib_rtl_gbl_t  m_bm_all;
    /// Number of connected buffers
    int            m_numBM;
    /// Alarm identifier when timer is active
    unsigned long  m_alarmID;
    /// Property: Timer facility used for WT (default:WT_FACILITY_TIMER1)
    int            m_timerFac;
    /// Property: use devices for clients connected to MBM
    int            m_deviceType;
    enum DevTypes   { NO_DEVICES = 0, FAT_DEVICES, THIN_DEVICES };

    void mapBuffers();
    void unmapBuffers();
    void publish();
    void revoke();
    void read();
    static int timerAction(unsigned int fac, void* par);
    static int timerAst(void* par);
    static int timerRearm(unsigned int fac, void* par);
  public:
    /// Retrieve interface ID
    static const InterfaceID& interfaceID() { return IID_IMBMMonitorSvc; }

    /// Standard service constructor
    MBMMonitorSvc(const std::string& nam, ISvcLocator* loc);

    /// Standard destructor
    virtual ~MBMMonitorSvc();
    /// Access to partition ID as specified in the job options
    //int partitionID()  const                 {   return m_partitionID;  }
    /// Access to process name
    //const std::string& processName()  const  {  return m_procName;      }
    /// Flag to indicate that MBM users should be represented as devices.
    int  deviceType() const                  {  return m_deviceType;    }

    /** Query interfaces of Interface
        @param riid       ID of Interface to be retrieved
        @param ppvUnknown Pointer to Location for interface pointer
    */
    virtual StatusCode queryInterface(const InterfaceID& riid,
                                      void** ppvInterface);

    /// IService overload: Initialize MEP manager service
    virtual StatusCode initialize();

    /// IService overload: Finalize MEP manager service
    virtual StatusCode finalize();

    /// Connect to optional MBM buffer
    StatusCode connectBuffer(const std::string& nam);

    /// Initialize buffers for MEP usage
    StatusCode initializeBuffers();

    /// Connect to specified buffers
    StatusCode connectBuffers();

    /// Cancel MBM connections
    StatusCode cancel();
  };
  typedef MBMMonitorSvc IMBMMonitorSvc;
}      // End namespace LHCb
#endif // GAUDIONLINE_MBMMONITORSVC_H
