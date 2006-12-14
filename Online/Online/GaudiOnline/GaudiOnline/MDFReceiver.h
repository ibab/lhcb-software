// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/GaudiOnline/MDFReceiver.h,v 1.1 2006-12-14 18:59:14 frankb Exp $
//	====================================================================
//  MDFReceiver.h
//	--------------------------------------------------------------------
//
//	Author    : Markus Frank
//
//	====================================================================
#ifndef ONLINE_GAUDIONLINE_MDFRECEIVER_H
#define ONLINE_GAUDIONLINE_MDFRECEIVER_H

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/xtoa.h"
#include "Event/RawBank.h"
#include "MDF/MDFHeader.h"
#include "MBM/Producer.h"
#include "MBM/mepdef.h"
#include "AMS/amsdef.h"
#include "WT/wtdef.h"

/*
 *  LHCb namespace declaration
 */
namespace LHCb  {

  /**@class MDFReceiver MDFReceiver.cpp
    *
    *
    * @author:  M.Frank
    * @version: 1.0
    */
  class MDFReceiver : public Algorithm   {
    /// Pointer to MBM producer
    MBM::Producer*       m_prod;
    /// Partition ID
    int             m_partitionID;
    /// MBM buffer name
    std::string     m_buffer, m_bm_name;
    /// Process name
    std::string     m_procName;
    /// Flag to indicate if a partitioned buffer should be connected
    bool            m_partitionBuffer;
    /// Monitoring quantity: Number of events received from network
    int             m_evtCount;

  public:
    /// Standard algorithm constructor
    MDFReceiver(const std::string& name, ISvcLocator* pSvcLocator);
    /// Standard Destructor
    virtual ~MDFReceiver();
    /// amsu Callback on receiving ams message
    static int s_receiveEvt(const amsuc_info* info, void* param)   
    {  return ((MDFReceiver*)param)->receiveEvent(info);        }
    /// amsu Callback on task dead
    static int s_taskDead(const amsuc_info* info, void* param)
    {  return ((MDFReceiver*)param)->taskDead(info);            }
    /// Callback on receiving event
    int receiveEvent(const amsuc_info* info);
    /// Callback on task dead
    int taskDead(const amsuc_info* info);
    /// Initialize Algorithm
    virtual StatusCode initialize();
    /// Finalize Algorithm
    virtual StatusCode finalize();
    /// Execute procedure
    virtual StatusCode execute();
  };
}
#endif // ONLINE_GAUDIONLINE_MDFRECEIVER_H
