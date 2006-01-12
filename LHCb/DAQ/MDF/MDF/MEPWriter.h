// $Header: /afs/cern.ch/project/cvs/reps/lhcb/DAQ/MDF/MDF/MEPWriter.h,v 1.3 2006-01-12 19:09:33 frankb Exp $
//	====================================================================
//  MEPWriter.h
//	--------------------------------------------------------------------
//
//	Author    : Markus Frank
//
//	====================================================================
#ifndef MEP_MEPWRITER_H
#define MEP_MEPWRITER_H

#include "MDF/MDFWriter.h"
#include <map>

/*
 *    LHCb namespace
 */
namespace LHCb    {

  // Forward declarations
  class RawEvent;

  /**@class MEPWriter 
    *
    *
    * @author:  M.Frank
    * @version: 1.0
    */
  class MEPWriter : public MDFWriter   {

  private:
    typedef std::map<unsigned int, RawEvent*> Events;
    int           m_packingFactor;
    Events        m_events;
    unsigned int  m_evID;
  public:

    /// Standard algorithm constructor
    MEPWriter(const std::string& name, ISvcLocator* pSvcLocator);

    /// Standard Destructor
    virtual ~MEPWriter();

    // Execute procedure
    virtual StatusCode execute();

    /// Dump stored events
    StatusCode dumpEvents(const Events& evts);
  };
}      // End namespace LHCb
#endif // MEP_MEPWRITER_H
