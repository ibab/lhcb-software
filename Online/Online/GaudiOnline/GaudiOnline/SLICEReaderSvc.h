//	============================================================
//
//	SLICEReaderSvc.h
//	------------------------------------------------------------
//
//	Package   : GaudiOnline
//
//	Authors    : Jean-Christophe Garnier
//				 S. Vijay Kartik
//
//	===========================================================
#ifndef GAUDIONLINE_SLICEREADERSVC_H
#define GAUDIONLINE_SLICEREADERSVC_H 1

#include <map>
#include <vector>
#include <string>
#include <queue>

#include <pthread.h>
#include <semaphore.h>

// Framework include files
#include "Event/RawEvent.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IRunable.h"
#include "GaudiKernel/Message.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IMonitorSvc.h"
#include "MBM/bmdef.h"
#include "MBM/Producer.h"

//XXX Don't think I need this to read files and put in SHM or Gaudi buffer or other structure
// Online stuff
#include "RTL/rtl.h"
#include "RTL/types.h"
#include "NET/IPHeader.h"
#include "GaudiOnline/MEPHdr.h"
#include "dis.hxx"

#include <mqueue.h>

// Forward declarations
class MsgStream;
class IIncidentSvc;
class ISvcLocator;

/*
 *  LHCb namespace declaration
 */
namespace LHCb  {

#define WHERE "File: " << __FILE__ << "[" << __LINE__ << "] " << __FUNCTION__ << "() : "

#define TWOMB 2097152
#define MDFHDRSZ 48 //12 word of 32 bits, or 4 bytes
#define BKSZMAX 65536


  /** @class SLICEReaderSvc SLICEReader.h GaudiOnline/SLICEReader.h
    *
    * Read 2MB slices of a file, find the event and push them to a shared buffer
    * 2 modes : bank reader, without MDFHeader nor DAQ bank, and event reader, with MDFHeader
    *
    *  @author Jean-Christophe Garnier
    *  @author S. Vijay Kartik
    *  @version 2.0
    */
  class SLICEReaderSvc : public Service,
                    virtual public IRunable,
                    virtual public IIncidentListener
  {
  public:
    typedef std::vector<std::string>        Requirements;

  protected:
    Requirements m_Req; /* Vector of strings which tells the options to access to buffer managers,             */
                        /* Most are useless and should be like this :                                          */
                        /* "EvType=2;TriggerMask=0xffffffff,0xffffffff,0xffffffff,0xffffffff;VetoMask=0,0,0,0; */
                        /*  MaskType=ANY;UserType=VIP;Frequency=PERC;Perc=100.0"                               */
                        /* But I wanted it to be parameterisable                                               */

    std::string m_ProcName;  /* RTL process name */

    int m_PartitionID; /* Partition ID of the events, read from option file and/or from Odin */

    std::string m_BufferName; /* Name of the buffer manager where to push events, partitionID is appended to it */
    BMID m_BMID;              /* Buffer manager id*/

    enum ReaderState { NOT_READY, READY, RUNNING, STOPPED }; /* FSM state of the Slice Reader Service */
    ReaderState  m_ReaderState; /* Current state */

    bool m_LoopOverFiles; /* Once all files have been read, starts from beginning */
    bool ignoreLumi; /* Ignore Lumi events that occur in files that are not specifically prepared for FEST */

    unsigned int m_CurFile; /* Current file descriptor */
    unsigned int m_NbEvtFromFile; /* Number of event read from the file */

    int m_SliceLen;   /* Length of the slice read, most of the time 2MB except when last slice of the file */
    int m_CurEvtLen;  /* Length of the current event */
    int m_OffsetEvt;  /* To manage the slice of 2MBs, address of the beginning of an event */
    int m_CurEvtIte;  /* To tell how many bytes were took in consideration in the current event, if it is splitted on 2 files */
                      /* while it is not equal to CurEvtLen, we're on the same event */
    int m_CurSliceIte;/* Current position on the slice */

    std::vector<std::string>    m_InputFiles; /* Files to read */

    char m_Buffer[TWOMB];   /* Buffer containing the chunk of 2MB read from file  */
    char m_CurEvent[TWOMB]; /* Buffer containing the current event, XXX assuming max size is 2MB ...*/

    IIncidentSvc*               m_IncidentSvc; /* Service to catch DIM commands */
    IMonitorSvc*                m_MonSvc;      /* Service to publish information*/

    int m_TotEvtsRead; /* Total of events read */
    int m_TotFilesRead;/* Total of files read */

    BMID getBuffer(); /* The method to connect to the buffer manager */
  public:

    // IInterface implementation : queryInterface
    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);
    // Standard Constructor
    SLICEReaderSvc(const std::string& name, ISvcLocator*parent);
    // Standard Destructor
    virtual ~SLICEReaderSvc(){}

    // Service overload: initialize()
    virtual StatusCode initialize();
    // Service overload: finalize()
    virtual StatusCode finalize();
    // IRunable implementation: Run the object
    virtual StatusCode run();
    // Handle
    virtual void handle(const Incident& incident);

    // Decode the slice of the MDF file
    StatusCode decodeMDF(char *slice, int size);
    // Read slices of a MDF file
    StatusCode readFile();
    // Return FAILURE and print information
    StatusCode error(const std::string & msg);
    // Push the event in the buffer manager
    StatusCode pushEvent(char *evt, int size);
    // Setup counters for monitoring
    int setupCounters();
    // Publish counters for monitoring
    void publishCounters();
    // Clear counters
    void clearCounters();

  private:
  };
}      // End namespace LHCb

#endif //  GAUDIONLINE_SLICEREADERSVC_H


