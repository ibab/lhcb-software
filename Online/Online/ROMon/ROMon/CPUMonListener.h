// $Id: CPUMonListener.h,v 1.1 2008-04-11 12:12:36 frankb Exp $
//====================================================================
//  ROMon
//--------------------------------------------------------------------
//
//  Package    : ROMon
//
//  Description: Readout monitoring in the LHCb experiment
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/CPUMonListener.h,v 1.1 2008-04-11 12:12:36 frankb Exp $
#ifndef ROMON_ROMONDIMNODELISTENER_H
#define ROMON_ROMONDIMNODELISTENER_H 1

// Framework includes
#include "ROMon/RODimListener.h"

/*
 *   ROMon namespace declaration
 */
namespace ROMon {

  /**@class CPUMonListener CPUMonListener.h ROMon/CPUMonListener.h
   *
   *   DIM FSM status listener to collect the full information from nodes
   *
   *   @author M.Frank
   */
  class CPUMonListener : public RODimListener {
  public:
    struct Descriptor {
      int    len;
      int    actual;
      time_t time;
      int    millitm;
      char*  data;
      Descriptor() : len(0), actual(0), time(0), millitm(0), data(0) {}
      void release() { if ( data ) ::free(data); data = 0; }
    };
  protected:
    /// String matching of services
    std::string m_match;
    std::string m_item;
  public:
    /// Standard constructor
    CPUMonListener(bool verbose=false) : RODimListener(verbose) {}
    /// Standard destructor
    virtual ~CPUMonListener();
    /// Change service name matching string
    void setMatch(const std::string& val) {  m_match = val;    }
    /// Access service name match
    const std::string& match() const      {  return m_match;   }    
    /// Change service name item string
    void setItem(const std::string& val)  {  m_item = val;     }
    /// Access service name item
    const std::string& item() const       {  return m_item;    }
    /// Add handler for a given message source
    virtual void addHandler(const std::string& node,const std::string& svc);
    /// Remove handler for a given message source
    virtual void removeHandler(const std::string& node, const std::string& svc);
    /// DimInfoHandler overload
    static void infoHandler(void* tag, void* address, int* size);
  };
}      // End namespace ROMon
#endif /* ROMON_ROMONDIMNODELISTENER_H */

