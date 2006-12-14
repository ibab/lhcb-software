// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/GaudiOnline/MDFSender.h,v 1.3 2006-12-14 18:59:15 frankb Exp $
//	====================================================================
//  MDFSender.h
//	--------------------------------------------------------------------
//
//	Author    : Markus Frank
//
//	====================================================================
#ifndef ONLINE_GAUDIONLINE_MDFSENDER_H
#define ONLINE_GAUDIONLINE_MDFSENDER_H

#include "MDF/MDFWriter.h"
#include <vector>

/*
 *  LHCb namespace declaration
 */
namespace LHCb  {

  /**@class MDFSender MDFSender.cpp
    *
    *
    * @author:  M.Frank
    * @version: 1.0
    */
  class MDFSender : public MDFWriter  {
  protected:
    typedef std::vector<std::string> Recipients;
    /// Property: Send target names
    Recipients             m_recipients;
    /// Property: printout frequence
    float                  m_freq;
    /// Round robing sending: current send target
    size_t                 m_current;
    /// Monitoring quantity: Counter of number of events sent
    int                    m_allEvtSent;
    /// Monitoring quantity: Counter of number of events sent
    std::vector<int>       m_evtSent;
    /// Monitoring quantity: Counter of number of event send errors
    int                    m_allEvtError;
    /// Monitoring quantity: Counter of number of event send errors
    std::vector<int>       m_evtError;
    /// Monitoring quantity: Counter of number of bytes sent
    int                    m_allBytesSent;
    /// Monitoring quantity: Counter of number of bytes sent
    std::vector<int>       m_bytesSent;

  public:
    /// Standard algorithm constructor
    MDFSender(const std::string& name, ISvcLocator* pSvcLocator);
    /// Standard Destructor
    virtual ~MDFSender();
    /// Initialize
    virtual StatusCode initialize();
    /// Finalize
    virtual StatusCode finalize();
    /// Execute procedure
    virtual StatusCode writeBuffer(void* const /* ioDesc */, const void* data, size_t len);
  };
}
#endif // ONLINE_GAUDIONLINE_MDFSENDER_H
