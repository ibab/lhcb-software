// $Id: HelpDisplay.h,v 1.1 2010-09-20 19:00:10 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/HelpDisplay.h,v 1.1 2010-09-20 19:00:10 frankb Exp $
#ifndef ONLINE_ROMON_HELPDISPLAY_H
#define ONLINE_ROMON_HELPDISPLAY_H 1

// Framework includes
#include "ROMon/InternalDisplay.h"
#include "CPP/Interactor.h"

/*
 *   ROMon namespace declaration
 */
namespace ROMon {

  /**@class HelpDisplay ROMon.h GaudiOnline/FarmDisplay.h
   *
   *   Help display. Dislay shows up on CTRL-h
   *   Help content is in $ROMONROOT/doc/farmMon.hlp
   *
   *   @author M.Frank
   */
  class HelpDisplay : public InternalDisplay {
  private:
    /// Private copy constructor
    HelpDisplay(const HelpDisplay&) : InternalDisplay() {}
    /// Private assignment operator
    HelpDisplay& operator=(const HelpDisplay&) { return *this; }
  public:
    /// Initializing constructor with default file
    HelpDisplay(InternalDisplay* parent, const std::string& title, const std::string& tag);
    /// Initializing constructor with file name
    HelpDisplay(InternalDisplay* parent, const std::string& title, const std::string& tag, const std::string& fname);
    /// Standard destructor
    virtual ~HelpDisplay() {}
    /// Initialize help display with data content
    void _init(const std::string& tag, const std::string& fname);
    /// Update display content
    virtual void update(const void*) {}
    /// Update display content
    virtual void update(const void* data, size_t len)  { this->InternalDisplay::update(data,len); }
  };

}      // End namespace ROMon
#endif /* ONLINE_ROMON_HELPDISPLAY_H */
