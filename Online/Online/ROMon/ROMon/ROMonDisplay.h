// $Id: ROMonDisplay.h,v 1.2 2008-02-08 21:20:39 frankm Exp $
//====================================================================
//  ROMon
//--------------------------------------------------------------------
//
//  Package    : ROMon
//
//  Description: Base class for displays for readout monitoring 
//               in the LHCb experiment
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/ROMonDisplay.h,v 1.2 2008-02-08 21:20:39 frankm Exp $
#ifndef ROMON_ROMONDISPLAY_H
#define ROMON_ROMONDISPLAY_H 1

// Framework includes
#include "RTL/rtl.h"
#include "SCR/ScrDisplay.h"

/*
 *   ROMon namespace declaration
 */
namespace ROMon {

  // Forward declarations
  class Nodeset;

  /**@class ROMonDisplay ROMon.h GaudiOnline/ROMon.h
   *
   *   DIM FSM status listener to collect the full information from nodes
   *
   *   @author M.Frank
   */
  class ROMonDisplay : public ScrDisplay  {
  public:
    struct Descriptor {
      size_t length;
      size_t actual;
      char*  pointer;
      Descriptor() : length(0), actual(0), pointer(0) {                                    }
      void   release()              { if(pointer) ::free(pointer); pointer=0; length=0; actual=0;}
      template<class T> T* data()   {	return (T*)pointer;                                }
      char*  reserve(size_t len);
    };

  protected:
    /// Name of the DIM service to connect
    std::string    m_svcName;
    /// DIC Client identifier
    int            m_svcID;
    /// Time delay in milliseconds between 2 updates
    int            m_delay;
    /// Handle to display lock
    lib_rtl_lock_t m_lock;
    /// Variable size data buffer
    Descriptor     m_data;
  public:
    /// Standard constructor
    ROMonDisplay();
    /// Standard destructor
    virtual ~ROMonDisplay();

    /// Initialize the data access
    virtual void initialize();

    /// Finalize data access
    virtual void finalize();

    /// Access to data buffer
    Descriptor& data()             {    return m_data; }

    /// Access to data buffer
    const Descriptor& data() const {    return m_data; }

    /// Access to lock handle
    lib_rtl_lock_t lock() const    {    return m_lock; }

    /// Start the update cycle
    virtual void update();

    /// Update all displays
    virtual void updateDisplay(const Nodeset& ns);

    /// Update all displays
    virtual void updateDisplay(const Node& ns);

    /// Run the interrupt loop
    virtual void run();

    /// DimInfoHandler overload
    static void infoHandler(void* tag, void* address, int* size);
  };
}      // End namespace ROMon
#endif /* ROMON_ROMONDISPLAY_H */

