// $Id: FMCMonListener.h,v 1.2 2008-06-25 22:53:23 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/FMCMonListener.h,v 1.2 2008-06-25 22:53:23 frankb Exp $
#ifndef ROMON_FMCMONLISTENER_H
#define ROMON_FMCMONLISTENER_H 1

// Framework includes
#include "ROMon/RODimListener.h"

/*
 *   ROMon namespace declaration
 */
namespace ROMon {

  /**@class FMCMonListener FMCMonListener.h ROMon/FMCMonListener.h
   *
   *   DIM FSM status listener to collect the full information from nodes
   *
   *   @author M.Frank
   */
  class FMCMonListener : public RODimListener {
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
    typedef std::vector<std::pair<std::string,std::string> > Match;
    /// String matching of services
    Match m_match;
    int     m_type;
  public:
    /// Standard constructor
    FMCMonListener(bool verbose=false);
    /// Standard destructor
    virtual ~FMCMonListener();
    /// Change monitoring type
    void setType(int val)                 {  m_type = val;    }
    /// Access monitoring type
    int type() const                      {  return m_type;   }    
    /// Change service name matching string
    void addMatch(const std::string& val, const std::string& item) 
    {  m_match.push_back(std::pair<std::string,std::string>(val,item));  }
    /// Add handler for a given message source
    virtual void addHandler(const std::string& node,const std::string& svc);
    /// Remove handler for a given message source
    virtual void removeHandler(const std::string& node, const std::string& svc);
    /// DimInfoHandler overload
    static void infoHandler(void* tag, void* address, int* size);
  };
}      // End namespace ROMon
#endif /* ROMON_FMCMONLISTENER_H */

