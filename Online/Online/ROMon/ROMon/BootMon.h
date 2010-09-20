// $Id: BootMon.h,v 1.1 2010-09-20 19:00:10 frankb Exp $
//====================================================================
//  ROMon
//--------------------------------------------------------------------
//
//  Package    : ROMon
//
//  Description: Structure of basic items used to monitor the boot status
//               of the CPU farm(s)
//
//  Author     : M.Frank
//  Created    : 20/09/2010
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/BootMon.h,v 1.1 2010-09-20 19:00:10 frankb Exp $
#ifndef ONLINE_ROMON_BOOTMON_H
#define ONLINE_ROMON_BOOTMON_H

// Framework include files
#include "ROMon/Collections.h"
#include "RTL/Pack.h"

// C++ include files
#include <cstring>

/*
 *   ROMon namespace declaration
 */
namespace ROMon {

  // Forward declarations
  class BootNodeStatus;
  class BootNodeStatusset;

  /**@class BootNodeStatus BootMon.h ROMon/BootMon.h
   *
   * Class which represents the boot status of a single node in a subfarm
   *
   * @author M.Frank
   */
  PACK_DATA(class) BootNodeStatus {
  public:
    char         name[16];
    long         dhcpReq;
    long         mountReq;
    long         fmcStart;
    unsigned int address;
    int          status;

    enum {
      DHCP_REQUESTED   = 1<<0,
      MOUNT_REQUESTED  = 1<<1,
      CPU_STARTED      = 1<<2,
      PCI_STARTED      = 1<<3,
      ETH0_STARTED     = 1<<4,
      ETH1_STARTED     = 1<<5,
      TCP_STARTED      = 1<<6,
      FMC_STARTED      = 1<<7
    };

    /// Default constructor
    BootNodeStatus() : dhcpReq(0), mountReq(0), fmcStart(0), address(0), status(0) { name[0]=0;}

    /// Copy constructor
    BootNodeStatus(const BootNodeStatus& c) 
      : dhcpReq(c.dhcpReq), mountReq(c.mountReq), fmcStart(c.fmcStart), address(c.address), status(c.status)
    {
      setName(c.name);
    }
    void setName(const std::string& n) {
      ::strncpy(name,n.c_str(),sizeof(name));
      name[sizeof(name)-1]=0;
    }
    BootNodeStatus& operator=(const BootNodeStatus& c) {
      setName(c.name);
      dhcpReq  = c.dhcpReq;
      mountReq = c.mountReq;
      fmcStart = c.fmcStart;
      address  = c.address;
      status   = c.status;
      return *this;
    }
    static std::string flagName(int flag) {
      if ( 0 != (flag&DHCP_REQUESTED)  ) return "DHCP_REQUESTED";
      if ( 0 != (flag&MOUNT_REQUESTED) ) return "MOUNT_REQUESTED";
      if ( 0 != (flag&PCI_STARTED)     ) return "PCI_STARTED";
      if ( 0 != (flag&CPU_STARTED)     ) return "CPU_STARTED";
      if ( 0 != (flag&ETH0_STARTED)    ) return "ETH0_STARTED";
      if ( 0 != (flag&ETH1_STARTED)    ) return "ETH1_STARTED";
      if ( 0 != (flag&TCP_STARTED)     ) return "TCP_STARTED";
      if ( 0 != (flag&FMC_STARTED)     ) return "FMC_STARTED";
      return "UNKNOWN_FLAG";
    }
  };

  /**@class BootNodeStatusset ROMon.h ROMon/ROMon.h
   *
   * Class which represents all BootNodeStatuses of a subfarm
   *
   * @author M.Frank
   */
  PACK_DATA(class) BootNodeStatusset {
  public:
    enum { TYPE = 8 };
    typedef FixItems<BootNodeStatus> Nodes;
    /// First word: Data type descriptor (MUST always be 6)
    int  type;
    /// Node name
    char   name[16];
    /// Time stamp of last information update
    int time;
    /// Time stamp of the monitor snapshot [milli seconds]
    unsigned int millitm;
    /// Variable size array of node items
    Nodes nodes;
    /// Default constructor
    BootNodeStatusset(const std::string& n);
    /// Reset object structure
    BootNodeStatusset* reset();
    /// Length of the object in bytes
    int length()  const {  return sizeof(BootNodeStatusset)+nodes.data_length(); }
  };

  struct BootClusterItem {
    char name[16];
  };
  typedef FixItems<BootClusterItem> BootClusterCollection;

} // End namespace ROMon

#endif // ONLINE_ROMON_BOOTMON_H
