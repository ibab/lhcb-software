// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/GaudiOnline/MDFSender.h,v 1.1 2006-07-04 17:03:42 frankb Exp $
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
    typedef std::vector<std::string> Recipients;
    /// Property: Send target names
    Recipients             m_recipients;
    /// Property: printout frequence
    float                  m_freq;
    /// Round robing sending: current send target
    Recipients::iterator   m_current;
    /// Counter of number of events sent
    long long int          m_evtSent;
    /// Counter of number of bytes sent
    long long int          m_bytesSent;
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
