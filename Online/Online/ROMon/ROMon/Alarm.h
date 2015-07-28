// $Id: Alarm.h,v 1.2 2009-06-02 16:21:23 frankb Exp $
//====================================================================
//  ROMon
//--------------------------------------------------------------------
//
//  Package    : ROMon
//
//  Description: Base class for monitors for readout monitoring 
//               in the LHCb experiment
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/Alarm.h,v 1.2 2009-06-02 16:21:23 frankb Exp $
#ifndef ROMON_ALARM_H
#define ROMON_ALARM_H 1

#include <string>
#include <vector>
#include <map>

// Framework includes

/*
 *   ROMon namespace declaration
 */
namespace ROMon {

  class Alarm;
  class AlarmTag;
  class Alarm;

  enum AlarmLevel {
    ERR_LVL_ALARM = 1<<31,
    ERR_LVL_ERROR = 1<<30,
    ERR_LVL_WARNING = 1<<29,
    ERR_LVL_MONITOR = 1<<28
  };

  enum AlarmCodes {
    ERR_NO_ERROR               = ERR_LVL_MONITOR,
    ERR_NO_UPDATES             = 1   +ERR_LVL_ALARM,
    ERR_NOT_USED               = 2   +ERR_LVL_MONITOR,
    ERR_SLOTS_LIMIT            = 3   +ERR_LVL_WARNING,
    ERR_SPACE_LIMIT            = 4   +ERR_LVL_WARNING,
    ERR_MEPRX_MISSING          = 5   +ERR_LVL_ALARM,
    ERR_MEPRX_STUCK            = 6   +ERR_LVL_ERROR, 
    ERR_RECEIVER_MISSING       = 7   +ERR_LVL_ALARM,
    ERR_RECEIVER_STUCK         = 8   +ERR_LVL_WARNING,
    ERR_SENDER_MISSING         = 9   +ERR_LVL_ALARM,
    ERR_SENDER_STUCK           = 10  +ERR_LVL_ERROR,
    ERR_MOORE_MISSING          = 11  +ERR_LVL_ALARM,
    ERR_MOORE_STUCK            = 12  +ERR_LVL_WARNING,
    ERR_MONITOR_MISSING        = 13  +ERR_LVL_ERROR,
    ERR_MONITOR_STUCK          = 14  +ERR_LVL_WARNING,
    ERR_NODAQ_ACTIVITY         = 15  +ERR_LVL_WARNING,
    ERR_NOHLT_ACTIVITY         = 16  +ERR_LVL_WARNING,
    ERR_NOSTORAGE_ACTIVITY     = 17  +ERR_LVL_WARNING,
    ERR_NOMONITORING_ACTIVITY  = 18  +ERR_LVL_MONITOR,
    ERR_NODE_STUCK             = 19  +ERR_LVL_ALARM,
    ERR_LAST_ERROR             = 20  +ERR_LVL_MONITOR,
    ERR_REMOVED                = 21  +ERR_LVL_MONITOR,
    ERR_REMOVEDALL             = 22  +ERR_LVL_MONITOR,


    ERR_NODE_DEAD              = 41  +ERR_LVL_ALARM,
    ERR_NO_PVSS_PROJECT        = 42  +ERR_LVL_ERROR,
    ERR_NO_CONNECTIONS         = 43  +ERR_LVL_ERROR,
    ERR_CONNECTION_MISSING     = 44  +ERR_LVL_WARNING,
    ERR_NO_TASKS               = 45  +ERR_LVL_ALARM,
    ERR_TASK_MISSING           = 46  +ERR_LVL_ERROR
  };

  class AlarmTag {
  public:
    int code;
    std::string subfarm, node;

    AlarmTag(int c, const std::string& s, const std::string& n) : code(c), subfarm(s), node(n) {    }
    AlarmTag(const AlarmTag& a) : code(a.code), subfarm(a.subfarm), node(a.node) {   }
    virtual ~AlarmTag() {}
    AlarmTag& operator=(const AlarmTag& a) {
      if ( &a != this ) {
        code = a.code;
        node = a.node;
        subfarm = a.subfarm;
      }
      return *this;
    }
  };

  class Alarm : public AlarmTag  {
  public:
    time_t when;
    std::string description;
    mutable std::string tag, optional;

    Alarm(const std::string& txt);
    Alarm(int c, time_t w, const std::string& s, const std::string& n, const std::string& dsc="",const std::string& opt="") 
      : AlarmTag(c,s,n), when(w), description(dsc), optional(opt) {    }
    Alarm(const Alarm& a) 
      : AlarmTag(a), when(a.when), description(a.description), tag(a.tag), optional(a.optional) {
    }
    Alarm(const Alarm& a, const std::string& t) 
      : AlarmTag(a), when(a.when), description(a.description), tag(t), optional(a.optional) {
    }
    virtual ~Alarm() {}
    static unsigned int hash32(const char* key);
    std::string toString(const std::string& t) const;
    void fromString(const std::string& s);
    int level() const;
    int color() const;
    std::string time() const;
    const char* message() const;
    
    Alarm& operator=(const Alarm& a) {
      if ( &a != this ) {
        this->AlarmTag::operator=(a);
        when = a.when;
        description = a.description;
        optional = a.optional;
      }
      return *this;
    }
  };

  typedef std::vector<Alarm>  Alarms;
  typedef std::vector<Alarm*> AlarmsPtr;
  typedef std::pair<std::string, Alarms> AlarmInfo;
  typedef std::map<char,int>             StateSummary;

  struct AlarmSum  {
    std::vector<Alarm*> nodes;
    AlarmSum() {}
    virtual ~AlarmSum() {}
  };
  typedef std::map<int,AlarmSum> AlarmSummary;

} // End namespace ROMon

#include <iostream>
std::ostream& operator<<(std::ostream& os, const ROMon::Alarm& a);

#endif /* ROMON_ALARM_H */
