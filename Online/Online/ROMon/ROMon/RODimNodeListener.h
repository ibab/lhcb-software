// $Id: RODimNodeListener.h,v 1.1 2008-02-01 17:41:45 frankm Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/RODimNodeListener.h,v 1.1 2008-02-01 17:41:45 frankm Exp $
#ifndef ROMON_ROMONDIMNODELISTENER_H
#define ROMON_ROMONDIMNODELISTENER_H 1

// Framework includes
#include "ROMon/RODimListener.h"

/*
 *   ROMon namespace declaration
 */
namespace ROMon {

  /**@class RODimNodeListener ROMon.h GaudiOnline/ROMon.h
   *
   *   DIM FSM status listener to collect the full information from nodes
   *
   *   @author M.Frank
   */
  class RODimNodeListener : public RODimListener {
  public:
    struct Descriptor {
      int len;
      int actual;
      char* data;
      Descriptor() : len(0), actual(0), data(0) {}
      void release() { if ( data ) ::free(data); data = 0; }
    };
  protected:
    /// String matching of services
    std::string m_match;

  public:
    /// Standard constructor
    RODimNodeListener(bool verbose=false) : RODimListener(verbose) {}
    /// Standard destructor
    virtual ~RODimNodeListener();
    /// Change service name matching string
    void setMatch(const std::string& val) {  m_match = val;    }
    /// Access service name match
    const std::string& match() const      {  return m_match;   }    
    /// Add handler for a given message source
    virtual void addHandler(const std::string& node,const std::string& svc);
    /// Remove handler for a given message source
    virtual void removeHandler(const std::string& node, const std::string& svc);
    /// DimInfoHandler overload
    static void infoHandler(void* tag, void* address, int* size);
  };
}      // End namespace ROMon
#endif /* ROMON_ROMONDIMNODELISTENER_H */

