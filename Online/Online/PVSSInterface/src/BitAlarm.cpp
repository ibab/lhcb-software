// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSInterface/src/BitAlarm.cpp,v 1.1 2007-03-01 10:39:49 frankb Exp $
//  ====================================================================
//  BitAlarm.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
// $ID: $

// Framework include files
#include "PVSS/CfgType.h"
#include "PVSS/DataPoint.h"
#include "PVSS/BitAlarm.h"

using namespace PVSS;

/// Initializing constructor
BitAlarm::BitAlarm(const std::string& dp_name) 
: BasicAlarm(dp_name) 
{
}

/// Initializing constructor
BitAlarm::BitAlarm(const DataPoint& dp) 
: BasicAlarm(dp.name()) 
{
}

/// Copy constructor
BitAlarm::BitAlarm(const BitAlarm& c)
: BasicAlarm(c), m_textOff(c.m_textOff), m_textOn(c.m_textOn), m_clazz(c.m_clazz),
  m_help(c.m_help), m_panel(c.m_panel), m_good(c.m_good)
{
}

/// Set alarm range
BitAlarm& BitAlarm::set(const std::string& text_on, 
                        const std::string& text_off, 
                        bool  good_bit,
                        const std::string& cl, 
                        const std::string& help, 
                        const std::string& panel)
{
  m_textOn = text_on;
  m_textOff = text_off;
  m_clazz = cl;
  m_help = help;
  m_good  = good_bit;
  m_panel = panel;
  return *this;
}

/// Apply alarm settings to datapoint identified by datapoint name
void BitAlarm::i_apply(Transaction& tr, const std::string& dp_name)   {
  tr->setValue(dp_name+".:_alert_hdl.._type", CfgType::ALERT_BINARYSIGNAL);
  tr->setValue(dp_name+".:_alert_hdl.._ok_range", m_good);
  tr->setValue(dp_name+".:_alert_hdl.._text0", m_textOff);
  tr->setValue(dp_name+".:_alert_hdl.._text1", m_textOn);
  if ( !m_clazz.empty() )  {
    tr->setValue(dp_name+".:_alert_hdl.._class", DPRef(m_clazz+"."));
  }
  if ( !m_help.empty() )  {
    tr->setValue(dp_name+".:_alert_hdl.._help", m_help);
  }
  if ( !m_panel.empty() ) {
    tr->setValue(dp_name+".:_alert_hdl.._panel", m_panel);
  }
  // tr->setValue(dp_name+".:_alert_hdl.._panel_param", ??);
  BasicAlarm::i_apply(tr,dp_name);
}

/// Assignment operator for insertion in STL containers
BitAlarm& BitAlarm::operator=(const BitAlarm& c)  {
  if ( &c != this )  {
    BasicAlarm::operator=(c);
    m_textOff   = c.m_textOff;
    m_textOn    = c.m_textOn;
    m_clazz     = c.m_clazz;
    m_help      = c.m_help;
    m_panel     = c.m_panel;
    m_good      = c.m_good;
  }
  return *this;
}

/// Equality operator for insertion in STL containers
bool BitAlarm::operator==(const BitAlarm& c) const  {
  if ( &c == this ) return true;
  return BasicAlarm::operator==(c) && m_textOff == c.m_textOff &&
         m_textOn == c.m_textOn && m_clazz == c.m_clazz &&
         m_help   == c.m_help   && m_panel == c.m_panel &&
         m_good   == c.m_good;
}

/// Operator less for insertion in STL associative classes
bool BitAlarm::operator<(const BitAlarm& c) const  {
  if ( &c == this ) return false;
  if ( BasicAlarm::operator<(c) ) return true;
  else if ( BasicAlarm::operator==(c) )  {
    if      ( m_textOff   < c.m_textOff ) return true;
    else if ( m_textOff   > c.m_textOff ) return false;
    else if ( m_textOn    < c.m_textOn  ) return true;
    else if ( m_textOn    > c.m_textOn  ) return false;
    else if ( m_clazz     < c.m_clazz   ) return true;
    else if ( m_clazz     > c.m_clazz   ) return false;
    else if ( m_help      < c.m_help    ) return true;
    else if ( m_help      > c.m_help    ) return false;
    else if ( m_panel     < c.m_panel   ) return true;
    else if ( m_panel     > c.m_panel   ) return false;
    else if ( m_good      < c.m_good    ) return true;
    else if ( m_good      > c.m_good    ) return false;
  }
  return false;
}
