// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSInterface/PVSS/BitAlarm.h,v 1.1 2007-03-01 10:39:49 frankb Exp $
//  ====================================================================
//  BitAlarm.h
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
#ifndef ONLINE_PVSS_BITALARM_H
#define ONLINE_PVSS_BITALARM_H

// Framework include files
#include "PVSS/BasicAlarm.h"

/*
 *   PVSS namespace declaration
 */
namespace PVSS {

  class DataPoint;

  /** @class BitAlarm   BitAlarm.h  PVSS/BitAlarm.h
    *
    *  PVSS configuration attribute.
    *
    *   @author  M.Frank
    *   @version 1.0
    */
  class BitAlarm : public BasicAlarm {
  protected:
    /// Text to be displayed when in OFF state
    std::string m_textOff;
    /// Text to be displayed when in ON state
    std::string m_textOn;
    /// Alarm class
    std::string m_clazz;
    /// Help string for the alarm
    std::string m_help;
    /// Panel identifier
    std::string m_panel;
    /// State of "good" bit
    bool        m_good;
    /// Apply alarm settings to datapoint identified by datapoint name
    void i_apply(Transaction& tr, const std::string& dp_name);

  public:
    /// Initializing constructor
    BitAlarm() : BasicAlarm()  {}
    /// Copy constructor
    BitAlarm(const BitAlarm& copy);
    /// Initializing constructor
    BitAlarm(const std::string& dp_name);
    /// Initializing constructor
    BitAlarm(const DataPoint& dp);
    /// Standard destructor
    virtual ~BitAlarm()   {}
    /// Set alarm range
    BitAlarm& set(const std::string& text_on="ON", 
                  const std::string& text_off="OFF", 
                  bool good_bit=true,
                  const std::string& cl="", 
                  const std::string& help="", 
                  const std::string& panel="");
    /// Assignment operator for insertion in STL containers
    BitAlarm& operator=(const BitAlarm& alm);
    /// Equality operator for insertion in STL containers
    bool operator==(const BitAlarm& alm) const;
    /// Operator less for insertion in STL associative classes
    bool operator<(const BitAlarm& alm) const;
  };
}      // End namespace PVSS
#endif // ONLINE_PVSS_BITALARM_H
