// $Header: /afs/cern.ch/project/cvs/reps/lhcb/DAQ/MDF/MDF/MEPWriter.h,v 1.6 2008-04-09 15:16:42 ocallot Exp $
//  ====================================================================
//  MEPWriter.h
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
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
    /// Property: packing factor for merged MEPS
    int           m_packingFactor;
    /// Property: flag tio indicate if TAE events should be forced
    bool          m_makeTAE;
    /// Intermediate buffer to store raw events
    Events        m_events;
    /// Event ID
    unsigned int  m_evID;
    /// half window size for TAE
    int           m_halfWindow;

  public:

    /// Standard algorithm constructor
    MEPWriter(const std::string& name, ISvcLocator* pSvcLocator);

    /// Standard Destructor
    virtual ~MEPWriter();

    // Initialize procedure
    virtual StatusCode initialize();

    // Execute procedure
    virtual StatusCode execute();

    /// Dump stored events
    StatusCode dumpEvents(const Events& evts);
  };
}      // End namespace LHCb
#endif // MEP_MEPWRITER_H
