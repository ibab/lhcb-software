// $Id: HltSummaryDisplay.h,v 1.1 2010-09-20 19:00:10 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/HltSummaryDisplay.h,v 1.1 2010-09-20 19:00:10 frankb Exp $
#ifndef ONLINE_ROMON_HLTSUMMARYDISPLAY_H
#define ONLINE_ROMON_HLTSUMMARYDISPLAY_H 1

// Framework includes
#include "ROMon/InternalDisplay.h"
#include "CPP/Interactor.h"

/*
 *   ROMon namespace declaration
 */
namespace ROMon {

  /**@class HltSummaryDisplay ROMon.h GaudiOnline/FarmDisplay.h
   *
   *   HltSummary display. Dislay shows up on CTRL-h
   *   HltSummary content is in $ROMONROOT/doc/farmMon.hlp
   *
   *   @author M.Frank
   */
  class HltSummaryDisplay : public InternalDisplay {
  public:
    /// Initializing constructor with default file
    HltSummaryDisplay(InternalDisplay* parent);
    /// Standard destructor
    virtual ~HltSummaryDisplay() {}
    /// Interactor overload: Display callback handler
    void handle(const CPP::Event& ev);
    /// Update display content
    virtual void update(const void*);
    /// Update display content
    virtual void update(const void* data, size_t len)  { this->InternalDisplay::update(data,len); }
  };

}      // End namespace ROMon
#endif /* ONLINE_ROMON_HLTSUMMARYDISPLAY_H */
